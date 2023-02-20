/** imrannazar.com/Gameboy-Z80-Opcode-Map **/
/** https://gb-archive.github.io/salvage/decoding_gbz80_opcodes/Decoding%20Gamboy%20Z80%20Opcodes.html **/

#ifndef iostream
    #include <iostream>
#endif
#ifndef sstream
    #include <sstream>
#endif
#ifndef iomanip
    #include <iomanip>
#endif
#ifndef algorithm
    #include <algorithm>
#endif

using namespace std;

string EntryPointTranslator(char* entry_point)
{
    stringstream sh;
    string instr0 = "", instr1 = "";
    switch(entry_point[0])
    {
        case 0x00: instr0 = "NOP"; break;
        case 0xF3: instr0 = "DI"; break;
    } 

    if((entry_point[1] & 0xff) == 0xc3)
    {
        instr1 = "JP ";
        sh << setw(2) << setfill('0') << hex << (int) (entry_point[3] & 0xff);
        sh << setw(2) << setfill('0') << hex << (int) (entry_point[2] & 0xff);
        instr1 += sh.str();
    }

    return instr0 + " " + instr1;
}


/*** ASM DECODER ***/

// flags
bool flag = false;
bool CBprefix = false;      //0
int LD_nn_SP = 0;           //1    (descending counter of nn bytes)    
bool d_n = false;           //2
int nn = 0;                 //3    (same as LD_nn_SP, but doesn't attach SP)
bool LD_0xff00pn_A = false; //4
bool np0xff00 = false;      //5 
int LD_nn_A = 0;            //6    (same as LD_nn_SP, but attaches A)
int nn_par = 0;             //7    (same as nn, but attaches (nn)) 

//z80 tables
string table_r[8] = { "B", "C", "D", "E", "H", "L", "(HL)", "A" };
string table_rp[4] = { "BC", "DE", "HL", "SP" };
string table_rp2[4] = { "BC", "DE", "HL", "AF" };
string table_cc[4] = { "NZ", "Z", "NC", "C" };
string table_alu[8] = { "ADD A,", "ADC A,", "SUB", "SBC A,", "AND", "XOR", "OR", "CP" };
string table_rot[8] = { "RLC", "RRC", "RL", "RR", "SLA", "SRA", "SWAP", "SRL" };

//buffer
char bufferbyte;

void RaiseFlag(int);
string CheckFlags(char);
string HexString(char);

string UnprefixedOP(char);
string PrefixedOP(char);

string DecodeByte(char b)
{
    /*
             7 6  5 4 3  2 1 0      
            [ x ][  y  ][  z  ]
                 [ p ][q]
    */
    if(flag) return CheckFlags(b);
    if(b == 0xCB) { RaiseFlag(0); return ""; }
    return UnprefixedOP(b);
}

string CheckFlags(char b)
{
    if(CBprefix) { RaiseFlag(0); return PrefixedOP(b); }
    if(LD_nn_SP == 2) { LD_nn_SP--; bufferbyte=b; return ""; }
    if(LD_nn_SP == 1) { RaiseFlag(1); return "(0x"+HexString(b)+HexString(bufferbyte)+"),SP"; }  
    if(d_n) { RaiseFlag(2); return "0x"+HexString(b); }
    if(nn == 2) { nn--; bufferbyte=b; return ""; }
    if(nn == 1) { RaiseFlag(3); return "0x"+HexString(b)+HexString(bufferbyte); }
    if(LD_0xff00pn_A) { RaiseFlag(4); return "(0xFF"+HexString(b)+"),A"; }
    if(np0xff00) { RaiseFlag(5); return "(0xFF"+HexString(b)+")"; }
    if(LD_nn_A == 2) { LD_nn_A--; bufferbyte=b; return ""; }
    if(LD_nn_A == 1) { RaiseFlag(1); return "(0x"+HexString(b)+HexString(bufferbyte)+"),A"; }  
    if(nn_par == 2) { nn_par--; bufferbyte=b; return ""; }
    if(nn_par == 1) { RaiseFlag(3); return "(0x"+HexString(b)+HexString(bufferbyte)+")"; }
}

void RaiseFlag(int f)
{
    switch(f)
    {
        case 0: 
            CBprefix = !CBprefix;
            break;
        case 1:
            if(LD_nn_SP == 0) LD_nn_SP = 2; //if i'm raising the flag, it means it expects the next two bytes to be nn
            else LD_nn_SP = 0;
            break;
        case 2:
            d_n = !d_n;
            break;
        case 3:
            if(nn == 0) nn = 2; //if i'm raising the flag, it means it expects the next two bytes to be nn
            else nn = 0;
            break;
        case 4:
            LD_0xff00pn_A = !LD_0xff00pn_A;
            break;
        case 5:
            np0xff00 = !np0xff00;
            break;
        case 6:
            if(LD_nn_A == 0) LD_nn_A = 2; //if i'm raising the flag, it means it expects the next two bytes to be nn
            else LD_nn_A = 0;
            break;
        case 7:
            if(nn_par == 0) nn_par = 2; //if i'm raising the flag, it means it expects the next two bytes to be nn
            else nn_par = 0;
            break;
    }
    flag = !flag;
}

