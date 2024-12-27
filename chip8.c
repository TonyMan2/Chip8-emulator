//#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <SDL2/SDL.h> 
#include "chip8.h"
#include <time.h>
#include <sys/stat.h>

// typedef struct{
//     SDL_Window *window;
//     SDL_Renderer *renderer;
//     SDL_Texture *screen;
//     SDL_Event *event;
//     SDL_Rect rect;

// } sdl_t;


int main(int argc, char**argv){
    if(argc < 2){
        printf("forgot rom ");
        return EXIT_FAILURE;
    }
    
    sdl_t sdl = {0};
    cpu_t cpu = {0};
    if(!init_SDL(&sdl)){
        return EXIT_FAILURE; 
    }
    init_cpu(&cpu);

    if (!load_rom(argv[1], cpu.memory, argv[2])){
        return EXIT_FAILURE; 
    }

    while(!cpu.quit){

        input_handler(&sdl, &cpu);    
        i_fetch(&cpu);
        decode(&cpu);
        if(cpu.draw_fl){
            draw(&sdl,&cpu);
            cpu.draw_fl = 0;
        }
        
       SDL_Delay(5);
       timers(&cpu);
    }
    
    
    SDL_DestroyRenderer(sdl.renderer); 
    SDL_DestroyWindow(sdl.window);
    SDL_Quit();
 
    // cpu_t cpu; 
    // init_cpu(&cpu)
    return EXIT_SUCCESS; 
}
void input_handler(sdl_t *sdl, cpu_t *cpu){
    while(SDL_PollEvent(&sdl->event)){
        switch ((sdl->event.type))
        {
        case SDL_QUIT:
            cpu->quit = 1;
            return;
        
        case SDL_KEYDOWN:
            switch (sdl->event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                cpu->quit = 1;
                return;
            case SDLK_SPACE:
                cpu->pause = (cpu->pause)?0:1;
                return; 
            case SDLK_x:cpu->keypad[0] = 1;break;
			case SDLK_1:cpu->keypad[1] = 1;break;
			case SDLK_2:cpu->keypad[2] = 1;break;
			case SDLK_3:cpu->keypad[3] = 1;break;
			case SDLK_q:cpu->keypad[4] = 1;break;
			case SDLK_w:cpu->keypad[5] = 1;break;
			case SDLK_e:cpu->keypad[6] = 1;break;
			case SDLK_a:cpu->keypad[7] = 1;break;
			case SDLK_s:cpu->keypad[8] = 1;break;
			case SDLK_d:cpu->keypad[9] = 1;break;
			case SDLK_z:cpu->keypad[0xA] = 1;break;
			case SDLK_c:cpu->keypad[0xB] = 1;break;
			case SDLK_4:cpu->keypad[0xC] = 1;break;
			case SDLK_r:cpu->keypad[0xD] = 1;break;
			case SDLK_f:cpu->keypad[0xE] = 1;break;
			case SDLK_v:cpu->keypad[0xF] = 1;break; 
            }
            break;
        case SDL_KEYUP:
             switch (sdl->event.key.keysym.sym)
            {
            case SDLK_x:cpu->keypad[0] = 0;break;
			case SDLK_1:cpu->keypad[1] = 0;break;
			case SDLK_2:cpu->keypad[2] = 0;break;
			case SDLK_3:cpu->keypad[3] = 0;break;
			case SDLK_q:cpu->keypad[4] = 0;break;
			case SDLK_w:cpu->keypad[5] = 0;break;
			case SDLK_e:cpu->keypad[6] = 0;break;
			case SDLK_a:cpu->keypad[7] = 0;break;
			case SDLK_s:cpu->keypad[8] = 0;break;
			case SDLK_d:cpu->keypad[9] = 0;break;
			case SDLK_z:cpu->keypad[0xA] = 0;break;
			case SDLK_c:cpu->keypad[0xB] = 0;break;
			case SDLK_4:cpu->keypad[0xC] = 0;break;
			case SDLK_r:cpu->keypad[0xD] = 0;break;
			case SDLK_f:cpu->keypad[0xE] = 0;break;
			case SDLK_v:cpu->keypad[0xF] = 0;break; 
            }
        break;
        }
        break;
    }
}
int init_SDL(sdl_t *sdl){

    if(SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("SDL error %s\n", SDL_GetError());
        return 0;
    }
    sdl->window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64*8, 32*8, 0);
  
     if(!sdl->window){
        SDL_Log("SDL could not open window %s\n", SDL_GetError());
         return 0;
     }
    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
    if(!sdl->renderer){
        SDL_Log("SDL could not open render %s\n", SDL_GetError());
        return 0;
    }
    
    SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 255);

    SDL_RenderClear(sdl->renderer);
     

     SDL_RenderPresent(sdl->renderer);
  
    //SDL_Delay(5000);

    return 1;
}

