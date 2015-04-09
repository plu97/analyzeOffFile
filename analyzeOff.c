#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "analyzeOff.h"

#define MAX_CHAR 256

int main(int argc, char **argv){
  if (argc != 2) {
    printf("Usage: analyzeOff [.off filename]\n");
    return 1;
  }
  
  int i;
  FILE* fp;
  char line[MAX_CHAR];
  int flag = 0;
  int vertexCount = 0;
  int faceCount = 0;
  int currentVertex = 0;
  int currentFace = 0;
  Vertex thisVertex;
  Vertex* vertexArray; //array to be malloc'd
  Vertex face[4];
  int faceIndex[4];
  CREAnalysis analysis;

  double worstAR = 1.0;
  double worstSkew = 0.0;
  double worstTaperX = 0.0;
  double worstTaperY = 0.0;
  
  double totalAR = 0.0;
  double totalSkew = 0.0;
  double totalSkewValue = 0.0;

  fp = fopen(argv[1], "r");
  
  if (fp == NULL) {
    printf("Can't open file: %s\n", argv[1]);
    exit(1);
  }
  
  while (fgets(line, MAX_CHAR, fp)) { //main loop
    switch(flag) {

    case 0: //ensure that this is a proper OFF file
      if (strcmp(line,"OFF\n") == 0) {
	flag = 1;
      }
      else {
	printf("File is not recognized as an .off file.\n");
        exit(1);
      }
      break;

    case 1: //read in OFF information
      if (sscanf(line, "%d %d %*d", &vertexCount, &faceCount) == 2) {
	vertexArray = (Vertex*) malloc(vertexCount*sizeof(Vertex));
	if (vertexArray == NULL) exit(1); //TODO error message
	flag = 2;
      }
      else {
	printf("Cannot read vertex/face count information from file.\n");
	exit(1);
      }
     
      break;


    case 2: //read and push vertex information
      if (currentVertex >= vertexCount) {
	flag = 3;
	//go to case 3 without reading new line
      }

      else if (sscanf(line, "%lf %lf %lf", &thisVertex.x, &thisVertex.y, &thisVertex.z) == 3) {
	vertexArray[currentVertex] = thisVertex;
	currentVertex++;
	break;
      }
      else {
	printf("Vertex %d cannot be read.\n", currentVertex);
	exit(1);
      }
      

    
    case 3: //analyze face
      if (currentFace >= faceCount) {
	flag = 4;
	break;
      }
      //TODO: generalize to non-quads
      sscanf(line, "%*d %d %d %d %d", &faceIndex[0] , &faceIndex[1], &faceIndex[2], &faceIndex[3]);
      for (i = 0; i < 4; i++) {
	face[i] = vertexArray[faceIndex[i]];
      }
      analysis = analyze(face);
      printf("%d\tAR: %lf\tSkew: %lf\tTaperX: %lf\tTaperY: %lf\n", currentFace, analysis.AR, analysis.skew, analysis.taperX, analysis.taperY);

      if (analysis.AR > worstAR) worstAR = analysis.AR;
      if (fabs(analysis.skew) > fabs(worstSkew)) worstSkew = analysis.skew;
      if (analysis.taperX > worstTaperX) worstTaperX = analysis.taperX;
      if (analysis.taperY > worstTaperY) worstTaperY = analysis.taperY;

      totalAR += analysis.AR;
      totalSkew += analysis.skew;
      totalSkewValue += fabs(analysis.skew);

      currentFace++;
      break;

    case 4:
      //nothing should run in here unless there's extra things in the .off file
      break;
    }
  }

  printf("Average AR:\t%lf\n", totalAR/faceCount);
  printf("Average skew:\t%lf\n", totalSkew/faceCount);
  printf("Average skew value:\t%lf\n", totalSkewValue/faceCount);
  printf("Worst AR:\t%lf\n", worstAR);
  printf("Worst Skew:\t%lf\n", worstSkew);
  printf("Worst TaperX:\t%lf\n", worstTaperX);
  printf("Worst TaperY:\t%lf\n", worstTaperY);

  if (fp == NULL);
  else fclose(fp);
  

  free(vertexArray);

  return 0;
}

