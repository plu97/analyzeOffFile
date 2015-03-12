#ifndef _COMMON_H_ 
#define _COMMON_H_ 

typedef struct {
  double x;
  double y;
  double z;
} Vertex;

typedef struct {
  Vertex vertices[4];
} Quad;

typedef struct {
  double AR;
  double skew;
  double taperX;
  double taperY;
  double jacobianDet;
} CREAnalysis;

int main();
CREAnalysis analyze(Vertex vertices[4]);

#endif
