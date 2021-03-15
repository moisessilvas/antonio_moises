#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
 
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
static float output3[(DIM_3+2)*(DIM_3+2)*N_FILTERS_128];
static float shortcut3[(DIM_3+2)*(DIM_3+2)*N_FILTERS_128];
static float output4[(DIM_4+2)*(DIM_4+2)*N_FILTERS_256];
static float shortcut4[(DIM_4+2)*(DIM_4+2)*N_FILTERS_256];
static float output5[(DIM_5+2)*(DIM_5+2)*N_FILTERS_512];
static float shortcut5[(DIM_5+2)*(DIM_5+2)*N_FILTERS_512];

float output2_temp[(DIM_2+2)*(DIM_2+2)*N_FILTERS_64];
float output2_temp2[(DIM_2+2)*(DIM_2+2)*N_FILTERS_64];
float output3_temp[(DIM_3+2)*(DIM_3+2)*N_FILTERS_128];
float output3_temp2[(DIM_3+2)*(DIM_3+2)*N_FILTERS_128];
float output4_temp[(DIM_4+2)*(DIM_4+2)*N_FILTERS_256];
float output4_temp2[(DIM_4+2)*(DIM_4+2)*N_FILTERS_256];
float output5_temp[(DIM_5+2)*(DIM_5+2)*N_FILTERS_512];
float output5_temp2[(DIM_5+2)*(DIM_5+2)*N_FILTERS_512];
float output_average[N_FILTERS_512];

static float weights_conv1[N_FILTERS_64*7*7];

static float weights_conv2_layer1a[N_FILTERS_64*3*3*N_FILTERS_64];
static float weights_conv2_layer2a[N_FILTERS_64*3*3*N_FILTERS_64];
static float weights_conv2_layer1b[N_FILTERS_64*3*3*N_FILTERS_64];
static float weights_conv2_layer2b[N_FILTERS_64*3*3*N_FILTERS_64];

static float weights_conv3_layer1a[N_FILTERS_128*3*3*N_FILTERS_64];
static float weights_conv3_layer2a[N_FILTERS_128*3*3*N_FILTERS_128];
static float weights_conv3_layer1b[N_FILTERS_128*3*3*N_FILTERS_128];
static float weights_conv3_layer2b[N_FILTERS_128*3*3*N_FILTERS_128];
static float weights_conv3_shortcut[N_FILTERS_128*N_FILTERS_64];

static float weights_conv4_layer1a[N_FILTERS_256*3*3*N_FILTERS_128];
static float weights_conv4_layer1b[N_FILTERS_256*3*3*N_FILTERS_256];
static float weights_conv4_layer2a[N_FILTERS_256*3*3*N_FILTERS_256];
static float weights_conv4_layer2b[N_FILTERS_256*3*3*N_FILTERS_256];
static float weights_conv4_shortcut[N_FILTERS_256*N_FILTERS_128];

static float weights_conv5_layer1a[N_FILTERS_512*3*3*N_FILTERS_256];
static float weights_conv5_layer1b[N_FILTERS_512*3*3*N_FILTERS_512];
static float weights_conv5_layer2a[N_FILTERS_512*3*3*N_FILTERS_512];
static float weights_conv5_layer2b[N_FILTERS_512*3*3*N_FILTERS_512];
static float weights_conv5_shortcut[N_FILTERS_512*N_FILTERS_256];

static float pooling_conv2[(DIM_2+2)*(DIM_2+2)*N_FILTERS_64];

/*
void relu(float data[])
{
    int data_size = sizeof(data)/sizeof(data[0]);

    for(int i = 0; i < data_size; i++)
    {
        if(data[i] < 0)
        {
            data[i] = 0;
        }
    }
}*/

