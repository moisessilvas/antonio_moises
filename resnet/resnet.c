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
static float output2_temp[(DIM_2+2)*(DIM_2+2)*N_FILTERS_64];
static float output3[(DIM_3+2)*(DIM_3+2)*N_FILTERS_128];
static float output3_temp[(DIM_3+2)*(DIM_3+2)*N_FILTERS_128];
static float output4[(DIM_4+2)*(DIM_4+2)*N_FILTERS_256];
static float output5[(DIM_5)*(DIM_5)*N_FILTERS_512];

static float weights_conv1[N_FILTERS_64*7*7];
static float weights_conv2_layer1[N_FILTERS_64*3*3*N_FILTERS_64];
static float weights_conv2_layer2[N_FILTERS_64*3*3*N_FILTERS_64];
static float weights_conv3_layer1[N_FILTERS_128*3*3*N_FILTERS_64];
static float weights_conv3_layer2[N_FILTERS_128*3*3*N_FILTERS_64];
static float weights_conv4[N_FILTERS_256*3*3];
static float weights_conv5[N_FILTERS_512*3*3];

static float pooling_conv2[(DIM_2+2)*(DIM_2+2)*N_FILTERS_64];

void init_weights_conv1()
{
    for(int i = 0; i < N_FILTERS_64*7*7; i++)
    {
        // filters[i] = rand()/RAND_MAX;
        weights_conv1[i] = 1.0f;
    }
}

void zero_padding_conv1(FILE* picture)
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
                    image[i*230 + j] = 0;
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

