# Pandapute - a mini computer built from scratch

## The Inspiration

So, one day i was working on my Split Keyboard project at my makespace, and i saw a friend of mine using a cardputer adv, and was curious.
I asked him what it it was, and he showed me, and i gotta say, i was skeptical at first, but then i was blown away - i just had one complaint when i saw the thing :
the keyboard. You see, the original cardputer uses this tiny ass 56-key keyboard, which is 1) too many keys in my opinion, and 2) extremeky hard to type on.

So, i decided to make my own, and get funding from Outpost.

## The Features

So, the Pandapute features every single featire the Cardputer has + more !

It has :

- The best ESP32 chip that has wifi and bluetooth capabilities
- a actual **extremely** low profile, compact, built in mechanical 32-key keyboard
- an Audio Jack
- A LiPo battery that will last ages
- a built in speaker
- a microphone
- exposed I2C, GPIO, Power, etc. Pins so you can connect external modules or breakout boards, like a secondary display
- A USB-C port for **both** flashing **and** charging
- a tilt sensor (to do cool stuff)
- a 2.8" display (around double as big as the cardputer)

## The Build

The Cardputer is built from scratch by me, Panda

It features an entirely custom

### Schematic

#### Power

![](assets/img/PowerFinal.png)

#### ESP

![](assets/img/ESPfinal.png)

#### Audio

![](assets/img/AudioFinal.png)

#### Display

![](assets/img/DisplayFinal.png)

#### Keyboard

![](assets/img/KeyboardFinal.png)

#### Peripherals

![](assets/img/PeripheralsFinal.png)

and

### PCB

![](assets/img/PCBfinal.png)

Now yer probably wondering, will you just carry a PCB around ? OF COURSE NOT. But you know what im also not gonna do ?
Put it in some lame ass 3d printed case, that would be boring, inaccurate, and wouldn't feel good in my hands. So ... whats my solution for the

### Case

You see, instead of the usual 3d printed stuff, i've decided im going to use the full potential of my makespace and make
a custom wood case with a nice finish and accurate measurements after ordering the components, using my local wood workshop.

### Firmware

The firmware is a full OS-like environment, not just a bunch of drivers. It runs on the Arduino framework with PlatformIO and gives you a desktop UI, terminal, text editor, file manager, and USB HID keyboard mode.

```
firmware/
├── platformio.ini               # dev + release builds (TFT_eSPI, SdFat)
├── include/
│   ├── config.h                 # all pin definitions
│   ├── kernel.h                 # OS kernel — app management, status bar
│   └── apps.h                   # app function declarations
└── src/
    ├── main.cpp                 # boots hardware + OS, main event loop
    ├── kernel.cpp               # app switching, status bar, event routing
    ├── apps/
    │   ├── desktop.cpp          # home screen — app grid launcher
    │   ├── terminal.cpp         # shell with commands (help, clear, echo, bat, reboot)
    │   ├── editor.cpp           # text editor — cursor, insert/delete, Ln/Col status
    │   ├── files.cpp            # SD card file browser
    │   ├── settings.cpp         # brightness, volume, USB mode toggles
    │   ├── piano.cpp            # keyboard piano — keys 1-8 play notes, octave shift
    │   ├── mic.cpp              # mic level visualizer — real-time bar graph
    │   └── tilt.cpp             # MPU6050 orientation — circle + dot + accel/gyro values
    ├── keyboard/                # 4×8 matrix scanner, debounce, HID keymap
    ├── display/                 # TFT_eSPI wrapper
    ├── audio/                   # I²S output + ES8311 codec + mic input
    ├── touch/                   # XPT2046 SPI touch with calibration
    ├── power/                   # IP5306 battery manager + ADC fallback
    ├── sd_card/                 # SdFat wrapper
    ├── usb_hid/                 # USB HID keyboard — plug in and type
    └── mpu/                     # MPU6050 accelerometer + gyroscope driver
```

#### OS Architecture

The **kernel** manages 8 apps and a persistent status bar. Every keypress is routed to the currently focused app. The loop runs at ~100 Hz:

1. **Keyboard** — matrix scan → debounce → HID keycode lookup
2. **USB HID** — if connected to a computer, sends the same keystrokes
3. **App** — keypress fed to current app's `on_key()` handler
4. **Touch** — polls XPT2046 for touch position
5. **Battery** — reads IP5306 every 10s, shows in status bar
6. **Status bar** — redrawn every 200ms with app name + battery %
7. **Sensors** — MPU6050 and microphone polled by their respective apps

#### Apps

