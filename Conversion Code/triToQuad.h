#ifndef _COMMON_H_
#define _COMMON_H_

typedef struct {
    double x;
    double y;
} Vertex;

typedef struct {
    Vertex vertex[3];
} Triangle;

typedef struct {
    long vertex[3];
} OffTriangle;

typedef struct {
    Vertex vertex[4];
} Quad;

typedef struct {
    long vertex[4];
} OffQuad;

typedef struct {
    Vertex center;
    Vertex left;
    Vertex right;
} Midpoint;

typedef struct {
    long left;
    long right;
    long center; //id
} OffMidpoint;

int main();

long midpointInList(Midpoint query, Midpoint* list, long size);
long offMidpointInList(OffMidpoint query, OffMidpoint* list, long size);
int vertexCmp(Vertex a, Vertex b);


#endif