int load_rom(char* filename, uint8_t *memory, char* dump){

    FILE *fp = fopen(filename,"rb");
    if(!fp){
        return 0;
    }

    fseek(fp,0,SEEK_END);
    int size = ftell(fp);
    rewind(fp);
    //uint8_t *rom_buffer = (uint8_t*) malloc(sizeof(uint8_t) * size);

    fread(memory + 0x200,size,1,fp);
    // for(int i = 0; i < size/2; i++) {
    //             if(i % 2)
    //                 memory[i + 0x200] = rom_buffer[i-1];
    //             else    
    //                 memory[i + 0x200] = rom_buffer[i+1];
    // }
    //uint8_t temp;
    // rewind(fp);
    // for(int i = 0; i < size; i++){
       
    //     memory[i+0x200] = rom_buffer[i];
    // }

    fclose(fp);
   
     //uint8_t *start = memory+0x200; 
     //used dump memory after rom was loaded
     fp = fopen(dump, "w");
    for( int i = 0; i < size; i+=2){
        fprintf(fp,"%x%x\n", memory[0x200+i],  memory[0x200+i+1]);
    }
     fclose(fp);
     //free(rom_buffer);
    return 1;
}

void init_cpu(cpu_t *cpu){
    cpu->pc = 0x200;
    cpu->sp = 0;
    cpu->delay_timer = 0;
    cpu->sound_timer = 0;
    memset(cpu->stack,0,256);
    memset(cpu->memory,0,MEMSIZE);
    memset(cpu->registers,0,sizeof(cpu->registers));
    memset(cpu->graphics,0,sizeof(cpu->graphics));
    memcpy(cpu->memory,FONTSET, sizeof(FONTSET));
    cpu->instruction = 0;
    cpu->draw_fl = 0;
    cpu->sound_fl = 0;
    cpu->sound_timer = 0;
    cpu->quit = 0;
    cpu->pause = 0;

}
void draw(sdl_t *sdl,cpu_t *cpu){
    SDL_SetRenderDrawColor(sdl->renderer, 0,0,0,255);

    SDL_RenderClear(sdl->renderer);

   SDL_SetRenderDrawColor(sdl->renderer, 255,255,255,255);

    for(int y = 0; y<32; y++){
        for(int x = 0; x < 64; x++){
            if(cpu->graphics[x][y]){
                sdl->rect.x = x *8;
                sdl->rect.y = y *8;
                sdl->rect.w = 8;
                sdl->rect.h = 8;
                SDL_RenderFillRect(sdl->renderer, &sdl->rect);
            }
        }
    }
    SDL_RenderPresent(sdl->renderer);
    return; 
}


void i_fetch(cpu_t *cpu){
    cpu->instruction = cpu->memory[cpu->pc] << 8;
    cpu->instruction |= cpu->memory[cpu->pc+1]; 
    printf("instruction: %x\n", cpu->instruction);
    return; 
}

