/* 
 * barg.c --
 *
 *	Bar graph generating program.  See the man page for details on
 *	how to use this program.
 *
 * Copyright (C) 1986 Regents of the University of California
 * All rights reserved.
 */

#include <stdio.h>

#define	BUF_LENGTH	512
char	outFile[100];
int 	xMinPixel = 128;
int 	yMinPixel = 0;
int 	xMaxPixel = 960;
int 	yMaxPixel = 384;
int 	numSets = -1;
int 	numPerSet = -1;
int 	xWidth = -1;
float 	yMaxVal;
float	yValInc;
float	yBaseVal;
int	yBaseline;

int 	yNum = -1;
int 	leftBorder = 48;
int 	rightBorder = 24;
int 	innerBarSpace = 8;
int 	innerSetSpace = -1;
int 	curXVal;
int 	pixPerSet;
int 	printPercent = 0;
char	xLabel[100];
char	yLabel[100];
char	graphTitle[100];
int	xLabeled = 0;
int	yLabeled = 0;
int	graphTitled = 0;

#define	MAX_INNER_SET_SPACE	64
#define	MAX_X_WIDTH		32
#define MAX_BARS		8
#define INNER_SET_SPACE		32
#define X_WIDTH			16
#define FONT_HEIGHT		15

char	barLabels[MAX_BARS][100];
int	barTypes[MAX_BARS];
int	gremFilledPolys[MAX_BARS] = {1, 3, 12, 14, 16, 19, 21, 23};
FILE	*inFP, *outFP;
char	buf[BUF_LENGTH];

int	leftKeyBorder = 64;
int	rightKeyBorder = 32;
int	keyWidth = 48;
int	keyHeight = 24;
int	yBaseline;

