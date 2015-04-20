#include <stdio.h>
#include <errno.h> 
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "generatePointSet.h"

#define MAX_CHAR 256
#define MAX 300
#define MIN 0

int main(int argc, char** argv) {

	char* p;
	int pointsToGenerate = 0;
    int i;

    FILE* fp;

    double x;
    double y;

  	if (argc != 3) {
    	printf("Usage: generatePointSet [Number of points to generate] [point set output file]\n");
    	return 1;
  	}

    errno = 0;
    long temp = strtol(argv[1], &p, 10); //assuming points < INT_MAX

    if (errno != 0 || *p != '\0') {
        printf("Cannot parse integer. Aborting.");
        return 1;
    }
    else {
        pointsToGenerate = (int) temp;
    }

    fp = fopen(argv[2], "w");
    if (fp == NULL) {
        printf("File cannot be created. Aborting.");
        return 1;
    }

    fprintf(fp, "%d\n", pointsToGenerate);
    srand(time(NULL));

    for (i = 0; i < pointsToGenerate; i++) {
        x = MIN + ((double) rand() / ((double)RAND_MAX/(MAX-MIN)));
        y = MIN + ((double) rand() / ((double)RAND_MAX/(MAX-MIN)));
        fprintf(fp, "%lf %lf\n", x, y);
    }

    return 0;
}