void init_weights_conv1(FILE* weights)
{   
    char number[50];
    memset(number,0,sizeof number);
    int c, count = 0, weight_count = 0;
    float x;
    do {
        c = fgetc(weights);

        if( feof(weights) || c == '\n') break;
        else if(c == '*'){
            x = atof(number);
            if(weight_count < N_FILTERS_64*7*7){
                weights_conv1[weight_count] = x;
                weight_count++;
            }
            memset(number,0,sizeof number);
            count = 0;
        }
        else{
            number[count] = c;
            count++;
        }
    }while(1);
    for(int i = 0; i < N_FILTERS_64*7*7; i++)
    {
        // filters[i] = rand()/RAND_MAX;
        //weights_conv1[i] = 1.0f;
        printf("%f\n", weights_conv1[i]);
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
        weights_conv2_layer1a[i] = 1.0f;
        weights_conv2_layer2a[i] = 1.0f;
        weights_conv2_layer1b[i] = 1.0f;
        weights_conv2_layer2b[i] = 1.0f;
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

void conv2_layer(float input[], bool first_conv)
{
    int filter;
    float conv;
    float window_layer1[3*3*N_FILTERS_64];
    float window_layer2[3*3*N_FILTERS_64];
    
    //float output2_temp[(DIM_2+2)*(DIM_2+2)*N_FILTERS_64];
    //float output2_temp2[(DIM_2+2)*(DIM_2+2)*N_FILTERS_64];
    
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

                if(first_conv)
                {
                    for(int k = 0; k < 3*3*N_FILTERS_64; k++)
                    {
                        conv = conv + (window_layer1[k] * weights_conv2_layer1a[k*filter]);
                    }
                }
                else
                {
                    for(int k = 0; k < 3*3*N_FILTERS_64; k++)
                    {
                        conv = conv + (window_layer1[k] * weights_conv2_layer1b[k*filter]);
                    }
                }
                
                output2_temp[filter*(DIM_2+2)*(DIM_2+2) + (i+1)*(DIM_2+2) + (j+1)] = conv;
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
                
                if(first_conv)
                {
                    for(int k = 0; k < 3*3*N_FILTERS_64; k++)
                    {
                        conv = conv + (window_layer2[k] * weights_conv2_layer2a[k*filter]);
                    }
                }
                else
                {
                    for(int k = 0; k < 3*3*N_FILTERS_64; k++)
                    {
                        conv = conv + (window_layer2[k] * weights_conv2_layer2b[k*filter]);
                    }
                }

                output2_temp2[filter*(DIM_2+2)*(DIM_2+2) + (i+1)*(DIM_2+2) + (j+1)] = conv;
            }
        }
    }

    for(int n = 0; n < (DIM_2+2)*(DIM_2+2)*N_FILTERS_64; n++)
    {
        output2[n] = input[n] + output2_temp2[n];
    }
}

void init_weights_conv3()
{
    for(int i = 0; i < N_FILTERS_128*3*3*N_FILTERS_64; i++)
    {
        // filters[i] = rand()/RAND_MAX;
        weights_conv3_layer1a[i] = 1.0f;
    }

    for(int i = 0; i < N_FILTERS_128*3*3*N_FILTERS_128; i++)
    {
        weights_conv3_layer2a[i] = 1.0f;
        weights_conv3_layer1b[i] = 1.0f;
        weights_conv3_layer2b[i] = 1.0f;
    }

    for(int i = 0; i < N_FILTERS_128*N_FILTERS_64; i++)
    {
        weights_conv3_shortcut[i] =1.0f;
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
                    //output2_temp[i*(DIM_2+2) + j] = 0;
                    //output2_temp2[i*(DIM_2+2) + j] = 0;
                }
                else 
                {
                    output2[(i+DIM_2)*(DIM_2+2) + j] = 0;
                    //output2_temp[(i+DIM_2)*(DIM_2+2) + j] = 0;
                    //output2_temp2[(i+DIM_2)*(DIM_2+2) + j] = 0;
                }
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
                    //output2_temp[i*(DIM_2 + 2)] = 0;
                    //output2_temp2[i*(DIM_2 + 2)] = 0;
                }
                else
                {
                    output2[(i+1)*(DIM_2 + 2) - 1] = 0;
                    //output2_temp[(i+1)*(DIM_2 + 2) - 1] = 0;
                    //output2_temp2[(i+1)*(DIM_2 + 2) - 1] = 0;
                }
            }
        }
    }
}

void conv3_shortcut()
{
    float conv;
    float window[N_FILTERS_64];

    for(int filter = 0; filter < N_FILTERS_128; filter++)
    {
        for(int i = 0; i < DIM_2; i = i + 2)
        {
            for(int j = 0; j < DIM_2; j = j + 2)
            {
                for (int depth = 0; depth < N_FILTERS_64; depth++)
                {
                    window[depth] = output2[(i+1)*(DIM_2+2) + (j+1) + depth*(DIM_2+2)*(DIM_2+2)];
                }

                conv = 0.0f;

                for(int k = 0; k < N_FILTERS_64; k++)
                {
                    conv = conv + (window[k] * weights_conv3_shortcut[k + filter*N_FILTERS_64]);
                }

                shortcut3[filter*(DIM_3+2)*(DIM_3+2) + (i/2 + 1)*(DIM_3+2) + (j/2 + 1)] = conv;
            }
        }
    }
    
}