main(argc, argv)
    int	 argc;
    char **argv;
{
    int		i;

    if (argc <= 1) {
        inFP = stdin;
	outFP = stdout;
    } else {
	inFP = fopen(argv[1], "r");
	if (inFP == NULL) {
	    fprintf(stderr, "Couldn't open %s for reading\n", argv[1]);
	    exit(1);
	}
	if (argc <= 2) {
	    outFP = stdout;
	} else {
	    outFP = fopen(argv[2], "w");
	    if (outFP == NULL) {
		fprintf(stderr, "Couldn't open %s for writing\n", argv[2]);
		exit(1);
	    }
	}
    }
    fprintf(outFP, "sungremlinfile\n");
    fprintf(outFP, "1 %d %d\n", xMinPixel, yMinPixel);
    while (fgets(buf, BUF_LENGTH, inFP) != NULL) {
	switch (buf[0]) {
	    case 'b':
		sscanf(&buf[1], "%d %d", &leftBorder, &rightBorder);
		break;
	    case 'd':
		DrawBars();
		break;
	    case 'h':
	        sscanf(&buf[1], "%d", &yMaxPixel);
		break;
	    case 'k':
		sscanf(&buf[1], "%d %d %d %d", &leftKeyBorder, &rightKeyBorder,
					    &keyWidth, &keyHeight);
		break;
	    case 'l': {
		if (buf[1] == 'x') {
		    GetString(&buf[2], xLabel);
		    xLabeled = 1;
		} else if (buf[1] == 'y') {
		    GetString(&buf[2], yLabel);
		    yLabeled = 1;
		} else {
		    fprintf(stderr, "Unknown label type\n");
		}
		break;
	    }
	    case 'n': {
		GetSetInfo();
		break;
	    }
	    case 'p':
		printPercent = 1;
		break;
	    case 's':
		sscanf(&buf[1], "%d %d", &innerBarSpace, &innerSetSpace);
		break;
	    case 't':
		GetString(&buf[1], graphTitle);
		graphTitled = 1;
		break;
	    case 'w':
		sscanf(&buf[1], "%d", &xMaxPixel);
		break;
	    case 'y': {
		sscanf(&buf[1], "%d %f %f", &yNum, &yValInc, &yBaseVal);
		yMaxVal = yNum * yValInc + yBaseVal;
		if (yBaseVal < 0) {
		    yBaseline = (-yBaseVal / (yMaxVal - yBaseVal)) *
				    (yMaxPixel - yMinPixel) + yMinPixel;
		} else {
		    yBaseline = yMinPixel;
		}
		break;
	    }
	    case 'W': 
		sscanf(&buf[1], "%d", &xWidth);
		break;
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8': {
		int	barNum;
		char	*strPtr;

		barNum = buf[0] - '0' - 1;
		sscanf(&buf[1], "%d", &barTypes[barNum]);
		strPtr = &buf[1];
		while (*strPtr == ' ') {
		    strPtr++;
		}
		while (*strPtr != ' ') {
		    strPtr++;
		}
		GetString(strPtr, barLabels[barNum]);
		break;
	    }
	    case '#':
	    case '\n':
	    case ' ':
		break;
	    default:
		fprintf(stderr, "Unknown command %s\n", buf);
		break;
	}
    }
    /*
     * Print the X and Y axises.
     */
    fprintf(outFP, "VECTOR\n");
    fprintf(outFP, "%d %d\n%d %d\n", xMinPixel, yBaseline, xMaxPixel,
				     yBaseline);
    fprintf(outFP, "*\n6 0\n0\n");
    fprintf(outFP, "VECTOR\n");
    fprintf(outFP, "%d %d\n%d %d\n", xMinPixel, yMinPixel, xMinPixel, 
				     yMaxPixel);
    fprintf(outFP, "*\n6 0\n0\n");

    /*
     * Print the dashed cross bars and labels here.
     */
    for (i = 0; i <= yNum; i++) {
	int	yPixel;
	char	label[100];

	yPixel = yMinPixel + (float)(yMaxPixel - yMinPixel) / yNum * i;
	if (yPixel != yBaseline) {
	    fprintf(outFP, "VECTOR\n");
	    fprintf(outFP, "%d %d\n%d %d\n", xMinPixel, yPixel, 
					     xMaxPixel, yPixel);
	    fprintf(outFP, "*\n1 0\n0\n");
	}
	if (yValInc - (int)yValInc == 0.0) {
	    sprintf(label, "%d", (int) (i * yValInc + yBaseVal));
	} else {
	    sprintf(label, "%0.2f", i * yValInc + yBaseVal);
	}
	if (printPercent) {
	    strcat(label, "%");
	}
	PutString(label, xMinPixel - 10, yPixel, "CENTRIGHT", 2);
    }

    /*
     * Print the axis labels.
     */
    if (xLabeled) {
	PutString(xLabel, xMinPixel + (xMaxPixel - xMinPixel) / 2,
		  yMinPixel - 48, "TOPCENT", 2);
    }
    if (yLabeled) {
	int	yPix;
	char	chBuf[2];
	char	*strPtr;

	chBuf[1] = '\0';
	yPix = yMaxPixel - 
	       (yMaxPixel - yMinPixel - strlen(yLabel) * FONT_HEIGHT) / 2;
	for (strPtr = yLabel;
	     *strPtr != '\0';
	     strPtr++, yPix -= FONT_HEIGHT) {
	    chBuf[0] = *strPtr;
	    PutString(chBuf, xMinPixel - 80, yPix, "TOPCENT", 2);
	}
    }

    /*
     * Print the graph title.
     */
    if (graphTitled) {
	PutString(graphTitle, xMinPixel + (xMaxPixel - xMinPixel) / 2,
		   yMaxPixel + 64, "TOPCENT", 3);
    }

    /*
     * Print out bar key.
     */
    PrintBarKey();

    fprintf(outFP, "-1\n");
    exit(0);
}

PutString(label, x, y, loc, font)
    char	*label;
    int		x;
    int		y;
    char	*loc;
    int		font;
{
    fprintf(outFP, "%s\n", loc);
    fprintf(outFP, "%d %d\n", x, y);
    fprintf(outFP, "0 0\n0 0\n0 0\n*\n1 %d\n", font);
    fprintf(outFP, "%d %s\n", strlen(label), label);
}

GetString(srcStr, destStr)
    char	*srcStr;
    char	*destStr;
{
    char	*strPtr;

    while (*srcStr == ' ') {
	srcStr++;
    }
    strPtr = srcStr;
    while (*strPtr != '\n') {
	strPtr++;
    }
    *strPtr = '\0';
    strcpy(destStr, srcStr);
}

DrawFilledRectangle(minX, minY, width, height, fillType)
    int	minX;
    int	minY;
    int width;
    int height;
    int fillType;
{
    fprintf(outFP, "POLYGON\n");
    fprintf(outFP, "%d %d\n", minX, minY + height);
    fprintf(outFP, "%d %d\n", minX + width, minY + height);
    fprintf(outFP, "%d %d\n", minX + width, minY);
    fprintf(outFP, "%d %d\n", minX, minY);
    fprintf(outFP, "*\n");
    fprintf(outFP, "5 %d\n", gremFilledPolys[fillType]);
    fprintf(outFP, "0\n");

}

