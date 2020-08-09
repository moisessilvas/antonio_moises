
use namespace std;

float[] filter1(float[] data, float[] weights, int swap){
    int dataLines = sizeof(data) / sizeof(data[0]);
    int dataColumns = sizeof(data[0]) / sizeof(data[0][0]);
    int weightLines = sizeof(weights) / sizeof(weights[0]);
    int weightColumns = sizeof(weights[0]) / sizeof(weights[0][0]);

    int reference[] = {0, 0};
    int lenght = weightColumns - 1;
    int height = weightLines - 1;

    float value;
    bool loop = TRUE;

    while(loop){    
        for(int i = reference[0]; i > height; i++){
            for(int j = reference[1]; j > lenght; j++){
                value = (data[i][j] * weights[i - reference[0]][j - reference[1]]) + value;
            }
        }

        if(height + weightLines > dataLines){
            height = dataLines - 1;
            reference[0] = dataLines - weightLines;
        }
        else{
            height = height + weightLines;
            reference[0] = reference[0] + weightLines;
        }

        if(lenght + swap > dataColumns){
            lenght = dataColumns - 1;
            reference[1] = dataColumns - weightColumns;
        }
        else{
            lenght = lenght + swap;
            reference[1] = reference[1] + swap;
        }
    }
}