void conv1_layer()
{
    int filter, id = 0;
    float conv;
    unsigned char window[7*7];
    
    for(filter = 0; filter < N_FILTERS_64; filter++)
    {
        for(int i = 0; i < DIM_0; i = i + 2)
        {
            for(int j = 0; j < DIM_0; j = j + 2)
            {
                window[0] = image[(i+0)*(DIM_0+6) + j+0];
                window[1] = image[(i+0)*(DIM_0+6) + j+1];
                window[2] = image[(i+0)*(DIM_0+6) + j+2];
                window[3] = image[(i+0)*(DIM_0+6) + j+3];
                window[4] = image[(i+0)*(DIM_0+6) + j+4];
                window[5] = image[(i+0)*(DIM_0+6) + j+5];
                window[6] = image[(i+0)*(DIM_0+6) + j+6];
                
                window[7] = image[(i+1)*(DIM_0+6) + j+0];
                window[8] = image[(i+1)*(DIM_0+6) + j+1];
                window[9] = image[(i+1)*(DIM_0+6) + j+2];
                window[10] = image[(i+1)*(DIM_0+6) + j+3];
                window[11] = image[(i+1)*(DIM_0+6) + j+4];
                window[12] = image[(i+1)*(DIM_0+6) + j+5];
                window[13] = image[(i+1)*(DIM_0+6) + j+6];
                
                window[14] = image[(i+2)*(DIM_0+6) + j+0];
                window[15] = image[(i+2)*(DIM_0+6) + j+1];
                window[16] = image[(i+2)*(DIM_0+6) + j+2];
                window[17] = image[(i+2)*(DIM_0+6) + j+3];
                window[18] = image[(i+2)*(DIM_0+6) + j+4];
                window[19] = image[(i+2)*(DIM_0+6) + j+5];
                window[20] = image[(i+2)*(DIM_0+6) + j+6];

                window[21] = image[(i+3)*(DIM_0+6) + j+0];
                window[22] = image[(i+3)*(DIM_0+6) + j+1];
                window[23] = image[(i+3)*(DIM_0+6) + j+2];
                window[24] = image[(i+3)*(DIM_0+6) + j+3];
                window[25] = image[(i+3)*(DIM_0+6) + j+4];
                window[26] = image[(i+3)*(DIM_0+6) + j+5];
                window[27] = image[(i+3)*(DIM_0+6) + j+6];

                window[28] = image[(i+4)*(DIM_0+6) + j+0];
                window[29] = image[(i+4)*(DIM_0+6) + j+1];
                window[30] = image[(i+4)*(DIM_0+6) + j+2];
                window[31] = image[(i+4)*(DIM_0+6) + j+3];
                window[32] = image[(i+4)*(DIM_0+6) + j+4];
                window[33] = image[(i+4)*(DIM_0+6) + j+5];
                window[34] = image[(i+4)*(DIM_0+6) + j+6];

                window[35] = image[(i+5)*(DIM_0+6) + j+0];
                window[36] = image[(i+5)*(DIM_0+6) + j+1];
                window[37] = image[(i+5)*(DIM_0+6) + j+2];
                window[38] = image[(i+5)*(DIM_0+6) + j+3];
                window[39] = image[(i+5)*(DIM_0+6) + j+4];
                window[40] = image[(i+5)*(DIM_0+6) + j+5];
                window[41] = image[(i+5)*(DIM_0+6) + j+6];

                window[42] = image[(i+6)*(DIM_0+6) + j+0];
                window[43] = image[(i+6)*(DIM_0+6) + j+1];
                window[44] = image[(i+6)*(DIM_0+6) + j+2];
                window[45] = image[(i+6)*(DIM_0+6) + j+3];
                window[46] = image[(i+6)*(DIM_0+6) + j+4];
                window[47] = image[(i+6)*(DIM_0+6) + j+5];
                window[48] = image[(i+6)*(DIM_0+6) + j+6];

                conv = 0.0f;

                for(int k = 0; k < 7*7; k++)
                {
                    conv = conv + (window[k] * weights_conv1[k + 7*7*filter]);
                }

                output1[filter*(DIM_1+2)*(DIM_1+2) + ((i/2 + 1)*(DIM_1+2) + (j/2 + 1))] = conv;
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

void init_weights_conv2()
{
    for(int i = 0; i < N_FILTERS_64*3*3*N_FILTERS_64; i++)
    {
        // filters[i] = rand()/RAND_MAX;
        weights_conv2_layer1[i] = 1.0f;
        weights_conv2_layer2[i] = 1.0f;
    }
}

void zero_padding_pool_conv2()
{
    for(int count = 0; count < 64; count = count + 1)
    {    
        //Popula as linhas nas extremidades
        for(int i = 0; i < 2; i++)
        {
            for(int j = 0; j < DIM_1 + 2; j++)
            {
                if(i < 1) 
                {
                    output1[i*(DIM_1+2) + j] = 0;
                }
                else output1[(i+DIM_1)*(DIM_1+2) + j] = 0;
            }
        }

        //Popula as colunas nas extremidades
        for(int j = 0; j < 2; j++)
        {
            for(int i = 0; i < DIM_1 + 2; i++)
            {
                if(j < 1) 
                {
                    output1[i*(DIM_1 + 2)] = 0;
                }
                else output1[(i+1)*(DIM_1 + 2) - 1] = 0;
            }
        }
    }
}

void max_pooling_conv2()
{
    float window[3*3];
    float max_pool;

    for (int depth = 0; depth < N_FILTERS_64; depth++)
    {
        for (int i = 0; i < DIM_1; i = i + 2)
        {
            for (int j = 0; j < DIM_1; j = j + 2)
            {
                window[0] = output1[(depth*(DIM_1+2)*(DIM_1+2)) + (i+0)*(DIM_1+2) + j+0];
                window[1] = output1[(depth*(DIM_1+2)*(DIM_1+2)) + (i+0)*(DIM_1+2) + j+1];
                window[2] = output1[(depth*(DIM_1+2)*(DIM_1+2)) + (i+0)*(DIM_1+2) + j+2];

                window[3] = output1[(depth*(DIM_1+2)*(DIM_1+2)) + (i+1)*(DIM_1+2) + j+0];
                window[4] = output1[(depth*(DIM_1+2)*(DIM_1+2)) + (i+1)*(DIM_1+2) + j+1];
                window[5] = output1[(depth*(DIM_1+2)*(DIM_1+2)) + (i+1)*(DIM_1+2) + j+2];

                window[6] = output1[(depth*(DIM_1+2)*(DIM_1+2)) + (i+2)*(DIM_1+2) + j+0];
                window[7] = output1[(depth*(DIM_1+2)*(DIM_1+2)) + (i+2)*(DIM_1+2) + j+1];
                window[8] = output1[(depth*(DIM_1+2)*(DIM_1+2)) + (i+2)*(DIM_1+2) + j+2];

                max_pool = 0.0f;

                for (int pool = 0; pool < 3*3; pool++)
                {
                    if(window[pool] > max_pool) max_pool = window[pool];
                }
                
                pooling_conv2[(depth*(DIM_2+2)*(DIM_2+2)) + (i/2 + 1)*(DIM_2+2) + (j/2 + 1)] = max_pool;
            }
            
        }
        
    }
    
}

void zero_padding_conv2()
{
    for(int count = 0; count < 64; count = count + 1)
    {    
        //Popula as linhas nas extremidades
        for(int i = 0; i < 2; i++)
        {
            for(int j = 0; j < DIM_2 + 2; j++)
            {
                if(i < 1) 
                {
                    pooling_conv2[i*(DIM_2+2) + j] = 0;
                }
                else pooling_conv2[(i+DIM_2)*(DIM_2+2) + j] = 0;
            }
        }

        //Popula as colunas nas extremidades
        for(int j = 0; j < 2; j++)
        {
            for(int i = 0; i < DIM_2 + 2; i++)
            {
                if(j < 1) 
                {
                    pooling_conv2[i*(DIM_2 + 2)] = 0;
                }
                else pooling_conv2[(i+1)*(DIM_2 + 2) - 1] = 0;
            }
        }
    }
}

void conv2_layer(float input[])
{
    int filter;
    float conv;
    float window_layer1[3*3*N_FILTERS_64];
    float window_layer2[3*3*N_FILTERS_64];
    
    for(filter = 0; filter < N_FILTERS_64; filter++)
    {
        for(int i = 0; i < DIM_2; i = i + 1)
        {
            for(int j = 0; j < DIM_2; j = j + 1)
            {
                for (int depth = 0; depth < N_FILTERS_64; depth++)
                {
                    window_layer1[0 + depth*9] = input[(i+0)*(DIM_2+2) + j+0 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer1[1 + depth*9] = input[(i+0)*(DIM_2+2) + j+1 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer1[2 + depth*9] = input[(i+0)*(DIM_2+2) + j+2 + depth*(DIM_2+2)*(DIM_2+2)];

                    window_layer1[3 + depth*9] = input[(i+1)*(DIM_2+2) + j+0 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer1[4 + depth*9] = input[(i+1)*(DIM_2+2) + j+1 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer1[5 + depth*9] = input[(i+1)*(DIM_2+2) + j+2 + depth*(DIM_2+2)*(DIM_2+2)];

                    window_layer1[6 + depth*9] = input[(i+2)*(DIM_2+2) + j+0 + depth*(DIM_2+2)*(DIM_2+2)]; 
                    window_layer1[7 + depth*9] = input[(i+2)*(DIM_2+2) + j+1 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer1[8 + depth*9] = input[(i+2)*(DIM_2+2) + j+2 + depth*(DIM_2+2)*(DIM_2+2)];
                }

                conv = 0.0f;

                for(int k = 0; k < 3*3*N_FILTERS_64; k++)
                {
                    conv = conv + (window_layer1[k] * weights_conv2_layer1[k + 3*3*N_FILTERS_64*filter]);
                }

                output2_temp[filter*(DIM_2+2)*(DIM_2+2) + (i+1)*DIM_2 + (j+1)] = conv;
            }
        }
    }

    for(filter = 0; filter < N_FILTERS_64; filter++)
    {
        for(int i = 0; i < DIM_2; i = i + 1)
        {
            for(int j = 0; j < DIM_2; j = j + 1)
            {
                for (int depth = 0; depth < N_FILTERS_64; depth++)
                {
                    window_layer2[0 + depth*9] = output2_temp[(i+0)*(DIM_2+2) + j+0 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer2[1 + depth*9] = output2_temp[(i+0)*(DIM_2+2) + j+1 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer2[2 + depth*9] = output2_temp[(i+0)*(DIM_2+2) + j+2 + depth*(DIM_2+2)*(DIM_2+2)];

                    window_layer2[3 + depth*9] = output2_temp[(i+1)*(DIM_2+2) + j+0 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer2[4 + depth*9] = output2_temp[(i+1)*(DIM_2+2) + j+1 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer2[5 + depth*9] = output2_temp[(i+1)*(DIM_2+2) + j+2 + depth*(DIM_2+2)*(DIM_2+2)];

                    window_layer2[6 + depth*9] = output2_temp[(i+2)*(DIM_2+2) + j+0 + depth*(DIM_2+2)*(DIM_2+2)]; 
                    window_layer2[7 + depth*9] = output2_temp[(i+2)*(DIM_2+2) + j+1 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer2[8 + depth*9] = output2_temp[(i+2)*(DIM_2+2) + j+2 + depth*(DIM_2+2)*(DIM_2+2)];
                }

                conv = 0.0f;

                for(int k = 0; k < 3*3*N_FILTERS_64; k++)
                {
                    conv = conv + (window_layer2[k] * weights_conv2_layer2[k + 3*3*N_FILTERS_64*filter]);
                }

                output2[filter*(DIM_2+2)*(DIM_2+2) + (i+1)*DIM_2 + (j+1)] = conv;
            }
        }
    }

    for(int n = 0; n < (DIM_2+2)*(DIM_2+2)*N_FILTERS_64; n++)
    {
        output2[n] = input[n] + output2[n];
    }
}

void init_weights_conv3()
{
    for(int i = 0; i < N_FILTERS_128*3*3*N_FILTERS_64; i++)
    {
        // filters[i] = rand()/RAND_MAX;
        weights_conv3_layer1[i] = 1;
        weights_conv3_layer2[i] = 1
    }
}

void zero_padding_conv3()
{
    for(int count = 0; count < 64; count = count + 1)
    {    
        //Popula as linhas nas extremidades
        for(int i = 0; i < 2; i++)
        {
            for(int j = 0; j < DIM_2 + 2; j++)
            {
                if(i < 1) 
                {
                    output2[i*(DIM_2+2) + j] = 0;
                }
                else output2[(i+DIM_2)*(DIM_2+2) + j] = 0;
            }
        }

        //Popula as colunas nas extremidades
        for(int j = 0; j < 2; j++)
        {
            for(int i = 0; i < DIM_2 + 2; i++)
            {
                if(j < 1) 
                {
                    output2[i*(DIM_2 + 2)] = 0;
                }
                else output2[(i+1)*(DIM_2 + 2) - 1] = 0;
            }
        }
    }
}

void conv3_layer(float input[], int stride)
{
    int filter;
    float conv;
    float window_layer1[3*3*N_FILTERS_64];
    float window_layer2[3*3*N_FILTERS_64];
    
    for(filter = 0; filter < N_FILTERS_128; filter++)
    {
        for(int i = 0; i < DIM_2; i = i + stride)
        {
            for(int j = 0; j < DIM_2; j = j + stride)
            {
                for (int depth = 0; depth < N_FILTERS_64; depth++)
                {
                    window_layer1[0 + depth*9] = input[(i+0)*(DIM_2+2) + j+0 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer1[1 + depth*9] = input[(i+0)*(DIM_2+2) + j+1 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer1[2 + depth*9] = input[(i+0)*(DIM_2+2) + j+2 + depth*(DIM_2+2)*(DIM_2+2)];

                    window_layer1[3 + depth*9] = input[(i+1)*(DIM_2+2) + j+0 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer1[4 + depth*9] = input[(i+1)*(DIM_2+2) + j+1 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer1[5 + depth*9] = input[(i+1)*(DIM_2+2) + j+2 + depth*(DIM_2+2)*(DIM_2+2)];

                    window_layer1[6 + depth*9] = input[(i+2)*(DIM_2+2) + j+0 + depth*(DIM_2+2)*(DIM_2+2)]; 
                    window_layer1[7 + depth*9] = input[(i+2)*(DIM_2+2) + j+1 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer1[8 + depth*9] = input[(i+2)*(DIM_2+2) + j+2 + depth*(DIM_2+2)*(DIM_2+2)];
                }

                conv = 0.0f;

                for(int k = 0; k < 3*3*N_FILTERS_64; k++)
                {
                    conv = conv + (window_layer1[k] * weights_conv3_layer1[k + 3*3*N_FILTERS_64*filter]);
                }

                output3_temp[filter*(DIM_3+2)*(DIM_3+2) + (i+1)*DIM_3 + (j+1)] = conv;
            }
        }
    }

    for(filter = 0; filter < N_FILTERS_128; filter++)
    {
        for(int i = 0; i < DIM_2; i = i + 1)
        {
            for(int j = 0; j < DIM_2; j = j + 1)
            {
                for (int depth = 0; depth < N_FILTERS_64; depth++)
                {
                    window_layer2[0 + depth*9] = output3_temp[(i+0)*(DIM_2+2) + j+0 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer2[1 + depth*9] = output3_temp[(i+0)*(DIM_2+2) + j+1 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer2[2 + depth*9] = output3_temp[(i+0)*(DIM_2+2) + j+2 + depth*(DIM_2+2)*(DIM_2+2)];

                    window_layer2[3 + depth*9] = output3_temp[(i+1)*(DIM_2+2) + j+0 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer2[4 + depth*9] = output3_temp[(i+1)*(DIM_2+2) + j+1 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer2[5 + depth*9] = output3_temp[(i+1)*(DIM_2+2) + j+2 + depth*(DIM_2+2)*(DIM_2+2)];

                    window_layer2[6 + depth*9] = output3_temp[(i+2)*(DIM_2+2) + j+0 + depth*(DIM_2+2)*(DIM_2+2)]; 
                    window_layer2[7 + depth*9] = output3_temp[(i+2)*(DIM_2+2) + j+1 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer2[8 + depth*9] = output3_temp[(i+2)*(DIM_2+2) + j+2 + depth*(DIM_2+2)*(DIM_2+2)];
                }

                conv = 0.0f;

                for(int k = 0; k < 3*3*N_FILTERS_64; k++)
                {
                    conv = conv + (window_layer2[k] * weights_conv3_layer2[k + 3*3*N_FILTERS_64*filter]);
                }

                output3[filter*(DIM_3+2)*(DIM_3+2) + (i+1)*DIM_3 + (j+1)] = conv;
            }
        }
    }

    for(int n = 0; n < (DIM_3+2)*(DIM_3+2)*N_FILTERS_128; n++)
    {
        output3[n] = input[n] + output3[n];
    }
}

void init_weights_conv4()
{
    for(int i = 0; i < N_FILTERS_256*3*3; i++)
    {
        // filters[i] = rand()/RAND_MAX;
        weights_conv4[i] = 1;
    }
}

void zero_padding_conv4()
{
    for(int count = 0; count < 128; count = count + 1)
    {    
        //Popula as linhas nas extremidades
        for(int i = 0; i < 2; i++)
        {
            for(int j = 0; j < DIM_3 + 2; j++)
            {
                if(i < 1) 
                {
                    output3[i*(DIM_3+2) + j] = 0;
                }
                else output3[(i+DIM_3)*(DIM_3+2) + j] = 0;
            }
        }

        //Popula as colunas nas extremidades
        for(int j = 0; j < 2; j++)
        {
            for(int i = 0; i < DIM_3 + 2; i++)
            {
                if(j < 1) 
                {
                    output3[i*(DIM_3 + 2)] = 0;
                }
                else output3[(i+1)*(DIM_3 + 2) - 1] = 0;
            }
        }
    }
}

void conv4_layer(float input[], int stride, bool first_conv)
{
    int dim = DIM_4; 
    int filter_size = N_FILTERS_256;
    
    if(first_conv)
    {
        dim = DIM_3;
        filter_size = N_FILTERS_128;
    } 
    
    int filter;
    float conv;
    float window_layer1[3*3*filter_size];
    float window_layer2[3*3*filter_size];
    
    for(filter = 0; filter < N_FILTERS_256; filter++)
    {
        for(int i = 0; i < dim; i = i + stride)
        {
            for(int j = 0; j < dim; j = j + stride)
            {
                for (int depth = 0; depth < filter_size; depth++)
                {
                    window_layer1[0 + depth*9] = input[(i+0)*(dim+2) + j+0 + depth*(dim+2)*(dim+2)];
                    window_layer1[1 + depth*9] = input[(i+0)*(dim+2) + j+1 + depth*(dim+2)*(dim+2)];
                    window_layer1[2 + depth*9] = input[(i+0)*(dim+2) + j+2 + depth*(dim+2)*(dim+2)];

                    window_layer1[3 + depth*9] = input[(i+1)*(dim+2) + j+0 + depth*(dim+2)*(dim+2)];
                    window_layer1[4 + depth*9] = input[(i+1)*(dim+2) + j+1 + depth*(dim+2)*(dim+2)];
                    window_layer1[5 + depth*9] = input[(i+1)*(dim+2) + j+2 + depth*(dim+2)*(dim+2)];

                    window_layer1[6 + depth*9] = input[(i+2)*(dim+2) + j+0 + depth*(dim+2)*(dim+2)]; 
                    window_layer1[7 + depth*9] = input[(i+2)*(dim+2) + j+1 + depth*(dim+2)*(dim+2)];
                    window_layer1[8 + depth*9] = input[(i+2)*(dim+2) + j+2 + depth*(dim+2)*(dim+2)];
                }

                conv = 0.0f;

                for(int k = 0; k < 3*3*filter_size; k++)
                {
                    conv = conv + (window_layer1[k] * weights_conv4_layer1[k + 3*3*filter_size*filter]);
                }

                output4_temp[filter*(DIM_4+2)*(DIM_4+2) + (i+1)*DIM_4 + (j+1)] = conv;
            }
        }
    }

    for(filter = 0; filter < N_FILTERS_256; filter++)
    {
        for(int i = 0; i < DIM_4; i = i + 1)
        {
            for(int j = 0; j < DIM_4; j = j + 1)
            {
                for (int depth = 0; depth < N_FILTERS_256; depth++)
                {
                    window_layer2[0 + depth*9] = output4_temp[(i+0)*(DIM_2+2) + j+0 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer2[1 + depth*9] = output4_temp[(i+0)*(DIM_2+2) + j+1 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer2[2 + depth*9] = output4_temp[(i+0)*(DIM_2+2) + j+2 + depth*(DIM_2+2)*(DIM_2+2)];

                    window_layer2[3 + depth*9] = output4_temp[(i+1)*(DIM_2+2) + j+0 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer2[4 + depth*9] = output4_temp[(i+1)*(DIM_2+2) + j+1 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer2[5 + depth*9] = output4_temp[(i+1)*(DIM_2+2) + j+2 + depth*(DIM_2+2)*(DIM_2+2)];

                    window_layer2[6 + depth*9] = output4_temp[(i+2)*(DIM_2+2) + j+0 + depth*(DIM_2+2)*(DIM_2+2)]; 
                    window_layer2[7 + depth*9] = output4_temp[(i+2)*(DIM_2+2) + j+1 + depth*(DIM_2+2)*(DIM_2+2)];
                    window_layer2[8 + depth*9] = output4_temp[(i+2)*(DIM_2+2) + j+2 + depth*(DIM_2+2)*(DIM_2+2)];
                }

                conv = 0.0f;

                for(int k = 0; k < 3*3*N_FILTERS_128; k++)
                {
                    conv = conv + (window_layer2[k] * weights_conv3_layer2[k + 3*3*N_FILTERS_128*filter]);
                }

                output3[filter*(DIM_4+2)*(DIM_4+2) + (i+1)*DIM_4 + (j+1)] = conv;
            }
        }
    }

    for(int n = 0; n < (DIM_4+2)*(DIM_4+2)*N_FILTERS_256; n++)
    {
        output4[n] = input[n] + output4[n];
    }
}

void init_weights_conv5()
{
    for(int i = 0; i < N_FILTERS_512*3*3; i++)
    {
        // filters[i] = rand()/RAND_MAX;
        weights_conv5[i] = 1;
    }
}

void zero_padding_conv5()
{
    for(int count = 0; count < 256; count = count + 1)
    {    
        //Popula as linhas nas extremidades
        for(int i = 0; i < 2; i++)
        {
            for(int j = 0; j < DIM_4 + 2; j++)
            {
                if(i < 1) 
                {
                    output4[i*(DIM_4+2) + j] = 0;
                }
                else output4[(i+DIM_4)*(DIM_4+2) + j] = 0;
            }
        }

        //Popula as colunas nas extremidades
        for(int j = 0; j < 2; j++)
        {
            for(int i = 0; i < DIM_4 + 2; i++)
            {
                if(j < 1) 
                {
                    output4[i*(DIM_4 + 2)] = 0;
                }
                else output4[(i+1)*(DIM_4 + 2) - 1] = 0;
            }
        }
    }
}

void conv5_layer()
{
    int filter, id = 0;
    float conv;
    unsigned char window[3*3];
    
    for(filter = 0; filter < N_FILTERS_512; filter++)
    {
        for(int i = 0; i < DIM_4; i = i + 2)
        {
            for(int j = 0; j < DIM_4; j = j + 2)
            {
                window[0] = image[(i+0)*(DIM_4+2) + j+0];
                window[1] = image[(i+0)*(DIM_4+2) + j+1];
                window[2] = image[(i+0)*(DIM_4+2) + j+2];

                window[3] = image[(i+1)*(DIM_4+2) + j+0];
                window[4] = image[(i+1)*(DIM_4+2) + j+1];
                window[5] = image[(i+1)*(DIM_4+2) + j+2];

                window[6] = image[(i+2)*(DIM_4+2) + j+0]; 
                window[7] = image[(i+2)*(DIM_4+2) + j+1];
                window[8] = image[(i+2)*(DIM_4+2) + j+2];

                conv = 0;

                for(int k = 0; k < 3*3; k++)
                {
                    conv = conv + (window[k] * weights_conv5[k + 3*3*filter]);
                }

                output5[filter*DIM_5*DIM_5 + ((i/2 + 2*(i%2))*DIM_5 + (j/2 + 2*(j%2)))] = conv;
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
    float dense = 0.0f;
    
    picture = fopen("/home/antonio/Imagens/Grayscale.pnm", "rb");

    init_weights_conv1();
    init_weights_conv2();
    //init_weights_conv3();
    //init_weights_conv4();
    //init_weights_conv5();

    zero_padding_conv1(picture);
    conv1_layer();
    
    zero_padding_pool_conv2();
    max_pooling_conv2();
    zero_padding_conv2();
    conv2_layer(pooling_conv2);
    zero_padding_conv3();
    conv2_layer(output2);
    
    zero_padding_conv3();
    conv3_layer(output2, 2);
    zero_padding_conv4();
    conv3_layer(output3, 1);

    zero_padding_conv4();
    //conv4_layer();
    //zero_padding_conv5();
    //conv5_layer();

    for(int id = 0; id < (DIM_2+2)*(DIM_2+2)*N_FILTERS_64; id++)
    {
        dense = dense + output2[id];
    }

    printf("%f\n", dense);

    /*
    for(int i = 0; i < DIM_2 + 2; i++)
    {
        for(int j = 0; j < DIM_2 + 2; j++)
        {
            printf("%f ", output2[(i*(DIM_2 + 2) + j)]);
        }

        printf("\n");
    }
    */
    return 0;
}