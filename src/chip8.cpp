#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <time.h>
#include <sys/stat.h>
#include <functional>
#include <unordered_map>
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

using OpcodeHandler = function<void(uint16_t)>;
unordered_map<uint16_t, OpcodeHandler> opcodeHandlers;

// Function to initialize the chip8 vir. cpu state
void chip8::initialize(){

    // Seed the rnd. w/ current time on each init.
    srand(time(NULL));

    prog_ctr = '0x200';
    idx_reg = 0;
    opcode = 0;
    stk_ptr = 0;

    // Clearing the stack, V-registers and the keypad
    // Used STK_SIZE = 16, but can use others. They're same
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
bool chip8::loadApp(const string& filename){

    initialize();
    cout << "Loading: " << filename << endl;

    ifstream file(filename, ios::binary);
    if (!file.is_open()){
        cerr << "Error opening file." << endl;
        return false;
    }

    // Calculating file size in bytes using seek method
    file.seekg(0, ios::end);
    streampos fileSize = file.tellg();
    file.seekg(0, ios::beg);

    // Allocate memory to contain the whole file
    vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize)){
        cerr << "Error reading file." << endl;
        return false;
    }

    // Check if the ROM fits into Chip8 memory
    if (fileSize > (MEMORY_SIZE - 512)){
        cerr << "Error: ROM too big for memory." << endl;
        return false;
    }

    // Copy buffer to Chip8 memory
    copy(buffer.begin(), buffer.end(), memory.begin() + 512);

    return true;

}

