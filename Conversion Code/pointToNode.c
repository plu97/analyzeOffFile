#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_CHAR 256

int main(int argc, char** argv) {

    FILE* fp;
    FILE* output;
    int flag = 0;
    int current = 0;
    int totalNodes;
    double nodeX;
    double nodeY;
    char line[MAX_CHAR];

    if (argc != 3) {
        printf("Usage: pointToNode [point set text filename] [.node output filename]\n");
        return 1;
    } 

    fp = fopen(argv[1], "r");
    output = fopen(argv[2], "w");

    if (fp == NULL) {
        printf("Can't open file: %s\n", argv[1]);
        exit(1);
    }

    while (fgets(line, MAX_CHAR, fp)) { //main loop
        switch(flag) {
            case 0:
                if (sscanf(line, "%d", &totalNodes) == 1) {
                    fprintf(output, "%d 2 0 0\n", totalNodes);
                    flag = 1;
                }

                else {
                    printf("Cannot scan in number of points.\n");
                    exit(1);
                }

                break;

            case 1:
                if (current >= totalNodes) { flag = 2; }
                if (sscanf(line, "%lf %lf", &nodeX, &nodeY) == 2) {
                    fprintf(output, "%d %lf %lf\n", current, nodeX, nodeY);
                    current++;
                }
                else {
                    printf("Error parsing in node %d\n", current);
                    exit(1);
                }

                break;

            case 2:
                break;

            default:
                break;
        }
    }

    fclose(fp);
    fclose(output);


    return 0;
}
