#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "triToQuad.h"

#define MAX_CHAR 256
#define DEBUG 0
#define EPSILON 0.000001


//assumption: input is properly triangulated mesh in .OFF file
int main(int argc, char **argv){

    int i;
    int j;
    char line[MAX_CHAR];

    FILE* triFile;
    FILE* quadFile;

    long maxTriangles;
    long maxVertices;

    long vertexCount;
    long faceCount;

    Triangle* triangles; //list
    OffTriangle* offTriangles; //list
    Quad* quads; //list
    OffQuad* offQuads; //list
    Vertex* vertex; //list
    Vertex* centroids; //list

    OffQuad thisOffQuad;

    Vertex newVertex;
    Vertex centroid;
    Triangle newTriangle;
    OffTriangle offTriangle;
    long triVertex[3];
    Midpoint* midpoints; //list
    OffMidpoint* offMidpoints; //list
    OffMidpoint thisOffMidpoint;
    long centroidPointer; //doubles as size
    long midpointPointer; //doubles as size
    long quadPointer; //doubles as size
    Midpoint triMidpoint[3];
    OffMidpoint offTriMidpoint[3];

    long vertexNum[4];

    midpointPointer = 0;
    centroidPointer = 0;
    quadPointer = 0;

    if (argc != 3) {
        printf("Usage: triToQuad [.OFF input file] [.OFF output file]\n");
        return 1;
    }

    triFile = fopen(argv[1], "r");
    quadFile = fopen(argv[2], "w");
  
    if (triFile == NULL) {
        printf("Can't open file: %s\n", argv[1]);
        exit(1);
    }

    fgets(line, MAX_CHAR, triFile); //gets first line
    if (strcmp(line,"OFF\n") != 0) {
        printf("File is not an .OFF file");
    }

    fprintf(quadFile, "OFF\n");

    fgets(line, MAX_CHAR, triFile); //gets second line

    //need to clean this thing up
    if (sscanf(line, "%ld %ld %*d", &vertexCount, &faceCount) == 2) {
        triangles = (Triangle*) malloc(sizeof(Triangle) * faceCount);
        vertex = (Vertex*) malloc(sizeof(Vertex) * vertexCount);
        midpoints = (Midpoint*) malloc(sizeof(Midpoint) * (1 + (2 * faceCount)));
        offMidpoints = (OffMidpoint*) malloc(sizeof(OffMidpoint) * (1 + (2 * faceCount))); //max

        quads = (Quad*) malloc(sizeof(Quad) * 3 * faceCount);
        offQuads = (OffQuad*) malloc(sizeof(OffQuad) * 3 * faceCount);
        centroids = (Vertex*) malloc(sizeof(Vertex) * faceCount);
        offTriangles = (OffTriangle*) malloc(sizeof(OffTriangle) * faceCount);

    }
    else {
        printf("Cannot parse face/vertex information.\n");
    }

    for (i = 0; i < vertexCount; i++) { //construct vertex list
        fgets(line, MAX_CHAR, triFile);
        if (sscanf(line, "%lf %lf %*f", &newVertex.x, &newVertex.y)) {
            vertex[i] = newVertex;
        }
        else {
            printf("Not enough vertices?");
        }
    }

    for (i = 0; i < faceCount; i++) { //construct triangle list
        fgets(line, MAX_CHAR, triFile);
        if (sscanf(line, "3 %ld %ld %ld", &triVertex[0], &triVertex[1], &triVertex[2])) {
            for (j = 0; j < 3; j++) {
                newTriangle.vertex[j] = vertex[triVertex[j]];
                offTriangle.vertex[j] = triVertex[j];
            }
        }
        triangles[i] = newTriangle;
        offTriangles[i] = offTriangle;
    }

    //for each triangle
    //  check if midpoint is in midpoints[] already
    //  if so:  use that midpoint to construct quad
    //  else:   find new midpoint, push to midpoints[], construct quad
    for (i = 0; i < faceCount; i++) { 

        for (j=0; j<3; j++) { //calculate the midpoints of the triangle
            offTriMidpoint[j].left = offTriangles[i].vertex[j];
            offTriMidpoint[j].right = offTriangles[i].vertex[(j+1)%3];

            if (offMidpointInList(offTriMidpoint[j], offMidpoints, midpointPointer) != -1) { //midpoint exists in list!
                long pos = offMidpointInList(offTriMidpoint[j], offMidpoints, midpointPointer);
                offTriMidpoint[j].center = pos;
            }
            else { //midpoint doesn't exist in list!
                offTriMidpoint[j].center = midpointPointer;
                offMidpoints[midpointPointer] = offTriMidpoint[j];
                midpointPointer++;
            }
        }

        centroid.x = 0;
        centroid.y = 0;
        for (j = 0; j<3; j++) { //calculating centroid
            centroid.x += triangles[i].vertex[j].x;
            centroid.y += triangles[i].vertex[j].y;        
        }   
    
        centroid.x /= 3; 
        centroid.y /= 3;

        centroids[centroidPointer] = centroid;



        for (j=0; j<3; j++) { //constructing quads
            //quad should be: Vertex, Midpoint, Centroid, Midpoint;
            //note that points are ordered from triFile vertices, to constructed centroids, to midpoints
            thisOffQuad.vertex[0] = offTriangles[i].vertex[j]; 
            thisOffQuad.vertex[1] = vertexCount + faceCount + offTriMidpoint[j].center;
            thisOffQuad.vertex[2] = vertexCount + centroidPointer;
            thisOffQuad.vertex[3] = vertexCount + faceCount + offTriMidpoint[(j+2)%3].center; //prior midpt
            offQuads[quadPointer] = thisOffQuad;
            quadPointer++;
        }   

        centroidPointer++;


        /*
        for (j = 0; j < 3; j++) {
            newVertex.x = (triangles[i].vertex[j].x + triangles[i].vertex[(j+1)%3].x) / 2;
            newVertex.y = (triangles[i].vertex[j].y + triangles[i].vertex[(j+1)%3].y) / 2;
            
            triMidpoints[j].center = newVertex;

            triMidpoints[j].left = triangles[i].vertex[j];
            triMidpoints[j].right = triangles[i].vertex[(j+1)%3];

            if (midpointInList(triMidpoints[j], midpoints, midpointPointer) == -1) {
                midpoints[midpointPointer] = triMidpoints[j];
                triVertex[j] = vertexCount + faceCount + midpointPointer;

                midpointPointer++;
            }
            else {
                triVertex[j] = midpointInList(triMidpoints[j], midpoints, midpointPointer);
            }
        }


        thisOffQuad.vertex[1] = triVertex[1];
        */
    }
    
    fprintf(quadFile, "%ld %ld 0\n", vertexCount + centroidPointer + midpointPointer, quadPointer); //second line of quadFile

    //writing vertices
    for (i=0; i<vertexCount; i++) { //old points from triFile
        fprintf(quadFile, "%lf %lf 0.0\n", vertex[i].x, vertex[i].y);
    }
    for (i=0; i<centroidPointer; i++) { //centroids
        fprintf(quadFile, "%lf %lf 0.0\n", centroids[i].x, centroids[i].y);
    }
    for (i=0; i<midpointPointer; i++) { //midpoints //TODO: check
        double x = (vertex[offMidpoints[i].left].x + vertex[offMidpoints[i].right].x) / 2;
        double y = (vertex[offMidpoints[i].left].y + vertex[offMidpoints[i].right].y) / 2; 

        fprintf(quadFile, "%lf %lf 0.0\n", x, y);
    }
    //writing quads
    for (i=0; i<quadPointer; i++) {
        fprintf(quadFile, "4 %ld %ld %ld %ld\n", 
            offQuads[i].vertex[0],
            offQuads[i].vertex[1],
            offQuads[i].vertex[2],
            offQuads[i].vertex[3]);

    }



    return 0;
}

long midpointInList(Midpoint query, Midpoint* list, long size){
    long i;

    for (i=0; i<size; i++) {
        if (vertexCmp(query.left, list[i].left) && vertexCmp(query.right, list[i].right)) return i;
        if (vertexCmp(query.left, list[i].right) && vertexCmp(query.right, list[i].left)) return i;
    }
    return -1;

}

long offMidpointInList(OffMidpoint query, OffMidpoint* list, long size){
    long i;
    for (i=0; i<size; i++) {
        if ((query.left == list[i].left && query.right == list[i].right) ||
            (query.left == list[i].right && query.right == list[i].left)) {
            return i;
        }

    }
    return -1;
};

int vertexCmp(Vertex a, Vertex b) {
    if (fabs(a.x - b.x) < EPSILON && fabs(a.y - b.y) < EPSILON) return 1;
    else return 0;
}






