/*
  M5StickC PLUS2 - Live Mic Audio over WiFi (Web page + WebSocket stream)

  Audio format: PCM S16LE, mono, 16000 Hz
  HTTP:  http://<device_ip>/
  WS:    ws://<device_ip>:81/

  Libraries:
    - M5Unified
    - WebSockets (Markus Sattler / Links2004)
*/

#include <M5Unified.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

/////////////////////// USER WIFI ///////////////////////
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
/////////////////////////////////////////////////////////

static constexpr uint32_t SAMPLE_RATE = 16000;
static constexpr uint32_t CHUNK_MS    = 20;                              // 20ms packets
static constexpr size_t   SAMPLES_PER_CHUNK = (SAMPLE_RATE * CHUNK_MS) / 1000; // 320 samples @ 16kHz
static constexpr size_t   QUEUE_DEPTH = 18;                              // ~360ms buffer

WebServer server(80);
WebSocketsServer ws(81);

struct AudioFrame {
  int16_t s[SAMPLES_PER_CHUNK];
};

QueueHandle_t audioQ;
volatile int wsClients = 0;

// Simple HTML player page (served from flash)
static const char INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html>
<head>
  <meta charset="utf-8"/>
  <meta name="viewport" content="width=device-width,initial-scale=1"/>
  <title>M5StickC PLUS2 - Live Mic</title>
  <style>
    body{background:#0e0f12;color:#e8e8e8;font-family:system-ui,Segoe UI,Arial;margin:18px}
    .card{background:#171922;border:1px solid #2a2e3a;border-radius:14px;padding:14px;max-width:720px}
    button{background:#2b6cff;color:white;border:0;border-radius:10px;padding:10px 14px;font-weight:700;cursor:pointer}
    button:disabled{opacity:.55;cursor:not-allowed}
    .row{display:flex;gap:10px;flex-wrap:wrap;align-items:center}
    input[type=range]{width:220px}
    code{background:#0b0c10;padding:2px 6px;border-radius:8px}
    .muted{color:#aab0c0}
    .ok{color:#7CFC98}
    .bad{color:#ff6b6b}
  </style>
</head>
<body>
  <div class="card">
    <h2 style="margin:0 0 8px 0;">M5StickC PLUS2 – Live Mic</h2>
    <div class="muted" style="margin-bottom:12px;">
      Audio: <code>PCM 16-bit</code> <code>mono</code> <code id="sr">16000 Hz</code> via WebSocket
    </div>

    <div class="row" style="margin-bottom:10px;">
      <button id="btnStart">Start listening</button>
      <button id="btnStop" disabled>Stop</button>

      <label class="muted">Gain</label>
      <input id="gain" type="range" min="0" max="3" step="0.01" value="1"/>
      <span id="gainVal" class="muted">1.00×</span>
    </div>

    <div id="status" class="muted">Not connected.</div>
    <div class="muted" style="margin-top:10px;">
      Tip: If you hear “crackles”, lower Gain or move closer to WiFi.
    </div>
  </div>

<script>
(() => {
  let ws;
  let audioCtx;
  let processor;
  let inRate = 16000;
  let gain = 1.0;

  // Ring buffer (Float32) at output sample-rate
  let ring, ringSize, r=0, w=0, avail=0;

  function setStatus(msg, cls="muted") {
    const el = document.getElementById("status");
    el.className = cls;
    el.textContent = msg;
  }

  function initRing(outRate) {
    const seconds = 2.0; // buffer length
    ringSize = Math.ceil(outRate * seconds);
    ring = new Float32Array(ringSize);
    r = w = avail = 0;
  }

  function pushSamples(arr) {
    // If overflow: drop oldest samples
    for (let i=0; i<arr.length; i++) {
      ring[w] = arr[i];
      w = (w + 1) % ringSize;
      if (avail < ringSize) avail++;
      else r = (r + 1) % ringSize;
    }
  }

  function pullSamples(out) {
    for (let i=0; i<out.length; i++) {
      if (avail > 0) {
        out[i] = ring[r];
        r = (r + 1) % ringSize;
        avail--;
      } else {
        out[i] = 0;
      }
    }
  }

  function resampleLinearS16ToF32(int16Arr, inRate, outRate) {
    const ratio = outRate / inRate;
    const outLen = Math.max(1, Math.round(int16Arr.length * ratio));
    const out = new Float32Array(outLen);
    for (let i=0; i<outLen; i++) {
      const pos = i / ratio;
      const idx = Math.floor(pos);
      const frac = pos - idx;

      const s0 = int16Arr[Math.min(idx, int16Arr.length - 1)];
      const s1 = int16Arr[Math.min(idx + 1, int16Arr.length - 1)];
      const s = s0 + (s1 - s0) * frac;

      out[i] = (s / 32768.0) * gain;
    }
    return out;
  }

  async function start() {
    document.getElementById("btnStart").disabled = true;

    // Must be user gesture (button click) to start audio in browsers
    audioCtx = new (window.AudioContext || window.webkitAudioContext)();
    initRing(audioCtx.sampleRate);

    processor = audioCtx.createScriptProcessor(4096, 0, 1);
    processor.onaudioprocess = (e) => {
      const ch0 = e.outputBuffer.getChannelData(0);
      pullSamples(ch0);
    };
    processor.connect(audioCtx.destination);

    const url = `ws://${location.hostname}:81/`;
    ws = new WebSocket(url);
    ws.binaryType = "arraybuffer";

    ws.onopen = () => {
      setStatus(`Connected. Listening… (ws: ${url})`, "ok");
      document.getElementById("btnStop").disabled = false;
    };

    ws.onclose = () => {
      setStatus("Disconnected.", "bad");
      stop(true);
    };

    ws.onerror = () => {
      setStatus("WebSocket error.", "bad");
    };

    ws.onmessage = (ev) => {
      if (typeof ev.data === "string") {
        try {
          const j = JSON.parse(ev.data);
          if (j && j.sr) {
            inRate = j.sr;
            document.getElementById("sr").textContent = `${inRate} Hz`;
          }
        } catch {}
        return;
      }

      const ab = ev.data;
      const pcm = new Int16Array(ab);
      const out = resampleLinearS16ToF32(pcm, inRate, audioCtx.sampleRate);
      pushSamples(out);
    };
  }

  function stop(fromWsClose=false) {
    if (!fromWsClose && ws) {
      try { ws.close(); } catch {}
    }
    ws = null;

    if (processor) {
      try { processor.disconnect(); } catch {}
      processor = null;
    }
    if (audioCtx) {
      try { audioCtx.close(); } catch {}
      audioCtx = null;
    }

    document.getElementById("btnStart").disabled = false;
    document.getElementById("btnStop").disabled = true;

    if (!fromWsClose) setStatus("Stopped.", "muted");
  }

  document.getElementById("btnStart").addEventListener("click", start);
  document.getElementById("btnStop").addEventListener("click", () => stop(false));

  const gainEl = document.getElementById("gain");
  const gainVal = document.getElementById("gainVal");
  gainEl.addEventListener("input", () => {
    gain = parseFloat(gainEl.value);
    gainVal.textContent = `${gain.toFixed(2)}×`;
  });
})();
</script>
</body>
</html>
)HTML";

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED: {
      wsClients++;
      // Tell client the input sample-rate (JSON text)
      String hello = String("{\"sr\":") + SAMPLE_RATE + ",\"bits\":16,\"ch\":1}";
      ws.sendTXT(num, hello);
    } break;

    case WStype_DISCONNECTED:
      wsClients = (wsClients > 0) ? (wsClients - 1) : 0;
      break;

    default:
      break;
  }
}

void micTask(void* pv) {
  AudioFrame frame;

  for (;;) {
    // Record one chunk (blocking until filled)
    if (M5.Mic.record(frame.s, SAMPLES_PER_CHUNK)) {
      // If queue is full, drop oldest by receiving one, then push new
      if (uxQueueSpacesAvailable(audioQ) == 0) {
        AudioFrame junk;
        xQueueReceive(audioQ, &junk, 0);
      }
      xQueueSend(audioQ, &frame, 0);
    } else {
      vTaskDelay(1);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // M5Unified init (enable internal mic)
  auto cfg = M5.config();
  cfg.internal_mic = true;     // use built-in microphone :contentReference[oaicite:4]{index=4}
  cfg.internal_spk = false;    // (PLUS2 has buzzer; keep speaker off)
  M5.begin(cfg);

  // Configure mic
  M5.Mic.begin();              // init mic :contentReference[oaicite:5]{index=5}
  M5.Mic.setSampleRate(SAMPLE_RATE); // :contentReference[oaicite:6]{index=6}

  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false); // lower latency for streaming
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  M5.Display.setRotation(1);
  M5.Display.clear();
  M5.Display.setTextSize(2);
  M5.Display.setCursor(0, 0);
  M5.Display.println("Mic Stream");
  M5.Display.println("Connecting...");

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  M5.Display.clear();
  M5.Display.setCursor(0, 0);
  M5.Display.println("Open:");
  M5.Display.setTextSize(1);
  M5.Display.println("http://");
  M5.Display.println(WiFi.localIP());

  // HTTP server
  server.on("/", handleRoot);
  server.begin();

  // WebSocket server
  ws.begin();
  ws.onEvent(webSocketEvent);

  // Audio queue + mic task
  audioQ = xQueueCreate(QUEUE_DEPTH, sizeof(AudioFrame));
  xTaskCreatePinnedToCore(micTask, "micTask", 4096, nullptr, 2, nullptr, 0);

  Serial.println("HTTP on :80, WS on :81");
}

void loop() {
  server.handleClient();
  ws.loop();

  // If no listeners, keep queue small (drop stale audio)
  if (wsClients <= 0) {
    AudioFrame junk;
    while (xQueueReceive(audioQ, &junk, 0) == pdTRUE) { /* flush */ }
    delay(10);
    return;
  }

  // Send as fast as frames arrive (low latency)
  AudioFrame frame;
  while (xQueueReceive(audioQ, &frame, 0) == pdTRUE) {
    ws.broadcastBIN((uint8_t*)frame.s, sizeof(frame.s));
  }
}
