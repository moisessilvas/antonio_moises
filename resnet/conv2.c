#include <math.h>
#include <stdlib.h>

#define N_FILTERS 64
#define STRIDE_POOL 2
#define STRIDE_CONV 1
#define WIDTH 112
#define HEIGHT 112
#define OUT_DIM 56

use namespace std;

static float filters[N_FILTERS*3*3];
static float feature_maps[(WIDTH+2)*(HEIGHT+2)];
static float image[(OUT_DIM+2)*(OUT_DIM+2)*N_FILTERS];
static float output[OUT_DIM*OUT_DIM*N_FILTERS];

void init_weights_3x3()
{
    for(int i = 0; i < N_FILTERS*3*3; i++)
    {
        //filters[i] = rand()/RAND_MAX;
        filters[i] = 1;
    }
}

void max_pool()
{
    float window[3*3];
    float max_value = 0;

    for(int layers = 0; layers < N_FILTERS; layers++)
    {    
        for(int i = 0; i < HEIGHT; i = i + STRIDE_POOL)
        {
            for(int j = 0; j < WIDTH; j = j + STRIDE_POOL)
            {
                window[0] = feature_maps[(i+0)*(WIDTH+2) + j+0];
                window[1] = feature_maps[(i+0)*(WIDTH+2) + j+1];
                window[2] = feature_maps[(i+0)*(WIDTH+2) + j+2];

                window[3] = feature_maps[(i+1)*(WIDTH+2) + j+0];
                window[4] = feature_maps[(i+1)*(WIDTH+2) + j+1];
                window[5] = feature_maps[(i+1)*(WIDTH+2) + j+2];

                window[6] = feature_maps[(i+2)*(WIDTH+2) + j+0];
                window[7] = feature_maps[(i+2)*(WIDTH+2) + j+1];
                window[8] = feature_maps[(i+2)*(WIDTH+2) + j+2];

                for(int k = 0; k < 3*3; k++)
                {
                    if(window[k] > max_value) max_value = window[k];
                }

                image[layers*(OUT_DIM+2)*(OUT_DIM+2) + (((i/2)+1)*(OUT_DIM+2) + ((j/2)+1))] = max_value;
            }
        }
    }
}

void conv2_layer()
{
    int filter, id = 0;
    float conv;
    unsigned char window[3*3];
    
    for(filter = 0; filter < N_FILTERS; filter++)
    {
        for(int i = 0; i < HEIGHT; i = i + STRIDE_CONV)
        {
            for(int j = 0; j < WIDTH; j = j + STRIDE_CONV)
            {
                window[0] = image[(i+0)*(WIDTH+2) + j+0];
                window[1] = image[(i+0)*(WIDTH+2) + j+1];
                window[2] = image[(i+0)*(WIDTH+2) + j+2];

                window[3] = image[(i+1)*(WIDTH+2) + j+0];
                window[4] = image[(i+1)*(WIDTH+2) + j+1];
                window[5] = image[(i+1)*(WIDTH+2) + j+2];

                window[6] = image[(i+2)*(WIDTH+2) + j+0];
                window[7] = image[(i+2)*(WIDTH+2) + j+1];
                window[8] = image[(i+2)*(WIDTH+2) + j+2];

                conv = 0;

                for(int k = 0; k < 3*3; k++)
                {
                    conv = conv + (window[k] * filters[k + 3*3*filter]);
                }

                output[filter*OUT_DIM*OUT_DIM + (i*OUT_DIM + j)] = conv;
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