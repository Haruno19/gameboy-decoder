# gameboy-decoder
Little project I developed for fun and learning purposes.
<br><br>
Given a <code>rom.gb</code> file, <code>romdecoder</code> prints out all the information included in the rom header, and produces a <code>rom.asm</code> file contining the bytes from <code>0x0150</code> to <code>EOF</code> reversed parsed (disassembled?) into Z80 ASM instructions.

### soruces and docs used
- https://www.devrs.com/gb/files/gbspec.txt
- https://gbdev.gg8.se/wiki/articles/The_Cartridge_Header
- https://imrannazar.com/Gameboy-Z80-Opcode-Map
- https://gb-archive.github.io/salvage/decoding_gbz80_opcodes/Decoding%20Gamboy%20Z80%20Opcodes.html