void conv3_layer(float input[], bool first_conv)
{
    int dim = DIM_3; 
    int filter_size = N_FILTERS_128;
    int stride = 1;
    
    if(first_conv)
    {
        dim = DIM_2;
        filter_size = N_FILTERS_64;
        stride = 2;
    } 

    int filter;
    float conv;
    float window_layer1[3*3*filter_size];
    float window_layer2[3*3*N_FILTERS_128];
    
    //float output3_temp[(DIM_3+2)*(DIM_3+2)*N_FILTERS_128];
    //float output3_temp2[(DIM_3+2)*(DIM_3+2)*N_FILTERS_128];
    
    for(filter = 0; filter < N_FILTERS_128; filter++)
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

                if(first_conv)
                {
                    for(int k = 0; k < 3*3*N_FILTERS_64; k++)
                    {
                        conv = conv + (window_layer1[k] * weights_conv3_layer1a[k + 3*3*filter]);
                    }
                }
                else
                {
                    for(int k = 0; k < 3*3*N_FILTERS_128; k++)
                    {
                        conv = conv + (window_layer1[k] * weights_conv3_layer1b[k + 3*3*N_FILTERS_128*filter]);
                    }
                }
                
                output3_temp[filter*(DIM_3+2)*(DIM_3+2) + (i+1)*(DIM_3+2) + (j+1)] = conv;
            }
        }
    }

    for(filter = 0; filter < N_FILTERS_128; filter++)
    {
        for(int i = 0; i < DIM_3; i = i + 1)
        {
            for(int j = 0; j < DIM_3; j = j + 1)
            {
                for (int depth = 0; depth < N_FILTERS_64; depth++)
                {
                    window_layer2[0 + depth*9] = output3_temp[(i+0)*(DIM_3+2) + j+0 + depth*(DIM_3+2)*(DIM_3+2)];
                    window_layer2[1 + depth*9] = output3_temp[(i+0)*(DIM_3+2) + j+1 + depth*(DIM_3+2)*(DIM_3+2)];
                    window_layer2[2 + depth*9] = output3_temp[(i+0)*(DIM_3+2) + j+2 + depth*(DIM_3+2)*(DIM_3+2)];

                    window_layer2[3 + depth*9] = output3_temp[(i+1)*(DIM_3+2) + j+0 + depth*(DIM_3+2)*(DIM_3+2)];
                    window_layer2[4 + depth*9] = output3_temp[(i+1)*(DIM_3+2) + j+1 + depth*(DIM_3+2)*(DIM_3+2)];
                    window_layer2[5 + depth*9] = output3_temp[(i+1)*(DIM_3+2) + j+2 + depth*(DIM_3+2)*(DIM_3+2)];

                    window_layer2[6 + depth*9] = output3_temp[(i+2)*(DIM_3+2) + j+0 + depth*(DIM_3+2)*(DIM_3+2)]; 
                    window_layer2[7 + depth*9] = output3_temp[(i+2)*(DIM_3+2) + j+1 + depth*(DIM_3+2)*(DIM_3+2)];
                    window_layer2[8 + depth*9] = output3_temp[(i+2)*(DIM_3+2) + j+2 + depth*(DIM_3+2)*(DIM_3+2)];
                }

                conv = 0.0f;

                if(first_conv)
                {
                    for(int k = 0; k < 3*3*N_FILTERS_128; k++)
                    {
                        conv = conv + (window_layer2[k] * weights_conv3_layer2a[k + 3*3*N_FILTERS_128*filter]);
                    }
                }
                else
                {
                    for(int k = 0; k < 3*3*N_FILTERS_128; k++)
                    {
                        conv = conv + (window_layer2[k] * weights_conv3_layer2b[k + 3*3*N_FILTERS_128*filter]);
                    }
                }
                
                output3_temp2[filter*(DIM_3+2)*(DIM_3+2) + (i+1)*(DIM_3+2) + (j+1)] = conv;
            }
        }
    }

    if(first_conv)
    {
        for(int n = 0; n < (DIM_3+2)*(DIM_3+2)*N_FILTERS_128; n++)
        {
            output3[n] = shortcut3[n] + output3_temp2[n];
        }
    }
    else
    {
        for(int n = 0; n < (DIM_3+2)*(DIM_3+2)*N_FILTERS_128; n++)
        {
            output3[n] = input[n] + output3_temp2[n];
        }
    }
    
}

