# Delay Plugin

* The plugin is purpose-built, real-time delay. It focuses on clean, tempo-syncable echo processing with additional shaping controls.

---

## ✨ Features

- **Tempo-Synced Delay**  
  Switch between free delay time (in ms) and musical note divisions.

- **Stereo Feedback Control**  
  Includes ping-pong delay behavior via a dedicated stereo width knob.

- **Tone Shaping**  
  Adjustable low cut, high cut, and Q factor to filter delay tails.

- **Drive Section**  
  Add harmonic saturation to delay signal with a smooth waveshaper.

- **Output Controls**  
  Mix, gain, and an elegant stereo level meter for accurate monitoring.

- **Preset System**  
  Save, load, cycle, and delete named presets via a custom panel.

---

## 🎛️ UI & UX Design

- **Dark Interface** with deep blacks and subtle gradients
- **Highlight Colors**: Regal purples and glowing oranges
- **Custom Knobs** with asymmetrical sweep and hover tooltips
- **Bypass Icon Button** and tempo sync toggle for clarity

---

## 🛠️ Build Instructions

### Requirements

- [JUCE 7 or 8](https://juce.com/)
- C++17 or newer
- Visual Studio 2022 / Xcode 14+ / CLion
- VST3 SDK (optional: for external plugin formats)

### Build Steps

1. Clone the repository
2. Open `DelayPlugin.jucer` in the Projucer (or use CMake setup)
3. Set your preferred plugin formats
4. Export the project for your IDE
5. Build → run as standalone, or test via DAW

---

## 📁 Folder Structure

