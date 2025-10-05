# Pervasive Computing – Gandalf Staff

ESP32-E project to build an interactive Gandalf staff.

## Features

1. Capture voice input to set a target location.
2. Calibrate direction using sensors.
3. Provide navigation guidance toward the target.
4. Automatically switch on the light when it is dark.

# Pin Allocation
```
| Usage        | Left  | Right | Usage        |
|--------------|-------|-------|--------------|
| —            | —     | VCC   | —            |
| —            | —     | 3V3   | —            |
| —            | —     | GND   | —            |
| —            | 18    | 22    | —            |
| —            | 23    | 21    | —            |
| —            | 19    | 15    | —            |
| —            | 22    | 35    | —            |
| —            | 21    | 34    | —            |
| —            | 2     | 39    | —            |
| —            | 13    | 36    | —            |
| —            | 14    | 12    | —            |
| —            | 0     | 4     | —            |
| —            | 26    | 16    | —            |
| —            | 25    | 17    | —            |
```

## Repository Structure

```
📂 Root
│── 📂 01_Code
│    │── 📂 firmware
│         │── 📂 src -> Implementation files
│         │── 📂 include -> headers
│         │── 📂 platformio.ini -> Build configs
│
│── 📂 02_Doc
│    │── 📂 01_Example -> Example sketches
│
│── 📂 03_Enclosure -> The 3D .STL model files
│── 📜 README.md
│── 📜 .gitignore
```
