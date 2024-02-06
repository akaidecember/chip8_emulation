#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <time.h>
#include <sys/stat.h>
#include "chip8.h"

using namespace::std;

unsigned char chip8_fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

// Function to initialize the chip8 vir. cpu state
void chip8::initialize(){

    // Seed the rnd. w/ current time on each init.
    srand(time(NULL));

    prog_ctr = '0x200';
    idx_reg = 0;
    opcode = 0;
    stk_ptr = 0;

    // Clearing the stack, V-registers and the keypad
    for(int i = 0; i < STK_SIZE; i++){
        stack[i] = V_reg[i] = keypad[i] = 0;
    }

    // Loading the fontset and clearing the memory
    for(int i = 0; i < MEMORY_SIZE; i++){
        // If memory loc. b/w 0 and 80 then load fontset
        if(i >= 0 && i < 80){
            memory[i] = chip8_fontset[i];
        }
        // Else init. to 0
        else{
            memory[i] = 0;
        }
    }

    // Reset the timers
    delay_timer = 0;
    sound_timer = 0;

    // Setting the drawing flag
    drawingFlag = true;
 
}

// Function to load a given program for chip8 into memory
bool chip8::loadApp(const string& filename) {

    initialize();
    cout << "Loading: " << filename << endl;

    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return false;
    }

    file.seekg(0, ios::end);
    streampos fileSize = file.tellg();
    file.seekg(0, ios::beg);

    // Allocate memory to contain the whole file
    vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize)) {
        cerr << "Error reading file." << endl;
        return false;
    }

    // Check if the ROM fits into Chip8 memory
    if (fileSize > (MEMORY_SIZE - 512)) {
        cerr << "Error: ROM too big for memory." << endl;
        return false;
    }

    // Copy buffer to Chip8 memory
    copy(buffer.begin(), buffer.end(), memory.begin() + 512);

    return true;
    
}
