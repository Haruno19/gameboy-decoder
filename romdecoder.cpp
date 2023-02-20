/** www.devrs.com/gb/files/gbspec.txt                   **/
/** gbdev.gg8.se/wiki/articles/The_Cartridge_Header     **/

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "licensecodes.h"
#include "z80asm.h"

#define input_rom "rom.gb"
#define asm_output "rom.asm"

using namespace std;

struct {
    char* ENTRY_POINT;
    char* TITLE;
    char CGB;
    char* LICENSE_CODE;
    char SGB;
    char CART_TYPE;
    char ROM_SIZE;
    char RAM_SIZE;
    char DEST_CODE;
    char VERSION;
    char HEADER_CHECKSUM;   
    char* GLOBAL_CHECKSUM;
} rom_h;

ifstream rom;

void OpenRom();
void Header();          //  0100 - 014F
void Tail();

void EntryPoint();      //  0100 - 0103
void NintendoLogo();    //  0104 - 0133
void Title();           //  0134 - 1042
void CGB();             //  0143
void LicenseCode();     //  0144 - 0145
void SGB();             //  0146
void Type();            //  0147
void ROMSize();         //  0148
void RAMSize();         //  0149
void DestinationCode(); //  014A
void OldLicenseCode();  //  014B
void RomVersion();      //  014C
void HeaderChecksum();  //  014D
void GlobalChecksum();  //  014E - 014F

int main()
{
    OpenRom();
    Header();
    Tail();

    cout << endl;
    rom.close();
    return 0;
}

void OpenRom()
{
    rom.open(input_rom, ios::binary | ios::in);
    cout << endl << "*--- ROM loaded ---*" << endl;
}

void Header()       //  0100 - 014F 
{   
    cout << endl << "-*** ROM HEADER ***-" << endl;
    EntryPoint();
    NintendoLogo();
    Title();
    CGB();
    LicenseCode();
    SGB();
    Type();
    ROMSize();
    RAMSize();
    DestinationCode();
    OldLicenseCode();
    RomVersion();
    HeaderChecksum();
    GlobalChecksum();
    cout << endl << "-*** END OF HEADER ***-" << endl;
}

void Tail()
{
    ofstream rom_asm;
    char b; int i = 0, s;
    cout << endl << "-*** ROM TAIL ***-" << endl << endl << " ";
    rom.seekg(0, ios::end);
    s=int(rom.tellg())-0x014F;
    rom.seekg(0x0150, ios::beg);
    rom_asm.open(asm_output);
    while(!rom.eof())
    {
        rom.read(&b, sizeof(b));
        cout << "\rDecoding byte "+to_string(++i)+" of "+to_string(s);
        rom_asm << DecodeByte(b);        
    }
    rom_asm.close();
    cout << endl << "Done." << endl << endl << "-*** END OF FILE ***-" << endl;
}


void EntryPoint()           //  0100 - 0103 
{
    int i=0x0100;
    cout << endl << "*-- Entry Point Instruction: " << endl << "[";
    rom_h.ENTRY_POINT = (char*) calloc(4, sizeof(char));
    rom.seekg(i, ios::beg);
    for(i; i<0x0104; i++)
    {
        rom.read(&rom_h.ENTRY_POINT[i-0x0100], sizeof(rom_h.ENTRY_POINT[i-0x0100]));
        cout << setw(2) << setfill('0') << hex << (int) (rom_h.ENTRY_POINT[i-0x0100] & 0xff) << " ";
    }
    cout << "\b]" << endl << EntryPointTranslator(rom_h.ENTRY_POINT) << endl;
}

void NintendoLogo()         //  0104 - 0133 
{
    char b; int i=0x0104;
    cout << endl << "*-- Nintendo Logo Bitmap Hexdump: " << endl;
    rom.seekg(i, ios::beg);
    for(i; i<0x0134; i++)
    {
        rom.read(&b, sizeof(b));
        cout << setw(2) << setfill('0') << hex << (int) (b & 0xff) << " ";
        if((i-0x0104+1)%16==0) cout << endl;
    }
}

void Title()                //  0134 - 1042 
{
    int i=0x0134;
    cout << endl << "*-- ROM Title: " << endl;
    rom_h.TITLE = (char*) calloc(8, sizeof(char));
    rom.seekg(i, ios::beg);
    for(i; i<0x0143; i++)
    {
        rom.read(&rom_h.TITLE[i-0x0134], sizeof(rom_h.TITLE[i-0x0134]));
        cout << rom_h.TITLE[i-0x0134];
    }
    cout << endl;
}

