all: analyzeOff

analyzeOff: analyzeOff.c
	gcc analyzeOff.c -g -lm -o analyzeOff

clean:
	rm analyzeOff

