#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include <math.h>
#include "ggraph.h"
#include "ggraphdefs.h"

/****************************************************************
 *								*
 *	plotpoints - write the points of a line			*
 *								*
 ****************************************************************/
plotpoints () {
    register int    i;
    register float   llabxl, llabyl;
    register float oscalex, oscaley;
    int clipped = 0;
    
    for (curline = 0; ((curline != cg.maxlines) && (cl != NULL)); ++curline) {
	if (cl->lonoff) {
	    if (symbsw) {
		for (i = 0; i < cl->maxpoint; i++) {
		/* put a circle around the point */
		    if(cg.logxsw){/* reset scale for logs */
		      oscalex = cg.scalex;/* save old scale */
		      cg.scalex = (XPLOTMAX - cg.xorigin) /
			cg.numtickx;
		      graphx = ((( (float) (log10((double)cl->xpoints[i])/
			log10((double)cg.logxtick)))
			 - cg.xoffset) * cg.scalex) + cg.xorigin;
		      cg.scalex = oscalex;
		    }else
		      graphx = ((cl->xpoints[i] - cg.xoffset) * cg.scalex)
			+ cg.xorigin;
		    if(cg.logysw){
		      oscaley = cg.scaley;
		      cg.scaley = (YPLOTMAX - cg.yorigin) / 
			cg.numticky;
		      graphy = (((float)(log10((double)cl->ypoints[i])/
			log10((double)cg.logytick))
			 - cg.yoffset) * cg.scaley) + cg.yorigin;
		      cg.scaley = oscaley;
		    }else
		      graphy = ((cl->ypoints[i] - cg.yoffset) * cg.scaley)
			+ cg.yorigin;
		    if (!((graphy < 0.0) || (graphy > 512.0) ||
				(graphx < 0.0) || (graphx > 512.0))){
				draw_symbol(cl->mtype, graphx, graphy);
		    }else
			fprintf (stderr,
			"%s: Symbol point out of range x %f y %f\npoint %f %f\n", 
graphname, graphx, graphy, cl->xpoints[i], cl->ypoints[i]);
		}
	    }
	/* connect the dots */
	    if (cl->ctype) {
		if(version == SUN_GREMLIN)
		  fprintf (outfile, "%s\n",
		      (cl->ctype == LINE) ? "VECTOR" : "CURVE");
		else
		  fprintf (outfile, "%d\n", cl->ctype);
		for (i = 0; i < cl->maxpoint; i++) {
		    if(cg.logxsw){
		      oscalex = cg.scalex;
		      cg.scalex = (XPLOTMAX - cg.xorigin) /
			 cg.numtickx;
		      graphx = (((float)(log10((double)cl->xpoints[i])/
			log10((double)cg.logxtick))
			 - cg.xoffset) * cg.scalex) + cg.xorigin;
		      cg.scalex = oscalex;
		    }else
		      graphx = ((cl->xpoints[i] - cg.xoffset) * cg.scalex)
			+ cg.xorigin;
		    if(cg.logysw){
		      oscaley = cg.scaley;
		      cg.scaley = (YPLOTMAX - cg.yorigin) / 
			cg.numticky;
/* printf("point %f partial %f off %f scale %f\n",
 cl->ypoints[i],(float)(log10((double)cl->ypoints[i])/
  log10((double)cg.logytick)),cg.yoffset,cg.scaley); */
		      graphy = (((float)(log10((double)cl->ypoints[i])/
			log10((double)cg.logytick))
			 - cg.yoffset) * cg.scaley) + cg.yorigin;
		      cg.scaley = oscaley;
		    }else
		      graphy = ((cl->ypoints[i] - cg.yoffset) * cg.scaley)
			+ cg.yorigin;
		    if ((graphy < 0.0) || (graphy > 512.0) ||
			    (graphx < 0.0) || (graphx > 512.0)) {
			    fprintf (stderr,
			"%s: Line point out of range x %f y %f\npoint %f %f\n", 
			graphname, graphx, graphy, cl->xpoints[i],
			cl->ypoints[i]);
			    if (!clipped) {
				    fprintf(outfile, 
				       (version == SUN_GREMLIN) ?
				       "*\n" : "-1.00 -1.00\n");
				    fprintf (outfile, "%d %d\n%d\n",
						cl->ltype, 0, 0);
				    clipped = 1;
			    } 
		    } else {
			    if (clipped) {
				    fprintf (outfile, "%d\n", cl->ctype);
				    clipped = 0;
			    } 
			fprintf (outfile, "%4.1f %4.1f\n", graphx, graphy);
/*			if(ddebug)printf ("point %4.1f %4.1f\n",
                           graphx, graphy);*/
		    }
		}
		fprintf(outfile, (version == SUN_GREMLIN) ? "*\n" : "-1.00 -1.00\n");
		fprintf (outfile, "%d %d\n%d\n", cl->ltype, 0, 0);
	    }
	    if (cl->llabsw) {
		if (cl->llabel.t_text[0] == NULL)
		    strcpy (cl->llabel.t_text, cl->lname);
		if (!cl->llabel.t_xpos)
		    if(cg.logxsw){
		      oscalex = cg.scalex;
		      cg.scalex = (XPLOTMAX - cg.xorigin) / 
			cg.numtickx;
		      llabxl = (((float)(log10((double)cl->xpoints[cl->maxpoint-1])/
			log10((double)cg.logxtick))
			 - cg.xoffset) * cg.scalex) + cg.xorigin;
		      cg.scalex = oscalex;
		    }else
		    llabxl = ((cl->xpoints[cl->maxpoint - 1] -cg.xoffset) * cg.scalex)
			+ cg.xorigin+5.0;
		else
		    llabxl = (cl->llabel.t_xpos * cg.scalex) + cg.xorigin;
		if (!cl->llabel.t_ypos)
		    if(cg.logysw){
		      oscaley = cg.scaley;
		      cg.scaley = (YPLOTMAX - cg.yorigin) /
			 cg.numticky;
		      llabyl = (((float)(log10((double)cl->ypoints[cl->maxpoint-1])/
			log10((double)cg.logytick))
			 - cg.yoffset) * cg.scaley) + cg.yorigin;
		      cg.scaley = oscaley;
		    }else
		    llabyl = ((cl->ypoints[cl->maxpoint - 1] - cg.yoffset)
		      * cg.scaley) + cg.yorigin + 5.0;
		else
		    llabyl = ((cl->llabel.t_ypos - cg.yoffset) * cg.scaley) + 
			cg.yorigin;
if(debug)printf("labels %f %f point %f %f high %d scale %f\n", llabxl, llabyl, 
 cl->xpoints[cl->maxpoint -1],cl->ypoints[cl->maxpoint -1], cl->maxpoint, 
cg.scalex);
		drawctext (llabxl, llabyl, cl->llabel.t_font, 
		  cl->llabel.t_size, cl->llabel.t_text,
			CENTERLEFT_TEXT);
	    }
	}
    }
}
/****************************************************************
 *								*
 *	readpoints - read the points in 			*
 *			returns point number			*
 *								*
 ****************************************************************/