PrintBarKey()
{
    int	perKeyWidth;
    int	baseX;
    int	baseY;
    int	i;

    perKeyWidth = (xMaxPixel - xMinPixel - leftKeyBorder - rightKeyBorder) / 
								    numPerSet;
    if (perKeyWidth < keyWidth * 3) {
	perKeyWidth = keyWidth * 3;
    }
    if (xLabeled) {
	baseY = yMinPixel - keyHeight - 96;
    } else {
	baseY = yMinPixel - keyHeight - 64;
    }
    for (i = 0; i < numPerSet; i++) {
	baseX = xMinPixel + leftKeyBorder + perKeyWidth * i;
	DrawFilledRectangle(baseX, baseY, keyWidth, keyHeight, barTypes[i]);
	PutString(barLabels[i], baseX + keyWidth + 16, baseY + keyHeight / 2,
		  "CENTLEFT", 2);
    }
}

DrawBars()
{
    float	val;
    int		yHeight;
    char	label[100];
    char	*labelPtr;
    int		i = 0;

    i = 1;
    while (buf[i] == ' ') {
	i++;
    }
    for (labelPtr = label; buf[i] != '\n'; i++, labelPtr++) {
	*labelPtr = buf[i];
    }
    *labelPtr = '\0';

    PutString(label, curXVal + pixPerSet / 2, yMinPixel - 10,
	      "TOPCENT", 2);
    i = 0;
    while (fgets(buf, BUF_LENGTH, inFP) != NULL) {
	if (buf[0] == 'e') {
	    break;
	}
	if (i >= numPerSet) {
	    fprintf(stderr, "Too many bars per set \"%s\"\n", label);
	    i++;
	    continue;
	}
	sscanf(buf, "%f", &val);
	if (val > yMaxVal) {
	    fprintf(stderr, "Y Value %4.2f clipped\n", val);
	    val = yMaxVal;
	} else if (val < yBaseVal) {
	    fprintf(stderr, "Y Value %4.2f clipped\n", val);
	    val = yBaseVal;
	}
	if (yBaseVal >= 0) {
	    yHeight = ((val - yBaseVal) / (yMaxVal - yBaseVal)) * 
						    (yMaxPixel - yMinPixel);
	} else if (val >= 0) {
	    yHeight = (val / yMaxVal) * (yMaxPixel - yBaseline);
	} else {
	    yHeight = (val / yBaseVal) * (yBaseline - yMinPixel);
	}
	if (yHeight < 4) {
	    fprintf(outFP, "VECTOR\n");
	    fprintf(outFP, "%d %d\n%d %d\n", curXVal, yBaseline + 2,					curXVal + xWidth, yBaseline + 2);
	    fprintf(outFP, "*\n3 0\n0\n");
	} else {
	    if (val < 0) {
		DrawFilledRectangle(curXVal, yBaseline - yHeight, xWidth,
				    yHeight, barTypes[i]);
	    } else {
		DrawFilledRectangle(curXVal, yBaseline, xWidth,
				    yHeight, barTypes[i]);
	    }
	}
	i++;
	curXVal += xWidth + innerBarSpace;
    }
    curXVal += innerSetSpace - innerBarSpace;
}

GetSetInfo()
{
    int	availPixels;
    int	width;
    
    sscanf(&buf[1], "%d %d", &numSets, &numPerSet);
    availPixels = xMaxPixel - xMinPixel - leftBorder - rightBorder;
    if (innerSetSpace == -1) {
	if (xWidth == -1) {
	    xWidth = X_WIDTH;
	}
	availPixels -= xWidth * numSets * numPerSet +
		       innerBarSpace * numSets * (numPerSet - 1);
	innerSetSpace = availPixels / (numSets - 1);
	if (innerSetSpace > MAX_INNER_SET_SPACE) {
	    innerSetSpace = MAX_INNER_SET_SPACE;
	}
    } else if (xWidth == -1) {
	availPixels -= (numSets - 1) * innerSetSpace;
	pixPerSet = availPixels / numSets;
	xWidth = (pixPerSet - (numPerSet - 1) * innerBarSpace) / 
						    numPerSet;
	if (xWidth > MAX_X_WIDTH) {
	    xWidth = MAX_X_WIDTH;
	}
    }
    pixPerSet = xWidth * numPerSet + 
		innerBarSpace * (numPerSet - 1);
    width = leftBorder + rightBorder + pixPerSet * numSets + 
	    innerSetSpace * (numSets - 1);
    xMaxPixel = xMinPixel + width;
    curXVal = xMinPixel + leftBorder;
}
