class chip8{

    public:
        unsigned char keypad[16];              // Keypad
        unsigned char gfx_buffer[2048];     // Graphics Buffer (64 * 32 = 2048)

        bool drawingFlag;
        void emulate();
		bool loadApp(const char * filename);
    
    private:

        unsigned short opcode;              // Current OP code
        unsigned short idx_reg;             // Index register
        unsigned short prog_ctr;            // Program counter

        unsigned char memory[4096];         // Memory 4KB
        unsigned char V_reg[16];            // V - registers

        unsigned char delay_timer;
        unsigned char sound_timer;

        unsigned short stack[16];           // Stack alloc.
        unsigned short stk_ptr;             // Stack pointer

        void initialize();

};

/*TOdo: Change static array allocation to dynamic using malloc and callc. Use free when necessary and define macros for sizes
Defining macros somewhere will enable to be freed and malloced in some place.*/