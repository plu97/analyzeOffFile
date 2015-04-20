all: pointToNode eleNodeToOff triToQuad generatePointSet

pointToNode: pointToNode.c
	gcc pointToNode.c -g -lm -o pointToNode

eleNodeToOff: eleNodeToOff.c
	gcc eleNodeToOff.c -g -lm -o eleNodeToOff

triToQuad: triToQuad.c
	gcc triToQuad.c -g -lm -o triToQuad

generatePointSet: generatePointSet.c
	gcc generatePointSet.c -g -lm -o generatePointSet

clean:
	rm pointToNode eleNodeToOff triToQuad generatePointSet
