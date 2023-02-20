# gameboy-decoder
little project I developed for fun and learning purposes.


## functioning
given a <code>rom.gb</code> file, <code>romdecoder</code> prints out all the information included in the rom header, and produces a <code>rom.asm</code> file contining the bytes from <code>0x0150</code> to <code>EOF</code> reversed parsed (disassembled?) into Z80 ASM instructions.

## files
- <code>romdecoder.cpp</code>  
the main source file, it decodes the rom header and launches the bytes to Z80-ASM instructions decoder.
- <code>licensecodes.h</code>  
contains functions to translate bytes to rom license codes.
- <code>z80asm.h</code>  
contains all the function to parse bytes into Z80-ASM instructions.

## environment
compiled and tested with <code>Microsoft cl version 19.34.31937 for x64</code>  

## soruces and docs used
gameboy rom header format:
- https://www.devrs.com/gb/files/gbspec.txt
- https://gbdev.gg8.se/wiki/articles/The_Cartridge_Header

gameboy Z80 ASM instruction set (and decoding algorithm):
- https://imrannazar.com/Gameboy-Z80-Opcode-Map
- https://gb-archive.github.io/salvage/decoding_gbz80_opcodes/Decoding%20Gamboy%20Z80%20Opcodes.html

Z80 ASM documentation:
- https://www.zilog.com/docs/z80/um0080.pdf
