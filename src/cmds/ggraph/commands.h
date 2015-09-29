/****************************************************************/
/*	commands for ggraph					*/
/*	xgr - X grid on/off					*/
/*		xgr on|off					*/
/*	ygr - Y grid on/off					*/
/*		ygr on|off					*/
/*	xax - X axis on/off					*/
/*		xax on|off					*/
/*	yax - Y axis on/off					*/
/*		yax on|off					*/
/*	xti - X ticks on/off					*/
/*		xti on|off					*/
/*	yti - Y ticks on/off					*/
/*		yti on|off					*/
/*	xtl - X label on/off					*/
/*		xtl on|off					*/
/*	ytl - Y label on/off					*/
/*		ytl on|off					*/
/*	gti - Graph title					*/
/*		gti <string>					*/
/*	das - Data start					*/
/*		das <string>					*/
/*	xla - X label						*/
/*		xla <string>					*/
/*	yla - Y label						*/
/*		yla <string>					*/
/*	tif - Graph title on/off				*/
/*		tif on|off					*/
/*	dae - Data end						*/
/*		dae						*/
/*	xst - Set X scaling					*/
/*	    xst <# of tick> <delta> <starting tick> <dev org>   */ 
/*	yst - Set Y scaling					*/
/*	    yst <# of tick> <delta> <starting tick> <dev org>   */ 
/*	lty - Line type 					*/
/*		lty <line name> <type #>			*/
/*	lcu - Curve type					*/
/*		lcu <line name> <curve type>			*/
/*	lof - Line on/off					*/
/*		lof <line name> on|off				*/
/*	lsy - Line symbol					*/
/*		lsy <line name> <symbol #>			*/
/*	dra - Draw graph					*/
/*		dra <output file name>				*/
/*	lla - Line label on/off					*/
/*		lla <line name> on|off				*/
/*	llp - Line label position				*/
/*		llp <line name> <X pos> <Y pos>			*/
/*	sla - Line label string					*/
/*		sla <line name> <string>			*/
/*	fra - graph frame on or off				*/
/*		fra on|off					*/
/*	frt - graph frame thickness				*/
/*		frt <size #>					*/
/*	xgt - X grid type					*/
/*    		xgt <grid #>					*/
/*	ygt - Y grid type					*/
/*    		ygt <grid #>					*/
/*	*gtp - Graph title position				*/
/*    		gtp <X coord> <Y coord>				*/
/*	*ytp - Y label position					*/
/*    		ytp <X coord> <Y coord>				*/
/*	*xtp - X label position					*/
/*    		xtp <X coord> <Y coord>				*/
/*	tis - Title text size					*/
/*    		tis <size #>					*/
/*	xts - X label text size					*/
/*    		xts <size #>					*/
/*	yts - Y label text size					*/
/*    		yts <size #>					*/
/*	*lth - line thickness					*/
/*    		lth <size #>					*/
/*	typ - type of graph					*/
/*    		typ <type #>					*/
/*	xpr - precision for X tick labels			*/
/*    		xpr <#> <#>					*/
/*	ypr - precision for Y tick labels			*/
/*    		ypr <#> <#>					*/
/*	syz - symbol size					*/
/*    		syz <size #>					*/
/*	ssw - symbols on or off					*/
/*    		ssw on|off					*/
/*	tft - title font					*/
/*    		tft <font #>					*/
/*	xft - X label font					*/
/*    		xft <font #>					*/
/*	yft - Y label font					*/
/*    		yft <font #>					*/
/*	*rea - read a command file				*/
/*    		rea <file name>					*/
/*	*ver - Y label vertical					*/
/*    		ver on|off					*/
/*	*lox - X coordinates logs				*/
/*    		lox on|off					*/
/*	*loy - Y coordinates logs				*/
/*    		loy on|off					*/
/*	*log - log-log graph					*/
/*    		log on|off					*/
/*	*xle - x axis length in units				*/
/*		xle <length>					*/
/*	*yle - y axis length in units				*/
/*		xle <length>					*/
/*								*/
/****************************************************************/
/* commands */
#define XGRID 0
#define YGRID 1
#define XAXIS 2
#define YAXIS 3
#define XTICK 4
#define YTICK 5
#define XTICKL 6
#define YTICKL 7
#define TITLE 8
#define DATASTART 9
#define XLABLE 10
#define YLABLE 11
#define TITFLG 12
#define DATAEND 13

#define SETX 14			/* temp hack to set parameters */
#define SETY 15			/* temp hack to set parameters */

#define LTYPE 16
#define LCURVE 17
#define LONOFF 18
#define LSYM 19
#define DRAW 20
#define LINELABEL 21
#define LLINELABPOS 22
#define SLINELABEL 23
#define FRAME 24
#define FRAMETHICK 25
#define XGRTYPE 26
#define YGRTYPE 27
#define TITLEPOS 28
#define YPOS 29
#define XPOS 30
#define TITLESIZE 31
#define XSIZE 32
#define YSIZE 33
#define LTHICK 34
#define GTYPE 35
#define XPRECISION 36
#define YPRECISION 37
#define SYMBOLSZ 38
#define SYMBOLSW 39
#define TITLEFT 40
#define XFONT 41
#define YFONT 42
#define READ 43
#define VERTICALT 44
#define LOGX 45
#define LOGY 46
#define LOGLOG 47
#define QUIT 48
#define FIRST_LAB 49
#define FIRST_LAB_X 50
#define FIRST_LAB_Y 51
#define COMMENT 52
#define AXCROSS 53
#define AXCROSS_X 54
#define AXCROSS_Y 55
#define LEGEND 56
#define LEGEND_BOX 57
#define LEGEND_SIDE 58
#define SET_LEGEND_LABEL 59
#define SET_LEGEND_HEADING 60
#define SET_LEGEND_HEAD_FONT 61
#define SET_LEGEND_HEAD_SIZE 62
#define XTICK_FONT 63
#define YTICK_FONT 64
#define XTICK_SIZE 65
#define YTICK_SIZE 66
#define XSET 67
#define YSET 68
#define LINE_LAB_FONT 69
#define LINE_LAB_SIZE 70
#define XAXIS_LENGTH 71
#define YAXIS_LENGTH 72
#define SET_UNITS 73

#define MAXCOMMAND 74