void chip8::initOpcodeHandlers(){

    // 0x0000
    opcodeHandlers[0x0000] = [this](uint16_t opcode){
        switch(opcode & 0x000F) {
            case 0x0000: 
                for(int i = 0; i < GFX_BUFFER_SIZE; ++i){
				    gfx_buffer[i] = 0x0;
                }
				drawingFlag = true;
				prog_ctr += 2;
                break;
            case 0x000E: 
                stk_ptr--;
                prog_ctr = stack[stk_ptr];
                prog_ctr += 2;
                break;
            default:
                cout << "Unknown opcode [0x0000]: 0x" << hex << opcode << endl;
                break;
        }
    };

    // 0x1000
    opcodeHandlers[0x1000] = [this](uint16_t opcode){
        prog_ctr = opcode & 0x0FFF;
    };

    // 0x2000
    opcodeHandlers[0x2000] = [this](uint16_t opcode){
        stack[stk_ptr] = prog_ctr;
        ++stk_ptr;
        prog_ctr = opcode & 0x0FFF;
    };

    // 0x3000
    opcodeHandlers[0x3000] = [this](uint16_t opcode){
        if(V_reg[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
            prog_ctr += 4;
        }
        else{
            prog_ctr += 2;
        }
    };

    // 0x4000
    opcodeHandlers[0x4000] = [this](uint16_t opcode){
        if(V_reg[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)){
            prog_ctr += 4;
        }
        else{
            prog_ctr += 2;
        }
    };

    // 0x5000
    opcodeHandlers[0x5000] = [this](uint16_t opcode){
        if(V_reg[(opcode & 0x0F00) >> 8] == V_reg[(opcode & 0x00F0) >> 4]){
            prog_ctr += 4;
        }
        else{
            prog_ctr += 2;
        }
    };

    // 0x6000
    opcodeHandlers[0x6000] = [this](uint16_t opcode){
        V_reg[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
        prog_ctr += 2;
    };

    // 0x7000
    opcodeHandlers[0x7000] = [this](uint16_t opcode){
        V_reg[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		prog_ctr += 2;
    };

    // 0x8000
    opcodeHandlers[0x8000] = [this](uint16_t opcode){
        switch(opcode & 0x000F) {
            case 0x0000: 
                V_reg[(opcode & 0x0F00) >> 8] = V_reg[(opcode & 0x00F0) >> 4];
				prog_ctr += 2;
                break;
            case 0x0001:
                V_reg[(opcode & 0x0F00) >> 8] |= V_reg[(opcode & 0x00F0) >> 4];
                prog_ctr += 2;
                break;
            case 0x0002:
                V_reg[(opcode & 0x0F00) >> 8] &= V_reg[(opcode & 0x00F0) >> 4];
                prog_ctr += 2;
                break;
            case 0x0003:
                V_reg[(opcode & 0x0F00) >> 8] ^= V_reg[(opcode & 0x00F0) >> 4];
                prog_ctr += 2;
                break;
            case 0x0004:
                if(V_reg[(opcode & 0x00F0) >> 4] > (0xFF - V_reg[(opcode & 0x0F00) >> 8])){
                    V_reg[0xF] = 1;
                }
                else{
                    V_reg[0xF] = 0;
                }
                V_reg[(opcode & 0x0F00) >> 8] += V_reg[(opcode & 0x00F0) >> 4];
                prog_ctr += 2;
                break;
            case 0x0005:
                if(V_reg[(opcode & 0x00F0) >> 4] > V_reg[(opcode & 0x00F0) >> 8]){
                    V_reg[0xF] = 0;
                }
                else{
                    V_reg[0xF] = 1;
                }
                V_reg[(opcode & 0x0F00) >> 8] -= V_reg[(opcode & 0x00F0) >> 4];
                prog_ctr += 2;
                break;
            case 0x0006:
                V_reg[0xF] = V_reg[(opcode & 0x0F00) >> 8] & 0x1;
                V_reg[(opcode & 0x0F00) >> 8] >>= 1;
                prog_ctr += 2;
                break;
            case 0x0007:
                if(V_reg[(opcode & 0x0F00) >> 8] > V_reg[(opcode & 0x00F0) >> 4]){
                    V_reg[0xF] = 0;
                }
                else{
                    V_reg[0xF] = 1;
                }
                V_reg[(opcode & 0x0F00) >> 8] = V_reg[(opcode & 0x00F0) >> 4] - V_reg[(opcode & 0x0F00) >> 8];				
                prog_ctr += 2;
                break;
            case 0x000E: 
				V_reg[0xF] = V_reg[(opcode & 0x0F00) >> 8] >> 7;
				V_reg[(opcode & 0x0F00) >> 8] <<= 1;
				prog_ctr += 2;
                break;
            default:
                cout << "Unknown opcode [0x8000]: 0x" << hex << opcode << endl;
                break;
        }
    };

    // 0x9000
    opcodeHandlers[0x9000] = [this](uint16_t opcode){
        if(V_reg[(opcode & 0x0F00) >> 8] != V_reg[(opcode & 0x00F0) >> 4]){
            prog_ctr += 4;
        }
        else{
            prog_ctr += 2;
        }
    };

    // 0xA000
    opcodeHandlers[0xA000] = [this](uint16_t opcode){
        idx_reg = opcode & 0x0FFF;
        prog_ctr += 2;
    };

    // 0xB000
    opcodeHandlers[0xB000] = [this](uint16_t opcode){
        prog_ctr = (opcode & 0x0FFF) + V_reg[0];
    };

    // 0xC000
    opcodeHandlers[0xC000] = [this](uint16_t opcode){
		V_reg[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
		prog_ctr += 2;
    };

    // 0xD000
    opcodeHandlers[0xD000] = [this](uint16_t opcode){
        const auto x_ax = V_reg[(opcode & 0x0F00) >> 8];
        const auto y_ax = V_reg[(opcode & 0x00F0) >> 4];
        const auto h = opcode & 0x000F;
        uint16_t pixel;

        V_reg[0xF] = 0;
        for(int i = 0; i < h; i++){
            pixel = memory[idx_reg + i];
            for(int j = 0; j < 8; j++){
                if(((0x80 >> j) & pixel) != 0){
                    if(gfx_buffer[(x_ax + j + ((y_ax + i) * 64))] == 1){
                        V_reg[0xF] = 1;
                    }
                    gfx_buffer[j + x_ax + ((y_ax + i) * 64)] ^= 1;
                }
            }
        }
        drawingFlag = true;
        prog_ctr += 2;
    };

    // 0xE000
    opcodeHandlers[0xE000] = [this](uint16_t opcode){
        switch(opcode & 0x00FF) {
            case 0x009E: 
                if (keypad[V_reg[(opcode & 0x0F00) >> 8]] != 0){
                    prog_ctr += 4;
                }
                else{
                    prog_ctr += 2;
                }
                break;
            case 0x00A1: 
                if (keypad[V_reg[(opcode & 0x0F00) >> 8]] == 0){
                    prog_ctr += 4;
                }
                else{
                    prog_ctr += 2;
                }
                break;
            default:
                cout << "Unknown opcode [0xE000]: 0x" << hex << opcode << endl;
                break;
        }
    };

    // 0xF000
    opcodeHandlers[0xF000] = [this](uint16_t opcode){
        switch(opcode & 0x00FF) {
            case 0x0007: 
                V_reg[(opcode & 0x0F00) >> 8] = delay_timer;
                prog_ctr += 2;
                break;
            case 0x000A: 
            {
                bool kp = false;
                for(int i = 0; i < 16; ++i){
                    if(keypad[i] != 0){
                        V_reg[(opcode & 0x0F00) >> 8] = i;
                        kp = true;
                        break;
                    }
                }
                if(!kp) return;

                prog_ctr += 2;
                break;
            }
            case 0x0015: 
                delay_timer = V_reg[(opcode & 0x0F00) >> 8];
                prog_ctr += 2;
                break;
            case 0x0018: 
                sound_timer = V_reg[(opcode & 0x0F00) >> 8];
                prog_ctr += 2;
                break;
            case 0x001E: 
                if(idx_reg + V_reg[(opcode & 0x0F00) >> 8] > 0xFFF){
                    V_reg[0xF] = 1;
                }
                else{
                    V_reg[0xF] = 0;
                }
                idx_reg += V_reg[(opcode & 0x0F00) >> 8];
                prog_ctr += 2;
                break;
            case 0x0029: 
                idx_reg = V_reg[(opcode & 0x0F00) >> 8] * 0x5;
                prog_ctr += 2;
                break;
            case 0x0033:
                memory[idx_reg] = V_reg[(opcode & 0x0F00) >> 8] / 100;
                memory[idx_reg + 1] = (V_reg[(opcode & 0x0F00) >> 8] / 10) % 10;
                memory[idx_reg + 2] = (V_reg[(opcode & 0x0F00) >> 8] % 100) % 10;
                prog_ctr += 2;
                break;
            case 0x0055: 
                for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i){
                    memory[idx_reg + i] = V_reg[i];
                }
                idx_reg += ((opcode & 0x0F00) >> 8) + 1;
                prog_ctr += 2;
                break;
            case 0x0065: 
                for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i){
                    V_reg[i] = memory[idx_reg + i];
                }
                idx_reg += ((opcode & 0x0F00) >> 8) + 1;
                prog_ctr += 2;
                break;
            default:
                cout << "Unknown opcode [0xF000]: 0x" << hex << opcode << endl;
                break;
        }
    };

}

// Function to emulate single cycle
void chip8::emulateCycle(){
    opcode = memory[prog_ctr] << 8 | memory[prog_ctr + 1];
    processOpcode();
    updateTimers();
}

void chip8::processOpcode(){

    uint16_t opcodePrefix = opcode & 0xF000;

    // Find corresponding handler in the map
    auto it = opcodeHandlers.find(opcodePrefix);

    if (it != opcodeHandlers.end()){
        it->second(opcode);
    } else{
        cout << "Unknown Opcode: " << opcode << endl;
    }
}

void chip8::updateTimers(){

    if (delay_timer > 0){
        delay_timer--;
    }

    if (sound_timer > 0){
        if (sound_timer == 1){
            cout << "BEEP!" << endl;
        }
        sound_timer--;
    }

}
