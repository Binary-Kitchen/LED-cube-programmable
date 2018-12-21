#include "cube.h"
#include <string.h>
#include <util/delay.h>

int main(){
    cube_4 cube{};
    cube.start();
    cube.set_all(0x1F);

    while(true){
        int b = 0;
        int dir = 1;
        
        while(true){
	    _delay_us(500);
            if(b == 0){
                dir = 1;
            } else if(b == 0x1F)
            {
                dir = -1;
            }
            b+=dir;
            for(int i = 0; i<4; i++){
               memset((void*)(cube.get_buffer()+i*16),b,16);
            }
    }
    }
}
