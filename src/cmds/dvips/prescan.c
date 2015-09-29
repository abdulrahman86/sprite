/*
 *   This is the main routine for the first (prescanning) pass.
 */
#include "structures.h" /* The copyright notice in that file is included too! */
/*
 *   These are all the external routines it calls:
 */
extern void error() ;
extern shalfword dvibyte() ;
extern integer signedquad() ;
extern int skipnop() ;
extern void skipover() ;
extern short scanpage() ;
extern void skippage() ;
/*
 *   These are the globals it accesses.
 */
#ifdef DEBUG
extern integer debug_flag;
#endif  /* DEBUG */
extern fontdesctype *fonthead ;
extern real conv ;
extern real alpha ;
extern integer firstpage ;
extern Boolean notfirst ;
extern integer fontmem ;
extern integer pagecount ;
extern integer pagenum ;
extern integer maxpages ;
extern sectiontype *sections ;
extern FILE *dvifile ;
extern integer mag ;
extern Boolean overridemag ;
extern integer swmem ;
extern int quiet ;
extern int actualdpi ;
extern Boolean reverse ;
extern int totalpages ;
/*
 *   We declare the type malloc returns.
 */
char *malloc() ;
/*
 *   This routine handles the processing of the preamble in the dvi file.
 */
void
readpreamble()
{
   register integer num, den;
   register int i ;

   if (dvibyte()!=247) error("! Bad DVI file: first byte not preamble") ;
   if (dvibyte()!=2) error("! Bad DVI file: id byte not 2") ;
   num = signedquad() ;
   den = signedquad() ;
   if (overridemag) (void)signedquad() ;
   else mag = signedquad() ;
   conv = (real) num * DPI * (real) mag / ( den * 254000000.0 ) ; 
   alpha = (((real)den / 7227.0) / 0x100000) * (25400000.0 / (real) num) ;
   if (! quiet) {
      (void)fprintf(stderr, "'") ;
      for(i=dvibyte();i>0;i--) (void)putc(dvibyte(), stderr) ;
      (void)fprintf(stderr, "'\n") ;
   } else
      skipover(dvibyte()) ;
}

/*
 *   Finally, here's our main prescan routine.
 */
