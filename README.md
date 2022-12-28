# Yet another Chip-8 ~~emulator~~ interpreter.

## History
Chip-8 was a virtual machine/programming language developed by RCA engineer Joseph Weisbecker in 1977 for the **COSMAC VIP** microcomputer. It was intended as a simple way to make small programs and games for the computer. Chip-8 soon spread to the other computers, like Finnish Telmac 1800, the Australian DREAM 6800, ETI-660 and MicroBee, and the Canadian ACE VDU.

## Technical information
The original Chip-8 was designed with the following specifications:

+ Memory: Chip-8 has direct access to up 4KB RAM
+ Display: 64x32 pixels monochrome, ie. Black or White
+ Keypad: 16 keys, labelled `0` through `F`, and were arranged in a 4x4 grid.
+ A _program counter_, often called "PC", which point at the current instruction in memory
+ One 16-bit _index register_ called "I" which is used to point at locations in memory
+ A _stack_ for 16-bit addresses, which is used to call subroutines/functions and return from them
+ An 8-bit _delay timer_ which is decremented at a rate of 60Hz (60 times per second) until reaches 0
+ An 8-bit _sound timer_ which functions like the delay timer, but which also gives off a beeping sound as long as it's not 0
+ 16 8-bit(one byte) general-purpose _variable_ registers numbered `0` through `F` hexadecimal, ie. 0 through 15 in decimal, called `V0` through `VF`
	- `VF` is also used as a _flag register_; many instructions will set it to either `1` or `0` based on some rule, for example using it as a carry flag

## Building
### Requirements
+ SDL2:
	- Minimum version required: 2.24.2
	- Required for rendering and input management.
+ CMake:
	- Minimum version requiered: 3.21
	- Required for automatic build.

### Procedures
### Linux
For building, you can use this procedure:  
```
$ mkdir build
$ cmake -B build -DCMAKE_BUILD_TYPE=Release
$ cmake --build build
```

### Windows
<sub>***Note***: Not tested, for while, there is no build procedure.</sub>

### Running
### Linux
After building with above procedure, the binary will be located in `build/bin/` directory.
Run the program using:
`$ ./build/bin/Chip8 <path-to-rom>`

<sub>There's some test roms located in `roms` directory, you can use them. :)</sub>

### Windows
I don't know.

## Command-line arguments
| long   | short | value | description                             |
|--------|-------|-------|-----------------------------------------|
| clock  |   c   |  int  | Set cpu clock speed(0-1000).            |
| width  |       |  int  | Set window width to desired resolution  |
| height |       |  int  | Set window height to desired resolution |
| help   |   h   |       | Show help message and then exits.       |
| vesbose|   v   |       | Enable log output on terminal.          |
| quiet  |   q   |       | Disbale log ouput on terminal.          |

<sub>***Note***: For while, the cpu screen will be stretched to the current resolution.</sub>

## Controls:
### Keyboard (This is the default keymap)
`1` `2` `3` `4`  
`Q` `W` `E` `R`  
`A` `S` `D` `F`  
`Z` `X` `C` `V`

### COSMAC VIP Keypad
`1` `2` `3` `C`  
`4` `5` `6` `D`  
`7` `8` `9` `E`  
`A` `0` `B` `F`

## TODOs
- [ ] Graphical user interface.
- [ ] Keyboard shortcuts for opening, reseting and exiting the emulator.

## Roms
+ [Revival Studio's Chip-8 Program Pack](https://github.com/kripod/chip8-roms)

## References
+ [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
+ [Tobias V.Langhoff](https://tobiasvl.github.io/blog/write-a-chip-8-emulator)
+ [Chip-8 Extensions and compatibility](https://chip-8.github.io/extensions)
+ [Chip-8 Test Suite](https://github.com/Timendus/chip8-test-suite)

## Contributing
Anyone is welcome to contribute!

## License
This project is licensed under the MIT license, so you are free to do almost everything you want.