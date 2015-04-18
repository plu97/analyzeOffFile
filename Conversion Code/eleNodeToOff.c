#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_CHAR 256

//See Shewchuk's Triangle
//INPUT: .ELE file and .NODE file
//OUTPUT: corresponding .OFF file
int main(int argc, char** argv) {

    FILE* eleFile;
    FILE* nodeFile;
    FILE* outputFile;
    int flag = 0;
    int current = 0;
    long totalNodes;
    long totalTriangles;
    double nodeX;
    double nodeY;
    char line[MAX_CHAR];
    char line2[MAX_CHAR];

    long vertex[3];

    if (argc != 4) {
        printf("Usage: eleNodeToOff [.ELE file] [.NODE file] [.OFF output file]\n");
        return 1;
    } 

    eleFile = fopen(argv[1], "r");
    nodeFile = fopen(argv[2], "r");
    outputFile = fopen(argv[3], "w");

    if (eleFile == NULL || nodeFile == NULL) {
        printf("File not found.\n");
        exit(1);
    }

    fgets(line, MAX_CHAR, nodeFile); //read first line of .NODE file
    fgets(line2, MAX_CHAR, eleFile); //read first line of .ELE file
    if (sscanf(line, "%ld 2 %*d %*d", &totalNodes) == 1 && sscanf(line2, "%ld 3 %*d", &totalTriangles)) {
        fprintf(outputFile, "OFF\n");
        fprintf(outputFile, "%ld %ld 0\n", totalNodes, totalTriangles);
    }
    else {
        printf("Cannot parse first line of either .ELE or NODE file.\n");    
    }

    while (fgets(line, MAX_CHAR, nodeFile)) { //read through rest of .NODE file
        if (line[0] == '#') {
            continue;
        }        
        else if (sscanf(line, "%*d %lf %lf %*d", &nodeX, &nodeY) == 2) {
            fprintf(outputFile, "%lf %lf 0.0\n", nodeX, nodeY);
        }
    }

    while (fgets(line, MAX_CHAR, eleFile)) {
        if (line[0] == '#') {
            continue;
        }
        else if (sscanf(line, "%*d %ld %ld %ld", &vertex[0], &vertex[1], &vertex[2])) {
            fprintf(outputFile,"3 %ld %ld %ld\n", vertex[0], vertex[1], vertex[2]);
        }
    }

    close(eleFile);
    close(nodeFile);
    close(outputFile);

    return 0;
}