void CGB()                  //  0143        
{
    int i=0x0143;
    cout << endl << "*-- CGB Byte: " << endl;
    rom.seekg(i, ios::beg);
    rom.read(&rom_h.CGB, sizeof(rom_h.CGB));
    cout << setw(2) << setfill('0') << hex << (int) (rom_h.CGB & 0xff) << " ";
    (rom_h.CGB == 0x80) ? cout <<  "(CBG Capable)" : cout <<  "(CGB Uncapable)";
    cout << endl;
}

void LicenseCode()          //  0144 - 0145 
{
    int i=0x0144;
    cout << endl << "*-- License Code: " << endl;
    rom_h.LICENSE_CODE = (char*) calloc(2, sizeof(char));
    rom.seekg(i, ios::beg);
    rom.read(&rom_h.LICENSE_CODE[1], sizeof(rom_h.LICENSE_CODE[1]));
    rom.read(&rom_h.LICENSE_CODE[0], sizeof(rom_h.LICENSE_CODE[0]));
    cout << rom_h.LICENSE_CODE[1] << rom_h.LICENSE_CODE[0] << " (" << LicenseTranslator(rom_h.LICENSE_CODE) << ") " << endl;
}

void SGB()                  //  0146        
{
    int i=0x0146;
    cout << endl << "*-- SGB Byte: " << endl;
    rom.seekg(i, ios::beg);
    rom.read(&rom_h.SGB, sizeof(rom_h.SGB));
    cout << setw(2) << setfill('0') << hex << (int) (rom_h.CGB & 0xff) << " ";
    (rom_h.CGB == 0x03) ? cout <<  "(SGB Capable)" : cout <<  "(SGB Uncapable)";
    cout << endl;
}

void Type()                 //  0147        
{
    int i=0x0147;
    cout << endl << "*-- Cartridge Type: " << endl;
    rom.seekg(i, ios::beg);
    rom.read(&rom_h.CART_TYPE, sizeof(rom_h.CART_TYPE));
    switch(rom_h.CART_TYPE)
    {
        case 0x00: cout << "ROM ONLY"; break;
        case 0x01: cout << "ROM+MBC1"; break;
        case 0x02: cout << "ROM+MBC1+RAM"; break;
        case 0x03: cout << "ROM+MBC1+RAM+BATT"; break;
        case 0x05: cout << "ROM+MBC2"; break;
        case 0x06: cout << "ROM+MBC2+BATTERY"; break;
        case 0x08: cout << "ROM+RAM"; break;
        case 0x09: cout << "ROM+RAM+BATTERY"; break;
        case 0x0B: cout << "ROM+MMM01"; break;
        case 0x0C: cout << "ROM+MMM01+SRAM"; break;
        case 0x0D: cout << "ROM+MMM01+SRAM+BATT"; break;
        case 0x0F: cout << "ROM+MBC3+TIMER+BATT"; break;
        case 0x10: cout << "ROM+MBC3+TIMER+RAM+BATT"; break;
        case 0x11: cout << "ROM+MBC3"; break;
        case 0x12: cout << "ROM+MBC3+RAM"; break;
        case 0x13: cout << "ROM+MBC3+RAM+BATT"; break;
        case 0x19: cout << "ROM+MBC5"; break;
        case 0x1A: cout << "ROM+MBC5+RAM"; break;
        case 0x1B: cout << "ROM+MBC5+RAM+BATT"; break;
        case 0x1C: cout << "ROM+MBC5+RUMBLE"; break;
        case 0x1D: cout << "ROM+MBC5+RUMBLE+SRAM"; break;
        case 0x1E: cout << "Pocket Camera"; break;
        case 0xFD: cout << "Bandai TAMA5"; break;
        case 0xFE: cout << "Hudson HuC-3"; break;
        case 0xFF: cout << "Hudson HuC-1"; break;
        defult: cout << "invalid";
    }
    cout << endl;
}