void init_weights_conv4()
{
    for(int i = 0; i < N_FILTERS_256*3*3*N_FILTERS_128; i++)
    {
        // filters[i] = rand()/RAND_MAX;
        weights_conv4_layer1a[i] = 1.0f;
    }

    for(int i = 0; i < N_FILTERS_256*3*3*N_FILTERS_256; i++)
    {
        weights_conv5_layer2a[i] = 1.0f;
        weights_conv5_layer1b[i] = 1.0f;
        weights_conv5_layer2b[i] = 1.0f;
    }
}

void zero_padding_conv4(float input[])
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
                    input[i*(DIM_3+2) + j] = 0;
                }
                else input[(i+DIM_3)*(DIM_3+2) + j] = 0;
            }
        }

        //Popula as colunas nas extremidades
        for(int j = 0; j < 2; j++)
        {
            for(int i = 0; i < DIM_3 + 2; i++)
            {
                if(j < 1) 
                {
                    input[i*(DIM_3 + 2)] = 0;
                }
                else input[(i+1)*(DIM_3 + 2) - 1] = 0;
            }
        }
    }
}

void conv4_shortcut()
{
    float conv;
    float window[N_FILTERS_128];

    for(int filter = 0; filter < N_FILTERS_256; filter++)
    {
        for(int i = 0; i < DIM_3; i = i + 2)
        {
            for(int j = 0; j < DIM_3; j = j + 2)
            {
                for (int depth = 0; depth < N_FILTERS_128; depth++)
                {
                    window[depth] = output3[i*(DIM_3+2) + j + depth*(DIM_3+2)*(DIM_3+2)];
                }

                conv = 0.0f;

                for(int k = 0; k < N_FILTERS_128; k++)
                {
                    conv = conv + (window[k] * weights_conv4_shortcut[k + filter*N_FILTERS_128]);
                }

                shortcut4[filter*(DIM_4+2)*(DIM_4+2) + (i/2 + 1)*(DIM_4+2) + (j/2 + 1)] = conv;
            }
        }
    }

}

void conv4_layer(float input[], bool first_conv)
{
    int dim = DIM_4; 
    int filter_size = N_FILTERS_256;
    int stride = 1;

    if(first_conv)
    {
        dim = DIM_3;
        filter_size = N_FILTERS_128;
        stride = 2;
    } 
    
    int filter;
    float conv;
    float window_layer1[3*3*filter_size];
    float window_layer2[3*3*N_FILTERS_256];
    
    //float output4_temp[(DIM_4+2)*(DIM_4+2)*N_FILTERS_256];
    //float output4_temp2[(DIM_4+2)*(DIM_4+2)*N_FILTERS_256];
    
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

                if(first_conv)
                {
                    for(int k = 0; k < 3*3*N_FILTERS_128; k++)
                    {
                        conv = conv + (window_layer1[k] * weights_conv4_layer1a[k + 3*3*N_FILTERS_128*filter]);
                    }
                }
                else
                {
                    for(int k = 0; k < 3*3*N_FILTERS_256; k++)
                    {
                        conv = conv + (window_layer1[k] * weights_conv4_layer1b[k + 3*3*N_FILTERS_256*filter]);
                    }
                }
                
                output4_temp[filter*(DIM_4+2)*(DIM_4+2) + (i+1)*(DIM_4+2) + (j+1)] = conv;
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
                    window_layer2[0 + depth*9] = output4_temp[(i+0)*(DIM_4+2) + j+0 + depth*(DIM_4+2)*(DIM_4+2)];
                    window_layer2[1 + depth*9] = output4_temp[(i+0)*(DIM_4+2) + j+1 + depth*(DIM_4+2)*(DIM_4+2)];
                    window_layer2[2 + depth*9] = output4_temp[(i+0)*(DIM_4+2) + j+2 + depth*(DIM_4+2)*(DIM_4+2)];

                    window_layer2[3 + depth*9] = output4_temp[(i+1)*(DIM_4+2) + j+0 + depth*(DIM_4+2)*(DIM_4+2)];
                    window_layer2[4 + depth*9] = output4_temp[(i+1)*(DIM_4+2) + j+1 + depth*(DIM_4+2)*(DIM_4+2)];
                    window_layer2[5 + depth*9] = output4_temp[(i+1)*(DIM_4+2) + j+2 + depth*(DIM_4+2)*(DIM_4+2)];

                    window_layer2[6 + depth*9] = output4_temp[(i+2)*(DIM_4+2) + j+0 + depth*(DIM_4+2)*(DIM_4+2)]; 
                    window_layer2[7 + depth*9] = output4_temp[(i+2)*(DIM_4+2) + j+1 + depth*(DIM_4+2)*(DIM_4+2)];
                    window_layer2[8 + depth*9] = output4_temp[(i+2)*(DIM_4+2) + j+2 + depth*(DIM_4+2)*(DIM_4+2)];
                }

                conv = 0.0f;

                if(first_conv)
                {
                    for(int k = 0; k < 3*3*N_FILTERS_256; k++)
                    {
                        conv = conv + (window_layer2[k] * weights_conv3_layer2a[k + 3*3*N_FILTERS_256*filter]);
                    }
                }
                else
                {
                    for(int k = 0; k < 3*3*N_FILTERS_256; k++)
                    {
                        conv = conv + (window_layer2[k] * weights_conv3_layer2b[k + 3*3*N_FILTERS_256*filter]);
                    }
                }
                
                output4_temp2[filter*(DIM_4+2)*(DIM_4+2) + (i+1)*(DIM_4+2) + (j+1)] = conv;
            }
        }
    }

    for(int n = 0; n < (DIM_4+2)*(DIM_4+2)*N_FILTERS_256; n++)
    {
        output4[n] = input[n] + output4_temp2[n];
    }
}

