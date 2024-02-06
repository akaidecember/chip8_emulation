#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace::std;

class chip8{

    protected:
        static constexpr size_t MEMORY_SIZE = 4096;
        static constexpr size_t REG_CNT = 16;
        static constexpr size_t STK_SIZE = 16;
        static constexpr size_t GFX_BUFFER_SIZE = 64 * 32;
    
    private:
        uint16_t opcode;              // Current OP code
        uint16_t idx_reg;             // Index register
        uint16_t prog_ctr;            // Program counter

        array<uint8_t, MEMORY_SIZE> memory;
        array<uint8_t, REG_CNT> V_reg;

        uint8_t delay_timer;
        uint8_t sound_timer;

        array<uint16_t, STK_SIZE> stack;      
        uint16_t stk_ptr;             

        void initialize();

    public:             
        array<uint8_t, 16> keypad;
        array<uint8_t, GFX_BUFFER_SIZE> gfx_buffer;        
        bool drawingFlag;

        void emulateCycle();
        void processOpcode();
        void initOpcodeHandlers();
        void updateTimers();
		bool loadApp(const string &filename);
};
