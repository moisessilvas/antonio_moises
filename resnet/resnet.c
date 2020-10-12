#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define N_FILTERS_64 64
#define N_FILTERS_128 128
#define N_FILTERS_256 256
#define N_FILTERS_512 512

#define DIM_0 224
#define DIM_1 112
#define DIM_2 56
#define DIM_3 28
#define DIM_4 14
#define DIM_5 7

static int image[(DIM_0+6)*(DIM_0+6)];
static float output1[(DIM_1+2)*(DIM_1+2)*N_FILTERS_64];
static float output2[(DIM_2+2)*(DIM_2+2)*N_FILTERS_64];

static float weights_conv1[N_FILTERS_64*7*7];

void zero_padding(FILE* picture)
{
    int c;
    int n = 0;
    int line = 0;
    int count = 0;
    bool isPadding = 1;

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

        image[52208] = 0;
    }
}

void init_weights_7x7()
{
    for(int i = 0; i < N_FILTERS_64*7*7; i++)
    {
        // filters[i] = rand()/RAND_MAX;
        weights_conv1[i] = 1;
    }
}

void conv1_layer()
{
    int filter, id = 0;
    float conv;
    unsigned char window[7*7];
    
    for(filter = 0; filter < N_FILTERS_64; filter++)
    {
        for(int i = 0; i < DIM_0); i = i + 2)
        {
            for(int j = 0; j < DIM_0; j = j + 2)
            {
                window[0] = image[(i+0)*(WIDTH+6) + j+0];
                window[1] = image[(i+0)*(WIDTH+6) + j+1];
                window[2] = image[(i+0)*(WIDTH+6) + j+2];
                window[3] = image[(i+0)*(WIDTH+6) + j+3];
                window[4] = image[(i+0)*(WIDTH+6) + j+4];
                window[5] = image[(i+0)*(WIDTH+6) + j+5];
                window[6] = image[(i+0)*(WIDTH+6) + j+6];
                
                window[7] = image[(i+1)*(WIDTH+6) + j+0];
                window[8] = image[(i+1)*(WIDTH+6) + j+1];
                window[9] = image[(i+1)*(WIDTH+6) + j+2];
                window[10] = image[(i+1)*(WIDTH+6) + j+3];
                window[11] = image[(i+1)*(WIDTH+6) + j+4];
                window[12] = image[(i+1)*(WIDTH+6) + j+5];
                window[13] = image[(i+1)*(WIDTH+6) + j+6];
                
                window[14] = image[(i+2)*(WIDTH+6) + j+0];
                window[15] = image[(i+2)*(WIDTH+6) + j+1];
                window[16] = image[(i+2)*(WIDTH+6) + j+2];
                window[17] = image[(i+2)*(WIDTH+6) + j+3];
                window[18] = image[(i+2)*(WIDTH+6) + j+4];
                window[19] = image[(i+2)*(WIDTH+6) + j+5];
                window[20] = image[(i+2)*(WIDTH+6) + j+6];

                window[21] = image[(i+3)*(WIDTH+6) + j+0];
                window[22] = image[(i+3)*(WIDTH+6) + j+1];
                window[23] = image[(i+3)*(WIDTH+6) + j+2];
                window[24] = image[(i+3)*(WIDTH+6) + j+3];
                window[25] = image[(i+3)*(WIDTH+6) + j+4];
                window[26] = image[(i+3)*(WIDTH+6) + j+5];
                window[27] = image[(i+3)*(WIDTH+6) + j+6];

                window[28] = image[(i+4)*(WIDTH+6) + j+0];
                window[29] = image[(i+4)*(WIDTH+6) + j+1];
                window[30] = image[(i+4)*(WIDTH+6) + j+2];
                window[31] = image[(i+4)*(WIDTH+6) + j+3];
                window[32] = image[(i+4)*(WIDTH+6) + j+4];
                window[33] = image[(i+4)*(WIDTH+6) + j+5];
                window[34] = image[(i+4)*(WIDTH+6) + j+6];

                window[35] = image[(i+5)*(WIDTH+6) + j+0];
                window[36] = image[(i+5)*(WIDTH+6) + j+1];
                window[37] = image[(i+5)*(WIDTH+6) + j+2];
                window[38] = image[(i+5)*(WIDTH+6) + j+3];
                window[39] = image[(i+5)*(WIDTH+6) + j+4];
                window[40] = image[(i+5)*(WIDTH+6) + j+5];
                window[41] = image[(i+5)*(WIDTH+6) + j+6];

                window[42] = image[(i+6)*(WIDTH+6) + j+0];
                window[43] = image[(i+6)*(WIDTH+6) + j+1];
                window[44] = image[(i+6)*(WIDTH+6) + j+2];
                window[45] = image[(i+6)*(WIDTH+6) + j+3];
                window[46] = image[(i+6)*(WIDTH+6) + j+4];
                window[47] = image[(i+6)*(WIDTH+6) + j+5];
                window[48] = image[(i+6)*(WIDTH+6) + j+6];

                conv = 0;

                for(int k = 0; k < 7*7; k++)
                {
                    conv = conv + (window[k] * filters[k + 7*7*filter]);
                }

                output1[filter*DIM_1*DIM_1 + ((i/1)*DIM_1 + (j/1))] = conv;
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

void conv2_layer()
{
    int filter, id = 0;
    float conv;
    unsigned char window[7*7];
    
    for(filter = 0; filter < N_FILTERS_64; filter++)
    {
        for(int i = 0; i < DIM_1); i = i + 2)
        {
            for(int j = 0; j < DIM_1; j = j + 2)
            {
                window[0] = image[(i+0)*(WIDTH+6) + j+0];
                window[1] = image[(i+0)*(WIDTH+6) + j+1];
                window[2] = image[(i+0)*(WIDTH+6) + j+2];
                window[3] = image[(i+0)*(WIDTH+6) + j+3];
                window[4] = image[(i+0)*(WIDTH+6) + j+4];
                window[5] = image[(i+0)*(WIDTH+6) + j+5];
                window[6] = image[(i+0)*(WIDTH+6) + j+6];
                
                window[7] = image[(i+1)*(WIDTH+6) + j+0];
                window[8] = image[(i+1)*(WIDTH+6) + j+1];
                window[9] = image[(i+1)*(WIDTH+6) + j+2];
                window[10] = image[(i+1)*(WIDTH+6) + j+3];
                window[11] = image[(i+1)*(WIDTH+6) + j+4];
                window[12] = image[(i+1)*(WIDTH+6) + j+5];
                window[13] = image[(i+1)*(WIDTH+6) + j+6];
                
                window[14] = image[(i+2)*(WIDTH+6) + j+0];
                window[15] = image[(i+2)*(WIDTH+6) + j+1];
                window[16] = image[(i+2)*(WIDTH+6) + j+2];
                window[17] = image[(i+2)*(WIDTH+6) + j+3];
                window[18] = image[(i+2)*(WIDTH+6) + j+4];
                window[19] = image[(i+2)*(WIDTH+6) + j+5];
                window[20] = image[(i+2)*(WIDTH+6) + j+6];

                window[21] = image[(i+3)*(WIDTH+6) + j+0];
                window[22] = image[(i+3)*(WIDTH+6) + j+1];
                window[23] = image[(i+3)*(WIDTH+6) + j+2];
                window[24] = image[(i+3)*(WIDTH+6) + j+3];
                window[25] = image[(i+3)*(WIDTH+6) + j+4];
                window[26] = image[(i+3)*(WIDTH+6) + j+5];
                window[27] = image[(i+3)*(WIDTH+6) + j+6];

                window[28] = image[(i+4)*(WIDTH+6) + j+0];
                window[29] = image[(i+4)*(WIDTH+6) + j+1];
                window[30] = image[(i+4)*(WIDTH+6) + j+2];
                window[31] = image[(i+4)*(WIDTH+6) + j+3];
                window[32] = image[(i+4)*(WIDTH+6) + j+4];
                window[33] = image[(i+4)*(WIDTH+6) + j+5];
                window[34] = image[(i+4)*(WIDTH+6) + j+6];

                window[35] = image[(i+5)*(WIDTH+6) + j+0];
                window[36] = image[(i+5)*(WIDTH+6) + j+1];
                window[37] = image[(i+5)*(WIDTH+6) + j+2];
                window[38] = image[(i+5)*(WIDTH+6) + j+3];
                window[39] = image[(i+5)*(WIDTH+6) + j+4];
                window[40] = image[(i+5)*(WIDTH+6) + j+5];
                window[41] = image[(i+5)*(WIDTH+6) + j+6];

                window[42] = image[(i+6)*(WIDTH+6) + j+0];
                window[43] = image[(i+6)*(WIDTH+6) + j+1];
                window[44] = image[(i+6)*(WIDTH+6) + j+2];
                window[45] = image[(i+6)*(WIDTH+6) + j+3];
                window[46] = image[(i+6)*(WIDTH+6) + j+4];
                window[47] = image[(i+6)*(WIDTH+6) + j+5];
                window[48] = image[(i+6)*(WIDTH+6) + j+6];

                conv = 0;

                for(int k = 0; k < 7*7; k++)
                {
                    conv = conv + (window[k] * filters[k + 7*7*filter]);
                }

                output2[filter*DIM_2*DIM_2 + ((i/1)*DIM_2 + (j/1))] = conv;
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

void conv3_layer()
{
    int filter, id = 0;
    float conv;
    unsigned char window[7*7];
    
    for(filter = 0; filter < N_FILTERS_128; filter++)
    {
        for(int i = 0; i < DIM_2); i = i + 2)
        {
            for(int j = 0; j < DIM_2; j = j + 2)
            {
                window[0] = image[(i+0)*(WIDTH+6) + j+0];
                window[1] = image[(i+0)*(WIDTH+6) + j+1];
                window[2] = image[(i+0)*(WIDTH+6) + j+2];
                window[3] = image[(i+0)*(WIDTH+6) + j+3];
                window[4] = image[(i+0)*(WIDTH+6) + j+4];
                window[5] = image[(i+0)*(WIDTH+6) + j+5];
                window[6] = image[(i+0)*(WIDTH+6) + j+6];
                
                window[7] = image[(i+1)*(WIDTH+6) + j+0];
                window[8] = image[(i+1)*(WIDTH+6) + j+1];
                window[9] = image[(i+1)*(WIDTH+6) + j+2];
                window[10] = image[(i+1)*(WIDTH+6) + j+3];
                window[11] = image[(i+1)*(WIDTH+6) + j+4];
                window[12] = image[(i+1)*(WIDTH+6) + j+5];
                window[13] = image[(i+1)*(WIDTH+6) + j+6];
                
                window[14] = image[(i+2)*(WIDTH+6) + j+0];
                window[15] = image[(i+2)*(WIDTH+6) + j+1];
                window[16] = image[(i+2)*(WIDTH+6) + j+2];
                window[17] = image[(i+2)*(WIDTH+6) + j+3];
                window[18] = image[(i+2)*(WIDTH+6) + j+4];
                window[19] = image[(i+2)*(WIDTH+6) + j+5];
                window[20] = image[(i+2)*(WIDTH+6) + j+6];

                window[21] = image[(i+3)*(WIDTH+6) + j+0];
                window[22] = image[(i+3)*(WIDTH+6) + j+1];
                window[23] = image[(i+3)*(WIDTH+6) + j+2];
                window[24] = image[(i+3)*(WIDTH+6) + j+3];
                window[25] = image[(i+3)*(WIDTH+6) + j+4];
                window[26] = image[(i+3)*(WIDTH+6) + j+5];
                window[27] = image[(i+3)*(WIDTH+6) + j+6];

                window[28] = image[(i+4)*(WIDTH+6) + j+0];
                window[29] = image[(i+4)*(WIDTH+6) + j+1];
                window[30] = image[(i+4)*(WIDTH+6) + j+2];
                window[31] = image[(i+4)*(WIDTH+6) + j+3];
                window[32] = image[(i+4)*(WIDTH+6) + j+4];
                window[33] = image[(i+4)*(WIDTH+6) + j+5];
                window[34] = image[(i+4)*(WIDTH+6) + j+6];

                window[35] = image[(i+5)*(WIDTH+6) + j+0];
                window[36] = image[(i+5)*(WIDTH+6) + j+1];
                window[37] = image[(i+5)*(WIDTH+6) + j+2];
                window[38] = image[(i+5)*(WIDTH+6) + j+3];
                window[39] = image[(i+5)*(WIDTH+6) + j+4];
                window[40] = image[(i+5)*(WIDTH+6) + j+5];
                window[41] = image[(i+5)*(WIDTH+6) + j+6];

                window[42] = image[(i+6)*(WIDTH+6) + j+0];
                window[43] = image[(i+6)*(WIDTH+6) + j+1];
                window[44] = image[(i+6)*(WIDTH+6) + j+2];
                window[45] = image[(i+6)*(WIDTH+6) + j+3];
                window[46] = image[(i+6)*(WIDTH+6) + j+4];
                window[47] = image[(i+6)*(WIDTH+6) + j+5];
                window[48] = image[(i+6)*(WIDTH+6) + j+6];

                conv = 0;

                for(int k = 0; k < 7*7; k++)
                {
                    conv = conv + (window[k] * filters[k + 7*7*filter]);
                }

                output3[filter*DIM_3*DIM_3 + ((i/1)*DIM_3 + (j/1))] = conv;
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

void conv4_layer()
{
    int filter, id = 0;
    float conv;
    unsigned char window[7*7];
    
    for(filter = 0; filter < N_FILTERS_256; filter++)
    {
        for(int i = 0; i < DIM_3); i = i + 2)
        {
            for(int j = 0; j < DIM_3; j = j + 2)
            {
                window[0] = image[(i+0)*(WIDTH+6) + j+0];
                window[1] = image[(i+0)*(WIDTH+6) + j+1];
                window[2] = image[(i+0)*(WIDTH+6) + j+2];
                window[3] = image[(i+0)*(WIDTH+6) + j+3];
                window[4] = image[(i+0)*(WIDTH+6) + j+4];
                window[5] = image[(i+0)*(WIDTH+6) + j+5];
                window[6] = image[(i+0)*(WIDTH+6) + j+6];
                
                window[7] = image[(i+1)*(WIDTH+6) + j+0];
                window[8] = image[(i+1)*(WIDTH+6) + j+1];
                window[9] = image[(i+1)*(WIDTH+6) + j+2];
                window[10] = image[(i+1)*(WIDTH+6) + j+3];
                window[11] = image[(i+1)*(WIDTH+6) + j+4];
                window[12] = image[(i+1)*(WIDTH+6) + j+5];
                window[13] = image[(i+1)*(WIDTH+6) + j+6];
                
                window[14] = image[(i+2)*(WIDTH+6) + j+0];
                window[15] = image[(i+2)*(WIDTH+6) + j+1];
                window[16] = image[(i+2)*(WIDTH+6) + j+2];
                window[17] = image[(i+2)*(WIDTH+6) + j+3];
                window[18] = image[(i+2)*(WIDTH+6) + j+4];
                window[19] = image[(i+2)*(WIDTH+6) + j+5];
                window[20] = image[(i+2)*(WIDTH+6) + j+6];

                window[21] = image[(i+3)*(WIDTH+6) + j+0];
                window[22] = image[(i+3)*(WIDTH+6) + j+1];
                window[23] = image[(i+3)*(WIDTH+6) + j+2];
                window[24] = image[(i+3)*(WIDTH+6) + j+3];
                window[25] = image[(i+3)*(WIDTH+6) + j+4];
                window[26] = image[(i+3)*(WIDTH+6) + j+5];
                window[27] = image[(i+3)*(WIDTH+6) + j+6];

                window[28] = image[(i+4)*(WIDTH+6) + j+0];
                window[29] = image[(i+4)*(WIDTH+6) + j+1];
                window[30] = image[(i+4)*(WIDTH+6) + j+2];
                window[31] = image[(i+4)*(WIDTH+6) + j+3];
                window[32] = image[(i+4)*(WIDTH+6) + j+4];
                window[33] = image[(i+4)*(WIDTH+6) + j+5];
                window[34] = image[(i+4)*(WIDTH+6) + j+6];

                window[35] = image[(i+5)*(WIDTH+6) + j+0];
                window[36] = image[(i+5)*(WIDTH+6) + j+1];
                window[37] = image[(i+5)*(WIDTH+6) + j+2];
                window[38] = image[(i+5)*(WIDTH+6) + j+3];
                window[39] = image[(i+5)*(WIDTH+6) + j+4];
                window[40] = image[(i+5)*(WIDTH+6) + j+5];
                window[41] = image[(i+5)*(WIDTH+6) + j+6];

                window[42] = image[(i+6)*(WIDTH+6) + j+0];
                window[43] = image[(i+6)*(WIDTH+6) + j+1];
                window[44] = image[(i+6)*(WIDTH+6) + j+2];
                window[45] = image[(i+6)*(WIDTH+6) + j+3];
                window[46] = image[(i+6)*(WIDTH+6) + j+4];
                window[47] = image[(i+6)*(WIDTH+6) + j+5];
                window[48] = image[(i+6)*(WIDTH+6) + j+6];

                conv = 0;

                for(int k = 0; k < 7*7; k++)
                {
                    conv = conv + (window[k] * filters[k + 7*7*filter]);
                }

                output4[filter*DIM_4*DIM_4 + ((i/1)*DIM_4 + (j/1))] = conv;
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

void conv5_layer()
{
    int filter, id = 0;
    float conv;
    unsigned char window[7*7];
    
    for(filter = 0; filter < N_FILTERS_512; filter++)
    {
        for(int i = 0; i < DIM_4); i = i + 2)
        {
            for(int j = 0; j < DIM_4; j = j + 2)
            {
                window[0] = image[(i+0)*(WIDTH+6) + j+0];
                window[1] = image[(i+0)*(WIDTH+6) + j+1];
                window[2] = image[(i+0)*(WIDTH+6) + j+2];
                window[3] = image[(i+0)*(WIDTH+6) + j+3];
                window[4] = image[(i+0)*(WIDTH+6) + j+4];
                window[5] = image[(i+0)*(WIDTH+6) + j+5];
                window[6] = image[(i+0)*(WIDTH+6) + j+6];
                
                window[7] = image[(i+1)*(WIDTH+6) + j+0];
                window[8] = image[(i+1)*(WIDTH+6) + j+1];
                window[9] = image[(i+1)*(WIDTH+6) + j+2];
                window[10] = image[(i+1)*(WIDTH+6) + j+3];
                window[11] = image[(i+1)*(WIDTH+6) + j+4];
                window[12] = image[(i+1)*(WIDTH+6) + j+5];
                window[13] = image[(i+1)*(WIDTH+6) + j+6];
                
                window[14] = image[(i+2)*(WIDTH+6) + j+0];
                window[15] = image[(i+2)*(WIDTH+6) + j+1];
                window[16] = image[(i+2)*(WIDTH+6) + j+2];
                window[17] = image[(i+2)*(WIDTH+6) + j+3];
                window[18] = image[(i+2)*(WIDTH+6) + j+4];
                window[19] = image[(i+2)*(WIDTH+6) + j+5];
                window[20] = image[(i+2)*(WIDTH+6) + j+6];

                window[21] = image[(i+3)*(WIDTH+6) + j+0];
                window[22] = image[(i+3)*(WIDTH+6) + j+1];
                window[23] = image[(i+3)*(WIDTH+6) + j+2];
                window[24] = image[(i+3)*(WIDTH+6) + j+3];
                window[25] = image[(i+3)*(WIDTH+6) + j+4];
                window[26] = image[(i+3)*(WIDTH+6) + j+5];
                window[27] = image[(i+3)*(WIDTH+6) + j+6];

                window[28] = image[(i+4)*(WIDTH+6) + j+0];
                window[29] = image[(i+4)*(WIDTH+6) + j+1];
                window[30] = image[(i+4)*(WIDTH+6) + j+2];
                window[31] = image[(i+4)*(WIDTH+6) + j+3];
                window[32] = image[(i+4)*(WIDTH+6) + j+4];
                window[33] = image[(i+4)*(WIDTH+6) + j+5];
                window[34] = image[(i+4)*(WIDTH+6) + j+6];

                window[35] = image[(i+5)*(WIDTH+6) + j+0];
                window[36] = image[(i+5)*(WIDTH+6) + j+1];
                window[37] = image[(i+5)*(WIDTH+6) + j+2];
                window[38] = image[(i+5)*(WIDTH+6) + j+3];
                window[39] = image[(i+5)*(WIDTH+6) + j+4];
                window[40] = image[(i+5)*(WIDTH+6) + j+5];
                window[41] = image[(i+5)*(WIDTH+6) + j+6];

                window[42] = image[(i+6)*(WIDTH+6) + j+0];
                window[43] = image[(i+6)*(WIDTH+6) + j+1];
                window[44] = image[(i+6)*(WIDTH+6) + j+2];
                window[45] = image[(i+6)*(WIDTH+6) + j+3];
                window[46] = image[(i+6)*(WIDTH+6) + j+4];
                window[47] = image[(i+6)*(WIDTH+6) + j+5];
                window[48] = image[(i+6)*(WIDTH+6) + j+6];

                conv = 0;

                for(int k = 0; k < 7*7; k++)
                {
                    conv = conv + (window[k] * filters[k + 7*7*filter]);
                }

                output5[filter*DIM_5*DIM_5 + ((i/1)*DIM_5 + (j/1))] = conv;
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
    
    picture = fopen("/home/antonio/Imagens/Grayscale.pnm", "rb");

    zero_padding(picture);
    init_weights_7x7();
    conv1_layer();
    conv2_layer();
    conv3_layer();
    conv4_layer();
    conv5_layer();

    /*for(int filter = 0; filter < N_FILTERS; filter++)
    {  
        for(int linha = 0; linha < OUT_DIM*2; linha++)
        {
            for(int coluna = 0; coluna < OUT_DIM*2; coluna++)
            {
                printf("%f ", output[filter*OUT_DIM*OUT_DIM*4 + linha*OUT_DIM*2 + coluna]);
            }
            printf("\n");
        }
    }*/

    return 0;
}