int     readpoints (infile, line_name)
FILE *infile;
char   *line_name;
{
    int     count;
    register int    i;
    register int    done;
    float   x,
            y;
    float   hix,
            hiy,
            lox,
            loy;
    char    iline[80];
    char   *fgets ();

    count = 0;
    while (count != -1) {	/* wait for EOF */
	count = 0;
	done = FALSE;
	hix = hiy = 0.0;
	lox = loy = HUGE;
	curline++;
				/* allocate a line structure */
	cl = (struct aline *)malloc(sizeof(struct aline));
	cl->forw_line = NULL;	/* link chain */
	cl->back_line = NULL;
	cg.lines[curline+1] = NULL;/* terminate chain */
	cl->mtype = -1;
	cl->ltype = -1;
	cl->ctype = -1;
	cl->llabel.t_size = -1;
	cl->llabel.t_font = -1;
	cl->llelabel.t_size = -1;
	cl->llelabel.t_font = -1;
	strcpy (cl->lname, line_name);
	cl->llabel.t_size = -1;
	cl->llabel.t_font = -1;
	cl->llabel.t_xpos = 0.0;
	cl->llabel.t_ypos = 0.0;
	cl->lonoff = 1;
	cl->ctype = -1;
	cl->mtype = -1;
	cl->ltype = -1;
	while (!done) {		/* loop around for a while */
	    if (fgets (iline, 80, infile) == NULL) {/* read numbers */
		done = TRUE;	/* EOF stop reading */
		count = -1;	/* let reader know */
	    }
	    else
		if (strncmp ("dae", iline, 3) == 0) {
		/* End-of-line-segment */
		    done = TRUE;
		}
		else {		/* calculate point on graph */
		    sscanf (iline, "%f %f", &x, &y);
		    if (x > hix)
			hix = x;
		    if (x < lox)
			lox = x;
		    if (y > hiy)
			hiy = y;
		    if (y < loy)
			loy = y;
		    cl->xpoints[count] = x;
		    cl->ypoints[count] = y;
		    count++;
		}
	}
    /* set count of number of points */
	cl->maxpoint = count;
	cl->minx = lox;
	cl->miny = loy;
	cl->maxx = hix;
	cl->maxy = hiy;
	cg.xoffset = lox;
	cg.yoffset = loy;
	if (cg.gminx > lox)
	    cg.gminx = cl->minx;
	if (cg.gminy > loy)
	    cg.gminy = cl->miny;
	if (cg.gmaxx < hix)
	    cg.gmaxx = cl->maxx;
	if (cg.gmaxy < hiy)
	    cg.gmaxy = cl->maxy;

	return (curline + 1);	/* return point number */
    }
    return (curline + 1);	/* return point number */
}
