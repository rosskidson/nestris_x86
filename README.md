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
- Save keyboard/gamepad key bindings

The features that are missing are not for any technical reason, the focus was simply placed on other features.  Feature requests or suggesstions are welcome. Simply make a issue on the project github page, or just email me directly: ross dot kidson at gmail dot com

### Input latency
As mentioned already, there is no emulation taking place, and therefore no emulation lag. Input lag experienced will be due to usb polling and monitor response times, with the latter being the likely culprit. Comparing tetris on a physical NES + CRT TV to this software using a gaming monitor the input lag is noticeable, however, at least in the developer's opinion, certainly very playable.  Mileage will vary depending on operating system, monitor, bluetooth vs usb controller etc. etc.

### Download / Installation

This game depends on SDL-mixer

Linux:
```
sudo apt install libsdl-mixer1.2-dev libsdl1.2-dev
```
MacOS:
```
brew install libpng sdl_mixer
```
Windows:
SDL has been packaged in the release for Windows.

Binaries available under releases:
https://github.com/rosskidson/nestris_x86/releases/tag/v0.7

### Building

#### Checkout
The repository has subrepositories, therefore clone recursive:
```
git clone --recursive https://github.com/rosskidson/nestris_x86.git
```
If you are reading this after having checked out without the extra flag, simply run these commands:
```
git submodule init
git submodule update
```

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
SDL for windows has been included in this repository. After checking out the code open the directory in Visual Studio and configure using CMake.

### Description of Game Options

##### Configure Keyboard
Change the keyboard bindings.  The new bindings will not be in use until after selecting 'accept'

##### Configure Gamepad
Change the bindings of a usb/bluetooth controller. The new bindings will not be in use until after selecting 'accept'

##### DAS Profile
Despite the name, this doesn't just apply to the DAS, but also the level speeds.  Using PAL will result in a faster lvl 18 and the lvl 19 kill (thrill) screen.

- NTSC - North America/Japan 60hz NTSC version
- PAL - Europe 50hz version
- None - No das.  This is useful for practicing tapping
- NTSC DAS, without any initial delay
- Custom - customize the game frequency, das initial delay and das repeat delay

##### Hard drop
Enables/disables hard drop.  Use the up arrow to do a hard drop.

##### Wall Kick
Enables/disables wall kicks.

##### Statistics
- NES - The tetromino counts as in the original tetris game
- Trey V - 'Trey Vision' - these are the statistics that are to be seen in a typical CTWC match, plus a few others:
  - BURN - How many lines have been cleared from singles, doubles or triples since the last tetris
  - TRT - Tetris rate - The percentage of the score that comes from scoring tetrises
  - Long bar drought - How many pieces since that last long bar
  - DAS - DAS chain - This is a counter of how many pieces that are dropped without reseting the das counter to zero
  - ENTRY DL - Entry delay - This goes green when the entry delay, aka 'ARE' is active. During this time one may redirect DAS without resetting it
  - WALL CHR - Wall charge - This goes green when a wall charge is performed

##### Show DAS meter
Will show the das counter as a meter. Green means das is charged

##### Show Controls
Displays a small visual showing which keys are being pressed. Potentially useful for checking keyboard/gamepad layout.

##### RNG Type
Here one can select different types of random number generators for the tetromino spawning.  In all cases the code attempts to use non deterministic random number generation hardware, if available.  This is typically available on modern computers.  If this is not available, the identical piece set will occur every time.

- NES - This is an approximation of the original implementation.  A reroll is performed once if a piece is repeated, and there is a very slight bias for and against certain pieces.
- UNIFORM - A true uniform distrubution across all tetrominos. No rerolls, no guarentees on drought lengths
- 7-BAG - Seven bag - This is the approach for modern tetris, where a 'bag' of all seven tetrominos is shuffled and then dealt.  When the first set is finished the next one will be shuffled.  Using this means there is guarenteed to be no bias across tetrominos, and it also means the maximum drought length is 14 (start of the first set, and end of the second set)
  
### Acknowledgements / References
This was built using OneLoneCoder PixelGameEngine
- https://github.com/OneLoneCoder/olcPixelGameEngine
- https://www.youtube.com/channel/UC-yuWVUplUJZvieEligKBkA

There is a super detailed techincal analysis of the original assembly source code for nes tetris. This was extremely helpful to get all the fine details right.
- https://meatfighter.com/nintendotetrisai/

The tetris wiki was also very useful in providing details about das mechanics and the line clear animation.
- https://tetris.wiki/Tetris_(NES,_Nintendo)

The emulator of choice for development on this project is fceux:
- http://fceux.com/web/home.html

### Bugs, suggestions, feedback
Any feedback is very welcome! Open an issue or simply email me directly: ross dot kidson at gmail dot com