void init_weights_conv5()
{
    for(int i = 0; i < N_FILTERS_512*3*3*N_FILTERS_256; i++)
    {
        // filters[i] = rand()/RAND_MAX;
        weights_conv5_layer1a[i] = 1.0f;
    }

    for(int i = 0; i < N_FILTERS_512*3*3*N_FILTERS_512; i++)
    {
        weights_conv5_layer2a[i] = 1.0f;
        weights_conv5_layer1b[i] = 1.0f;
        weights_conv5_layer2b[i] = 1.0f;
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

void conv5_shortcut()
{
    float conv;
    float window[N_FILTERS_256];

    for(int filter = 0; filter < N_FILTERS_512; filter++)
    {
        for(int i = 0; i < DIM_4; i = i + 2)
        {
            for(int j = 0; j < DIM_4; j = j + 2)
            {
                for (int depth = 0; depth < N_FILTERS_256; depth++)
                {
                    window[depth] = output2[i*(DIM_4+2) + j + depth*(DIM_4+2)*(DIM_4+2)];
                }

                conv = 0.0f;

                for(int k = 0; k < N_FILTERS_256; k++)
                {
                    conv = conv + (window[k] * weights_conv5_shortcut[k + filter*N_FILTERS_64]);
                }

                shortcut5[filter*(DIM_5+2)*(DIM_5+2) + (i/2 + 1)*(DIM_5+2) + (j/2 + 1)] = conv;
            }
        }
    }
}

void conv5_layer(float input[], bool first_conv)
{
    int dim = DIM_5; 
    int filter_size = N_FILTERS_512;
    int stride = 1;

    if(first_conv)
    {
        dim = DIM_4;
        filter_size = N_FILTERS_256;
        stride = 2;
    } 
    
    int filter;
    float conv;
    float window_layer1[3*3*filter_size];
    float window_layer2[3*3*N_FILTERS_512];
    
    //float output5_temp[(DIM_5+2)*(DIM_5+2)*N_FILTERS_512];
    //float output5_temp2[(DIM_5+2)*(DIM_5+2)*N_FILTERS_512];
    
    for(filter = 0; filter < N_FILTERS_512; filter++)
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

                if(first_conv)
                {
                    for(int k = 0; k < 3*3*N_FILTERS_256; k++)
                    {
                        conv = conv + (window_layer1[k] * weights_conv5_layer1a[k*filter]);
                    }
                }
                else
                {
                    for(int k = 0; k < 3*3*N_FILTERS_512; k++)
                    {
                        conv = conv + (window_layer1[k] * weights_conv5_layer1b[k*filter]);
                    }
                }
                
                output5_temp[filter*(DIM_5+2)*(DIM_5+2) + (i+1)*(DIM_5+2) + (j+1)] = conv;
            }
        }
    }

    for(filter = 0; filter < N_FILTERS_512; filter++)
    {
        for(int i = 0; i < DIM_5; i = i + 1)
        {
            for(int j = 0; j < DIM_5; j = j + 1)
            {
                for (int depth = 0; depth < N_FILTERS_512; depth++)
                {
                    window_layer2[0 + depth*9] = output5_temp[(i+0)*(DIM_5+2) + j+0 + depth*(DIM_5+2)*(DIM_5+2)];
                    window_layer2[1 + depth*9] = output5_temp[(i+0)*(DIM_5+2) + j+1 + depth*(DIM_5+2)*(DIM_5+2)];
                    window_layer2[2 + depth*9] = output5_temp[(i+0)*(DIM_5+2) + j+2 + depth*(DIM_5+2)*(DIM_5+2)];

                    window_layer2[3 + depth*9] = output5_temp[(i+1)*(DIM_5+2) + j+0 + depth*(DIM_5+2)*(DIM_5+2)];
                    window_layer2[4 + depth*9] = output5_temp[(i+1)*(DIM_5+2) + j+1 + depth*(DIM_5+2)*(DIM_5+2)];
                    window_layer2[5 + depth*9] = output5_temp[(i+1)*(DIM_5+2) + j+2 + depth*(DIM_5+2)*(DIM_5+2)];

                    window_layer2[6 + depth*9] = output5_temp[(i+2)*(DIM_5+2) + j+0 + depth*(DIM_5+2)*(DIM_5+2)]; 
                    window_layer2[7 + depth*9] = output5_temp[(i+2)*(DIM_5+2) + j+1 + depth*(DIM_5+2)*(DIM_5+2)];
                    window_layer2[8 + depth*9] = output5_temp[(i+2)*(DIM_5+2) + j+2 + depth*(DIM_5+2)*(DIM_5+2)];
                }

                conv = 0.0f;

                if(first_conv)
                {
                    for(int k = 0; k < 3*3*N_FILTERS_512; k++)
                    {
                        conv = conv + (window_layer2[k] * weights_conv4_layer2a[k*filter]);
                    }
                }
                else
                {
                    for(int k = 0; k < 3*3*N_FILTERS_512; k++)
                    {
                        conv = conv + (window_layer2[k] * weights_conv4_layer2b[k*filter]);
                    }
                }
                
                output5_temp2[filter*(DIM_5+2)*(DIM_5+2) + (i+1)*(DIM_5+2) + (j+1)] = conv;
            }
        }
    }

    for(int n = 0; n < (DIM_5+2)*(DIM_5+2)*N_FILTERS_512; n++)
    {
        output5[n] = input[n] + output5_temp2[n];
    }
}

