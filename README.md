# kwin-effect-osdclock
KWin effect that displays clock at the corner of the screen

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
