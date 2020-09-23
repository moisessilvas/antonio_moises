#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define N_FILTERS 64
#define STRIDE 2
#define WIDTH 224
#define HEIGHT 224


static float filters[N_FILTERS*7*7];
//static int image[(WIDTH+6)*(HEIGHT+6)];
//static float output[112*112*64];

void init_weights_7x7()
{
    for(int i = 0; i < N_FILTERS*7*7; i++)
    {
        // filters[i] = rand()/RAND_MAX;
        filters[i] = 1;
    }
}

void conv1_layer(int image[], float output[])
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
                window[0] = image[(i+0)*WIDTH + j+0];
                window[1] = image[(i+0)*WIDTH + j+1];
                window[2] = image[(i+0)*WIDTH + j+2];
                window[3] = image[(i+0)*WIDTH + j+3];
                window[4] = image[(i+0)*WIDTH + j+4];
                window[5] = image[(i+0)*WIDTH + j+5];
                window[6] = image[(i+0)*WIDTH + j+6];
                
                window[7] = image[(i+1)*WIDTH + j+0];
                window[8] = image[(i+1)*WIDTH + j+1];
                window[9] = image[(i+1)*WIDTH + j+2];
                window[10] = image[(i+1)*WIDTH + j+3];
                window[11] = image[(i+1)*WIDTH + j+4];
                window[12] = image[(i+1)*WIDTH + j+5];
                window[13] = image[(i+1)*WIDTH + j+6];
                
                window[14] = image[(i+2)*WIDTH + j+0];
                window[15] = image[(i+2)*WIDTH + j+1];
                window[16] = image[(i+2)*WIDTH + j+2];
                window[17] = image[(i+2)*WIDTH + j+3];
                window[18] = image[(i+2)*WIDTH + j+4];
                window[19] = image[(i+2)*WIDTH + j+5];
                window[20] = image[(i+2)*WIDTH + j+6];

                window[21] = image[(i+3)*WIDTH + j+0];
                window[22] = image[(i+3)*WIDTH + j+1];
                window[23] = image[(i+3)*WIDTH + j+2];
                window[24] = image[(i+3)*WIDTH + j+3];
                window[25] = image[(i+3)*WIDTH + j+4];
                window[26] = image[(i+3)*WIDTH + j+5];
                window[27] = image[(i+3)*WIDTH + j+6];

                window[28] = image[(i+4)*WIDTH + j+0];
                window[29] = image[(i+4)*WIDTH + j+1];
                window[30] = image[(i+4)*WIDTH + j+2];
                window[31] = image[(i+4)*WIDTH + j+3];
                window[32] = image[(i+4)*WIDTH + j+4];
                window[33] = image[(i+4)*WIDTH + j+5];
                window[34] = image[(i+4)*WIDTH + j+6];

                window[35] = image[(i+5)*WIDTH + j+0];
                window[36] = image[(i+5)*WIDTH + j+1];
                window[37] = image[(i+5)*WIDTH + j+2];
                window[38] = image[(i+5)*WIDTH + j+3];
                window[39] = image[(i+5)*WIDTH + j+4];
                window[40] = image[(i+5)*WIDTH + j+5];
                window[41] = image[(i+5)*WIDTH + j+6];

                window[42] = image[(i+6)*WIDTH + j+0];
                window[43] = image[(i+6)*WIDTH + j+1];
                window[44] = image[(i+6)*WIDTH + j+2];
                window[45] = image[(i+6)*WIDTH + j+3];
                window[46] = image[(i+6)*WIDTH + j+4];
                window[47] = image[(i+6)*WIDTH + j+5];
                window[48] = image[(i+6)*WIDTH + j+6];

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
    FILE *picture;
    int c;
    int n = 0;
    int line = 0;
    int count = 0;
    bool isPadding = 1;
    int image[230*230];

    picture = fopen("/home/antonio/Imagens/gray/g/Grayscale.pnm", "rb");
    if (picture != NULL)
    {
        for(int i = 0; i < 6; i++)
        {
            for(int j = 0; j < 230; j++)
            {
                if(i < 3) 
                {
                    image[(i+1)*230 + j] = 0;
                    count++;
                }
                else image[(i+224)*230 + j] = 0;
            }
        }

        do {
            c = fgetc(picture);
            
            if( feof(picture) ) break;
            else if(c == '\n' && n < 3) n++;
            else if(n >= 3)
            {
                while(isPadding)
                {
                    image[count] = 0;
                    count++;

                    if(line == 0 || line == 1 || line == 227 || line == 228) line++;
                    else if(line == 229) line = 0;
                    else if(line == 2) 
                    {
                        line++;
                        isPadding = 0;
                    }
                }
                
                image[count] = c;
                count++;

                if(line == 226) isPadding = 1;
                line++;
            }
        } while(1);
    }

    for(int linha = 0; linha < 230; linha++)
    {
        for(int coluna = 0; coluna < 230; coluna++)
        {
            printf("%i ", image[linha*230 + coluna]);
        }
        printf("\n");
    }

    //init_weights_7x7();
    //conv1_layer()
    return 0;
}