#include <math.h>
#include <stdlib.h>

#define N_FILTERS 512
#define STRIDE 2
#define WIDTH 7
#define HEIGHT 7

use namespace std;

static float filters[N_FILTERS*3*3];

void init_weights_3x3()
{
    for(int i = 0; i < N_FILTERS*3*3; i++)
    {
        filters[i] = rand()/RAND_MAX;
    }
}

void max_pool(float feature_maps[], float output[])
{
    float window[3*3];
    float max_value = 0;

    for(int layers = 0; layers < N_FILTERS; layers++)
    {    
        for(int i = 0; i < HEIGHT - 2; i = i + STRIDE)
        {
            for(int j = 0; j < WIDTH - 2; j = j + STRIDE)
            {
                window[0] = feature_maps[(i+0)*WIDTH + j+0];
                window[1] = feature_maps[(i+0)*WIDTH + j+1];
                window[2] = feature_maps[(i+0)*WIDTH + j+2];

                window[3] = feature_maps[(i+1)*WIDTH + j+0];
                window[4] = feature_maps[(i+1)*WIDTH + j+1];
                window[5] = feature_maps[(i+1)*WIDTH + j+2];

                window[6] = feature_maps[(i+2)*WIDTH + j+0];
                window[7] = feature_maps[(i+2)*WIDTH + j+1];
                window[8] = feature_maps[(i+2)*WIDTH + j+2];

                for(int k = 0; k < 3*3; k++)
                {
                    if(window[k] > max_value) max_value = window[k];
                }

                output[layers*(WIDTH-2)*(HEIGHT-2) + (i*(WIDTH-2) + j)] = max_value;
            }
        }
    }
}
