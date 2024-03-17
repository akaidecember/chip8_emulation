#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace::std;

class Chip8{

    protected:
        static const size_t MEMORY_SIZE = 4096;
        static const size_t REG_CNT = 16;
        static const size_t STK_SIZE = 16;
        static const size_t GFX_BUFFER_SIZE = 64 * 32;
    
    private:
        uint16_t opcode;              // Current OP code
        uint16_t idxReg;             // Index register
        uint16_t progCtr;            // Program counter

        array<uint8_t, MEMORY_SIZE> memory;
        array<uint8_t, REG_CNT> V_reg;

        uint8_t delayTimer;
        uint8_t soundTimer;

        array<uint16_t, STK_SIZE> stack;      
        uint16_t stkPtr;             

        void initialize();

    public:    
        array<uint8_t, 16> keypad;
        array<uint8_t, GFX_BUFFER_SIZE> gfxBuffer;        
        bool drawingFlag;

        void emulateCycle();
        void processOpcode();
        void initOpcodeHandlers();
        void updateTimers();
		bool loadApp(const string &);
};