void
prescanpages()
{
   register int cmd ;
   short ret ;
   register integer thispageloc, thissecloc ;
   register fontdesctype *f ;
   register shalfword c ;

   readpreamble() ;
/*
 *   Now we look for the first page to process.  If we get to the end of
 *   the file before the page, we complain (fatally).
 *   Incidentally, we don't use the DVI file's bop backpointer to skip
 *   over pages at high speed, because we want to look to for special
 *   header that might be in skipped pages.
 */
   while (1) {
      cmd = skipnop() ;
      if (cmd==248)
         error("! End of document before first specified page") ;
      if (cmd!=139)
         error("! Bad DVI file: expected bop") ;
      thispageloc = ftell(dvifile) ; /* the location FOLLOWING the bop */
#ifdef DEBUG
      if (dd(D_PAGE))
#ifdef SHORTINT
      (void)fprintf(stderr,"bop at %ld\n", thispageloc) ;
#else   /* ~SHORTINT */
      (void)fprintf(stderr,"bop at %d\n", thispageloc) ;
#endif  /* ~SHORTINT */
#endif  /* DEBUG */
      pagenum = signedquad() ;
      if (notfirst && pagenum != firstpage)
         skippage() ;
      else
         break ;
   }
/*
 *   Here we scan for each of the sections.  First we initialize some of
 *   the variables we need.
 */
   while (maxpages > 0 && cmd != 248) {
      for (f=fonthead; f; f=f->next) {
         f->psname = 0 ;
         if (f->loaded==1)
            for (c=255; c>=0; c--)
               f->chardesc[c].flags &= (EXISTS | REPACKED) ;
      }
      fontmem = swmem - OVERCOST ;
      if (fontmem <= 1000)
         error("! Too little VM in printer") ;

/*   The section begins at the bop command just before thispageloc (which may
 *   be a page that was aborted because the previous section overflowed memory).
 */
      pagecount = 0 ;
      (void)fseek(dvifile, (long)thispageloc+44, 0) ;
      thissecloc = thispageloc ;
/*
 *   Now we have the loop that actually scans the pages.  The scanpage routine
 *   returns 1 if the page scans okay; it returns 2 if the memory ran out
 *   before any pages were completed (in which case we'll try to carry on
 *   and hope for the best); it returns 0 if a page was aborted for lack
 *   of memory. After each page, we mark the characters seen on that page
 *   as seen for this section so that they will be downloaded.
 */
      while (maxpages>0 && (ret=scanpage())) {
         if (reverse)
            thissecloc = thispageloc ;
         pagecount++ ;
         maxpages-- ;
         for (f=fonthead; f; f=f->next)
            if (f->loaded==1)
               for (c=255; c>=0; c--)
                  if (f->chardesc[c].flags & THISPAGE)
                     f->chardesc[c].flags = PREVPAGE |
               (f->chardesc[c].flags & (EXISTS | REPACKED)) ;
         cmd=skipnop() ;
         if (cmd==248) break ;
         if (cmd!=139)
            error("! Bad DVI file: expected bop") ;
         thispageloc = ftell(dvifile) ;
#ifdef DEBUG
         if (dd(D_PAGE))
#ifdef SHORTINT
         (void)fprintf(stderr,"bop at %ld\n", thispageloc) ;
#else   /* ~SHORTINT */
         (void)fprintf(stderr,"bop at %d\n", thispageloc) ;
#endif  /* ~SHORTINT */
#endif  /* DEBUG */
         pagenum = signedquad() ;
         skipover(40) ;
         if (ret==2) break ;
      }
/*
 *   Now we have reached the end of a section for some reason.
 *   If there are any pages, we save the pagecount, section location,
 *   and continue.
 */
      if (pagecount>0) {
         register int fc = 0 ;
         register sectiontype *sp ;
         register charusetype *cp ;

         totalpages += pagecount ;
         for (f=fonthead; f; f=f->next)
            if (f->loaded==1)
               fc++ ;
         sp = (sectiontype *)malloc((unsigned int)(sizeof(sectiontype) + 
            fc * sizeof(charusetype) + sizeof(fontdesctype *))) ;
         if (sp==NULL)
            error("! out of memory") ;
         sp->bos = thissecloc ;
         if (reverse) {
            sp->next = sections ;
            sections = sp ;
         } else {
            register sectiontype *p ;

            sp->next = NULL ;
            if (sections == NULL)
               sections = sp ;
            else {
               for (p=sections; p->next != NULL; p = p->next) ;
               p->next = sp ;
            }
         }
         sp->numpages = pagecount ;
#ifdef DEBUG
        if (dd(D_PAGE))
#ifdef SHORTINT
         (void)fprintf(stderr,"Have a section: %ld pages at %ld fontmem %ld\n", 
#else   /* ~SHORTINT */
         (void)fprintf(stderr,"Have a section: %d pages at %d fontmem %d\n", 
#endif  /* ~SHORTINT */
             pagecount, thissecloc, fontmem) ;
#endif  /* DEBUG */
         cp = (charusetype *) (sp + 1) ;
         for (f=fonthead; f; f=f->next)
            if (f->loaded==1 && f->psname) {
               register halfword b, bit ;

               cp->fd = f ;
               c = 0 ;
               for (b=0; b<16; b++) {
                  cp->bitmap[b] = 0 ;
                  for (bit=32768; bit!=0; bit>>=1) {
                     if (f->chardesc[c].flags & PREVPAGE)
                        cp->bitmap[b] |= bit ;
                  c++ ;
                  }
               }
               cp++ ;
            }
         cp->fd = NULL ;
      }
   }
}
