
// #define chip8
#include <stdint.h>
#include <stdlib.h>
#include <SDL2/SDL.h> 
#define MEMSIZE 4096 //size of memory


//0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
// 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
// 0x200-0xFFF - Program ROM and work RAM
static const uint8_t FONTSET[80] = { 
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

typedef struct{
uint16_t instruction;
uint8_t memory[MEMSIZE];
uint8_t registers[16];
uint16_t Index_register;
uint16_t pc;
uint16_t stack[16];
uint8_t sp;
uint8_t delay_timer; //used for timing
uint8_t sound_timer; //timer for sound
uint8_t keypad[16];
uint8_t fontset[80];
uint32_t graphics[64][32];
int draw_fl;
int sound_fl;
int quit;
int pause; 
}cpu_t;

typedef struct{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *screen;
    SDL_Event event;
    SDL_Rect rect;

}sdl_t;
int load_rom(char* filename, uint8_t *memory, char* dump_file);
void i_fetch(cpu_t *cpu);
int decode(cpu_t *cpu);
void draw(sdl_t *sdl, cpu_t *cpu);
int init_SDL(sdl_t*);
void input_handler(sdl_t *sdl, cpu_t *cpu);
void init_cpu(cpu_t *cpu);
void timers(cpu_t *cpu);

