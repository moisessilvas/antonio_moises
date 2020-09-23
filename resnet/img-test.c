#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char * argv[])
{
    FILE *picture;
    int c;
    int n = 0;
    int count = 0;
    int image[224*224];

    picture = fopen("/home/antonio/Imagens/gray/g/Grayscale.pnm", "rb");
    if (picture != NULL)
    {

        do {
            c = fgetc(picture);
            
            if( feof(picture) ) break;
            else if(c == '\n' && n < 3) n++;
            else if(n >= 3)
            {
                image[count] = c;
                count++;
            }
        } while(1);
    }
    return 0;
}