void ROMSize()              //  0148        
{
    int i=0x0148;
    cout << endl << "*-- ROM Size: " << endl;
    rom.seekg(i, ios::beg);
    rom.read(&rom_h.ROM_SIZE, sizeof(rom_h.ROM_SIZE));
    switch(rom_h.ROM_SIZE)
    {
        case 0x00: cout << "[256 Kbit] [32 KByte] [2 banks]"; break;
        case 0x01: cout << "[512 Kbit] [64 KByte] [4 banks]"; break;
        case 0x02: cout << "[1 Mbit] [128 KByte] [8 banks]"; break;
        case 0x03: cout << "[2 Mbit] [256 KByte] [16 banks]"; break;
        case 0x04: cout << "[4 Mbit] [512 KByte] [32 banks]"; break;
        case 0x05: cout << "[8 Mbit] [1 MByte] [64 banks]"; break;
        case 0x06: cout << "[16 Mbit] [2 MByte] [128 banks]"; break;
        case 0x52: cout << "[9 Mbit] [1,1 MByte] [72 banks]"; break;
        case 0x53: cout << "[10 Mbit] [1,2 MByte] [80 banks]"; break;
        case 0x34: cout << "[12 Mbit] [1,5 MByte] [96 banks]"; break;
        defult: cout << "invalid";
    }
    cout << endl;
}

void RAMSize()              //  0149        
{
    int i=0x0149;
    cout << endl << "*-- RAM Size: " << endl;
    rom.seekg(i, ios::beg);
    rom.read(&rom_h.RAM_SIZE, sizeof(rom_h.RAM_SIZE));
    switch(rom_h.RAM_SIZE)
    {
        case 0x00: cout << "[Non]"; break;
        case 0x01: cout << "[16 Kbit] [2 KByte] [1 banks]"; break;
        case 0x02: cout << "[64 Kbit] [8 KByte] [1 banks]"; break;
        case 0x03: cout << "[256 Kbit] [32 KByte] [4 banks]"; break;
        case 0x04: cout << "[1 Mbit] [128 KByte] [16 banks]"; break;
        defult: cout << "invalid";
    }
    cout << endl;
}

void DestinationCode()      //  014A        
{
    int i=0x014A;
    cout << endl << "*-- Destination Code: " << endl;
    rom.seekg(i, ios::beg);
    rom.read(&rom_h.DEST_CODE, sizeof(rom_h.DEST_CODE));
    switch(rom_h.DEST_CODE)
    {
        case 0x00: cout << "Japanese"; break;
        case 0x01: cout << "Non-Japanese"; break;
        defult: cout << "invalid";
    }
    cout << endl;
}

void OldLicenseCode()       //  014B        
{
    int i=0x014B; char c;
    cout << endl << "*-- Old License Code: " << endl;
    rom.seekg(i, ios::beg);
    rom.read(&c, sizeof(c));
    if(c == 0x33)   cout << "none (new license code)";
    else {
        rom_h.LICENSE_CODE[0] = c;
        cout << setw(2) << setfill('0') << hex << (int) (rom_h.LICENSE_CODE[0] & 0xff) << LicenseTranslatorI(rom_h.LICENSE_CODE[0]);
    }
    cout << endl;
}

void RomVersion()           //  014C        
{
    int i=0x014C;
    cout << endl << "*-- Rom Version: " << endl;
    rom.seekg(i, ios::beg);
    rom.read(&rom_h.VERSION, sizeof(rom_h.VERSION));
    cout << setw(2) << setfill('0') << hex << (int) (rom_h.VERSION & 0xff) << " " << endl;
}

void HeaderChecksum()       //  014D        
{
    int i=0x014D;
    cout << endl << "*-- Header Checksum: " << endl;
    rom.seekg(i, ios::beg);
    rom.read(&rom_h.HEADER_CHECKSUM, sizeof(rom_h.HEADER_CHECKSUM));
    cout << setw(2) << setfill('0') << hex << (int) (rom_h.HEADER_CHECKSUM & 0xff) << " " << endl;
}

void GlobalChecksum()       //  014E - 014F 
{
    int i=0x014E;
    cout << endl << "*-- Global Checksum: " << endl;
    rom_h.GLOBAL_CHECKSUM = (char*) calloc(2, sizeof(char));;
    rom.seekg(i, ios::beg);
    rom.read(&rom_h.GLOBAL_CHECKSUM[0], sizeof(rom_h.GLOBAL_CHECKSUM[0]));
    rom.read(&rom_h.GLOBAL_CHECKSUM[1], sizeof(rom_h.GLOBAL_CHECKSUM[1]));
    cout << setw(2) << setfill('0') << hex << (int) (rom_h.GLOBAL_CHECKSUM[1] & 0xff) << (int) (rom_h.GLOBAL_CHECKSUM[0] & 0xff) << endl;
}