| App | Icon | What it does |
|-----|------|-------------|
| Desktop | ~ | Grid launcher, arrow keys to navigate, Enter to launch, ESC back |
| Terminal | > | Shell with `help, clear, echo, bat, apps, reboot` |
| Editor | E | Notepad with cursor movement, insert/delete, modified indicator |
| Files | F | SD card directory listing |
| Settings | S | Brightness slider (PWM), volume (I²C codec), USB mode toggles |
| Piano | P | 8-key piano (keys 1-8), UP/DN change octave, visual key press |
| Mic | M | Real-time microphone level bar graph, record/pause toggle |
| Tilt | T | 3D orientation circle with dot, live accelerometer/gyro values |

Pressing **ESC** from any app returns to the Desktop.

#### USB HID

When connected to a computer via USB-C with `-DARDUINO_USB_MODE=1` and `-DARDUINO_USB_CDC_ON_BOOT=1`, the Pandapute shows up as a standard USB keyboard. Everything you type on the mechanical keyboard goes straight to the computer. Modifier keys (Shift, Ctrl, Alt) work too.

#### Building

```bash
cd firmware
pio run -e esp32-s3-dev
pio run -e esp32-s3-dev -t upload
pio device monitor -b 115200
```

Display pins are set in `platformio.ini` via `build_flags`. Touch and SD pins are in `config.h`.

#### Keymap

The keymap in `keyboard/keymap.h` uses standard USB HID keycodes (0x04–0xE7). Modify the `KEYMAP[4][8]` array to match your physical key placement. There are placeholder arrays for `KEYMAP_FN` and `KEYMAP_SYM` layers if you want dual-layer action later.

#### config.h

Single file with every pin, I²C address, and system constant. Change the PCB pinout → edit one file.

### BOM

| # | Qty | Part | Package / Notes |
|---|-----|------|-----------------|
| 1 | 1 | ESP32-S3-WROOM-1-N16R8 | module 18×25.5mm |
| 2 | 1 | IP5306 | QFN, Injoinic power bank IC |
| 3 | 1 | NS4150B | MSOP-8, 3W Class D amp |
| 4 | 1 | AP2112K-3.3 | SOT-23-5, 3.3V LDO |
| 5 | 1 | ES8311 | QFN, audio codec I²S |
| 6 | 1 | TCA8418RTWR | QFN, keyboard matrix controller |
| 7 | 1 | USB-C receptacle HRO TYPE-C-31-M-12 | 16-pin, USB 2.0 |
| 8 | 37 | 1N4148 | DO-35, keyboard matrix diodes |
| 9 | 32 | Kailh Choc v1 switch | low-profile mechanical |
| 10 | 2 | LED 0805 | power + charging indicators |
| 11 | 1 | Resistor kit 0805 | 10k×5, 5.1k×2, 100k×3, 4.7k×2, 2.2k×1, 1k×2 |
| 12 | 1 | Capacitor kit 0603 | 22µF×2, 10µF×2, 2.2µF×1, 1µF×12, 0.1µF×3 |
| 13 | 1 | 2.2µH inductor | 1206, IP5306 boost |
| 14 | 1 | Pin header 1×40 (male) | snap to size |
| 15 | 1 | Pin socket 1×40 (female) | snap to size |
| 16 | 1 | Slide switch SPDT | battery on/off |
| 17 | 3 | Omron B3F tactile 6mm | reset, boot, user |
| 18 | 1 | SKRHABE010 | 5-way nav switch, Alps Alpine |
| 19 | 2 | JST PH 1×02 2.0mm | speaker connectors |
| 20 | 1 | PJ-342B | 3.5mm audio jack THT |
| 21 | 1 | CMA-4544PF-W | electret microphone |
| 22 | 1 | MPU6050 | 6-axis IMU I²C |
| 23 | 1 | 804050 LiPo 2000mAh 3.7V | 80×40×5mm with protection |
| 24 | 1 | 2.8" ILI9341 + touch + SD | SPI TFT module (TBD exact model) |
| 25 | 1 | Speaker 4Ω 3W | 35mm or 40mm |
| 26 | 32 | Choc v1 keycaps | low-profile 1u |
| 27 | 1 | M3 standoff + screw kit | assorted hex standoffs |
| 28 | 1 | M3 heatset insert kit | brass threaded inserts |
| 29 | 1 | Rubber feet kit | adhesive silicone |
| 30 | 1 | External SPI TFT | Waveshare-style 1.44"-2.0" |
| 31 | 1 | 14-pin dupont ribbon cable | F-F for external display |