int decode(cpu_t *cpu){
    
    //return 0 on failure
    uint16_t x = (cpu->instruction & 0x0F00) >> 8; //register 
    uint16_t y = (cpu->instruction & 0x00F0) >> 4; //register
    switch (cpu->instruction & 0xF000){
        case 0x0000:
            switch(cpu->instruction & 0x00FF){
                case 0x00E0:
                    //clear disp
                    memset(cpu->graphics,0,sizeof(cpu->graphics));
                    cpu->draw_fl = 1;
                    cpu->pc+=2;
                    break;
                case 0x00EE:
                    cpu->pc = cpu->stack[cpu->sp];
                    cpu->sp--;
                    cpu->pc += 2;
                break;
                
                default:
                    cpu->pc+=2;
                    return 0; 
        }
        break;  

        case 0x1000: //jump 
            cpu->pc = cpu->instruction & 0x0FFF;
            printf("1xxx\n");
            break;
        case 0x2000: //function call
            cpu->sp++;
            cpu->stack[cpu->sp] = cpu->pc; //save return pc
            cpu->pc = cpu->instruction & 0x0FFF;
             printf("2xxx\n");
            break;
        case 0x3000:
            if(cpu->registers[x] == (cpu->instruction & 0x00FF)){
                cpu->pc += 2;
            }
            cpu->pc+=2;
             printf("3xxx\n");
            break;
        case 0x4000:
            if(cpu->registers[x] != (cpu->instruction & 0x00FF)){
                cpu->pc+=2;
            }
            cpu->pc+=2;
             printf("4xxx\n");
            break;
        case 0x5000:
            if(cpu->registers[x] == cpu->registers[y]){
                cpu->pc+=2;
            }
            cpu->pc+=2;
            printf("5xxx\n");
            break;
        case 0x6000:
            cpu->registers[x] = (cpu->instruction & 0x00FF);
            cpu->pc+=2;
             printf("6xxx\n");
            break;
        case 0x7000:
            cpu->registers[x] += (cpu->instruction & 0x00FF);
            cpu->pc+=2;
             printf("7xxx\n");
            break;

        case 0x8000:
            switch(cpu->instruction & 0x000F){
                case 0x0000: 
                    cpu->registers[x] = cpu->registers[y];
                    cpu->pc+=2;
                     printf("8xx0\n");
                    break;
                case 0x0001:
                    cpu->registers[x] |= cpu->registers[y];
                    cpu->pc+=2;
                       printf("8xx1\n");
                    break;
                case 0x0002:
                    cpu->registers[x] &= cpu->registers[y];
                    cpu->pc+=2;
                       printf("8xx2\n");
                    break;
                case 0x0003:
                    cpu->registers[x] ^= cpu->registers[y];
                    cpu->pc+=2;
                       printf("8xx3\n");
                    break;
                case 0x0004:
                    cpu->registers[x] += cpu->registers[y];
                    //to do: check overflow
                    cpu->pc+=2;
                       printf("8xx4\n");
                    break;
                case 0x0005:
                    cpu->registers[x] -= cpu->registers[y];
                      //to do: check overflow
                    cpu->pc+=2;
                       printf("8xx5\n");
                    break; 
                 case 0x0006:
                    cpu->registers[0xf] = cpu->registers[x] & 0x1;
                    cpu->registers[x] >>= 1;
                    
                    cpu->pc+=2;
                       printf("8xx6\n");
                    break; 
                case 0x0007:
                    cpu->registers[x] = cpu->registers[y] - cpu->registers[x];
                      //to do: check overflow
                    cpu->registers[0xf] = (cpu->registers[y] >= cpu->registers[x]);
                    cpu->pc+=2;
                       printf("8xx7\n");
                    break; 
                case 0x000E:
                    cpu->registers[0xF] = cpu->registers[x]>>7;
                    cpu->registers[x] <<= 1;
                    cpu->pc+=2;
                       printf("8xxE\n");
                    break;   
            }
            break;
        case 0x9000:
             if(cpu->registers[x] != cpu->registers[y]){
                cpu->pc+=2;
            }
                cpu->pc+=2;
               printf("9xxx\n");
            break;
        case 0xA000:
            cpu->Index_register = cpu->instruction & 0x0FFF;
            cpu->pc+=2;
               printf("Axxx\n");
            break;
        case  0xB000:
            cpu->pc = cpu->registers[0] + (cpu->instruction & 0x0FFF);
                           printf("Bxxx\n");

            break;
        case 0xC000:
            cpu->registers[x] = rand() & (cpu->instruction & 0x00FF);
            cpu->pc +=2;
                           printf("Cxxx\n");

            break;
        case 0xD000:
                       printf("Dxxx\n");

            //draw
            cpu->draw_fl = 1;
            uint16_t height = cpu->instruction & 0x000f;
            uint8_t pixel = 0;
            for(int rows = 0; rows< height; rows++){
                pixel = cpu->memory[cpu->Index_register + rows];
                for(int cols = 0; cols < 8; cols++){
                    if ((pixel & (0x80 >> cols)) != 0) {
                       
                        if (cpu->graphics[cpu->registers[x] + cols] [cpu->registers[y] + rows] == 1) {
                            cpu->registers[0xF] = 1;
                        }

                        cpu->graphics[cpu->registers[x] + cols][cpu->registers[y] + rows] ^= 1;
                }
            }
            }
            cpu->pc+=2;
            break;
        case 0xE000:
            switch (cpu->instruction & 0x00ff)
            {
            case 0x009E:
                 printf("Ex9E\n");

                if(cpu->keypad[cpu->registers[x]]){
                    cpu->pc+=2;
                }
                cpu->pc+=2;
                break;
            
            case 0x00A1:
                if(!cpu->keypad[cpu->registers[x]]){
                    cpu->pc+=2;
                }
                printf("ExA1\n");
                cpu->pc+=2;
                break;
            }
            break;
        case 0xF000:
            switch(cpu->instruction & 0x00ff){
                case 0x0007:
                    cpu->registers[x] = cpu->delay_timer;
                    cpu->pc+=2;
                      printf("Fx07\n");
                    break;
                  
                case 0x000A:
                    printf("Fx07\n");
                    for(int i = 0; i <16; i++){
                        if(cpu->keypad[i]){
                            cpu->registers[x] = i;
                            break;
                        } 
                    }
                    printf("Fx0A\n");
                    cpu->pc+=2;
                    break;
                case 0x0015:
                     printf("Fx15\n");
                    cpu->delay_timer = cpu->registers[x];
                    cpu->pc+=2;
                    break;
                case 0x0018:
                   printf("Fx18\n");
                    cpu->sound_timer = cpu->registers[x];
                    cpu->pc+=2;
                    break;
                case 0x001E:
                       printf("Fx1E\n");
                    cpu->Index_register += cpu->registers[x];
                    cpu->pc+=2;
                    break;
                case 0x0029:
                   printf("Fx29\n");
                    cpu->Index_register = cpu->registers[x] * 5;
                    cpu->pc+=2;
                    break;
                case 0x0033:
                   printf("Fx33\n");
                    cpu->memory[cpu->Index_register] = (cpu->registers[x] % 1000) / 100;
                    cpu->memory[cpu->Index_register + 1] = (cpu->registers[x] % 100) / 10;
                    cpu->memory[cpu->Index_register + 2] = (cpu->registers[x] % 10);
                    cpu->pc+=2;
                    break;
                
                case 0x0055:
                   printf("Fx55\n");
                    for(int j = 0; j <= x; j++){
                        cpu->memory[cpu->Index_register + j] = cpu->registers[j];
                    }
                    cpu->pc+=2;
                    break;
                
                case 0x0065:
                       printf("Fx6\n");
                    for(int j = 0; j <= x; j++){
                        cpu->registers[j]= cpu->memory[cpu->Index_register + j];
                    }
                    cpu->pc+=2;
                    break;
            }
            break;
    }
        return 1;
    }



void timers(cpu_t *cpu){
    if(cpu->delay_timer > 0){
        cpu->delay_timer --;
    }

    if(cpu->sound_timer>0){
        cpu->sound_fl = 1;
        cpu->sound_timer --;
    }
    return;
}


