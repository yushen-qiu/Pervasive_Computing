# Pervasive Computing – Gandalf Staff

ESP32-E project to build an interactive Gandalf staff.

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
│    │── 📂 01_Report -> Project Documentations
│    │── 📂 02_Example -> Example sketches
│
│── 📂 03_Enclosure -> The 3D .STL model files
│── 📜 README.md
│── 📜 .gitignore
```

## Tools
Format all C/C++ files in the repository
```bash
sudo apt update
sudo apt install clang-format -y
# for macOS: brew install clang-format
# for Windows: winget install LLVM.LLVM

find 01_Code/ -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -exec clang-format -style=file -i {} +
```
