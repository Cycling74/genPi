# Installing genPi

## OSX:
1. get Homebrew
2. `brew install rt-audio rtmidi`
3. `cd /path/to/genPi`
4. `mkdir build-xcode`
5. `cd build-xcode`

### If you want to work in Xcode:
- `cmake .. -G Xcode`
- Open the generated Xcode project (in the build-xcode directory).

### If you want to work from the CLI:
- `cmake ..`
- `make`
- `./genPi`

## Raspberry Pi
1. `sudo apt-get install librtaudio-dev librtmidi-dev cmake g++-4.9`
2. if you want to use wiringPi (for GPIO hardware support), `sudo apt-get install wiringpi`
3. `cd /path/to/genPi`
4. `mkdir build`
5. `cd build`
6. `cmake ..`
7. `make`
8. `./genPi`

## Replace genexport.cpp with your own exported code!