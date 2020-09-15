#include <math.h>
#include <stdlib.h>

#define N_FILTERS 512
#define STRIDE 1
#define WIDHT 9
#define HEIGTH 9
#define FILTER_DIM 9

use namespace std;

static float filters[N_FILTERS*3*3*512];

void init_weights_3x3()
{
    for(int i = 0; i < N_FILTERS*FILTER_DIM*512; i++)
    {
        filters[i] = rand()/RAND_MAX;
    }
}

void intermediate_conv_layer(float feature_maps[], float output[])
{
    float window[FILTER_DIM*512];
    float conv;

    for(int filter = 0; filter < N_FILTERS; filter++)
    {
        for(int i = 0; i =< HEIGTH - 2; i = i + STRIDE)
        {
            for (int j = 0; j =< WIDHT - 2; j = j + STRIDE)
            {
                for (int depth = 0; depth < 512; depth++)
                {
                    window[depth*FILTER_DIM + 0] = feature_maps[depth*FILTER_DIM + (i+0)*WIDHT + j+0];
                    window[depth*FILTER_DIM + 1] = feature_maps[depth*FILTER_DIM + (i+0)*WIDHT + j+1];
                    window[depth*FILTER_DIM + 2] = feature_maps[depth*FILTER_DIM + (i+0)*WIDHT + j+2];

                    window[depth*FILTER_DIM + 3] = feature_maps[depth*FILTER_DIM + (i+1)*WIDHT + j+0];
                    window[depth*FILTER_DIM + 4] = feature_maps[depth*FILTER_DIM + (i+1)*WIDHT + j+1];
                    window[depth*FILTER_DIM + 5] = feature_maps[depth*FILTER_DIM + (i+1)*WIDHT + j+2];

                    window[depth*FILTER_DIM + 6] = feature_maps[depth*FILTER_DIM + (i+2)*WIDHT + j+0];
                    window[depth*FILTER_DIM + 7] = feature_maps[depth*FILTER_DIM + (i+2)*WIDHT + j+1];
                    window[depth*FILTER_DIM + 8] = feature_maps[depth*FILTER_DIM + (i+2)*WIDHT + j+2];
                }

                conv = 0;

                for (int k = 0; k < FILTER_DIM*512; k++)
                {
                    conv = conv + window[k] * filters[k + FILTER_DIM*512*filter];
                }
                
                output[filter*(WIDHT - 2)*(HEIGTH - 2) + i*(WIDHT - 2) + j] = conv;
            }
            
        }
    }
}