CREAnalysis analyze(Vertex vertices[4]) {

  CREAnalysis result;
  Vertex centroid;
  Vertex midpoint[4];
  int j;
  Vertex v06, v07;
  double v06Length;
  Vertex localX, localY, localZ;
  Vertex v06xv07; //V06 cross with V07
  Vertex v0[4];
  Vertex localCoord[4];
  double e1, e2, e3, e4, f1, f2, f3, f4;

  double thisAR = 0.0;
  double worstAR = 1.0;
  

  centroid.x = 0.25 * (vertices[0].x + vertices[1].x + vertices[2].x + vertices[3].x);
  centroid.y = 0.25 * (vertices[0].y + vertices[1].y + vertices[2].y + vertices[3].y);

  for (j=0; j<4; j++) {
    midpoint[j].x = 0.5 * (vertices[j].x + vertices[(j+1)%4].x);
    midpoint[j].y = 0.5 * (vertices[j].y + vertices[(j+1)%4].y);
  }

  v06.x = midpoint[1].x - centroid.x;
  v06.y = midpoint[1].y - centroid.y;
  v06Length = sqrt(pow(v06.x, 2) + pow(v06.y, 2));

  localX.x = v06.x/v06Length;
  localX.y = v06.y/v06Length;

  v07.x = midpoint[2].x - centroid.x;
  v07.y = midpoint[2].y - centroid.y;

  v06xv07.x = 0;
  v06xv07.y = 0;
  v06xv07.z = (v06.x * v07.y) + (v06.y * v07.x);

  localZ.z = 1;

  localY.x = -localX.y;
  localY.y = localX.x;

  for (j=0; j<4; j++) {
    v0[j].x = vertices[j].x - centroid.x;
    v0[j].y = vertices[j].y - centroid.y;

    localCoord[j].x = (v0[j].x * localX.x) + (v0[j].y * localX.y);
    localCoord[j].y = (v0[j].x * localY.x) + (v0[j].y * localY.y);
  }

    e1 = 0.25 * (localCoord[0].x + localCoord[1].x + localCoord[2].x + localCoord[3].x);
    e2 = 0.25 * (-localCoord[0].x + localCoord[1].x + localCoord[2].x - localCoord[3].x);
    e3 = 0.25 * (-localCoord[0].x - localCoord[1].x + localCoord[2].x + localCoord[3].x);
    e4 = 0.25 * (localCoord[0].x - localCoord[1].x + localCoord[2].x - localCoord[3].x);

    f1 = 0.25 * (localCoord[0].y + localCoord[1].y + localCoord[2].y + localCoord[3].y);
    f2 = 0.25 * (-localCoord[0].y + localCoord[1].y + localCoord[2].y - localCoord[3].y);
    f3 = 0.25 * (-localCoord[0].y - localCoord[1].y + localCoord[2].y + localCoord[3].y);
    f4 = 0.25 * (localCoord[0].y - localCoord[1].y + localCoord[2].y - localCoord[3].y);

    for (j=0; j<4;j++) {
      printf("x%d:%lf y%d:%lf\n", j, localCoord[j].x, j, localCoord[j].y); 
    }

    //printf("e2:%lf\te3:%lf\tf2:%lf\tf3:%lf\n", e2, e3, f2, f3);
    //printf("e2/f3:%lf\tf3/e2:%lf\n", e2/f3, f3/e2);
    //printf("e3/f3:%lf\tf2/e2:%lf\n", e3/f3, f2/e2);

    if (e2/f3 > f3/e2) result.AR = e2/f3;
    else result.AR = f3/e2;

    result.skew = e3/f3;
    result.taperX = f4/f3;
    result.taperY = e4/e2;
    result.jacobianDet = pow(f3, 2) * result.AR;

    return result;
    //printf("holy shit this worked %lf", v06.x);
}
