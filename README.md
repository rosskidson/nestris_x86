# Nestris x86

### Summary
This is a reimplementation of the original 1989 NES Tetris game using modern c++ compilers for modern computer hardware.  Nothing from the NES is being emulated here, this is a native implementation of the game for Windows / Linux / MacOs.  As a result there is no emulator lag.  However being a port, it is not identical, there are some aspects of the original game missing, whilst other features have been added.  It is the intent of this project to be as close as possible to the original NES game in terms of gameplay mechanics.

### Features
- Very low input latency
- Identical DAS mechanics
- Controller support (Windows and Linux)
- NTSC and PAL modes
- Identical rotation system
- Same RNG piece distribution

#### Additional features
- Customizable DAS speeds
- Customizable framerate
- Customizable RNG (e.g. uniform, 7-bag)
- CTWC "Trey Vision" statistics (tetris rate, droughts, burn etc.)
- Hard drop (may be enabled/disabled)
- Wall kick (may be enabled/disabled)
- DAS charge visualization

#### Missing features
- Type B
- Rocket launch endings
- Original music
- High scores

The features that are missing are not for any technical reason, I was simply focusing on what I wanted in the game and didn't consider these things important enough.  Feature requests or suggesstions are welcome. Simply make a issue on the project github page, or just email me directly: ross . kidson at gmail . com

### Input latency
As mentioned already, there is no emulation taking place, and therefore no emulation lag. Input lag experienced will be due to usb polling and monitor response times, with the latter being the likely culprit. I am a DAS player capable of lvl 19 speeds and I have a PB of ~470K on a TV CRT/physical console setup. Comparing the TV with this software using a gaming monitor I found that the input lag is noticeable, however certainly very playable.  However when testing on an older non gaming monitor there was a noticeble difference.

### Building
This depends on libsdl mixer for sound.

#### Linux
```
sudo apt install libsdl-mixer1.2-dev libsdl1.2-dev
```
```
mkdir build
cd build
cmake ..
make -j8
```
#### MacOS
```
brew install libpng sdl_mixer
```
```
mkdir build
cd build
cmake ..
make -j8
```
#### Windows
Haven't built this on windows yet....

### Acknowledgements / References
This was built using OneLoneCoder PixelGameEngine
- https://github.com/OneLoneCoder/olcPixelGameEngine
- https://www.youtube.com/channel/UC-yuWVUplUJZvieEligKBkA

There is a super detailed techincal analysis of the original assembly source code for nes tetris. This was extremely helpful to get all the fine details right.
- https://meatfighter.com/nintendotetrisai/

The tetris wiki was also very useful in providing details about das mechanics and the line clear animation.
- https://tetris.wiki/Tetris_(NES,_Nintendo)

My emulator of choice for development is fceux:
- http://fceux.com/web/home.html

### Bugs, suggestions, feedback
Open an issue or simply email me directly: ross . kidson @ gmail . com
