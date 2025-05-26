# HDDLed

A lightweight daemon for visualizing disk activity using an LED on Raspberry Pi.  
Especially useful when using an external hard drive as the boot drive or when the Raspberry Pi is enclosed in a case.

---

## 📦 Installation

### 🔧 Build and install

```bash
git clone https://github.com/rsnakin/HDDLed.git
cd HDDLed
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
sudo make install
cd /usr/local/etc
sudo cp hddled.ini.default hddled.ini
sudo nano hddled.ini
```

### 🗂 The following components will be installed:

| File              | Installation path          | Purpose                                   |
|-------------------|----------------------------|-------------------------------------------|
| `hddled`          | `/usr/local/bin/`          | Disk activity LED daemon                  |
| `hddled.ini`      | `/usr/local/etc/`          | Configuration file                        |
| `hddled.service`  | `/etc/systemd/system/`     | systemd service to autostart the daemon   |

---

## 🔌 Running as a systemd service

```bash
sudo systemctl daemon-reload
sudo systemctl enable hddled.service
sudo systemctl start hddled.service
```

Check status and monitor:

```bash
systemctl status hddled.service
```

Stop the service:

```bash
sudo systemctl stop hddled.service
```

---

## ⚙️ Configuration

File: `/usr/local/etc/hddled.ini`

Example contents:

```ini
[setup]
PIN_NUMBERING = WPI                   ; BCM_GPIO or WPI
PIN = 25                              ; PIN number (see WiringPi/pins/pins.pdf or use `gpio readall`)
DELAY = 20000                         ; microseconds

[files]
PATH_TO_DISK_DATA = /proc/diskstats   ; path to diskstats
PATH_TO_LOG = /var/log/hddled.log     ; optional log file path
```

---

## 🧾 Project structure

```
HDDLed/
├── CMakeLists.txt
├── src/
│   ├── HDDLed.cpp
│   ├── Log.cpp
│   ├── cfgini.cpp
│   └── showActivity.cpp
├── include/
│   ├── Log.hpp
│   ├── cfgini.hpp
│   └── showActivity.hpp
├── config/
│   └── hddled.ini.default
├── service/
│   └── hddled.service.in
└── README.md
```

---

## 📚 Dependencies

- GCC or Clang
- CMake ≥ 3.18
- systemd
- [wiringPi](https://github.com/WiringPi/WiringPi)

---

## 📄 License

The project is licensed under the [MIT License](LICENSE).

---

## 👤 Author

[RSnakin](https://github.com/rsnakin)