string UnprefixedOP(char b)
{
    int x = (b & 0b11000000) >> 6;
    int y = (b & 0b00111000) >> 3;
    int z = (b & 0b00000111) >> 3;
    int p = (b & 0b00110000) >> 4;
    int q = (b & 0b00001000) >> 3;
    
    if(x==0)
    {
        if(z==0)
        {
            //if(y==0) return "\nNOP";
            if(y==0) return "";
            if(y==1) { RaiseFlag(1); return "\nLD "; }
            if(y==2) return "\nSTOP";
            if(y==3) { RaiseFlag(2); return "\nJR "; }
            RaiseFlag(2);
            return "\nJR "+table_cc[y-4]+","; 
        }
        if(z==1)
        {
            if(q==0) { RaiseFlag(3); return "\nLD "+table_rp[p]+","; }
            if(q==1) return "\nADD HL,"+table_rp[p];
        }
        if(z==2)
        {
            if(q==0)
            {
                if(p==0) return "\nLD (CB),A";
                if(p==1) return "\nLD (DE),A";
                if(p==2) return "\nLD (HL+),A";
                if(p==3) return "\nLD (HL-),A";
            }
                if(p==0) return "\nLD A,(BC)";
                if(p==1) return "\nLD A,(DE)";
                if(p==2) return "\nLD A,(HL+)";
                if(p==3) return "\nLD A,(HL-)";
        }
        if(z==3)
        {
            if(q==0) return "\nINC"+table_rp[p];
            if(q==1) return "\nDEC"+table_rp[p];
        }
        if(z==4) return "\nINC "+table_r[y];
        if(z==5) return "\nDEC "+table_r[y];
        if(z==6) { RaiseFlag(2); return "\nLD "+table_r[y]+","; }
        if(z==7)
        {
            if(y==0) return "\nRLCA";
            if(y==1) return "\nRRCA";
            if(y==2) return "\nRLA";
            if(y==3) return "\nRRA";
            if(y==4) return "\nDAA";
            if(y==5) return "\nCPL";
            if(y==6) return "\nSCF";
            if(y==7) return "\nCCF";
        }
    }
    if(x==1)
    {
        if(z==6)
        {
            if(y==6) return "\nHALT";
        }else{
            return "\nLD "+table_r[y]+","+table_r[z];
        }
    }
    if(x==2)
    {
        return "\n"+table_alu[y]+" "+table_r[z];
    }
    if(x==3)
    {
        if(z==0)
        {
            if(y<4) return "\nRET "+table_cc[y];
            if(y==4) { RaiseFlag(4); return "\nLD "; };
            if(y==5) { RaiseFlag(2); return "\nADD SP,"; }
            if(y==6) { RaiseFlag(5); return "\nLD A,"; }
            if(y==7) { RaiseFlag(2); return "\nLD HL,SP+"; }
        }
        if(z==1)
        {
            if(q==0) return "\nPOP "+table_rp2[p];
            if(q==1)
            {
                if(p==0) return "\nRET";
                if(p==1) return "\nRETI";
                if(p==2) return "\nJP HL";
                if(p==3) return "\nLD SP,HL";
            }
        }
        if(z==2)
        {
            if(y<4) { RaiseFlag(3); return "\nJP "+table_cc[y]+","; }
            if(y==4) return "\nLD (0xFF00+C), A";
            if(y==5) { RaiseFlag(6); return "\nLD "; }
            if(y==6) { return "\nLD A,(0xFF00+C)"; }
            if(y==7) { RaiseFlag(7); return "\nLD A,"; }
        }
        if(z==3)
        {
            if(y==0) { RaiseFlag(3); return "\nJP "; }
            if(y==6) { return "\nDI "; }
            if(y==7) { return "\nEI "; }
        }
        if(z==4)
        {
            if(y<4) { RaiseFlag(3); return "\nCALL "+table_cc[y]+","; }
        }
        if(z==5)
        {
            if(q==0) return "\nPUSH "+table_rp2[p];
            if(q==1)
            {
                if(p==0) { RaiseFlag(3); return "CALL "; }
            }
        }
        if(z==6)
        {
            RaiseFlag(2); return "\n"+table_alu[y]+" ";
        }
        if(z==7)
        {
            return "RST "+to_string(y*8);
        }
    }

    return "\n(unrecognized) "+HexString(b);
}

string PrefixedOP(char b)
{
    int x = (b & 0b11000000) >> 6;
    int y = (b & 0b00111000) >> 3;
    int z = (b & 0b00000111) >> 3;

    if(x==0) return "\n"+table_rot[y]+" "+table_r[z];
    if(x==1) return "\nBIT "+to_string(y)+","+table_r[z];
    if(x==2) return "\nRES "+to_string(y)+","+table_r[z];
    if(x==3) return "\nSET "+to_string(y)+","+table_r[z];

    return "\n(unrecognized CB prefixed) "+HexString(b);
}

string HexString(char b)
{
    stringstream sh;
    string s;
    sh << setw(2) << setfill('0') << hex << (int) (b & 0xff);
    s=sh.str();
    transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}