# MPV live sampler
## Description
Sampler/looper streaming live from media streams (local or internet). MIDI
controls.
## Requirements
- [MPV](https://github.com/mpv-player/mpv)
    - Requires [ffmpeg](https://www.ffmpeg.org/) build with `rubberband` enabled
- yt-dlp
- [ncurses](https://invisible-island.net/ncurses/)
- [RtMidi](https://github.com/thestk/rtmidi)
- [liblo](https://liblo.sourceforge.net/)
## Installation
```bash
mkdir build
cd build
cmake ..
make
```
Currently, `ytmus` looks in the current working directory for `config.ini` and `vids.txt`. You will have to `cd ..` to the project root and run with `./build/ytmus` for it to find the config files.
## Configuration
`config.ini` contains the MIDI mapping options. Most functionality is controlled with MIDI CC knobs, which can be mapped in the `[midi]` section of the config file. The `relative` setting will allow you to use "relative knobs" mode on certain midi controllers with infinitely turning knobs, where the knob outputs a specific number for direction enabling more flexible knob sensitivity over vs normal MIDI values.

To input videos, use `vids.txt`, entering one URL per line. Currently working on dynamically adding videos via some kind of command system.
