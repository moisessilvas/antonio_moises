#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_opengl.h>

#define N_FILTERS 64
#define STRIDE 2
#define WIDTH 224
#define HEIGHT 224


static float filters[N_FILTERS*7*7];

void init_weights_7x7()
{
    for(int i = 0; i < N_FILTERS*7*7; i++)
    {
        // filters[i] = rand()/RAND_MAX;
        filters[i] = 1;
    }
}

void conv1_layer(float data[], float output[])
{
    int filter, id = 0;
    float conv;
    unsigned char window[7*7];
    
    for(filter = 0; filter < N_FILTERS; filter++)
    {
        for(int i = 0; i < HEIGHT; i = i + STRIDE)
        {
            for(int j = 0; j < WIDTH; j = j + STRIDE)
            {
                window[0] = data[(i+0)*WIDTH + j+0];
                window[1] = data[(i+0)*WIDTH + j+1];
                window[2] = data[(i+0)*WIDTH + j+2];
                window[3] = data[(i+0)*WIDTH + j+3];
                window[4] = data[(i+0)*WIDTH + j+4];
                window[5] = data[(i+0)*WIDTH + j+5];
                window[6] = data[(i+0)*WIDTH + j+6];
                
                window[7] = data[(i+1)*WIDTH + j+0];
                window[8] = data[(i+1)*WIDTH + j+1];
                window[9] = data[(i+1)*WIDTH + j+2];
                window[10] = data[(i+1)*WIDTH + j+3];
                window[11] = data[(i+1)*WIDTH + j+4];
                window[12] = data[(i+1)*WIDTH + j+5];
                window[13] = data[(i+1)*WIDTH + j+6];
                
                window[14] = data[(i+2)*WIDTH + j+0];
                window[15] = data[(i+2)*WIDTH + j+1];
                window[16] = data[(i+2)*WIDTH + j+2];
                window[17] = data[(i+2)*WIDTH + j+3];
                window[18] = data[(i+2)*WIDTH + j+4];
                window[19] = data[(i+2)*WIDTH + j+5];
                window[20] = data[(i+2)*WIDTH + j+6];

                window[21] = data[(i+3)*WIDTH + j+0];
                window[22] = data[(i+3)*WIDTH + j+1];
                window[23] = data[(i+3)*WIDTH + j+2];
                window[24] = data[(i+3)*WIDTH + j+3];
                window[25] = data[(i+3)*WIDTH + j+4];
                window[26] = data[(i+3)*WIDTH + j+5];
                window[27] = data[(i+3)*WIDTH + j+6];

                window[28] = data[(i+4)*WIDTH + j+0];
                window[29] = data[(i+4)*WIDTH + j+1];
                window[30] = data[(i+4)*WIDTH + j+2];
                window[31] = data[(i+4)*WIDTH + j+3];
                window[32] = data[(i+4)*WIDTH + j+4];
                window[33] = data[(i+4)*WIDTH + j+5];
                window[34] = data[(i+4)*WIDTH + j+6];

                window[35] = data[(i+5)*WIDTH + j+0];
                window[36] = data[(i+5)*WIDTH + j+1];
                window[37] = data[(i+5)*WIDTH + j+2];
                window[38] = data[(i+5)*WIDTH + j+3];
                window[39] = data[(i+5)*WIDTH + j+4];
                window[40] = data[(i+5)*WIDTH + j+5];
                window[41] = data[(i+5)*WIDTH + j+6];

                window[42] = data[(i+6)*WIDTH + j+0];
                window[43] = data[(i+6)*WIDTH + j+1];
                window[44] = data[(i+6)*WIDTH + j+2];
                window[45] = data[(i+6)*WIDTH + j+3];
                window[46] = data[(i+6)*WIDTH + j+4];
                window[47] = data[(i+6)*WIDTH + j+5];
                window[48] = data[(i+6)*WIDTH + j+6];

                conv = 0;

                for(int k = 0; k < 7*7; k++)
                {
                    conv = conv + (window[k] * filters[k + 7*7*filter]);
                }

                output[filter*(WIDTH)*(HEIGHT) + (i*(WIDTH) + j)] = conv;
            }
        }
    }
/*
    filter++;

    for(int id = 0; id < filter*(WIDTH-3)*(HEIGHT-3); id++)
    {
        output[id] = 1 / (1 + expf(-1*output[id]));
    }
*/
}

int main(int argc, const char * argv[])
{
    /*
    FILE *picture;
    int c;
    int n = 0;

    picture = fopen("/home/antonio/Imagens/gray/g/Grayscale.jpg", "rb");
    if (picture != NULL)
    {
        do {
            c = fgetc(picture);
            if( feof(picture) ) break;
            printf("%c", c);
            n++;
        } while(1);

        printf("%i", n);
    }
    */

    SDL_Surface * hello = NULL;

    if(IMG_Init(IMG_INIT_JPG) == 0) printf("Erro ao abrir inicializador\n");
    else if((hello = IMG_Load("/home/antonio/Imagens/gray/g/Grayscale.jpg")) == NULL) printf("Erro ao abrir imagem\n");
    else printf("Tudo suave\n");

    return 0;
}