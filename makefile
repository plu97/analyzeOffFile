all: analyzeOff

analyzeOff: analyzeOff.c
	gcc analyzeOff.c -lm -o analyzeOff

clean:
	rm analyzeOff

