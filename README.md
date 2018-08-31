# kwin-effect-osdclock
KWin effect that displays transparent click-through clock at the corner of the screen

## Screenshots
![](/screenshots/0.png)

## Dependencies
* kwin

## Build dependencies
### Common
* cmake
* extra-cmake-modules
* git

### Fedora
* dbus-devel

### Ubuntu / Debian
* libdbus-1-dev

## Build and install
```bash
git clone https://github.com/laloch/kwin-effect-osdclock.git
cd kwin-effect-osdclock
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib ..
make
sudo make install
```

## TODO
* add 12h display format
* add an option to show/hide date
* adjustable date display format