void average_layer()
{
    float nums[DIM_5*DIM_5];
    float result;

    for(int filter = 0; filter < N_FILTERS_512; filter++)
    {
        for(int i = 0; i < 7; i++)
        {
            for(int j = 0; j < 7; j++)
            {
                nums[(i+1)*(DIM_5+2) + (j+1)] = output5[(i+1)*(DIM_5+2) + (j+1) + filter*(DIM_5+2)*(DIM_5+2)];
            }
        }

        result = 0;

        for(int k = 0; k < DIM_5*DIM_5; k++)
        {
            result = result + (nums[k]/DIM_5*DIM_5);
        }

        output_average[filter] = result;
    }
}

int main(int argc, const char * argv[])
{
    FILE *picture, *weights;
    float dense = 0.0f;
    
    picture = fopen("/home/aluno/TCC/antonio_moises-master/Grayscale.pnm", "rb");
    weights = fopen("weights.txt", "r");

    init_weights_conv1(weights);
    init_weights_conv2();
    //init_weights_conv3();
    //init_weights_conv4();
    //init_weights_conv5();

    zero_padding_conv1(picture);
    conv1_layer();
    zero_padding_pool_conv2();
    max_pooling_conv2();
    zero_padding_conv2();
    conv2_layer(pooling_conv2, 1);
    zero_padding_conv3();
    //conv2_layer(output2, 0);
    
    //zero_padding_conv4(shortcut3);
    //conv3_shortcut();
    //zero_padding_conv3();
    //conv3_layer(output2, 1);
    //zero_padding_conv4(output3);
    //conv3_layer(output3, 0);

    //zero_padding_conv4(output3);
    //conv4_layer();
    //zero_padding_conv5();
    //conv5_layer();

    for(int id = 0; id < (DIM_2+2)*(DIM_2+2)*N_FILTERS_64; id++)
    {
        dense = dense + output2[id];
        //printf("%f ", output2[id]);
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