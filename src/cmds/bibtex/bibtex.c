#include "bibtex.h"
jmp_buf jmp9998, jmp32; int lab31=0;
/* 9998 9999 31 32 9932 */ 
#define bufsize 3000 
#define minprintline 3 
#define maxprintline 79 
#define auxstacksize 20 
#define maxbibfiles 20 
#define poolsize 120000L 
#define maxstrings 4000 
#define maxcites 750 
#define mincrossrefs 2 
#define wizfnspace 3000 
#define singlefnspace 100 
#define maxentints 3000 
#define maxentstrs 3000 
#define entstrsize 100 
#define globstrsize 1000 
#define maxfields 17250 
#define litstksize 100 
#define numbltinfns 37 
typedef schar ASCIIcode  ; 
typedef schar lextype  ; 
typedef schar idtype  ; 
typedef file_ptr /* of  char */ alphafile  ; 
typedef integer bufpointer  ; 
typedef ASCIIcode buftype [bufsize + 1] ; 
typedef integer poolpointer  ; 
typedef integer strnumber  ; 
typedef short hashloc  ; 
typedef short hashpointer  ; 
typedef schar strilk  ; 
typedef schar pdsloc  ; 
typedef schar pdslen  ; 
typedef char pdstype [13] ; 
typedef integer auxnumber  ; 
typedef integer bibnumber  ; 
typedef integer citenumber  ; 
typedef schar fnclass  ; 
typedef integer wizfnloc  ; 
typedef integer intentloc  ; 
typedef integer strentloc  ; 
typedef schar strglobloc  ; 
typedef integer fieldloc  ; 
typedef short hashptr2  ; 
typedef integer litstkloc  ; 
typedef schar stktype  ; 
typedef integer bltinrange  ; 
integer bad  ; 
schar history  ; 
integer errcount  ; 
ASCIIcode xord[128]  ; 
char xchr[128]  ; 
lextype lexclass[128]  ; 
idtype idclass[128]  ; 
integer charwidth[128]  ; 
integer stringwidth  ; 
char nameoffile[1025], realnameoffile[1025]  ; 
short namelength  ; 
short nameptr  ; 
buftype buffer  ; 
bufpointer last  ; 
buftype svbuffer  ; 
bufpointer svptr1  ; 
bufpointer svptr2  ; 
integer tmpptr, tmpendptr  ; 
ASCIIcode strpool[poolsize + 1]  ; 
poolpointer strstart[maxstrings + 1]  ; 
poolpointer poolptr  ; 
strnumber strptr  ; 
strnumber strnum  ; 
poolpointer pptr1, pptr2  ; 
hashpointer hashnext[5001]  ; 
strnumber hashtext[5001]  ; 
strilk hashilk[5001]  ; 
integer ilkinfo[5001]  ; 
short hashused  ; 
boolean hashfound  ; 
hashloc dummyloc  ; 
strnumber sauxextension  ; 
strnumber slogextension  ; 
strnumber sbblextension  ; 
strnumber sbstextension  ; 
strnumber sbibextension  ; 
strnumber sbstarea  ; 
strnumber sbibarea  ; 
hashloc predefloc  ; 
integer commandnum  ; 
bufpointer bufptr1  ; 
bufpointer bufptr2  ; 
schar scanresult  ; 
integer tokenvalue  ; 
short auxnamelength  ; 
alphafile auxfile[auxstacksize + 1]  ; 
strnumber auxlist[auxstacksize + 1]  ; 
auxnumber auxptr  ; 
integer auxlnstack[auxstacksize + 1]  ; 
strnumber toplevstr  ; 
alphafile logfile  ; 
alphafile bblfile  ; 
strnumber biblist[maxbibfiles + 1]  ; 
bibnumber bibptr  ; 
bibnumber numbibfiles  ; 
boolean bibseen  ; 
alphafile bibfile[maxbibfiles + 1]  ; 
boolean bstseen  ; 
strnumber bststr  ; 
alphafile bstfile  ; 
strnumber citelist[maxcites + 1]  ; 
citenumber citeptr  ; 
citenumber entryciteptr  ; 
citenumber numcites  ; 
citenumber oldnumcites  ; 
boolean citationseen  ; 
hashloc citeloc  ; 
hashloc lcciteloc  ; 
hashloc lcxciteloc  ; 
boolean citefound  ; 
boolean allentries  ; 
citenumber allmarker  ; 
integer bbllinenum  ; 
integer bstlinenum  ; 
hashloc fnloc  ; 
hashloc wizloc  ; 
hashloc literalloc  ; 
hashloc macronameloc  ; 
hashloc macrodefloc  ; 
fnclass fntype[5001]  ; 
wizfnloc wizdefptr  ; 
wizfnloc wizfnptr  ; 
hashptr2 wizfunctions[wizfnspace + 1]  ; 
intentloc intentptr  ; 
integer entryints[maxentints + 1]  ; 
intentloc numentints  ; 
strentloc strentptr  ; 
ASCIIcode entrystrs[maxentstrs + 1][entstrsize + 1]  ; 
strentloc numentstrs  ; 
schar strglbptr  ; 
strnumber glbstrptr[10]  ; 
ASCIIcode globalstrs[10][globstrsize + 1]  ; 
integer glbstrend[10]  ; 
schar numglbstrs  ; 
fieldloc fieldptr  ; 
fieldloc fieldparentptr, fieldendptr  ; 
citenumber citeparentptr, citexptr  ; 
strnumber fieldinfo[maxfields + 1]  ; 
fieldloc numfields  ; 
fieldloc numpredefinedfields  ; 
fieldloc crossrefnum  ; 
boolean nofields  ; 
boolean entryseen  ; 
boolean readseen  ; 
boolean readperformed  ; 
boolean readingcompleted  ; 
boolean readcompleted  ; 
integer implfnnum  ; 
integer biblinenum  ; 
hashloc entrytypeloc  ; 
hashptr2 typelist[maxcites + 1]  ; 
boolean typeexists  ; 
boolean entryexists[maxcites + 1]  ; 
boolean storeentry  ; 
hashloc fieldnameloc  ; 
hashloc fieldvalloc  ; 
boolean storefield  ; 
boolean storetoken  ; 
ASCIIcode rightouterdelim  ; 
ASCIIcode rightstrdelim  ; 
boolean atbibcommand  ; 
hashloc curmacroloc  ; 
strnumber citeinfo[maxcites + 1]  ; 
boolean citehashfound  ; 
bibnumber preambleptr  ; 
bibnumber numpreamblestrings  ; 
integer bibbracelevel  ; 
integer litstack[litstksize + 1]  ; 
stktype litstktype[litstksize + 1]  ; 
litstkloc litstkptr  ; 
strnumber cmdstrptr  ; 
integer entchrptr  ; 
integer globchrptr  ; 
buftype exbuf  ; 
bufpointer exbufptr  ; 
bufpointer exbuflength  ; 
buftype outbuf  ; 
bufpointer outbufptr  ; 
bufpointer outbuflength  ; 
boolean messwithentries  ; 
citenumber sortciteptr  ; 
strentloc sortkeynum  ; 
integer bracelevel  ; 
hashloc bequals  ; 
hashloc bgreaterthan  ; 
hashloc blessthan  ; 
hashloc bplus  ; 
hashloc bminus  ; 
hashloc bconcatenate  ; 
hashloc bgets  ; 
hashloc baddperiod  ; 
hashloc bcalltype  ; 
hashloc bchangecase  ; 
hashloc bchrtoint  ; 
hashloc bcite  ; 
hashloc bduplicate  ; 
hashloc bempty  ; 
hashloc bformatname  ; 
hashloc bif  ; 
hashloc binttochr  ; 
hashloc binttostr  ; 
hashloc bmissing  ; 
hashloc bnewline  ; 
hashloc bnumnames  ; 
hashloc bpop  ; 
hashloc bpreamble  ; 
hashloc bpurify  ; 
hashloc bquote  ; 
hashloc bskip  ; 
hashloc bstack  ; 
hashloc bsubstring  ; 
hashloc bswap  ; 
hashloc btextlength  ; 
hashloc btextprefix  ; 
hashloc btopstack  ; 
hashloc btype  ; 
hashloc bwarning  ; 
hashloc bwhile  ; 
hashloc bwidth  ; 
hashloc bwrite  ; 
hashloc bdefault  ; 
#ifdef STAT
hashloc bltinloc[numbltinfns + 1]  ; 
integer executioncount[numbltinfns + 1]  ; 
integer totalexcount  ; 
bltinrange bltinptr  ; 
#endif /* STAT */
strnumber snull  ; 
strnumber sdefault  ; 
strnumber st  ; 
strnumber sl  ; 
strnumber su  ; 
strnumber spreamble[maxbibfiles + 1]  ; 
integer poplit1, poplit2, poplit3  ; 
stktype poptyp1, poptyp2, poptyp3  ; 
poolpointer spptr  ; 
poolpointer spxptr1, spxptr2  ; 
poolpointer spend  ; 
poolpointer splength, sp2length  ; 
integer spbracelevel  ; 
bufpointer exbufxptr, exbufyptr  ; 
hashloc controlseqloc  ; 
boolean precedingwhite  ; 
boolean andfound  ; 
integer numnames  ; 
bufpointer namebfptr  ; 
bufpointer namebfxptr, namebfyptr  ; 
integer nmbracelevel  ; 
bufpointer nametok[bufsize + 1]  ; 
ASCIIcode namesepchar[bufsize + 1]  ; 
bufpointer numtokens  ; 
boolean tokenstarting  ; 
boolean alphafound  ; 
boolean doubleletter, endofgroup, tobewritten  ; 
bufpointer firststart  ; 
bufpointer firstend  ; 
bufpointer lastend  ; 
bufpointer vonstart  ; 
bufpointer vonend  ; 
bufpointer jrend  ; 
bufpointer curtoken, lasttoken  ; 
boolean usedefault  ; 
bufpointer numcommas  ; 
bufpointer comma1, comma2  ; 
bufpointer numtextchars  ; 
schar conversiontype  ; 
boolean prevcolon  ; 

#include "coerce.h"
void printanewline ( ) 
{(void) putc('\n',  logfile );
  (void) putc('\n',  stdout );
} 
void markwarning ( ) 
{if ( ( history == 1 ) ) 
  incr ( errcount ) ; 
  else if ( ( history == 0 ) ) 
  {
    history = 1 ; 
    errcount = 1 ; 
  } 
} 
void markerror ( ) 
{if ( ( history < 2 ) ) 
  {
    history = 2 ; 
    errcount = 1 ; 
  } 
  else incr ( errcount ) ; 
} 
void markfatal ( ) 
{history = 3 ; 
} 
void printoverflow ( ) 
{{
    
    (void) Fputs( logfile ,  "Sorry---you've exceeded BibTeX's " ) ; 
    (void) Fputs( stdout ,  "Sorry---you've exceeded BibTeX's " ) ; 
  } 
  markfatal () ; 
} 
void printconfusion ( ) 
{{
    
    (void) fprintf( logfile , "%s\n",  "---this can't happen" ) ; 
    (void) fprintf( stdout , "%s\n",  "---this can't happen" ) ; 
  } 
  {
    (void) fprintf( logfile , "%s\n",  "*Please notify the BibTeX maintainer*" ) ; 
    (void) fprintf( stdout , "%s\n",  "*Please notify the BibTeX maintainer*" ) ; 
  } 
  markfatal () ; 
} 
void bufferoverflow ( ) 
{{
    
    printoverflow () ; 
    {
      (void) fprintf( logfile , "%s%ld\n",  "buffer size " , (long)bufsize ) ; 
      (void) fprintf( stdout , "%s%ld\n",  "buffer size " , (long)bufsize ) ; 
    } 
    longjmp(jmp9998,1) ; 
  } 
} 
boolean zinputln ( f ) 
alphafile f ; 
{/* 15 */ register boolean Result; last = 0 ; 
  if ( eof ( f ) ) 
  Result = false ; 
  else {
      
    lineread ( f , bufsize ) ; 
    if ( last >= bufsize ) 
    {
      printoverflow () ; 
      {
	(void) fprintf( logfile , "%s%ld\n",  "buffer size " , (long)bufsize ) ; 
	(void) fprintf( stdout , "%s%ld\n",  "buffer size " , (long)bufsize ) ; 
      } 
      longjmp(jmp9998,1) ; 
    } 
    while ( ( last > 0 ) ) if ( lexclass [ buffer [ last - 1 ] ] == 1 ) 
    decr ( last ) ; 
    else goto lab15 ; 
    lab15: Result = true ; 
  } 
  return(Result) ; 
} 
void zoutpoolstr ( f , s ) 
alphafile f ; 
strnumber s ; 
{poolpointer i  ; 
  if ( ( ( s < 0 ) || ( s >= strptr + 3 ) || ( s >= maxstrings ) ) ) 
  {
    {
      (void) fprintf( logfile , "%s%ld",  "Illegal string number:" , (long)s ) ; 
      (void) fprintf( stdout , "%s%ld",  "Illegal string number:" , (long)s ) ; 
    } 
    printconfusion () ; 
    longjmp(jmp9998,1) ; 
  } 
  {register integer for_end; i = strstart [ s ] ; for_end = strstart [ s + 1 
  ] - 1 ; if ( i <= for_end) do 
    (void) putc( xchr [ strpool [ i ] ] ,  f );
  while ( i++ < for_end ) ; } 
} 
void zprintapoolstr ( s ) 
strnumber s ; 
{outpoolstr ( stdout , s ) ; 
  outpoolstr ( logfile , s ) ; 
} 
void pooloverflow ( ) 
{{
    
    printoverflow () ; 
    {
      (void) fprintf( logfile , "%s%ld\n",  "pool size " , (long)poolsize ) ; 
      (void) fprintf( stdout , "%s%ld\n",  "pool size " , (long)poolsize ) ; 
    } 
    longjmp(jmp9998,1) ; 
  } 
} 
void filenmsizeoverflow ( ) 
{{
    
    printoverflow () ; 
    {
      (void) fprintf( logfile , "%s%ld\n",  "file name size " , (long)1024 ) ; 
      (void) fprintf( stdout , "%s%ld\n",  "file name size " , (long)1024 ) ; 
    } 
    longjmp(jmp9998,1) ; 
  } 
} 
void zouttoken ( f ) 
alphafile f ; 
{bufpointer i  ; 
  i = bufptr1 ; 
  while ( ( i < bufptr2 ) ) {
      
    (void) putc( xchr [ buffer [ i ] ] ,  f );
    incr ( i ) ; 
  } 
} 
void printatoken ( ) 
{outtoken ( stdout ) ; 
  outtoken ( logfile ) ; 
} 
void printbadinputline ( ) 
{bufpointer bfptr  ; 
  {
    (void) Fputs( logfile ,  " : " ) ; 
    (void) Fputs( stdout ,  " : " ) ; 
  } 
  bfptr = 0 ; 
  while ( ( bfptr < bufptr2 ) ) {
      
    if ( ( lexclass [ buffer [ bfptr ] ] == 1 ) ) 
    {
      (void) putc( xchr [ 32 ] ,  logfile );
      (void) putc( xchr [ 32 ] ,  stdout );
    } 
    else {
	
      (void) putc( xchr [ buffer [ bfptr ] ] ,  logfile );
      (void) putc( xchr [ buffer [ bfptr ] ] ,  stdout );
    } 
    incr ( bfptr ) ; 
  } 
  printanewline () ; 
  {
    (void) Fputs( logfile ,  " : " ) ; 
    (void) Fputs( stdout ,  " : " ) ; 
  } 
  bfptr = 0 ; 
  while ( ( bfptr < bufptr2 ) ) {
      
    {
      (void) putc( xchr [ 32 ] ,  logfile );
      (void) putc( xchr [ 32 ] ,  stdout );
    } 
    incr ( bfptr ) ; 
  } 
  bfptr = bufptr2 ; 
  while ( ( bfptr < last ) ) {
      
    if ( ( lexclass [ buffer [ bfptr ] ] == 1 ) ) 
    {
      (void) putc( xchr [ 32 ] ,  logfile );
      (void) putc( xchr [ 32 ] ,  stdout );
    } 
    else {
	
      (void) putc( xchr [ buffer [ bfptr ] ] ,  logfile );
      (void) putc( xchr [ buffer [ bfptr ] ] ,  stdout );
    } 
    incr ( bfptr ) ; 
  } 
  printanewline () ; 
  bfptr = 0 ; 
  while ( ( ( bfptr < bufptr2 ) && ( lexclass [ buffer [ bfptr ] ] == 1 ) ) ) 
  incr ( bfptr ) ; 
  if ( ( bfptr == bufptr2 ) ) 
  {
    (void) fprintf( logfile , "%s\n",  "(Error may have been on previous line)" ) ; 
    (void) fprintf( stdout , "%s\n",  "(Error may have been on previous line)" ) ; 
  } 
  markerror () ; 
} 
void printskippingwhateve ( ) 
{{
    
    (void) Fputs( logfile ,  "I'm skipping whatever remains of this " ) ; 
    (void) Fputs( stdout ,  "I'm skipping whatever remains of this " ) ; 
  } 
} 
void samtoolongfilenamepr ( ) 
{(void) Fputs( stdout ,  "File name `" ) ; 
  nameptr = 0 ; 
  while ( ( nameptr < auxnamelength ) ) {
      
    (void) putc( nameoffile [ nameptr ] ,  stdout );
    incr ( nameptr ) ; 
  } 
  (void) fprintf( stdout , "%s\n",  "' is too long" ) ; 
} 
void samwrongfilenameprin ( ) 
{(void) Fputs( stdout ,  "I couldn't open file name `" ) ; 
  nameptr = 0 ; 
  while ( ( nameptr < namelength ) ) {
      
    (void) putc( nameoffile [ nameptr ] ,  stdout );
    incr ( nameptr ) ; 
  } 
  (void) fprintf( stdout , "%c\n",  '\'' ) ; 
} 
void printauxname ( ) 
{printapoolstr ( auxlist [ auxptr ] ) ; 
  printanewline () ; 
} 
void auxerrprint ( ) 
{{
    
    (void) fprintf( logfile , "%s%ld%s",  "---line " , (long)auxlnstack [ auxptr ] , " of file " ) ; 
    (void) fprintf( stdout , "%s%ld%s",  "---line " , (long)auxlnstack [ auxptr ] , " of file " ) ; 
  } 
  printauxname () ; 
  printbadinputline () ; 
  printskippingwhateve () ; 
  {
    (void) fprintf( logfile , "%s\n",  "command" ) ; 
    (void) fprintf( stdout , "%s\n",  "command" ) ; 
  } 
} 
void zauxerrillegalanother ( cmdnum ) 
integer cmdnum ; 
{{
    
    (void) Fputs( logfile ,  "Illegal, another \\bib" ) ; 
    (void) Fputs( stdout ,  "Illegal, another \\bib" ) ; 
  } 
  switch ( ( cmdnum ) ) 
  {case 0 : 
    {
      (void) Fputs( logfile ,  "data" ) ; 
      (void) Fputs( stdout ,  "data" ) ; 
    } 
    break ; 
  case 1 : 
    {
      (void) Fputs( logfile ,  "style" ) ; 
      (void) Fputs( stdout ,  "style" ) ; 
    } 
    break ; 
    default: 
    {
      {
	(void) Fputs( logfile ,  "Illegal auxiliary-file command" ) ; 
	(void) Fputs( stdout ,  "Illegal auxiliary-file command" ) ; 
      } 
      printconfusion () ; 
      longjmp(jmp9998,1) ; 
    } 
    break ; 
  } 
  {
    (void) Fputs( logfile ,  " command" ) ; 
    (void) Fputs( stdout ,  " command" ) ; 
  } 
} 
void auxerrnorightbracepr ( ) 
{{
    
    (void) fprintf( logfile , "%s%c%c",  "No \"" , xchr [ 125 ] , '"' ) ; 
    (void) fprintf( stdout , "%s%c%c",  "No \"" , xchr [ 125 ] , '"' ) ; 
  } 
} 
void auxerrstuffafterrigh ( ) 
{{
    
    (void) fprintf( logfile , "%s%c%c",  "Stuff after \"" , xchr [ 125 ] , '"' ) ; 
    (void) fprintf( stdout , "%s%c%c",  "Stuff after \"" , xchr [ 125 ] , '"' ) ; 
  } 
} 
void auxerrwhitespaceinar ( ) 
{{
    
    (void) Fputs( logfile ,  "White space in argument" ) ; 
    (void) Fputs( stdout ,  "White space in argument" ) ; 
  } 
} 
void printbibname ( ) 
{printapoolstr ( biblist [ bibptr ] ) ; 
  printapoolstr ( sbibextension ) ; 
  printanewline () ; 
} 
void printbstname ( ) 
{printapoolstr ( bststr ) ; 
  printapoolstr ( sbstextension ) ; 
  printanewline () ; 
} 
void hashciteconfusion ( ) 
{{
    
    {
      (void) Fputs( logfile ,  "Cite hash error" ) ; 
      (void) Fputs( stdout ,  "Cite hash error" ) ; 
    } 
    printconfusion () ; 
    longjmp(jmp9998,1) ; 
  } 
} 
void zcheckciteoverflow ( lastcite ) 
citenumber lastcite ; 
{if ( ( lastcite == maxcites ) ) 
  {
    printapoolstr ( hashtext [ citeloc ] ) ; 
    {
      (void) fprintf( logfile , "%s\n",  " is the key:" ) ; 
      (void) fprintf( stdout , "%s\n",  " is the key:" ) ; 
    } 
    {
      printoverflow () ; 
      {
	(void) fprintf( logfile , "%s%ld\n",  "number of cite keys " , (long)maxcites ) ; 
	(void) fprintf( stdout , "%s%ld\n",  "number of cite keys " , (long)maxcites ) ; 
      } 
      longjmp(jmp9998,1) ; 
    } 
  } 
} 
void auxend1errprint ( ) 
{{
    
    (void) Fputs( logfile ,  "I found no " ) ; 
    (void) Fputs( stdout ,  "I found no " ) ; 
  } 
} 
void auxend2errprint ( ) 
{{
    
    (void) Fputs( logfile ,  "---while reading file " ) ; 
    (void) Fputs( stdout ,  "---while reading file " ) ; 
  } 
  printauxname () ; 
  markerror () ; 
} 
void bstlnnumprint ( ) 
{{
    
    (void) fprintf( logfile , "%s%ld%s",  "--line " , (long)bstlinenum , " of file " ) ; 
    (void) fprintf( stdout , "%s%ld%s",  "--line " , (long)bstlinenum , " of file " ) ; 
  } 
  printbstname () ; 
} 
void bsterrprintandlookfo ( ) 
{{
    
    (void) putc( '-' ,  logfile );
    (void) putc( '-' ,  stdout );
  } 
  bstlnnumprint () ; 
  printbadinputline () ; 
  while ( ( last != 0 ) ) if ( ( ! inputln ( bstfile ) ) ) 
  longjmp(jmp32,1) ; 
  else incr ( bstlinenum ) ; 
  bufptr2 = last ; 
} 
void bstwarnprint ( ) 
{bstlnnumprint () ; 
  markwarning () ; 
} 
void eatbstprint ( ) 
{{
    
    (void) Fputs( logfile ,  "Illegal end of style file in command: " ) ; 
    (void) Fputs( stdout ,  "Illegal end of style file in command: " ) ; 
  } 
} 
void unknwnfunctionclassc ( ) 
{{
    
    {
      (void) Fputs( logfile ,  "Unknown function class" ) ; 
      (void) Fputs( stdout ,  "Unknown function class" ) ; 
    } 
    printconfusion () ; 
    longjmp(jmp9998,1) ; 
  } 
} 
void zprintfnclass ( fnloc ) 
hashloc fnloc ; 
{switch ( ( fntype [ fnloc ] ) ) 
  {case 0 : 
    {
      (void) Fputs( logfile ,  "built-in" ) ; 
      (void) Fputs( stdout ,  "built-in" ) ; 
    } 
    break ; 
  case 1 : 
    {
      (void) Fputs( logfile ,  "wizard-defined" ) ; 
      (void) Fputs( stdout ,  "wizard-defined" ) ; 
    } 
    break ; 
  case 2 : 
    {
      (void) Fputs( logfile ,  "integer-literal" ) ; 
      (void) Fputs( stdout ,  "integer-literal" ) ; 
    } 
    break ; 
  case 3 : 
    {
      (void) Fputs( logfile ,  "string-literal" ) ; 
      (void) Fputs( stdout ,  "string-literal" ) ; 
    } 
    break ; 
  case 4 : 
    {
      (void) Fputs( logfile ,  "field" ) ; 
      (void) Fputs( stdout ,  "field" ) ; 
    } 
    break ; 
  case 5 : 
    {
      (void) Fputs( logfile ,  "integer-entry-variable" ) ; 
      (void) Fputs( stdout ,  "integer-entry-variable" ) ; 
    } 
    break ; 
  case 6 : 
    {
      (void) Fputs( logfile ,  "string-entry-variable" ) ; 
      (void) Fputs( stdout ,  "string-entry-variable" ) ; 
    } 
    break ; 
  case 7 : 
    {
      (void) Fputs( logfile ,  "integer-global-variable" ) ; 
      (void) Fputs( stdout ,  "integer-global-variable" ) ; 
    } 
    break ; 
  case 8 : 
    {
      (void) Fputs( logfile ,  "string-global-variable" ) ; 
      (void) Fputs( stdout ,  "string-global-variable" ) ; 
    } 
    break ; 
    default: 
    unknwnfunctionclassc () ; 
    break ; 
  } 
} 
#ifdef TRACE
void ztraceprfnclass ( fnloc ) 
hashloc fnloc ; 
{switch ( ( fntype [ fnloc ] ) ) 
  {case 0 : 
    {
      (void) Fputs( logfile ,  "built-in" ) ; 
    } 
    break ; 
  case 1 : 
    {
      (void) Fputs( logfile ,  "wizard-defined" ) ; 
    } 
    break ; 
  case 2 : 
    {
      (void) Fputs( logfile ,  "integer-literal" ) ; 
    } 
    break ; 
  case 3 : 
    {
      (void) Fputs( logfile ,  "string-literal" ) ; 
    } 
    break ; 
  case 4 : 
    {
      (void) Fputs( logfile ,  "field" ) ; 
    } 
    break ; 
  case 5 : 
    {
      (void) Fputs( logfile ,  "integer-entry-variable" ) ; 
    } 
    break ; 
  case 6 : 
    {
      (void) Fputs( logfile ,  "string-entry-variable" ) ; 
    } 
    break ; 
  case 7 : 
    {
      (void) Fputs( logfile ,  "integer-global-variable" ) ; 
    } 
    break ; 
  case 8 : 
    {
      (void) Fputs( logfile ,  "string-global-variable" ) ; 
    } 
    break ; 
    default: 
    unknwnfunctionclassc () ; 
    break ; 
  } 
} 
#endif /* TRACE */
void idscanningconfusion ( ) 
{{
    
    {
      (void) Fputs( logfile ,  "Identifier scanning error" ) ; 
      (void) Fputs( stdout ,  "Identifier scanning error" ) ; 
    } 
    printconfusion () ; 
    longjmp(jmp9998,1) ; 
  } 
} 
void bstidprint ( ) 
{if ( ( scanresult == 0 ) ) 
  {
    (void) fprintf( logfile , "%c%c%s",  '"' , xchr [ buffer [ bufptr2 ] ] ,     "\" begins identifier, command: " ) ; 
    (void) fprintf( stdout , "%c%c%s",  '"' , xchr [ buffer [ bufptr2 ] ] ,     "\" begins identifier, command: " ) ; 
  } 
  else if ( ( scanresult == 2 ) ) 
  {
    (void) fprintf( logfile , "%c%c%s",  '"' , xchr [ buffer [ bufptr2 ] ] ,     "\" immediately follows identifier, command: " ) ; 
    (void) fprintf( stdout , "%c%c%s",  '"' , xchr [ buffer [ bufptr2 ] ] ,     "\" immediately follows identifier, command: " ) ; 
  } 
  else idscanningconfusion () ; 
} 
void bstleftbraceprint ( ) 
{{
    
    (void) fprintf( logfile , "%c%c%s",  '"' , xchr [ 123 ] , "\" is missing in command: " ) ; 
    (void) fprintf( stdout , "%c%c%s",  '"' , xchr [ 123 ] , "\" is missing in command: " ) ; 
  } 
} 
void bstrightbraceprint ( ) 
{{
    
    (void) fprintf( logfile , "%c%c%s",  '"' , xchr [ 125 ] , "\" is missing in command: " ) ; 
    (void) fprintf( stdout , "%c%c%s",  '"' , xchr [ 125 ] , "\" is missing in command: " ) ; 
  } 
} 
void zalreadyseenfunctionp ( seenfnloc ) 
hashloc seenfnloc ; 
{/* 10 */ printapoolstr ( hashtext [ seenfnloc ] ) ; 
  {
    (void) Fputs( logfile ,  " is already a type \"" ) ; 
    (void) Fputs( stdout ,  " is already a type \"" ) ; 
  } 
  printfnclass ( seenfnloc ) ; 
  {
    (void) fprintf( logfile , "%s\n",  "\" function name" ) ; 
    (void) fprintf( stdout , "%s\n",  "\" function name" ) ; 
  } 
  {
    bsterrprintandlookfo () ; 
    goto lab10 ; 
  } 
  lab10: ; 
} 
void singlfnoverflow ( ) 
{{
    
    printoverflow () ; 
    {
      (void) fprintf( logfile , "%s%ld\n",  "single function space " , (long)singlefnspace ) ; 
      (void) fprintf( stdout , "%s%ld\n",  "single function space " , (long)singlefnspace ) ; 
    } 
    longjmp(jmp9998,1) ; 
  } 
} 
void biblnnumprint ( ) 
{{
    
    (void) fprintf( logfile , "%s%ld%s",  "--line " , (long)biblinenum , " of file " ) ; 
    (void) fprintf( stdout , "%s%ld%s",  "--line " , (long)biblinenum , " of file " ) ; 
  } 
  printbibname () ; 
} 
void biberrprint ( ) 
{{
    
    (void) putc( '-' ,  logfile );
    (void) putc( '-' ,  stdout );
  } 
  biblnnumprint () ; 
  printbadinputline () ; 
  printskippingwhateve () ; 
  if ( ( atbibcommand ) ) 
  {
    (void) fprintf( logfile , "%s\n",  "command" ) ; 
    (void) fprintf( stdout , "%s\n",  "command" ) ; 
  } 
  else {
      
    (void) fprintf( logfile , "%s\n",  "entry" ) ; 
    (void) fprintf( stdout , "%s\n",  "entry" ) ; 
  } 
} 
void bibwarnprint ( ) 
{biblnnumprint () ; 
  markwarning () ; 
} 
void zcheckfieldoverflow ( totalfields ) 
integer totalfields ; 
{if ( ( totalfields > maxfields ) ) 
  {
    {
      (void) fprintf( logfile , "%ld%s\n",  (long)totalfields , " fields:" ) ; 
      (void) fprintf( stdout , "%ld%s\n",  (long)totalfields , " fields:" ) ; 
    } 
    {
      printoverflow () ; 
      {
	(void) fprintf( logfile , "%s%ld\n",  "total number of fields " , (long)maxfields ) ; 
	(void) fprintf( stdout , "%s%ld\n",  "total number of fields " , (long)maxfields ) ; 
      } 
      longjmp(jmp9998,1) ; 
    } 
  } 
} 
void eatbibprint ( ) 
{/* 10 */ {
    
    {
      (void) Fputs( logfile ,  "Illegal end of database file" ) ; 
      (void) Fputs( stdout ,  "Illegal end of database file" ) ; 
    } 
    biberrprint () ; 
    goto lab10 ; 
  } 
  lab10: ; 
} 
void zbiboneoftwoprint ( char1 , char2 ) 
ASCIIcode char1 ; 
ASCIIcode char2 ; 
{/* 10 */ {
    
    {
      (void) fprintf( logfile , "%s%c%s%c%c",  "I was expecting a `" , xchr [ char1 ] , "' or a `" ,       xchr [ char2 ] , '\'' ) ; 
      (void) fprintf( stdout , "%s%c%s%c%c",  "I was expecting a `" , xchr [ char1 ] , "' or a `" ,       xchr [ char2 ] , '\'' ) ; 
    } 
    biberrprint () ; 
    goto lab10 ; 
  } 
  lab10: ; 
} 
void bibequalssignprint ( ) 
{/* 10 */ {
    
    {
      (void) fprintf( logfile , "%s%c%c",  "I was expecting an \"" , xchr [ 61 ] , '"' ) ; 
      (void) fprintf( stdout , "%s%c%c",  "I was expecting an \"" , xchr [ 61 ] , '"' ) ; 
    } 
    biberrprint () ; 
    goto lab10 ; 
  } 
  lab10: ; 
} 
void bibunbalancedbracesp ( ) 
{/* 10 */ {
    
    {
      (void) Fputs( logfile ,  "Unbalanced braces" ) ; 
      (void) Fputs( stdout ,  "Unbalanced braces" ) ; 
    } 
    biberrprint () ; 
    goto lab10 ; 
  } 
  lab10: ; 
} 
void bibfieldtoolongprint ( ) 
{/* 10 */ {
    
    {
      (void) fprintf( logfile , "%s%ld%s",  "Your field is more than " , (long)bufsize , " characters" ) 
      ; 
      (void) fprintf( stdout , "%s%ld%s",  "Your field is more than " , (long)bufsize , " characters" ) 
      ; 
    } 
    biberrprint () ; 
    goto lab10 ; 
  } 
  lab10: ; 
} 
void macrowarnprint ( ) 
{{
    
    (void) Fputs( logfile ,  "Warning--string name \"" ) ; 
    (void) Fputs( stdout ,  "Warning--string name \"" ) ; 
  } 
  printatoken () ; 
  {
    (void) Fputs( logfile ,  "\" is " ) ; 
    (void) Fputs( stdout ,  "\" is " ) ; 
  } 
} 
void bibidprint ( ) 
{if ( ( scanresult == 0 ) ) 
  {
    (void) Fputs( logfile ,  "You're missing " ) ; 
    (void) Fputs( stdout ,  "You're missing " ) ; 
  } 
  else if ( ( scanresult == 2 ) ) 
  {
    (void) fprintf( logfile , "%c%c%s",  '"' , xchr [ buffer [ bufptr2 ] ] ,     "\" immediately follows " ) ; 
    (void) fprintf( stdout , "%c%c%s",  '"' , xchr [ buffer [ bufptr2 ] ] ,     "\" immediately follows " ) ; 
  } 
  else idscanningconfusion () ; 
} 
void bibcmdconfusion ( ) 
{{
    
    {
      (void) Fputs( logfile ,  "Unknown database-file command" ) ; 
      (void) Fputs( stdout ,  "Unknown database-file command" ) ; 
    } 
    printconfusion () ; 
    longjmp(jmp9998,1) ; 
  } 
} 
void citekeydisappearedco ( ) 
{{
    
    {
      (void) Fputs( logfile ,  "A cite key disappeared" ) ; 
      (void) Fputs( stdout ,  "A cite key disappeared" ) ; 
    } 
    printconfusion () ; 
    longjmp(jmp9998,1) ; 
  } 
} 
void zbadcrossreferencepri ( s ) 
strnumber s ; 
{{
    
    (void) Fputs( logfile ,  "--entry \"" ) ; 
    (void) Fputs( stdout ,  "--entry \"" ) ; 
  } 
  printapoolstr ( citelist [ citeptr ] ) ; 
  {
    (void) fprintf( logfile , "%c\n",  '"' ) ; 
    (void) fprintf( stdout , "%c\n",  '"' ) ; 
  } 
  {
    (void) Fputs( logfile ,  "refers to entry \"" ) ; 
    (void) Fputs( stdout ,  "refers to entry \"" ) ; 
  } 
  printapoolstr ( s ) ; 
} 
void nonexistentcrossrefe ( ) 
{{
    
    (void) Fputs( logfile ,  "A bad cross reference-" ) ; 
    (void) Fputs( stdout ,  "A bad cross reference-" ) ; 
  } 
  badcrossreferencepri ( fieldinfo [ fieldptr ] ) ; 
  {
    (void) fprintf( logfile , "%s\n",  "\", which doesn't exist" ) ; 
    (void) fprintf( stdout , "%s\n",  "\", which doesn't exist" ) ; 
  } 
  markerror () ; 
} 
void zprintmissingentry ( s ) 
strnumber s ; 
{{
    
    (void) Fputs( logfile ,  "Warning--I didn't find a database entry for \"" ) ; 
    (void) Fputs( stdout ,  "Warning--I didn't find a database entry for \"" ) ; 
  } 
  printapoolstr ( s ) ; 
  {
    (void) fprintf( logfile , "%c\n",  '"' ) ; 
    (void) fprintf( stdout , "%c\n",  '"' ) ; 
  } 
  markwarning () ; 
} 
void bstexwarnprint ( ) 
{if ( ( messwithentries ) ) 
  {
    {
      (void) Fputs( logfile ,  " for entry " ) ; 
      (void) Fputs( stdout ,  " for entry " ) ; 
    } 
    printapoolstr ( citelist [ citeptr ] ) ; 
  } 
  printanewline () ; 
  {
    (void) Fputs( logfile ,  "while executing-" ) ; 
    (void) Fputs( stdout ,  "while executing-" ) ; 
  } 
  bstlnnumprint () ; 
  markerror () ; 
} 
void bstmildexwarnprint ( ) 
{if ( ( messwithentries ) ) 
  {
    {
      (void) Fputs( logfile ,  " for entry " ) ; 
      (void) Fputs( stdout ,  " for entry " ) ; 
    } 
    printapoolstr ( citelist [ citeptr ] ) ; 
  } 
  printanewline () ; 
  {
    {
      (void) Fputs( logfile ,  "while executing" ) ; 
      (void) Fputs( stdout ,  "while executing" ) ; 
    } 
    bstwarnprint () ; 
  } 
} 
void bstcantmesswithentri ( ) 
{{
    
    {
      (void) Fputs( logfile ,  "You can't mess with entries here" ) ; 
      (void) Fputs( stdout ,  "You can't mess with entries here" ) ; 
    } 
    bstexwarnprint () ; 
  } 
} 
void illeglliteralconfusi ( ) 
{{
    
    {
      (void) Fputs( logfile ,  "Illegal literal type" ) ; 
      (void) Fputs( stdout ,  "Illegal literal type" ) ; 
    } 
    printconfusion () ; 
    longjmp(jmp9998,1) ; 
  } 
} 
void unknwnliteralconfusi ( ) 
{{
    
    {
      (void) Fputs( logfile ,  "Unknown literal type" ) ; 
      (void) Fputs( stdout ,  "Unknown literal type" ) ; 
    } 
    printconfusion () ; 
    longjmp(jmp9998,1) ; 
  } 
} 
void zprintstklit ( stklt , stktp ) 
integer stklt ; 
stktype stktp ; 
{switch ( ( stktp ) ) 
  {case 0 : 
    {
      (void) fprintf( logfile , "%ld%s",  (long)stklt , " is an integer literal" ) ; 
      (void) fprintf( stdout , "%ld%s",  (long)stklt , " is an integer literal" ) ; 
    } 
    break ; 
  case 1 : 
    {
      {
	(void) putc( '"' ,  logfile );
	(void) putc( '"' ,  stdout );
      } 
      printapoolstr ( stklt ) ; 
      {
	(void) Fputs( logfile ,  "\" is a string literal" ) ; 
	(void) Fputs( stdout ,  "\" is a string literal" ) ; 
      } 
    } 
    break ; 
  case 2 : 
    {
      {
	(void) putc( '`' ,  logfile );
	(void) putc( '`' ,  stdout );
      } 
      printapoolstr ( hashtext [ stklt ] ) ; 
      {
	(void) Fputs( logfile ,  "' is a function literal" ) ; 
	(void) Fputs( stdout ,  "' is a function literal" ) ; 
      } 
    } 
    break ; 
  case 3 : 
    {
      {
	(void) putc( '`' ,  logfile );
	(void) putc( '`' ,  stdout );
      } 
      printapoolstr ( stklt ) ; 
      {
	(void) Fputs( logfile ,  "' is a missing field" ) ; 
	(void) Fputs( stdout ,  "' is a missing field" ) ; 
      } 
    } 
    break ; 
  case 4 : 
    illeglliteralconfusi () ; 
    break ; 
    default: 
    unknwnliteralconfusi () ; 
    break ; 
  } 
} 
void zprintlit ( stklt , stktp ) 
integer stklt ; 
stktype stktp ; 
{switch ( ( stktp ) ) 
  {case 0 : 
    {
      (void) fprintf( logfile , "%ld\n",  (long)stklt ) ; 
      (void) fprintf( stdout , "%ld\n",  (long)stklt ) ; 
    } 
    break ; 
  case 1 : 
    {
      printapoolstr ( stklt ) ; 
      printanewline () ; 
    } 
    break ; 
  case 2 : 
    {
      printapoolstr ( hashtext [ stklt ] ) ; 
      printanewline () ; 
    } 
    break ; 
  case 3 : 
    {
      printapoolstr ( stklt ) ; 
      printanewline () ; 
    } 
    break ; 
  case 4 : 
    illeglliteralconfusi () ; 
    break ; 
    default: 
    unknwnliteralconfusi () ; 
    break ; 
  } 
} 
void outputbblline ( ) 
{/* 15 10 */ if ( ( outbuflength != 0 ) ) 
  {
    while ( ( outbuflength > 0 ) ) if ( ( lexclass [ outbuf [ outbuflength - 1 
    ] ] == 1 ) ) 
    decr ( outbuflength ) ; 
    else goto lab15 ; 
    lab15: if ( ( outbuflength == 0 ) ) 
    goto lab10 ; 
    outbufptr = 0 ; 
    while ( ( outbufptr < outbuflength ) ) {
	
      (void) putc( xchr [ outbuf [ outbufptr ] ] ,  bblfile );
      incr ( outbufptr ) ; 
    } 
  } 
  (void) putc('\n',  bblfile );
  incr ( bbllinenum ) ; 
  outbuflength = 0 ; 
  lab10: ; 
} 
void bst1printstringsizee ( ) 
{{
    
    (void) Fputs( logfile ,  "Warning--you've exceeded " ) ; 
    (void) Fputs( stdout ,  "Warning--you've exceeded " ) ; 
  } 
} 
void bst2printstringsizee ( ) 
{{
    
    (void) Fputs( logfile ,  "-string-size," ) ; 
    (void) Fputs( stdout ,  "-string-size," ) ; 
  } 
  bstmildexwarnprint () ; 
  {
    (void) fprintf( logfile , "%s\n",  "*Please notify the bibstyle designer*" ) ; 
    (void) fprintf( stdout , "%s\n",  "*Please notify the bibstyle designer*" ) ; 
  } 
} 
void zbracesunbalancedcomp ( poplitvar ) 
strnumber poplitvar ; 
{{
    
    (void) Fputs( logfile ,  "Warning--\"" ) ; 
    (void) Fputs( stdout ,  "Warning--\"" ) ; 
  } 
  printapoolstr ( poplitvar ) ; 
  {
    {
      (void) Fputs( logfile ,  "\" isn't a brace-balanced string" ) ; 
      (void) Fputs( stdout ,  "\" isn't a brace-balanced string" ) ; 
    } 
    bstmildexwarnprint () ; 
  } 
} 
void caseconversionconfus ( ) 
{{
    
    {
      (void) Fputs( logfile ,  "Unknown type of case conversion" ) ; 
      (void) Fputs( stdout ,  "Unknown type of case conversion" ) ; 
    } 
    printconfusion () ; 
    longjmp(jmp9998,1) ; 
  } 
} 
void traceandstatprinting ( ) 
{
	;
#ifdef TRACE
  {
    if ( ( numbibfiles == 1 ) ) 
    {
      (void) fprintf( logfile , "%s\n",  "The 1 database file is" ) ; 
    } 
    else {
	
      (void) fprintf( logfile , "%s%ld%s\n",  "The " , (long)numbibfiles , " database files are" ) ; 
    } 
    if ( ( numbibfiles == 0 ) ) 
    {
      (void) fprintf( logfile , "%s\n",  "   undefined" ) ; 
    } 
    else {
	
      bibptr = 0 ; 
      while ( ( bibptr < numbibfiles ) ) {
	  
	{
	  (void) Fputs( logfile ,  "   " ) ; 
	} 
	{
	  outpoolstr ( logfile , biblist [ bibptr ] ) ; 
	} 
	{
	  outpoolstr ( logfile , sbibextension ) ; 
	} 
	{
	  (void) putc('\n',  logfile );
	} 
	incr ( bibptr ) ; 
      } 
    } 
    {
      (void) Fputs( logfile ,  "The style file is " ) ; 
    } 
    if ( ( bststr == 0 ) ) 
    {
      (void) fprintf( logfile , "%s\n",  "undefined" ) ; 
    } 
    else {
	
      {
	outpoolstr ( logfile , bststr ) ; 
      } 
      {
	outpoolstr ( logfile , sbstextension ) ; 
      } 
      {
	(void) putc('\n',  logfile );
      } 
    } 
  } 
  {
    if ( ( allentries ) ) 
    {
      (void) fprintf( logfile , "%s%ld%s",  "all_marker=" , (long)allmarker , ", " ) ; 
    } 
    if ( ( readperformed ) ) 
    {
      (void) fprintf( logfile , "%s%ld\n",  "old_num_cites=" , (long)oldnumcites ) ; 
    } 
    else {
	
      (void) putc('\n',  logfile );
    } 
    {
      (void) fprintf( logfile , "%s%ld",  "The " , (long)numcites ) ; 
    } 
    if ( ( numcites == 1 ) ) 
    {
      (void) fprintf( logfile , "%s\n",  " entry:" ) ; 
    } 
    else {
	
      (void) fprintf( logfile , "%s\n",  " entries:" ) ; 
    } 
    if ( ( numcites == 0 ) ) 
    {
      (void) fprintf( logfile , "%s\n",  "   undefined" ) ; 
    } 
    else {
	
      sortciteptr = 0 ; 
      while ( ( sortciteptr < numcites ) ) {
	  
	if ( ( ! readcompleted ) ) 
	citeptr = sortciteptr ; 
	else citeptr = citeinfo [ sortciteptr ] ; 
	{
	  outpoolstr ( logfile , citelist [ citeptr ] ) ; 
	} 
	if ( ( readperformed ) ) 
	{
	  {
	    (void) Fputs( logfile ,  ", entry-type " ) ; 
	  } 
	  if ( ( typelist [ citeptr ] == 5001 ) ) 
	  lab5001: {
	      
	    (void) Fputs( logfile ,  "unknown" ) ; 
	  } 
	  else if ( ( typelist [ citeptr ] == 0 ) ) 
	  {
	    (void) Fputs( logfile ,  "--- no type found" ) ; 
	  } 
	  else {
	      
	    outpoolstr ( logfile , hashtext [ typelist [ citeptr ] ] ) ; 
	  } 
	  {
	    (void) fprintf( logfile , "%s\n",  ", has entry strings" ) ; 
	  } 
	  {
	    if ( ( numentstrs == 0 ) ) 
	    {
	      (void) fprintf( logfile , "%s\n",  "    undefined" ) ; 
	    } 
	    else if ( ( ! readcompleted ) ) 
	    {
	      (void) fprintf( logfile , "%s\n",  "    uninitialized" ) ; 
	    } 
	    else {
		
	      strentptr = citeptr * numentstrs ; 
	      while ( ( strentptr < ( citeptr + 1 ) * numentstrs ) ) {
		  
		entchrptr = 0 ; 
		{
		  (void) Fputs( logfile ,  "    \"" ) ; 
		} 
		while ( ( entrystrs [ strentptr ] [ entchrptr ] != 127 ) ) {
		    
		  {
		    (void) putc( xchr [ entrystrs [ strentptr ] [                     entchrptr ] ] ,  logfile );
		  } 
		  incr ( entchrptr ) ; 
		} 
		{
		  (void) fprintf( logfile , "%c\n",  '"' ) ; 
		} 
		incr ( strentptr ) ; 
	      } 
	    } 
	  } 
	  {
	    (void) Fputs( logfile ,  "  has entry integers" ) ; 
	  } 
	  {
	    if ( ( numentints == 0 ) ) 
	    {
	      (void) Fputs( logfile ,  " undefined" ) ; 
	    } 
	    else if ( ( ! readcompleted ) ) 
	    {
	      (void) Fputs( logfile ,  " uninitialized" ) ; 
	    } 
	    else {
		
	      intentptr = citeptr * numentints ; 
	      while ( ( intentptr < ( citeptr + 1 ) * numentints ) ) {
		  
		{
		  (void) fprintf( logfile , "%c%ld",  ' ' , (long)entryints [ intentptr ] ) ; 
		} 
		incr ( intentptr ) ; 
	      } 
	    } 
	    {
	      (void) putc('\n',  logfile );
	    } 
	  } 
	  {
	    (void) fprintf( logfile , "%s\n",  "  and has fields" ) ; 
	  } 
	  {
	    if ( ( ! readperformed ) ) 
	    {
	      (void) fprintf( logfile , "%s\n",  "    uninitialized" ) ; 
	    } 
	    else {
		
	      fieldptr = citeptr * numfields ; 
	      fieldendptr = fieldptr + numfields ; 
	      nofields = true ; 
	      while ( ( fieldptr < fieldendptr ) ) {
		  
		if ( ( fieldinfo [ fieldptr ] != 0 ) ) 
		{
		  {
		    (void) Fputs( logfile ,  "    \"" ) ; 
		  } 
		  {
		    outpoolstr ( logfile , fieldinfo [ fieldptr ] ) ; 
		  } 
		  {
		    (void) fprintf( logfile , "%c\n",  '"' ) ; 
		  } 
		  nofields = false ; 
		} 
		incr ( fieldptr ) ; 
	      } 
	      if ( ( nofields ) ) 
	      {
		(void) fprintf( logfile , "%s\n",  "    missing" ) ; 
	      } 
	    } 
	  } 
	} 
	else {
	    
	  (void) putc('\n',  logfile );
	} 
	incr ( sortciteptr ) ; 
      } 
    } 
  } 
  {
    {
      (void) fprintf( logfile , "%s\n",  "The wiz-defined functions are" ) ; 
    } 
    if ( ( wizdefptr == 0 ) ) 
    {
      (void) fprintf( logfile , "%s\n",  "   nonexistent" ) ; 
    } 
    else {
	
      wizfnptr = 0 ; 
      while ( ( wizfnptr < wizdefptr ) ) {
	  
	if ( ( wizfunctions [ wizfnptr ] == 5001 ) ) 
	{
	  (void) fprintf( logfile , "%ld%s\n",  (long)wizfnptr , "--end-of-def--" ) ; 
	} 
	else if ( ( wizfunctions [ wizfnptr ] == 0 ) ) 
	{
	  (void) fprintf( logfile , "%ld%s",  (long)wizfnptr , "  quote_next_function    " ) ; 
	} 
	else {
	    
	  {
	    (void) fprintf( logfile , "%ld%s",  (long)wizfnptr , "  `" ) ; 
	  } 
	  {
	    outpoolstr ( logfile , hashtext [ wizfunctions [ wizfnptr ] ] ) ; 
	  } 
	  {
	    (void) fprintf( logfile , "%c\n",  '\'' ) ; 
	  } 
	} 
	incr ( wizfnptr ) ; 
      } 
    } 
  } 
  {
    {
      (void) fprintf( logfile , "%s\n",  "The string pool is" ) ; 
    } 
    strnum = 1 ; 
    while ( ( strnum < strptr ) ) {
	
      {
	(void) fprintf( logfile , "%ld%ld%s",  (long)strnum , (long)strstart [ strnum ] , " \"" ) ; 
      } 
      {
	outpoolstr ( logfile , strnum ) ; 
      } 
      {
	(void) fprintf( logfile , "%c\n",  '"' ) ; 
      } 
      incr ( strnum ) ; 
    } 
  } 
#endif /* TRACE */
	;
#ifdef STAT
  {
    {
      (void) fprintf( logfile , "%s%ld",  "You've used " , (long)numcites ) ; 
    } 
    if ( ( numcites == 1 ) ) 
    {
      (void) fprintf( logfile , "%s\n",  " entry," ) ; 
    } 
    else {
	
      (void) fprintf( logfile , "%s\n",  " entries," ) ; 
    } 
    {
      (void) fprintf( logfile , "%s%ld%s\n",  "            " , (long)wizdefptr ,       " wiz_defined-function locations," ) ; 
    } 
    {
      (void) fprintf( logfile , "%s%ld%s%ld%s\n",  "            " , (long)strptr , " strings with " ,       (long)strstart [ strptr ] , " characters," ) ; 
    } 
    bltinptr = 0 ; 
    totalexcount = 0 ; 
    while ( ( bltinptr < numbltinfns ) ) {
	
      totalexcount = totalexcount + executioncount [ bltinptr ] ; 
      incr ( bltinptr ) ; 
    } 
    {
      (void) fprintf( logfile , "%s%ld%s\n",  "and the built_in function-call counts, " ,       (long)totalexcount , " in all, are:" ) ; 
    } 
    bltinptr = 0 ; 
    while ( ( bltinptr < numbltinfns ) ) {
	
      {
	outpoolstr ( logfile , hashtext [ bltinloc [ bltinptr ] ] ) ; 
      } 
      {
	(void) fprintf( logfile , "%s%ld\n",  " -- " , (long)executioncount [ bltinptr ] ) ; 
      } 
      incr ( bltinptr ) ; 
    } 
  } 
#endif /* STAT */
} 
boolean zaopenin ( f , pathspec ) 
palphafile * f ; 
integer pathspec ; 
{register boolean Result; boolean ok  ; 
  if ( testaccess ( 4 , pathspec ) ) 
  {
    reset ( *f , realnameoffile ) ; 
    ok = true ; 
  } 
  else ok = false ; 
  Result = ok ; 
  return(Result) ; 
} 
boolean zaopenout ( f ) 
palphafile * f ; 
{register boolean Result; boolean ok  ; 
  if ( testaccess ( 2 , 0 ) ) 
  {
    rewrite ( *f , realnameoffile ) ; 
    ok = true ; 
  } 
  else ok = false ; 
  Result = ok ; 
  return(Result) ; 
} 
void zstartname ( filename ) 
strnumber filename ; 
{poolpointer pptr  ; 
  if ( ( ( strstart [ filename + 1 ] - strstart [ filename ] ) > 1024 ) ) 
  {
    {
      (void) Fputs( logfile ,  "File=" ) ; 
      (void) Fputs( stdout ,  "File=" ) ; 
    } 
    printapoolstr ( filename ) ; 
    {
      (void) fprintf( logfile , "%c\n",  ',' ) ; 
      (void) fprintf( stdout , "%c\n",  ',' ) ; 
    } 
    filenmsizeoverflow () ; 
  } 
  nameptr = 0 ; 
  pptr = strstart [ filename ] ; 
  while ( ( pptr < strstart [ filename + 1 ] ) ) {
      
    nameoffile [ nameptr ] = chr ( strpool [ pptr ] ) ; 
    incr ( nameptr ) ; 
    incr ( pptr ) ; 
  } 
  namelength = ( strstart [ filename + 1 ] - strstart [ filename ] ) ; 
} 
void zaddextension ( ext ) 
strnumber ext ; 
{poolpointer pptr  ; 
  if ( ( namelength + ( strstart [ ext + 1 ] - strstart [ ext ] ) > 1024 ) ) 
  {
    {
      (void) fprintf( logfile , "%s%c%s",  "File=" , nameoffile , ", extension=" ) ; 
      (void) fprintf( stdout , "%s%c%s",  "File=" , nameoffile , ", extension=" ) ; 
    } 
    printapoolstr ( ext ) ; 
    {
      (void) fprintf( logfile , "%c\n",  ',' ) ; 
      (void) fprintf( stdout , "%c\n",  ',' ) ; 
    } 
    filenmsizeoverflow () ; 
  } 
  nameptr = namelength ; 
  pptr = strstart [ ext ] ; 
  while ( ( pptr < strstart [ ext + 1 ] ) ) {
      
    nameoffile [ nameptr ] = chr ( strpool [ pptr ] ) ; 
    incr ( nameptr ) ; 
    incr ( pptr ) ; 
  } 
  namelength = namelength + ( strstart [ ext + 1 ] - strstart [ ext ] ) ; 
  nameoffile [ namelength ] = ' ' ; 
} 
void zaddarea ( area ) 
strnumber area ; 
{poolpointer pptr  ; 
  if ( ( namelength + ( strstart [ area + 1 ] - strstart [ area ] ) > 1024 ) ) 
  {
    {
      (void) Fputs( logfile ,  "File=" ) ; 
      (void) Fputs( stdout ,  "File=" ) ; 
    } 
    printapoolstr ( area ) ; 
    printstr ( nameoffile , ',' ) ; 
    filenmsizeoverflow () ; 
  } 
  nameptr = namelength ; 
  while ( ( nameptr > 0 ) ) {
      
    nameoffile [ nameptr + ( strstart [ area + 1 ] - strstart [ area ] ) ] = 
    nameoffile [ nameptr ] ; 
    decr ( nameptr ) ; 
  } 
  nameptr = 0 ; 
  pptr = strstart [ area ] ; 
  while ( ( pptr < strstart [ area + 1 ] ) ) {
      
    nameoffile [ nameptr ] = chr ( strpool [ pptr ] ) ; 
    incr ( nameptr ) ; 
    incr ( pptr ) ; 
  } 
  namelength = namelength + ( strstart [ area + 1 ] - strstart [ area ] ) ; 
} 
strnumber makestring ( ) 
{register strnumber Result; if ( ( strptr == maxstrings ) ) 
  {
    printoverflow () ; 
    {
      (void) fprintf( logfile , "%s%ld\n",  "number of strings " , (long)maxstrings ) ; 
      (void) fprintf( stdout , "%s%ld\n",  "number of strings " , (long)maxstrings ) ; 
    } 
    longjmp(jmp9998,1) ; 
  } 
  incr ( strptr ) ; 
  strstart [ strptr ] = poolptr ; 
  Result = strptr - 1 ; 
  return(Result) ; 
} 
boolean zstreqbuf ( s , buf , bfptr , len ) 
strnumber s ; 
buftype buf ; 
bufpointer bfptr ; 
bufpointer len ; 
{/* 10 */ register boolean Result; bufpointer i  ; 
  poolpointer j  ; 
  if ( ( ( strstart [ s + 1 ] - strstart [ s ] ) != len ) ) 
  {
    Result = false ; 
    goto lab10 ; 
  } 
  i = bfptr ; 
  j = strstart [ s ] ; 
  while ( ( j < strstart [ s + 1 ] ) ) {
      
    if ( ( strpool [ j ] != buf [ i ] ) ) 
    {
      Result = false ; 
      goto lab10 ; 
    } 
    incr ( i ) ; 
    incr ( j ) ; 
  } 
  Result = true ; 
  lab10: ; 
  return(Result) ; 
} 
boolean zstreqstr ( s1 , s2 ) 
strnumber s1 ; 
strnumber s2 ; 
{/* 10 */ register boolean Result; if ( ( ( strstart [ s1 + 1 ] - strstart [ 
  s1 ] ) != ( strstart [ s2 + 1 ] - strstart [ s2 ] ) ) ) 
  {
    Result = false ; 
    goto lab10 ; 
  } 
  pptr1 = strstart [ s1 ] ; 
  pptr2 = strstart [ s2 ] ; 
  while ( ( pptr1 < strstart [ s1 + 1 ] ) ) {
      
    if ( ( strpool [ pptr1 ] != strpool [ pptr2 ] ) ) 
    {
      Result = false ; 
      goto lab10 ; 
    } 
    incr ( pptr1 ) ; 
    incr ( pptr2 ) ; 
  } 
  Result = true ; 
  lab10: ; 
  return(Result) ; 
} 
void zlowercase ( buf , bfptr , len ) 
buftype buf ; 
bufpointer bfptr ; 
bufpointer len ; 
{bufpointer i  ; 
  if ( ( len > 0 ) ) 
  {register integer for_end; i = bfptr ; for_end = bfptr + len - 1 ; if ( i 
  <= for_end) do 
    if ( ( ( buf [ i ] >= 65 ) && ( buf [ i ] <= 90 ) ) ) 
    buf [ i ] = buf [ i ] + 32 ; 
  while ( i++ < for_end ) ; } 
} 
void zuppercase ( buf , bfptr , len ) 
buftype buf ; 
bufpointer bfptr ; 
bufpointer len ; 
{bufpointer i  ; 
  if ( ( len > 0 ) ) 
  {register integer for_end; i = bfptr ; for_end = bfptr + len - 1 ; if ( i 
  <= for_end) do 
    if ( ( ( buf [ i ] >= 97 ) && ( buf [ i ] <= 122 ) ) ) 
    buf [ i ] = buf [ i ] - 32 ; 
  while ( i++ < for_end ) ; } 
} 
hashloc zstrlookup ( buf , j , l , ilk , insertit ) 
buftype buf ; 
bufpointer j ; 
bufpointer l ; 
strilk ilk ; 
boolean insertit ; 
{/* 40 45 */ register hashloc Result; short h  ; 
  hashloc p  ; 
  bufpointer k  ; 
  boolean oldstring  ; 
  strnumber strnum  ; 
  {
    h = 0 ; 
    k = j ; 
    while ( ( k < j + l ) ) {
	
      h = h + h + buf [ k ] ; 
      while ( ( h >= 4253 ) ) h = h - 4253 ; 
      incr ( k ) ; 
    } 
  } 
  p = h + 1 ; 
  hashfound = false ; 
  oldstring = false ; 
  while ( true ) {
      
    {
      if ( ( hashtext [ p ] > 0 ) ) 
      if ( ( streqbuf ( hashtext [ p ] , buf , j , l ) ) ) 
      if ( ( hashilk [ p ] == ilk ) ) 
      {
	hashfound = true ; 
	goto lab40 ; 
      } 
      else {
	  
	oldstring = true ; 
	strnum = hashtext [ p ] ; 
      } 
    } 
    if ( ( hashnext [ p ] == 0 ) ) 
    {
      if ( ( ! insertit ) ) 
      goto lab45 ; 
      {
	if ( ( hashtext [ p ] > 0 ) ) 
	{
	  do {
	      if ( ( ( hashused == 1 ) ) ) 
	    {
	      printoverflow () ; 
	      {
		(void) fprintf( logfile , "%s%ld\n",  "hash size " , (long)5000 ) ; 
		(void) fprintf( stdout , "%s%ld\n",  "hash size " , (long)5000 ) ; 
	      } 
	      longjmp(jmp9998,1) ; 
	    } 
	    decr ( hashused ) ; 
	  } while ( ! ( ( hashtext [ hashused ] == 0 ) ) ) ; 
	  hashnext [ p ] = hashused ; 
	  p = hashused ; 
	} 
	if ( ( oldstring ) ) 
	hashtext [ p ] = strnum ; 
	else {
	    
	  {
	    if ( ( poolptr + l > poolsize ) ) 
	    pooloverflow () ; 
	  } 
	  k = j ; 
	  while ( ( k < j + l ) ) {
	      
	    {
	      strpool [ poolptr ] = buf [ k ] ; 
	      incr ( poolptr ) ; 
	    } 
	    incr ( k ) ; 
	  } 
	  hashtext [ p ] = makestring () ; 
	} 
	hashilk [ p ] = ilk ; 
      } 
      goto lab40 ; 
    } 
    p = hashnext [ p ] ; 
  } 
  lab45: ; 
  lab40: Result = p ; 
  return(Result) ; 
} 
void zpredefine ( pds , len , ilk ) 
pdstype pds ; 
pdslen len ; 
strilk ilk ; 
{pdslen i  ; 
  {register integer for_end; i = 1 ; for_end = len ; if ( i <= for_end) do 
    buffer [ i ] = xord [ pds [ i - 1 ] ] ; 
  while ( i++ < for_end ) ; } 
  predefloc = strlookup ( buffer , 1 , len , ilk , true ) ; 
} 
void zinttoASCII ( inte , intbuf , intbegin , intend ) 
integer inte ; 
buftype intbuf ; 
bufpointer intbegin ; 
bufpointer * intend ; 
{bufpointer intptr, intxptr  ; 
  ASCIIcode inttmpval  ; 
  intptr = intbegin ; 
  if ( ( inte < 0 ) ) 
  {
    {
      if ( ( intptr == bufsize ) ) 
      bufferoverflow () ; 
      intbuf [ intptr ] = 45 ; 
      incr ( intptr ) ; 
    } 
    inte = - (integer) inte ; 
  } 
  intxptr = intptr ; 
  do {
      { 
      if ( ( intptr == bufsize ) ) 
      bufferoverflow () ; 
      intbuf [ intptr ] = 48 + ( inte % 10 ) ; 
      incr ( intptr ) ; 
    } 
    inte = inte / 10 ; 
  } while ( ! ( ( inte == 0 ) ) ) ; 
*  intend = intptr ; 
  decr ( intptr ) ; 
  while ( ( intxptr < intptr ) ) {
      
    inttmpval = intbuf [ intxptr ] ; 
    intbuf [ intxptr ] = intbuf [ intptr ] ; 
    intbuf [ intptr ] = inttmpval ; 
    decr ( intptr ) ; 
    incr ( intxptr ) ; 
  } 
} 
void zadddatabasecite ( newcite ) 
citenumber * newcite ; 
{checkciteoverflow ( *newcite ) ; 
  checkfieldoverflow ( numfields * *newcite ) ; 
  citelist [ *newcite ] = hashtext [ citeloc ] ; 
  ilkinfo [ citeloc ] = *newcite ; 
  ilkinfo [ lcciteloc ] = citeloc ; 
  incr ( *newcite ) ; 
} 
boolean zfindcitelocsforthisc ( citestr ) 
strnumber citestr ; 
{register boolean Result; exbufptr = 0 ; 
  tmpptr = strstart [ citestr ] ; 
  tmpendptr = strstart [ citestr + 1 ] ; 
  while ( ( tmpptr < tmpendptr ) ) {
      
    exbuf [ exbufptr ] = strpool [ tmpptr ] ; 
    incr ( exbufptr ) ; 
    incr ( tmpptr ) ; 
  } 
  citeloc = strlookup ( exbuf , 0 , ( strstart [ citestr + 1 ] - strstart [ 
  citestr ] ) , 9 , false ) ; 
  citehashfound = hashfound ; 
  lowercase ( exbuf , 0 , ( strstart [ citestr + 1 ] - strstart [ citestr ] ) 
  ) ; 
  lcciteloc = strlookup ( exbuf , 0 , ( strstart [ citestr + 1 ] - strstart [ 
  citestr ] ) , 10 , false ) ; 
  if ( ( hashfound ) ) 
  Result = true ; 
  else Result = false ; 
  return(Result) ; 
} 
void zswap ( swap1 , swap2 ) 
citenumber swap1 ; 
citenumber swap2 ; 
{citenumber innocentbystander  ; 
  innocentbystander = citeinfo [ swap2 ] ; 
  citeinfo [ swap2 ] = citeinfo [ swap1 ] ; 
  citeinfo [ swap1 ] = innocentbystander ; 
} 
boolean zlessthan ( arg1 , arg2 ) 
citenumber arg1 ; 
citenumber arg2 ; 
{/* 10 */ register boolean Result; integer charptr  ; 
  strentloc ptr1, ptr2  ; 
  ASCIIcode char1, char2  ; 
  ptr1 = arg1 * numentstrs + sortkeynum ; 
  ptr2 = arg2 * numentstrs + sortkeynum ; 
  charptr = 0 ; 
  while ( true ) {
      
    char1 = entrystrs [ ptr1 ] [ charptr ] ; 
    char2 = entrystrs [ ptr2 ] [ charptr ] ; 
    if ( ( char1 == 127 ) ) 
    if ( ( char2 == 127 ) ) 
    if ( ( arg1 < arg2 ) ) 
    {
      Result = true ; 
      goto lab10 ; 
    } 
    else if ( ( arg1 > arg2 ) ) 
    {
      Result = false ; 
      goto lab10 ; 
    } 
    else {
	
      {
	(void) Fputs( logfile ,  "Duplicate sort key" ) ; 
	(void) Fputs( stdout ,  "Duplicate sort key" ) ; 
      } 
      printconfusion () ; 
      longjmp(jmp9998,1) ; 
    } 
    else {
	
      Result = true ; 
      goto lab10 ; 
    } 
    else if ( ( char2 == 127 ) ) 
    {
      Result = false ; 
      goto lab10 ; 
    } 
    else if ( ( char1 < char2 ) ) 
    {
      Result = true ; 
      goto lab10 ; 
    } 
    else if ( ( char1 > char2 ) ) 
    {
      Result = false ; 
      goto lab10 ; 
    } 
    incr ( charptr ) ; 
  } 
  lab10: ; 
  return(Result) ; 
} 
void zquicksort ( leftend , rightend ) 
citenumber leftend ; 
citenumber rightend ; 
{/* 24 */ citenumber left, right  ; 
  citenumber insertptr  ; 
  citenumber middle  ; 
  citenumber partition  ; 
	;
#ifdef TRACE
  {
    (void) fprintf( logfile , "%s%ld%s%ld\n",  "Sorting " , (long)leftend , " through " , (long)rightend ) ; 
  } 
#endif /* TRACE */
  if ( ( rightend - leftend < 10 ) ) 
  {
    {register integer for_end; insertptr = leftend + 1 ; for_end = rightend 
    ; if ( insertptr <= for_end) do 
      {
	{register integer for_end; right = insertptr ; for_end = leftend + 1 
	; if ( right >= for_end) do 
	  {
	    if ( ( lessthan ( citeinfo [ right - 1 ] , citeinfo [ right ] ) ) 
	    ) 
	    goto lab24 ; 
	    swap ( right - 1 , right ) ; 
	  } 
	while ( right-- > for_end ) ; } 
	lab24: ; 
      } 
    while ( insertptr++ < for_end ) ; } 
  } 
  else {
      
    {
      left = leftend + 4 ; 
      middle = ( leftend + rightend ) / 2 ; 
      right = rightend - 4 ; 
      if ( ( lessthan ( citeinfo [ left ] , citeinfo [ middle ] ) ) ) 
      if ( ( lessthan ( citeinfo [ middle ] , citeinfo [ right ] ) ) ) 
      swap ( leftend , middle ) ; 
      else if ( ( lessthan ( citeinfo [ left ] , citeinfo [ right ] ) ) ) 
      swap ( leftend , right ) ; 
      else swap ( leftend , left ) ; 
      else if ( ( lessthan ( citeinfo [ right ] , citeinfo [ middle ] ) ) ) 
      swap ( leftend , middle ) ; 
      else if ( ( lessthan ( citeinfo [ right ] , citeinfo [ left ] ) ) ) 
      swap ( leftend , right ) ; 
      else swap ( leftend , left ) ; 
    } 
    {
      partition = citeinfo [ leftend ] ; 
      left = leftend + 1 ; 
      right = rightend ; 
      do {
	  while ( ( lessthan ( citeinfo [ left ] , partition ) ) ) incr ( 
	left ) ; 
	while ( ( lessthan ( partition , citeinfo [ right ] ) ) ) decr ( right 
	) ; 
	if ( ( left < right ) ) 
	{
	  swap ( left , right ) ; 
	  incr ( left ) ; 
	  decr ( right ) ; 
	} 
      } while ( ! ( ( left == right + 1 ) ) ) ; 
      swap ( leftend , right ) ; 
      quicksort ( leftend , right - 1 ) ; 
      quicksort ( left , rightend ) ; 
    } 
  } 
} 
void zbuildin ( pds , len , fnhashloc , bltinnum ) 
pdstype pds ; 
pdslen len ; 
hashloc * fnhashloc ; 
bltinrange bltinnum ; 
{predefine ( pds , len , 11 ) ; 
*  fnhashloc = predefloc ; 
  fntype [ *fnhashloc ] = 0 ; 
  ilkinfo [ *fnhashloc ] = bltinnum ; 
	;
#ifdef STAT
  bltinloc [ bltinnum ] = *fnhashloc ; 
  executioncount [ bltinnum ] = 0 ; 
#endif /* STAT */
} 
void predefcertainstrings ( ) 
{predefine ( ".aux        " , 4 , 7 ) ; 
  sauxextension = hashtext [ predefloc ] ; 
  predefine ( ".bbl        " , 4 , 7 ) ; 
  sbblextension = hashtext [ predefloc ] ; 
  predefine ( ".blg        " , 4 , 7 ) ; 
  slogextension = hashtext [ predefloc ] ; 
  predefine ( ".bst        " , 4 , 7 ) ; 
  sbstextension = hashtext [ predefloc ] ; 
  predefine ( ".bib        " , 4 , 7 ) ; 
  sbibextension = hashtext [ predefloc ] ; 
  predefine ( "texinputs:  " , 10 , 8 ) ; 
  sbstarea = hashtext [ predefloc ] ; 
  predefine ( "texbib:     " , 7 , 8 ) ; 
  sbibarea = hashtext [ predefloc ] ; 
  predefine ( "\\citation   " , 9 , 2 ) ; 
  ilkinfo [ predefloc ] = 2 ; 
  predefine ( "\\bibdata    " , 8 , 2 ) ; 
  ilkinfo [ predefloc ] = 0 ; 
  predefine ( "\\bibstyle   " , 9 , 2 ) ; 
  ilkinfo [ predefloc ] = 1 ; 
  predefine ( "\\@input     " , 7 , 2 ) ; 
  ilkinfo [ predefloc ] = 3 ; 
  predefine ( "entry       " , 5 , 4 ) ; 
  ilkinfo [ predefloc ] = 0 ; 
  predefine ( "execute     " , 7 , 4 ) ; 
  ilkinfo [ predefloc ] = 1 ; 
  predefine ( "function    " , 8 , 4 ) ; 
  ilkinfo [ predefloc ] = 2 ; 
  predefine ( "integers    " , 8 , 4 ) ; 
  ilkinfo [ predefloc ] = 3 ; 
  predefine ( "iterate     " , 7 , 4 ) ; 
  ilkinfo [ predefloc ] = 4 ; 
  predefine ( "macro       " , 5 , 4 ) ; 
  ilkinfo [ predefloc ] = 5 ; 
  predefine ( "read        " , 4 , 4 ) ; 
  ilkinfo [ predefloc ] = 6 ; 
  predefine ( "reverse     " , 7 , 4 ) ; 
  ilkinfo [ predefloc ] = 7 ; 
  predefine ( "sort        " , 4 , 4 ) ; 
  ilkinfo [ predefloc ] = 8 ; 
  predefine ( "strings     " , 7 , 4 ) ; 
  ilkinfo [ predefloc ] = 9 ; 
  predefine ( "comment     " , 7 , 12 ) ; 
  ilkinfo [ predefloc ] = 0 ; 
  predefine ( "preamble    " , 8 , 12 ) ; 
  ilkinfo [ predefloc ] = 1 ; 
  predefine ( "string      " , 6 , 12 ) ; 
  ilkinfo [ predefloc ] = 2 ; 
  buildin ( "=           " , 1 , bequals , 0 ) ; 
  buildin ( ">           " , 1 , bgreaterthan , 1 ) ; 
  buildin ( "<           " , 1 , blessthan , 2 ) ; 
  buildin ( "+           " , 1 , bplus , 3 ) ; 
  buildin ( "-           " , 1 , bminus , 4 ) ; 
  buildin ( "*           " , 1 , bconcatenate , 5 ) ; 
  buildin ( ":=          " , 2 , bgets , 6 ) ; 
  buildin ( "add.period$ " , 11 , baddperiod , 7 ) ; 
  buildin ( "call.type$  " , 10 , bcalltype , 8 ) ; 
  buildin ( "change.case$" , 12 , bchangecase , 9 ) ; 
  buildin ( "chr.to.int$ " , 11 , bchrtoint , 10 ) ; 
  buildin ( "cite$       " , 5 , bcite , 11 ) ; 
  buildin ( "duplicate$  " , 10 , bduplicate , 12 ) ; 
  buildin ( "empty$      " , 6 , bempty , 13 ) ; 
  buildin ( "format.name$" , 12 , bformatname , 14 ) ; 
  buildin ( "if$         " , 3 , bif , 15 ) ; 
  buildin ( "int.to.chr$ " , 11 , binttochr , 16 ) ; 
  buildin ( "int.to.str$ " , 11 , binttostr , 17 ) ; 
  buildin ( "missing$    " , 8 , bmissing , 18 ) ; 
  buildin ( "newline$    " , 8 , bnewline , 19 ) ; 
  buildin ( "num.names$  " , 10 , bnumnames , 20 ) ; 
  buildin ( "pop$        " , 4 , bpop , 21 ) ; 
  buildin ( "preamble$   " , 9 , bpreamble , 22 ) ; 
  buildin ( "purify$     " , 7 , bpurify , 23 ) ; 
  buildin ( "quote$      " , 6 , bquote , 24 ) ; 
  buildin ( "skip$       " , 5 , bskip , 25 ) ; 
  buildin ( "stack$      " , 6 , bstack , 26 ) ; 
  buildin ( "substring$  " , 10 , bsubstring , 27 ) ; 
  buildin ( "swap$       " , 5 , bswap , 28 ) ; 
  buildin ( "text.length$" , 12 , btextlength , 29 ) ; 
  buildin ( "text.prefix$" , 12 , btextprefix , 30 ) ; 
  buildin ( "top$        " , 4 , btopstack , 31 ) ; 
  buildin ( "type$       " , 5 , btype , 32 ) ; 
  buildin ( "warning$    " , 8 , bwarning , 33 ) ; 
  buildin ( "width$      " , 6 , bwidth , 35 ) ; 
  buildin ( "while$      " , 6 , bwhile , 34 ) ; 
  buildin ( "width$      " , 6 , bwidth , 35 ) ; 
  buildin ( "write$      " , 6 , bwrite , 36 ) ; 
  predefine ( "            " , 0 , 0 ) ; 
  snull = hashtext [ predefloc ] ; 
  fntype [ predefloc ] = 3 ; 
  predefine ( "default.type" , 12 , 0 ) ; 
  sdefault = hashtext [ predefloc ] ; 
  fntype [ predefloc ] = 3 ; 
  bdefault = bskip ; 
  preambleptr = 0 ; 
  predefine ( "i           " , 1 , 14 ) ; 
  ilkinfo [ predefloc ] = 0 ; 
  predefine ( "j           " , 1 , 14 ) ; 
  ilkinfo [ predefloc ] = 1 ; 
  predefine ( "oe          " , 2 , 14 ) ; 
  ilkinfo [ predefloc ] = 2 ; 
  predefine ( "OE          " , 2 , 14 ) ; 
  ilkinfo [ predefloc ] = 3 ; 
  predefine ( "ae          " , 2 , 14 ) ; 
  ilkinfo [ predefloc ] = 4 ; 
  predefine ( "AE          " , 2 , 14 ) ; 
  ilkinfo [ predefloc ] = 5 ; 
  predefine ( "aa          " , 2 , 14 ) ; 
  ilkinfo [ predefloc ] = 6 ; 
  predefine ( "AA          " , 2 , 14 ) ; 
  ilkinfo [ predefloc ] = 7 ; 
  predefine ( "o           " , 1 , 14 ) ; 
  ilkinfo [ predefloc ] = 8 ; 
  predefine ( "O           " , 1 , 14 ) ; 
  ilkinfo [ predefloc ] = 9 ; 
  predefine ( "l           " , 1 , 14 ) ; 
  ilkinfo [ predefloc ] = 10 ; 
  predefine ( "L           " , 1 , 14 ) ; 
  ilkinfo [ predefloc ] = 11 ; 
  predefine ( "ss          " , 2 , 14 ) ; 
  ilkinfo [ predefloc ] = 12 ; 
  predefine ( "crossref    " , 8 , 11 ) ; 
  fntype [ predefloc ] = 4 ; 
  ilkinfo [ predefloc ] = numfields ; 
  crossrefnum = numfields ; 
  incr ( numfields ) ; 
  numpredefinedfields = numfields ; 
  predefine ( "sort.key$   " , 9 , 11 ) ; 
  fntype [ predefloc ] = 6 ; 
  ilkinfo [ predefloc ] = numentstrs ; 
  sortkeynum = numentstrs ; 
  incr ( numentstrs ) ; 
  predefine ( "entry.max$  " , 10 , 11 ) ; 
  fntype [ predefloc ] = 7 ; 
  ilkinfo [ predefloc ] = entstrsize ; 
  predefine ( "global.max$ " , 11 , 11 ) ; 
  fntype [ predefloc ] = 7 ; 
  ilkinfo [ predefloc ] = globstrsize ; 
} 
boolean zscan1 ( char1 ) 
ASCIIcode char1 ; 
{register boolean Result; bufptr1 = bufptr2 ; 
  while ( ( ( buffer [ bufptr2 ] != char1 ) && ( bufptr2 < last ) ) ) incr ( 
  bufptr2 ) ; 
  if ( ( bufptr2 < last ) ) 
  Result = true ; 
  else Result = false ; 
  return(Result) ; 
} 
boolean zscan1white ( char1 ) 
ASCIIcode char1 ; 
{register boolean Result; bufptr1 = bufptr2 ; 
  while ( ( ( lexclass [ buffer [ bufptr2 ] ] != 1 ) && ( buffer [ bufptr2 ] 
  != char1 ) && ( bufptr2 < last ) ) ) incr ( bufptr2 ) ; 
  if ( ( bufptr2 < last ) ) 
  Result = true ; 
  else Result = false ; 
  return(Result) ; 
} 
boolean zscan2 ( char1 , char2 ) 
ASCIIcode char1 ; 
ASCIIcode char2 ; 
{register boolean Result; bufptr1 = bufptr2 ; 
  while ( ( ( buffer [ bufptr2 ] != char1 ) && ( buffer [ bufptr2 ] != char2 ) 
  && ( bufptr2 < last ) ) ) incr ( bufptr2 ) ; 
  if ( ( bufptr2 < last ) ) 
  Result = true ; 
  else Result = false ; 
  return(Result) ; 
} 
boolean zscan2white ( char1 , char2 ) 
ASCIIcode char1 ; 
ASCIIcode char2 ; 
{register boolean Result; bufptr1 = bufptr2 ; 
  while ( ( ( buffer [ bufptr2 ] != char1 ) && ( buffer [ bufptr2 ] != char2 ) 
  && ( lexclass [ buffer [ bufptr2 ] ] != 1 ) && ( bufptr2 < last ) ) ) incr ( 
  bufptr2 ) ; 
  if ( ( bufptr2 < last ) ) 
  Result = true ; 
  else Result = false ; 
  return(Result) ; 
} 
boolean zscan3 ( char1 , char2 , char3 ) 
ASCIIcode char1 ; 
ASCIIcode char2 ; 
ASCIIcode char3 ; 
{register boolean Result; bufptr1 = bufptr2 ; 
  while ( ( ( buffer [ bufptr2 ] != char1 ) && ( buffer [ bufptr2 ] != char2 ) 
  && ( buffer [ bufptr2 ] != char3 ) && ( bufptr2 < last ) ) ) incr ( bufptr2 
  ) ; 
  if ( ( bufptr2 < last ) ) 
  Result = true ; 
  else Result = false ; 
  return(Result) ; 
} 
boolean scanalpha ( ) 
{register boolean Result; bufptr1 = bufptr2 ; 
  while ( ( ( lexclass [ buffer [ bufptr2 ] ] == 2 ) && ( bufptr2 < last ) ) ) 
  incr ( bufptr2 ) ; 
  if ( ( ( bufptr2 - bufptr1 ) == 0 ) ) 
  Result = false ; 
  else Result = true ; 
  return(Result) ; 
} 
void zscanidentifier ( char1 , char2 , char3 ) 
ASCIIcode char1 ; 
ASCIIcode char2 ; 
ASCIIcode char3 ; 
{bufptr1 = bufptr2 ; 
  if ( ( lexclass [ buffer [ bufptr2 ] ] != 3 ) ) 
  while ( ( ( idclass [ buffer [ bufptr2 ] ] == 1 ) && ( bufptr2 < last ) ) ) 
  incr ( bufptr2 ) ; 
  if ( ( ( bufptr2 - bufptr1 ) == 0 ) ) 
  scanresult = 0 ; 
  else if ( ( ( lexclass [ buffer [ bufptr2 ] ] == 1 ) || ( bufptr2 == last ) 
  ) ) 
  scanresult = 3 ; 
  else if ( ( ( buffer [ bufptr2 ] == char1 ) || ( buffer [ bufptr2 ] == char2 
  ) || ( buffer [ bufptr2 ] == char3 ) ) ) 
  scanresult = 1 ; 
  else scanresult = 2 ; 
} 
boolean scannonneginteger ( ) 
{register boolean Result; bufptr1 = bufptr2 ; 
  tokenvalue = 0 ; 
  while ( ( ( lexclass [ buffer [ bufptr2 ] ] == 3 ) && ( bufptr2 < last ) ) ) 
  {
    tokenvalue = tokenvalue * 10 + ( buffer [ bufptr2 ] - 48 ) ; 
    incr ( bufptr2 ) ; 
  } 
  if ( ( ( bufptr2 - bufptr1 ) == 0 ) ) 
  Result = false ; 
  else Result = true ; 
  return(Result) ; 
} 
boolean scaninteger ( ) 
{register boolean Result; schar signlength  ; 
  bufptr1 = bufptr2 ; 
  if ( ( buffer [ bufptr2 ] == 45 ) ) 
  {
    signlength = 1 ; 
    incr ( bufptr2 ) ; 
  } 
  else signlength = 0 ; 
  tokenvalue = 0 ; 
  while ( ( ( lexclass [ buffer [ bufptr2 ] ] == 3 ) && ( bufptr2 < last ) ) ) 
  {
    tokenvalue = tokenvalue * 10 + ( buffer [ bufptr2 ] - 48 ) ; 
    incr ( bufptr2 ) ; 
  } 
  if ( ( ( signlength == 1 ) ) ) 
  tokenvalue = - (integer) tokenvalue ; 
  if ( ( ( bufptr2 - bufptr1 ) == signlength ) ) 
  Result = false ; 
  else Result = true ; 
  return(Result) ; 
} 
boolean scanwhitespace ( ) 
{register boolean Result; while ( ( ( lexclass [ buffer [ bufptr2 ] ] == 1 ) 
  && ( bufptr2 < last ) ) ) incr ( bufptr2 ) ; 
  if ( ( bufptr2 < last ) ) 
  Result = true ; 
  else Result = false ; 
  return(Result) ; 
} 
boolean eatbstwhitespace ( ) 
{/* 10 */ register boolean Result; while ( true ) {
    
    if ( ( scanwhitespace () ) ) 
    if ( ( buffer [ bufptr2 ] != 37 ) ) 
    {
      Result = true ; 
      goto lab10 ; 
    } 
    if ( ( ! inputln ( bstfile ) ) ) 
    {
      Result = false ; 
      goto lab10 ; 
    } 
    incr ( bstlinenum ) ; 
    bufptr2 = 0 ; 
  } 
  lab10: ; 
  return(Result) ; 
} 
void skiptokenprint ( ) 
{{
    
    (void) putc( '-' ,  logfile );
    (void) putc( '-' ,  stdout );
  } 
  bstlnnumprint () ; 
  markerror () ; 
  if ( ( scan2white ( 125 , 37 ) ) ) 
  ; 
} 
void printrecursionillega ( ) 
{
	;
#ifdef TRACE
  {
    (void) putc('\n',  logfile );
  } 
#endif /* TRACE */
  {
    (void) fprintf( logfile , "%s\n",  "Curse you, wizard, before you recurse me:" ) ; 
    (void) fprintf( stdout , "%s\n",  "Curse you, wizard, before you recurse me:" ) ; 
  } 
  {
    (void) Fputs( logfile ,  "function " ) ; 
    (void) Fputs( stdout ,  "function " ) ; 
  } 
  printatoken () ; 
  {
    (void) fprintf( logfile , "%s\n",  " is illegal in its own definition" ) ; 
    (void) fprintf( stdout , "%s\n",  " is illegal in its own definition" ) ; 
  } 
  skiptokenprint () ; 
} 
void skptokenunknownfunct ( ) 
{printatoken () ; 
  {
    (void) Fputs( logfile ,  " is an unknown function" ) ; 
    (void) Fputs( stdout ,  " is an unknown function" ) ; 
  } 
  skiptokenprint () ; 
} 
void skipillegalstuffafte ( ) 
{{
    
    (void) fprintf( logfile , "%c%c%s",  '"' , xchr [ buffer [ bufptr2 ] ] ,     "\" can't follow a literal" ) ; 
    (void) fprintf( stdout , "%c%c%s",  '"' , xchr [ buffer [ bufptr2 ] ] ,     "\" can't follow a literal" ) ; 
  } 
  skiptokenprint () ; 
} 
void zscanfndef ( fnhashloc ) 
hashloc fnhashloc ; 
{/* 25 10 */ typedef integer fndefloc  ; 
  hashptr2 singlfunction[singlefnspace + 1]  ; 
  fndefloc singleptr  ; 
  fndefloc copyptr  ; 
  bufpointer endofnum  ; 
  hashloc implfnloc  ; 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "function" ) ; 
	  (void) Fputs( stdout ,  "function" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  singleptr = 0 ; 
  while ( ( buffer [ bufptr2 ] != 125 ) ) {
      
    switch ( ( buffer [ bufptr2 ] ) ) 
    {case 35 : 
      {
	incr ( bufptr2 ) ; 
	if ( ( ! scaninteger () ) ) 
	{
	  {
	    (void) Fputs( logfile ,  "Illegal integer in integer literal" ) ; 
	    (void) Fputs( stdout ,  "Illegal integer in integer literal" ) ; 
	  } 
	  skiptokenprint () ; 
	  goto lab25 ; 
	} 
	;
#ifdef TRACE
	{
	  (void) putc( '#' ,  logfile );
	} 
	{
	  outtoken ( logfile ) ; 
	} 
	{
	  (void) fprintf( logfile , "%s%ld\n",  " is an integer literal with value " ,           (long)tokenvalue ) ; 
	} 
#endif /* TRACE */
	literalloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 1 
	, true ) ; 
	if ( ( ! hashfound ) ) 
	{
	  fntype [ literalloc ] = 2 ; 
	  ilkinfo [ literalloc ] = tokenvalue ; 
	} 
	if ( ( ( lexclass [ buffer [ bufptr2 ] ] != 1 ) && ( bufptr2 < last ) 
	&& ( buffer [ bufptr2 ] != 125 ) && ( buffer [ bufptr2 ] != 37 ) ) ) 
	{
	  skipillegalstuffafte () ; 
	  goto lab25 ; 
	} 
	{
	  singlfunction [ singleptr ] = literalloc ; 
	  if ( ( singleptr == singlefnspace ) ) 
	  singlfnoverflow () ; 
	  incr ( singleptr ) ; 
	} 
      } 
      break ; 
    case 34 : 
      {
	incr ( bufptr2 ) ; 
	if ( ( ! scan1 ( 34 ) ) ) 
	{
	  {
	    (void) fprintf( logfile , "%s%c%s",  "No `" , xchr [ 34 ] , "' to end string literal"             ) ; 
	    (void) fprintf( stdout , "%s%c%s",  "No `" , xchr [ 34 ] , "' to end string literal"             ) ; 
	  } 
	  skiptokenprint () ; 
	  goto lab25 ; 
	} 
	;
#ifdef TRACE
	{
	  (void) putc( '"' ,  logfile );
	} 
	{
	  outtoken ( logfile ) ; 
	} 
	{
	  (void) putc( '"' ,  logfile );
	} 
	{
	  (void) fprintf( logfile , "%s\n",  " is a string literal" ) ; 
	} 
#endif /* TRACE */
	literalloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 0 
	, true ) ; 
	fntype [ literalloc ] = 3 ; 
	incr ( bufptr2 ) ; 
	if ( ( ( lexclass [ buffer [ bufptr2 ] ] != 1 ) && ( bufptr2 < last ) 
	&& ( buffer [ bufptr2 ] != 125 ) && ( buffer [ bufptr2 ] != 37 ) ) ) 
	{
	  skipillegalstuffafte () ; 
	  goto lab25 ; 
	} 
	{
	  singlfunction [ singleptr ] = literalloc ; 
	  if ( ( singleptr == singlefnspace ) ) 
	  singlfnoverflow () ; 
	  incr ( singleptr ) ; 
	} 
      } 
      break ; 
    case 39 : 
      {
	incr ( bufptr2 ) ; 
	if ( ( scan2white ( 125 , 37 ) ) ) 
	; 
	;
#ifdef TRACE
	{
	  (void) putc( '\'' ,  logfile );
	} 
	{
	  outtoken ( logfile ) ; 
	} 
	{
	  (void) Fputs( logfile ,  " is a quoted function " ) ; 
	} 
#endif /* TRACE */
	lowercase ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
	fnloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 11 , 
	false ) ; 
	if ( ( ! hashfound ) ) 
	{
	  skptokenunknownfunct () ; 
	  goto lab25 ; 
	} 
	else {
	    
	  if ( ( fnloc == wizloc ) ) 
	  {
	    printrecursionillega () ; 
	    goto lab25 ; 
	  } 
	  else {
	      
	;
#ifdef TRACE
	    {
	      (void) Fputs( logfile ,  "of type " ) ; 
	    } 
	    traceprfnclass ( fnloc ) ; 
	    {
	      (void) putc('\n',  logfile );
	    } 
#endif /* TRACE */
	    {
	      singlfunction [ singleptr ] = 0 ; 
	      if ( ( singleptr == singlefnspace ) ) 
	      singlfnoverflow () ; 
	      incr ( singleptr ) ; 
	    } 
	    {
	      singlfunction [ singleptr ] = fnloc ; 
	      if ( ( singleptr == singlefnspace ) ) 
	      singlfnoverflow () ; 
	      incr ( singleptr ) ; 
	    } 
	  } 
	} 
      } 
      break ; 
    case 123 : 
      {
	exbuf [ 0 ] = 39 ; 
	inttoASCII ( implfnnum , exbuf , 1 , endofnum ) ; 
	implfnloc = strlookup ( exbuf , 0 , endofnum , 11 , true ) ; 
	if ( ( hashfound ) ) 
	{
	  {
	    (void) Fputs( logfile ,  "Already encountered implicit function" ) ; 
	    (void) Fputs( stdout ,  "Already encountered implicit function" ) ; 
	  } 
	  printconfusion () ; 
	  longjmp(jmp9998,1) ; 
	} 
	;
#ifdef TRACE
	{
	  outpoolstr ( logfile , hashtext [ implfnloc ] ) ; 
	} 
	{
	  (void) fprintf( logfile , "%s\n",  " is an implicit function" ) ; 
	} 
#endif /* TRACE */
	incr ( implfnnum ) ; 
	fntype [ implfnloc ] = 1 ; 
	{
	  singlfunction [ singleptr ] = 0 ; 
	  if ( ( singleptr == singlefnspace ) ) 
	  singlfnoverflow () ; 
	  incr ( singleptr ) ; 
	} 
	{
	  singlfunction [ singleptr ] = implfnloc ; 
	  if ( ( singleptr == singlefnspace ) ) 
	  singlfnoverflow () ; 
	  incr ( singleptr ) ; 
	} 
	incr ( bufptr2 ) ; 
	scanfndef ( implfnloc ) ; 
      } 
      break ; 
      default: 
      {
	if ( ( scan2white ( 125 , 37 ) ) ) 
	; 
	;
#ifdef TRACE
	{
	  outtoken ( logfile ) ; 
	} 
	{
	  (void) Fputs( logfile ,  " is a function " ) ; 
	} 
#endif /* TRACE */
	lowercase ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
	fnloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 11 , 
	false ) ; 
	if ( ( ! hashfound ) ) 
	{
	  skptokenunknownfunct () ; 
	  goto lab25 ; 
	} 
	else if ( ( fnloc == wizloc ) ) 
	{
	  printrecursionillega () ; 
	  goto lab25 ; 
	} 
	else {
	    
	;
#ifdef TRACE
	  {
	    (void) Fputs( logfile ,  "of type " ) ; 
	  } 
	  traceprfnclass ( fnloc ) ; 
	  {
	    (void) putc('\n',  logfile );
	  } 
#endif /* TRACE */
	  {
	    singlfunction [ singleptr ] = fnloc ; 
	    if ( ( singleptr == singlefnspace ) ) 
	    singlfnoverflow () ; 
	    incr ( singleptr ) ; 
	  } 
	} 
      } 
      break ; 
    } 
    lab25: {
	
      if ( ( ! eatbstwhitespace () ) ) 
      {
	eatbstprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "function" ) ; 
	    (void) Fputs( stdout ,  "function" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
  } 
  {
    {
      singlfunction [ singleptr ] = 5001 ; 
      if ( ( singleptr == singlefnspace ) ) 
      singlfnoverflow () ; 
      incr ( singleptr ) ; 
    } 
    if ( ( singleptr + wizdefptr > wizfnspace ) ) 
    {
      {
	(void) fprintf( logfile , "%ld%s",  (long)singleptr + wizdefptr , ": " ) ; 
	(void) fprintf( stdout , "%ld%s",  (long)singleptr + wizdefptr , ": " ) ; 
      } 
      {
	printoverflow () ; 
	{
	  (void) fprintf( logfile , "%s%ld\n",  "wizard-defined function space " , (long)wizfnspace ) 
	  ; 
	  (void) fprintf( stdout , "%s%ld\n",  "wizard-defined function space " , (long)wizfnspace ) ; 
	} 
	longjmp(jmp9998,1) ; 
      } 
    } 
    ilkinfo [ fnhashloc ] = wizdefptr ; 
    copyptr = 0 ; 
    while ( ( copyptr < singleptr ) ) {
	
      wizfunctions [ wizdefptr ] = singlfunction [ copyptr ] ; 
      incr ( copyptr ) ; 
      incr ( wizdefptr ) ; 
    } 
  } 
  incr ( bufptr2 ) ; 
  lab10: ; 
} 
boolean eatbibwhitespace ( ) 
{/* 10 */ register boolean Result; while ( ( ! scanwhitespace () ) ) {
    
    if ( ( ! inputln ( bibfile [ bibptr ] ) ) ) 
    {
      Result = false ; 
      goto lab10 ; 
    } 
    incr ( biblinenum ) ; 
    bufptr2 = 0 ; 
  } 
  Result = true ; 
  lab10: ; 
  return(Result) ; 
} 
boolean compressbibwhite ( ) 
{/* 10 */ register boolean Result; Result = false ; 
  {
    if ( ( exbufptr == bufsize ) ) 
    {
      bibfieldtoolongprint () ; 
      goto lab10 ; 
    } 
    else {
	
      exbuf [ exbufptr ] = 32 ; 
      incr ( exbufptr ) ; 
    } 
  } 
  while ( ( ! scanwhitespace () ) ) {
      
    if ( ( ! inputln ( bibfile [ bibptr ] ) ) ) 
    {
      eatbibprint () ; 
      goto lab10 ; 
    } 
    incr ( biblinenum ) ; 
    bufptr2 = 0 ; 
  } 
  Result = true ; 
  lab10: ; 
  return(Result) ; 
} 
boolean scanbalancedbraces ( ) 
{/* 15 10 */ register boolean Result; Result = false ; 
  incr ( bufptr2 ) ; 
  {
    if ( ( ( lexclass [ buffer [ bufptr2 ] ] == 1 ) || ( bufptr2 == last ) ) ) 
    if ( ( ! compressbibwhite () ) ) 
    goto lab10 ; 
  } 
  if ( ( exbufptr > 1 ) ) 
  if ( ( exbuf [ exbufptr - 1 ] == 32 ) ) 
  if ( ( exbuf [ exbufptr - 2 ] == 32 ) ) 
  decr ( exbufptr ) ; 
  bibbracelevel = 0 ; 
  if ( ( storefield ) ) 
  {
    while ( ( buffer [ bufptr2 ] != rightstrdelim ) ) switch ( ( buffer [ 
    bufptr2 ] ) ) 
    {case 123 : 
      {
	incr ( bibbracelevel ) ; 
	{
	  if ( ( exbufptr == bufsize ) ) 
	  {
	    bibfieldtoolongprint () ; 
	    goto lab10 ; 
	  } 
	  else {
	      
	    exbuf [ exbufptr ] = 123 ; 
	    incr ( exbufptr ) ; 
	  } 
	} 
	incr ( bufptr2 ) ; 
	{
	  if ( ( ( lexclass [ buffer [ bufptr2 ] ] == 1 ) || ( bufptr2 == last 
	  ) ) ) 
	  if ( ( ! compressbibwhite () ) ) 
	  goto lab10 ; 
	} 
	{
	  while ( true ) switch ( ( buffer [ bufptr2 ] ) ) 
	  {case 125 : 
	    {
	      decr ( bibbracelevel ) ; 
	      {
		if ( ( exbufptr == bufsize ) ) 
		{
		  bibfieldtoolongprint () ; 
		  goto lab10 ; 
		} 
		else {
		    
		  exbuf [ exbufptr ] = 125 ; 
		  incr ( exbufptr ) ; 
		} 
	      } 
	      incr ( bufptr2 ) ; 
	      {
		if ( ( ( lexclass [ buffer [ bufptr2 ] ] == 1 ) || ( bufptr2 
		== last ) ) ) 
		if ( ( ! compressbibwhite () ) ) 
		goto lab10 ; 
	      } 
	      if ( ( bibbracelevel == 0 ) ) 
	      goto lab15 ; 
	    } 
	    break ; 
	  case 123 : 
	    {
	      incr ( bibbracelevel ) ; 
	      {
		if ( ( exbufptr == bufsize ) ) 
		{
		  bibfieldtoolongprint () ; 
		  goto lab10 ; 
		} 
		else {
		    
		  exbuf [ exbufptr ] = 123 ; 
		  incr ( exbufptr ) ; 
		} 
	      } 
	      incr ( bufptr2 ) ; 
	      {
		if ( ( ( lexclass [ buffer [ bufptr2 ] ] == 1 ) || ( bufptr2 
		== last ) ) ) 
		if ( ( ! compressbibwhite () ) ) 
		goto lab10 ; 
	      } 
	    } 
	    break ; 
	    default: 
	    {
	      {
		if ( ( exbufptr == bufsize ) ) 
		{
		  bibfieldtoolongprint () ; 
		  goto lab10 ; 
		} 
		else {
		    
		  exbuf [ exbufptr ] = buffer [ bufptr2 ] ; 
		  incr ( exbufptr ) ; 
		} 
	      } 
	      incr ( bufptr2 ) ; 
	      {
		if ( ( ( lexclass [ buffer [ bufptr2 ] ] == 1 ) || ( bufptr2 
		== last ) ) ) 
		if ( ( ! compressbibwhite () ) ) 
		goto lab10 ; 
	      } 
	    } 
	    break ; 
	  } 
	  lab15: ; 
	} 
      } 
      break ; 
    case 125 : 
      {
	bibunbalancedbracesp () ; 
	goto lab10 ; 
      } 
      break ; 
      default: 
      {
	{
	  if ( ( exbufptr == bufsize ) ) 
	  {
	    bibfieldtoolongprint () ; 
	    goto lab10 ; 
	  } 
	  else {
	      
	    exbuf [ exbufptr ] = buffer [ bufptr2 ] ; 
	    incr ( exbufptr ) ; 
	  } 
	} 
	incr ( bufptr2 ) ; 
	{
	  if ( ( ( lexclass [ buffer [ bufptr2 ] ] == 1 ) || ( bufptr2 == last 
	  ) ) ) 
	  if ( ( ! compressbibwhite () ) ) 
	  goto lab10 ; 
	} 
      } 
      break ; 
    } 
  } 
  else {
      
    while ( ( buffer [ bufptr2 ] != rightstrdelim ) ) if ( ( buffer [ bufptr2 
    ] == 123 ) ) 
    {
      incr ( bibbracelevel ) ; 
      incr ( bufptr2 ) ; 
      {
	if ( ( ! eatbibwhitespace () ) ) 
	{
	  eatbibprint () ; 
	  goto lab10 ; 
	} 
      } 
      while ( ( bibbracelevel > 0 ) ) {
	  
	if ( ( buffer [ bufptr2 ] == 125 ) ) 
	{
	  decr ( bibbracelevel ) ; 
	  incr ( bufptr2 ) ; 
	  {
	    if ( ( ! eatbibwhitespace () ) ) 
	    {
	      eatbibprint () ; 
	      goto lab10 ; 
	    } 
	  } 
	} 
	else if ( ( buffer [ bufptr2 ] == 123 ) ) 
	{
	  incr ( bibbracelevel ) ; 
	  incr ( bufptr2 ) ; 
	  {
	    if ( ( ! eatbibwhitespace () ) ) 
	    {
	      eatbibprint () ; 
	      goto lab10 ; 
	    } 
	  } 
	} 
	else {
	    
	  incr ( bufptr2 ) ; 
	  if ( ( ! scan2 ( 125 , 123 ) ) ) 
	  {
	    if ( ( ! eatbibwhitespace () ) ) 
	    {
	      eatbibprint () ; 
	      goto lab10 ; 
	    } 
	  } 
	} 
      } 
    } 
    else if ( ( buffer [ bufptr2 ] == 125 ) ) 
    {
      bibunbalancedbracesp () ; 
      goto lab10 ; 
    } 
    else {
	
      incr ( bufptr2 ) ; 
      if ( ( ! scan3 ( rightstrdelim , 123 , 125 ) ) ) 
      {
	if ( ( ! eatbibwhitespace () ) ) 
	{
	  eatbibprint () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  incr ( bufptr2 ) ; 
  Result = true ; 
  lab10: ; 
  return(Result) ; 
} 
boolean scanafieldtokenandea ( ) 
{/* 10 */ register boolean Result; Result = false ; 
  switch ( ( buffer [ bufptr2 ] ) ) 
  {case 123 : 
    {
      rightstrdelim = 125 ; 
      if ( ( ! scanbalancedbraces () ) ) 
      goto lab10 ; 
    } 
    break ; 
  case 34 : 
    {
      rightstrdelim = 34 ; 
      if ( ( ! scanbalancedbraces () ) ) 
      goto lab10 ; 
    } 
    break ; 
  case 48 : 
  case 49 : 
  case 50 : 
  case 51 : 
  case 52 : 
  case 53 : 
  case 54 : 
  case 55 : 
  case 56 : 
  case 57 : 
    {
      if ( ( ! scannonneginteger () ) ) 
      {
	{
	  (void) Fputs( logfile ,  "A digit disappeared" ) ; 
	  (void) Fputs( stdout ,  "A digit disappeared" ) ; 
	} 
	printconfusion () ; 
	longjmp(jmp9998,1) ; 
      } 
      if ( ( storefield ) ) 
      {
	tmpptr = bufptr1 ; 
	while ( ( tmpptr < bufptr2 ) ) {
	    
	  {
	    if ( ( exbufptr == bufsize ) ) 
	    {
	      bibfieldtoolongprint () ; 
	      goto lab10 ; 
	    } 
	    else {
		
	      exbuf [ exbufptr ] = buffer [ tmpptr ] ; 
	      incr ( exbufptr ) ; 
	    } 
	  } 
	  incr ( tmpptr ) ; 
	} 
      } 
    } 
    break ; 
    default: 
    {
      scanidentifier ( 44 , rightouterdelim , 35 ) ; 
      {
	if ( ( ( scanresult == 3 ) || ( scanresult == 1 ) ) ) 
	; 
	else {
	    
	  bibidprint () ; 
	  {
	    {
	      (void) Fputs( logfile ,  "a field part" ) ; 
	      (void) Fputs( stdout ,  "a field part" ) ; 
	    } 
	    biberrprint () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
      if ( ( storefield ) ) 
      {
	lowercase ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
	macronameloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 
	13 , false ) ; 
	storetoken = true ; 
	if ( ( atbibcommand ) ) 
	if ( ( commandnum == 2 ) ) 
	if ( ( macronameloc == curmacroloc ) ) 
	{
	  storetoken = false ; 
	  {
	    macrowarnprint () ; 
	    {
	      {
		(void) fprintf( logfile , "%s\n",  "used in its own definition" ) ; 
		(void) fprintf( stdout , "%s\n",  "used in its own definition" ) ; 
	      } 
	      bibwarnprint () ; 
	    } 
	  } 
	} 
	if ( ( ! hashfound ) ) 
	{
	  storetoken = false ; 
	  {
	    macrowarnprint () ; 
	    {
	      {
		(void) fprintf( logfile , "%s\n",  "undefined" ) ; 
		(void) fprintf( stdout , "%s\n",  "undefined" ) ; 
	      } 
	      bibwarnprint () ; 
	    } 
	  } 
	} 
	if ( ( storetoken ) ) 
	{
	  tmpptr = strstart [ ilkinfo [ macronameloc ] ] ; 
	  tmpendptr = strstart [ ilkinfo [ macronameloc ] + 1 ] ; 
	  if ( ( exbufptr == 0 ) ) 
	  if ( ( ( lexclass [ strpool [ tmpptr ] ] == 1 ) && ( tmpptr < 
	  tmpendptr ) ) ) 
	  {
	    {
	      if ( ( exbufptr == bufsize ) ) 
	      {
		bibfieldtoolongprint () ; 
		goto lab10 ; 
	      } 
	      else {
		  
		exbuf [ exbufptr ] = 32 ; 
		incr ( exbufptr ) ; 
	      } 
	    } 
	    incr ( tmpptr ) ; 
	    while ( ( ( lexclass [ strpool [ tmpptr ] ] == 1 ) && ( tmpptr < 
	    tmpendptr ) ) ) incr ( tmpptr ) ; 
	  } 
	  while ( ( tmpptr < tmpendptr ) ) {
	      
	    if ( ( lexclass [ strpool [ tmpptr ] ] != 1 ) ) 
	    {
	      if ( ( exbufptr == bufsize ) ) 
	      {
		bibfieldtoolongprint () ; 
		goto lab10 ; 
	      } 
	      else {
		  
		exbuf [ exbufptr ] = strpool [ tmpptr ] ; 
		incr ( exbufptr ) ; 
	      } 
	    } 
	    else if ( ( exbuf [ exbufptr - 1 ] != 32 ) ) 
	    {
	      if ( ( exbufptr == bufsize ) ) 
	      {
		bibfieldtoolongprint () ; 
		goto lab10 ; 
	      } 
	      else {
		  
		exbuf [ exbufptr ] = 32 ; 
		incr ( exbufptr ) ; 
	      } 
	    } 
	    incr ( tmpptr ) ; 
	  } 
	} 
      } 
    } 
    break ; 
  } 
  {
    if ( ( ! eatbibwhitespace () ) ) 
    {
      eatbibprint () ; 
      goto lab10 ; 
    } 
  } 
  Result = true ; 
  lab10: ; 
  return(Result) ; 
} 
boolean scanandstorethefield ( ) 
{/* 10 */ register boolean Result; Result = false ; 
  exbufptr = 0 ; 
  if ( ( ! scanafieldtokenandea () ) ) 
  goto lab10 ; 
  while ( ( buffer [ bufptr2 ] == 35 ) ) {
      
    incr ( bufptr2 ) ; 
    {
      if ( ( ! eatbibwhitespace () ) ) 
      {
	eatbibprint () ; 
	goto lab10 ; 
      } 
    } 
    if ( ( ! scanafieldtokenandea () ) ) 
    goto lab10 ; 
  } 
  if ( ( storefield ) ) 
  {
    if ( ( ! atbibcommand ) ) 
    if ( ( exbufptr > 0 ) ) 
    if ( ( exbuf [ exbufptr - 1 ] == 32 ) ) 
    decr ( exbufptr ) ; 
    if ( ( ( ! atbibcommand ) && ( exbuf [ 0 ] == 32 ) && ( exbufptr > 0 ) ) ) 
    exbufxptr = 1 ; 
    else exbufxptr = 0 ; 
    fieldvalloc = strlookup ( exbuf , exbufxptr , exbufptr - exbufxptr , 0 , 
    true ) ; 
    fntype [ fieldvalloc ] = 3 ; 
	;
#ifdef TRACE
    {
      (void) putc( '"' ,  logfile );
    } 
    {
      outpoolstr ( logfile , hashtext [ fieldvalloc ] ) ; 
    } 
    {
      (void) fprintf( logfile , "%s\n",  "\" is a field value" ) ; 
    } 
#endif /* TRACE */
    if ( ( atbibcommand ) ) 
    {
      switch ( ( commandnum ) ) 
      {case 1 : 
	{
	  spreamble [ preambleptr ] = hashtext [ fieldvalloc ] ; 
	  incr ( preambleptr ) ; 
	} 
	break ; 
      case 2 : 
	ilkinfo [ curmacroloc ] = hashtext [ fieldvalloc ] ; 
	break ; 
	default: 
	bibcmdconfusion () ; 
	break ; 
      } 
    } 
    else {
	
      fieldptr = entryciteptr * numfields + ilkinfo [ fieldnameloc ] ; 
      if ( ( fieldinfo [ fieldptr ] != 0 ) ) 
      {
	{
	  (void) Fputs( logfile ,  "Warning--I'm ignoring " ) ; 
	  (void) Fputs( stdout ,  "Warning--I'm ignoring " ) ; 
	} 
	printapoolstr ( citelist [ entryciteptr ] ) ; 
	{
	  (void) Fputs( logfile ,  "'s extra \"" ) ; 
	  (void) Fputs( stdout ,  "'s extra \"" ) ; 
	} 
	printapoolstr ( hashtext [ fieldnameloc ] ) ; 
	{
	  {
	    (void) fprintf( logfile , "%s\n",  "\" field" ) ; 
	    (void) fprintf( stdout , "%s\n",  "\" field" ) ; 
	  } 
	  bibwarnprint () ; 
	} 
      } 
      else {
	  
	fieldinfo [ fieldptr ] = hashtext [ fieldvalloc ] ; 
	if ( ( ( ilkinfo [ fieldnameloc ] == crossrefnum ) && ( ! allentries ) 
	) ) 
	{
	  tmpptr = exbufxptr ; 
	  while ( ( tmpptr < exbufptr ) ) {
	      
	    outbuf [ tmpptr ] = exbuf [ tmpptr ] ; 
	    incr ( tmpptr ) ; 
	  } 
	  lowercase ( outbuf , exbufxptr , exbufptr - exbufxptr ) ; 
	  lcciteloc = strlookup ( outbuf , exbufxptr , exbufptr - exbufxptr , 
	  10 , true ) ; 
	  if ( ( hashfound ) ) 
	  {
	    citeloc = ilkinfo [ lcciteloc ] ; 
	    if ( ( ilkinfo [ citeloc ] >= oldnumcites ) ) 
	    incr ( citeinfo [ ilkinfo [ citeloc ] ] ) ; 
	  } 
	  else {
	      
	    citeloc = strlookup ( exbuf , exbufxptr , exbufptr - exbufxptr , 9 
	    , true ) ; 
	    if ( ( hashfound ) ) 
	    hashciteconfusion () ; 
	    adddatabasecite ( citeptr ) ; 
	    citeinfo [ ilkinfo [ citeloc ] ] = 1 ; 
	  } 
	} 
      } 
    } 
  } 
  Result = true ; 
  lab10: ; 
  return(Result) ; 
} 
void zdecrbracelevel ( poplitvar ) 
strnumber poplitvar ; 
{if ( ( bracelevel == 0 ) ) 
  bracesunbalancedcomp ( poplitvar ) ; 
  else decr ( bracelevel ) ; 
} 
void zcheckbracelevel ( poplitvar ) 
strnumber poplitvar ; 
{if ( ( bracelevel > 0 ) ) 
  bracesunbalancedcomp ( poplitvar ) ; 
} 
void znamescanforand ( poplitvar ) 
strnumber poplitvar ; 
{bracelevel = 0 ; 
  precedingwhite = false ; 
  andfound = false ; 
  while ( ( ( ! andfound ) && ( exbufptr < exbuflength ) ) ) switch ( ( exbuf 
  [ exbufptr ] ) ) 
  {case 97 : 
  case 65 : 
    {
      incr ( exbufptr ) ; 
      if ( ( precedingwhite ) ) 
      {
	if ( ( exbufptr <= ( exbuflength - 3 ) ) ) 
	if ( ( ( exbuf [ exbufptr ] == 110 ) || ( exbuf [ exbufptr ] == 78 ) ) 
	) 
	if ( ( ( exbuf [ exbufptr + 1 ] == 100 ) || ( exbuf [ exbufptr + 1 ] 
	== 68 ) ) ) 
	if ( ( lexclass [ exbuf [ exbufptr + 2 ] ] == 1 ) ) 
	{
	  exbufptr = exbufptr + 2 ; 
	  andfound = true ; 
	} 
      } 
      precedingwhite = false ; 
    } 
    break ; 
  case 123 : 
    {
      incr ( bracelevel ) ; 
      incr ( exbufptr ) ; 
      while ( ( ( bracelevel > 0 ) && ( exbufptr < exbuflength ) ) ) {
	  
	if ( ( exbuf [ exbufptr ] == 125 ) ) 
	decr ( bracelevel ) ; 
	else if ( ( exbuf [ exbufptr ] == 123 ) ) 
	incr ( bracelevel ) ; 
	incr ( exbufptr ) ; 
      } 
      precedingwhite = false ; 
    } 
    break ; 
  case 125 : 
    {
      decrbracelevel ( poplitvar ) ; 
      incr ( exbufptr ) ; 
      precedingwhite = false ; 
    } 
    break ; 
    default: 
    if ( ( lexclass [ exbuf [ exbufptr ] ] == 1 ) ) 
    {
      incr ( exbufptr ) ; 
      precedingwhite = true ; 
    } 
    else {
	
      incr ( exbufptr ) ; 
      precedingwhite = false ; 
    } 
    break ; 
  } 
  checkbracelevel ( poplitvar ) ; 
} 
boolean vontokenfound ( ) 
{/* 10 */ register boolean Result; nmbracelevel = 0 ; 
  Result = false ; 
  while ( ( namebfptr < namebfxptr ) ) if ( ( ( svbuffer [ namebfptr ] >= 65 ) 
  && ( svbuffer [ namebfptr ] <= 90 ) ) ) 
  goto lab10 ; 
  else if ( ( ( svbuffer [ namebfptr ] >= 97 ) && ( svbuffer [ namebfptr ] <= 
  122 ) ) ) 
  {
    Result = true ; 
    goto lab10 ; 
  } 
  else if ( ( svbuffer [ namebfptr ] == 123 ) ) 
  {
    incr ( nmbracelevel ) ; 
    incr ( namebfptr ) ; 
    if ( ( ( namebfptr + 2 < namebfxptr ) && ( svbuffer [ namebfptr ] == 92 ) 
    ) ) 
    {
      incr ( namebfptr ) ; 
      namebfyptr = namebfptr ; 
      while ( ( ( namebfptr < namebfxptr ) && ( lexclass [ svbuffer [ 
      namebfptr ] ] == 2 ) ) ) incr ( namebfptr ) ; 
      controlseqloc = strlookup ( svbuffer , namebfyptr , namebfptr - 
      namebfyptr , 14 , false ) ; 
      if ( ( hashfound ) ) 
      {
	switch ( ( ilkinfo [ controlseqloc ] ) ) 
	{case 3 : 
	case 5 : 
	case 7 : 
	case 9 : 
	case 11 : 
	  goto lab10 ; 
	  break ; 
	case 0 : 
	case 1 : 
	case 2 : 
	case 4 : 
	case 6 : 
	case 8 : 
	case 10 : 
	case 12 : 
	  {
	    Result = true ; 
	    goto lab10 ; 
	  } 
	  break ; 
	  default: 
	  {
	    {
	      (void) Fputs( logfile ,  "Control-sequence hash error" ) ; 
	      (void) Fputs( stdout ,  "Control-sequence hash error" ) ; 
	    } 
	    printconfusion () ; 
	    longjmp(jmp9998,1) ; 
	  } 
	  break ; 
	} 
      } 
      while ( ( ( namebfptr < namebfxptr ) && ( nmbracelevel > 0 ) ) ) {
	  
	if ( ( ( svbuffer [ namebfptr ] >= 65 ) && ( svbuffer [ namebfptr ] <= 
	90 ) ) ) 
	goto lab10 ; 
	else if ( ( ( svbuffer [ namebfptr ] >= 97 ) && ( svbuffer [ namebfptr 
	] <= 122 ) ) ) 
	{
	  Result = true ; 
	  goto lab10 ; 
	} 
	else if ( ( svbuffer [ namebfptr ] == 125 ) ) 
	decr ( nmbracelevel ) ; 
	else if ( ( svbuffer [ namebfptr ] == 123 ) ) 
	incr ( nmbracelevel ) ; 
	incr ( namebfptr ) ; 
      } 
      goto lab10 ; 
    } 
    else while ( ( ( nmbracelevel > 0 ) && ( namebfptr < namebfxptr ) ) ) {
	
      if ( ( svbuffer [ namebfptr ] == 125 ) ) 
      decr ( nmbracelevel ) ; 
      else if ( ( svbuffer [ namebfptr ] == 123 ) ) 
      incr ( nmbracelevel ) ; 
      incr ( namebfptr ) ; 
    } 
  } 
  else incr ( namebfptr ) ; 
  lab10: ; 
  return(Result) ; 
} 
void vonnameendsandlastna ( ) 
{/* 10 */ vonend = lastend - 1 ; 
  while ( ( vonend > vonstart ) ) {
      
    namebfptr = nametok [ vonend - 1 ] ; 
    namebfxptr = nametok [ vonend ] ; 
    if ( ( vontokenfound () ) ) 
    goto lab10 ; 
    decr ( vonend ) ; 
  } 
  lab10: ; 
} 
void skipstuffatspbracele ( ) 
{while ( ( ( spbracelevel > 1 ) && ( spptr < spend ) ) ) {
    
    if ( ( strpool [ spptr ] == 125 ) ) 
    decr ( spbracelevel ) ; 
    else if ( ( strpool [ spptr ] == 123 ) ) 
    incr ( spbracelevel ) ; 
    incr ( spptr ) ; 
  } 
} 
void bracelvlonelettersco ( ) 
{{
    
    (void) Fputs( logfile ,  "The format string \"" ) ; 
    (void) Fputs( stdout ,  "The format string \"" ) ; 
  } 
  printapoolstr ( poplit1 ) ; 
  {
    {
      (void) Fputs( logfile ,  "\" has an illegal brace-level-1 letter" ) ; 
      (void) Fputs( stdout ,  "\" has an illegal brace-level-1 letter" ) ; 
    } 
    bstexwarnprint () ; 
  } 
} 
boolean zenoughtextchars ( enoughchars ) 
bufpointer enoughchars ; 
{register boolean Result; numtextchars = 0 ; 
  exbufyptr = exbufxptr ; 
  while ( ( ( exbufyptr < exbufptr ) && ( numtextchars < enoughchars ) ) ) {
      
    incr ( exbufyptr ) ; 
    if ( ( exbuf [ exbufyptr - 1 ] == 123 ) ) 
    {
      incr ( bracelevel ) ; 
      if ( ( ( bracelevel == 1 ) && ( exbufyptr < exbufptr ) ) ) 
      if ( ( exbuf [ exbufyptr ] == 92 ) ) 
      {
	incr ( exbufyptr ) ; 
	while ( ( ( exbufyptr < exbufptr ) && ( bracelevel > 0 ) ) ) {
	    
	  if ( ( exbuf [ exbufyptr ] == 125 ) ) 
	  decr ( bracelevel ) ; 
	  else if ( ( exbuf [ exbufyptr ] == 123 ) ) 
	  incr ( bracelevel ) ; 
	  incr ( exbufyptr ) ; 
	} 
      } 
    } 
    else if ( ( exbuf [ exbufyptr - 1 ] == 125 ) ) 
    decr ( bracelevel ) ; 
    incr ( numtextchars ) ; 
  } 
  if ( ( numtextchars < enoughchars ) ) 
  Result = false ; 
  else Result = true ; 
  return(Result) ; 
} 
void figureouttheformatte ( ) 
{/* 15 */ {
    
    exbufptr = 0 ; 
    spbracelevel = 0 ; 
    spptr = strstart [ poplit1 ] ; 
    spend = strstart [ poplit1 + 1 ] ; 
    while ( ( spptr < spend ) ) if ( ( strpool [ spptr ] == 123 ) ) 
    {
      incr ( spbracelevel ) ; 
      incr ( spptr ) ; 
      {
	spxptr1 = spptr ; 
	alphafound = false ; 
	doubleletter = false ; 
	endofgroup = false ; 
	tobewritten = true ; 
	while ( ( ( ! endofgroup ) && ( spptr < spend ) ) ) if ( ( lexclass [ 
	strpool [ spptr ] ] == 2 ) ) 
	{
	  incr ( spptr ) ; 
	  {
	    if ( ( alphafound ) ) 
	    {
	      bracelvlonelettersco () ; 
	      tobewritten = false ; 
	    } 
	    else {
		
	      switch ( ( strpool [ spptr - 1 ] ) ) 
	      {case 102 : 
	      case 70 : 
		{
		  curtoken = firststart ; 
		  lasttoken = firstend ; 
		  if ( ( curtoken == lasttoken ) ) 
		  tobewritten = false ; 
		  if ( ( ( strpool [ spptr ] == 102 ) || ( strpool [ spptr ] 
		  == 70 ) ) ) 
		  doubleletter = true ; 
		} 
		break ; 
	      case 118 : 
	      case 86 : 
		{
		  curtoken = vonstart ; 
		  lasttoken = vonend ; 
		  if ( ( curtoken == lasttoken ) ) 
		  tobewritten = false ; 
		  if ( ( ( strpool [ spptr ] == 118 ) || ( strpool [ spptr ] 
		  == 86 ) ) ) 
		  doubleletter = true ; 
		} 
		break ; 
	      case 108 : 
	      case 76 : 
		{
		  curtoken = vonend ; 
		  lasttoken = lastend ; 
		  if ( ( curtoken == lasttoken ) ) 
		  tobewritten = false ; 
		  if ( ( ( strpool [ spptr ] == 108 ) || ( strpool [ spptr ] 
		  == 76 ) ) ) 
		  doubleletter = true ; 
		} 
		break ; 
	      case 106 : 
	      case 74 : 
		{
		  curtoken = lastend ; 
		  lasttoken = jrend ; 
		  if ( ( curtoken == lasttoken ) ) 
		  tobewritten = false ; 
		  if ( ( ( strpool [ spptr ] == 106 ) || ( strpool [ spptr ] 
		  == 74 ) ) ) 
		  doubleletter = true ; 
		} 
		break ; 
		default: 
		{
		  bracelvlonelettersco () ; 
		  tobewritten = false ; 
		} 
		break ; 
	      } 
	      if ( ( doubleletter ) ) 
	      incr ( spptr ) ; 
	    } 
	    alphafound = true ; 
	  } 
	} 
	else if ( ( strpool [ spptr ] == 125 ) ) 
	{
	  decr ( spbracelevel ) ; 
	  incr ( spptr ) ; 
	  endofgroup = true ; 
	} 
	else if ( ( strpool [ spptr ] == 123 ) ) 
	{
	  incr ( spbracelevel ) ; 
	  incr ( spptr ) ; 
	  skipstuffatspbracele () ; 
	} 
	else incr ( spptr ) ; 
	if ( ( ( endofgroup ) && ( tobewritten ) ) ) 
	{
	  exbufxptr = exbufptr ; 
	  spptr = spxptr1 ; 
	  spbracelevel = 1 ; 
	  while ( ( spbracelevel > 0 ) ) if ( ( ( lexclass [ strpool [ spptr ] 
	  ] == 2 ) && ( spbracelevel == 1 ) ) ) 
	  {
	    incr ( spptr ) ; 
	    {
	      if ( ( doubleletter ) ) 
	      incr ( spptr ) ; 
	      usedefault = true ; 
	      spxptr2 = spptr ; 
	      if ( ( strpool [ spptr ] == 123 ) ) 
	      {
		usedefault = false ; 
		incr ( spbracelevel ) ; 
		incr ( spptr ) ; 
		spxptr1 = spptr ; 
		skipstuffatspbracele () ; 
		spxptr2 = spptr - 1 ; 
	      } 
	      while ( ( curtoken < lasttoken ) ) {
		  
		if ( ( doubleletter ) ) 
		{
		  namebfptr = nametok [ curtoken ] ; 
		  namebfxptr = nametok [ curtoken + 1 ] ; 
		  if ( ( exbuflength + ( namebfxptr - namebfptr ) > bufsize ) 
		  ) 
		  bufferoverflow () ; 
		  while ( ( namebfptr < namebfxptr ) ) {
		      
		    {
		      exbuf [ exbufptr ] = svbuffer [ namebfptr ] ; 
		      incr ( exbufptr ) ; 
		    } 
		    incr ( namebfptr ) ; 
		  } 
		} 
		else {
		    
		  namebfptr = nametok [ curtoken ] ; 
		  namebfxptr = nametok [ curtoken + 1 ] ; 
		  while ( ( namebfptr < namebfxptr ) ) {
		      
		    if ( ( lexclass [ svbuffer [ namebfptr ] ] == 2 ) ) 
		    {
		      {
			if ( ( exbufptr == bufsize ) ) 
			bufferoverflow () ; 
			{
			  exbuf [ exbufptr ] = svbuffer [ namebfptr ] ; 
			  incr ( exbufptr ) ; 
			} 
		      } 
		      goto lab15 ; 
		    } 
		    else if ( ( ( svbuffer [ namebfptr ] == 123 ) && ( 
		    namebfptr + 1 < namebfxptr ) ) ) 
		    if ( ( svbuffer [ namebfptr + 1 ] == 92 ) ) 
		    {
		      if ( ( exbufptr + 2 > bufsize ) ) 
		      bufferoverflow () ; 
		      {
			exbuf [ exbufptr ] = 123 ; 
			incr ( exbufptr ) ; 
		      } 
		      {
			exbuf [ exbufptr ] = 92 ; 
			incr ( exbufptr ) ; 
		      } 
		      namebfptr = namebfptr + 2 ; 
		      nmbracelevel = 1 ; 
		      while ( ( ( namebfptr < namebfxptr ) && ( nmbracelevel > 
		      0 ) ) ) {
			  
			if ( ( svbuffer [ namebfptr ] == 125 ) ) 
			decr ( nmbracelevel ) ; 
			else if ( ( svbuffer [ namebfptr ] == 123 ) ) 
			incr ( nmbracelevel ) ; 
			{
			  if ( ( exbufptr == bufsize ) ) 
			  bufferoverflow () ; 
			  {
			    exbuf [ exbufptr ] = svbuffer [ namebfptr ] ; 
			    incr ( exbufptr ) ; 
			  } 
			} 
			incr ( namebfptr ) ; 
		      } 
		      goto lab15 ; 
		    } 
		    incr ( namebfptr ) ; 
		  } 
		  lab15: ; 
		} 
		incr ( curtoken ) ; 
		if ( ( curtoken < lasttoken ) ) 
		{
		  if ( ( usedefault ) ) 
		  {
		    if ( ( ! doubleletter ) ) 
		    {
		      if ( ( exbufptr == bufsize ) ) 
		      bufferoverflow () ; 
		      {
			exbuf [ exbufptr ] = 46 ; 
			incr ( exbufptr ) ; 
		      } 
		    } 
		    if ( ( lexclass [ namesepchar [ curtoken ] ] == 4 ) ) 
		    {
		      if ( ( exbufptr == bufsize ) ) 
		      bufferoverflow () ; 
		      {
			exbuf [ exbufptr ] = namesepchar [ curtoken ] ; 
			incr ( exbufptr ) ; 
		      } 
		    } 
		    else if ( ( ( curtoken == lasttoken - 1 ) || ( ! 
		    enoughtextchars ( 3 ) ) ) ) 
		    {
		      if ( ( exbufptr == bufsize ) ) 
		      bufferoverflow () ; 
		      {
			exbuf [ exbufptr ] = 126 ; 
			incr ( exbufptr ) ; 
		      } 
		    } 
		    else {
			
		      if ( ( exbufptr == bufsize ) ) 
		      bufferoverflow () ; 
		      {
			exbuf [ exbufptr ] = 32 ; 
			incr ( exbufptr ) ; 
		      } 
		    } 
		  } 
		  else {
		      
		    if ( ( exbuflength + ( spxptr2 - spxptr1 ) > bufsize ) ) 
		    bufferoverflow () ; 
		    spptr = spxptr1 ; 
		    while ( ( spptr < spxptr2 ) ) {
			
		      {
			exbuf [ exbufptr ] = strpool [ spptr ] ; 
			incr ( exbufptr ) ; 
		      } 
		      incr ( spptr ) ; 
		    } 
		  } 
		} 
	      } 
	      if ( ( ! usedefault ) ) 
	      spptr = spxptr2 + 1 ; 
	    } 
	  } 
	  else if ( ( strpool [ spptr ] == 125 ) ) 
	  {
	    decr ( spbracelevel ) ; 
	    incr ( spptr ) ; 
	    if ( ( spbracelevel > 0 ) ) 
	    {
	      if ( ( exbufptr == bufsize ) ) 
	      bufferoverflow () ; 
	      {
		exbuf [ exbufptr ] = 125 ; 
		incr ( exbufptr ) ; 
	      } 
	    } 
	  } 
	  else if ( ( strpool [ spptr ] == 123 ) ) 
	  {
	    incr ( spbracelevel ) ; 
	    incr ( spptr ) ; 
	    {
	      if ( ( exbufptr == bufsize ) ) 
	      bufferoverflow () ; 
	      {
		exbuf [ exbufptr ] = 123 ; 
		incr ( exbufptr ) ; 
	      } 
	    } 
	  } 
	  else {
	      
	    {
	      if ( ( exbufptr == bufsize ) ) 
	      bufferoverflow () ; 
	      {
		exbuf [ exbufptr ] = strpool [ spptr ] ; 
		incr ( exbufptr ) ; 
	      } 
	    } 
	    incr ( spptr ) ; 
	  } 
	  if ( ( exbufptr > 0 ) ) 
	  if ( ( exbuf [ exbufptr - 1 ] == 126 ) ) 
	  {
	    decr ( exbufptr ) ; 
	    if ( ( exbuf [ exbufptr - 1 ] == 126 ) ) 
	    ; 
	    else if ( ( ! enoughtextchars ( 3 ) ) ) 
	    incr ( exbufptr ) ; 
	    else {
		
	      exbuf [ exbufptr ] = 32 ; 
	      incr ( exbufptr ) ; 
	    } 
	  } 
	} 
      } 
    } 
    else if ( ( strpool [ spptr ] == 125 ) ) 
    {
      bracesunbalancedcomp ( poplit1 ) ; 
      incr ( spptr ) ; 
    } 
    else {
	
      {
	if ( ( exbufptr == bufsize ) ) 
	bufferoverflow () ; 
	{
	  exbuf [ exbufptr ] = strpool [ spptr ] ; 
	  incr ( exbufptr ) ; 
	} 
      } 
      incr ( spptr ) ; 
    } 
    if ( ( spbracelevel > 0 ) ) 
    bracesunbalancedcomp ( poplit1 ) ; 
    exbuflength = exbufptr ; 
  } 
} 
void zpushlitstk ( pushlt , pushtype ) 
integer pushlt ; 
stktype pushtype ; 
{
#ifdef TRACE
  litstkloc dumptr  ; 
#endif /* TRACE */
  litstack [ litstkptr ] = pushlt ; 
  litstktype [ litstkptr ] = pushtype ; 
	;
#ifdef TRACE
  {register integer for_end; dumptr = 0 ; for_end = litstkptr ; if ( dumptr 
  <= for_end) do 
    {
      (void) Fputs( logfile ,  "  " ) ; 
    } 
  while ( dumptr++ < for_end ) ; } 
  {
    (void) Fputs( logfile ,  "Pushing " ) ; 
  } 
  switch ( ( litstktype [ litstkptr ] ) ) 
  {case 0 : 
    {
      (void) fprintf( logfile , "%ld\n",  (long)litstack [ litstkptr ] ) ; 
    } 
    break ; 
  case 1 : 
    {
      {
	(void) putc( '"' ,  logfile );
      } 
      {
	outpoolstr ( logfile , litstack [ litstkptr ] ) ; 
      } 
      {
	(void) fprintf( logfile , "%c\n",  '"' ) ; 
      } 
    } 
    break ; 
  case 2 : 
    {
      {
	(void) putc( '`' ,  logfile );
      } 
      {
	outpoolstr ( logfile , hashtext [ litstack [ litstkptr ] ] ) ; 
      } 
      {
	(void) fprintf( logfile , "%c\n",  '\'' ) ; 
      } 
    } 
    break ; 
  case 3 : 
    {
      {
	(void) Fputs( logfile ,  "missing field `" ) ; 
      } 
      {
	outpoolstr ( logfile , litstack [ litstkptr ] ) ; 
      } 
      {
	(void) fprintf( logfile , "%c\n",  '\'' ) ; 
      } 
    } 
    break ; 
  case 4 : 
    {
      (void) fprintf( logfile , "%s\n",  "a bad literal--popped from an empty stack" ) ; 
    } 
    break ; 
    default: 
    unknwnliteralconfusi () ; 
    break ; 
  } 
#endif /* TRACE */
  if ( ( litstkptr == litstksize ) ) 
  {
    printoverflow () ; 
    {
      (void) fprintf( logfile , "%s%ld\n",  "literal-stack size " , (long)litstksize ) ; 
      (void) fprintf( stdout , "%s%ld\n",  "literal-stack size " , (long)litstksize ) ; 
    } 
    longjmp(jmp9998,1) ; 
  } 
  incr ( litstkptr ) ; 
} 
void zpoplitstk ( poplit , poptype ) 
integer * poplit ; 
stktype * poptype ; 
{if ( ( litstkptr == 0 ) ) 
  {
    {
      {
	(void) Fputs( logfile ,  "You can't pop an empty literal stack" ) ; 
	(void) Fputs( stdout ,  "You can't pop an empty literal stack" ) ; 
      } 
      bstexwarnprint () ; 
    } 
*    poptype = 4 ; 
  } 
  else {
      
    decr ( litstkptr ) ; 
*    poplit = litstack [ litstkptr ] ; 
*    poptype = litstktype [ litstkptr ] ; 
    if ( ( *poptype == 1 ) ) 
    if ( ( *poplit >= cmdstrptr ) ) 
    {
      if ( ( *poplit != strptr - 1 ) ) 
      {
	{
	  (void) Fputs( logfile ,  "Nontop top of string stack" ) ; 
	  (void) Fputs( stdout ,  "Nontop top of string stack" ) ; 
	} 
	printconfusion () ; 
	longjmp(jmp9998,1) ; 
      } 
      {
	decr ( strptr ) ; 
	poolptr = strstart [ strptr ] ; 
      } 
    } 
  } 
} 
void zprintwrongstklit ( stklt , stktp1 , stktp2 ) 
integer stklt ; 
stktype stktp1 ; 
stktype stktp2 ; 
{if ( ( stktp1 != 4 ) ) 
  {
    printstklit ( stklt , stktp1 ) ; 
    switch ( ( stktp2 ) ) 
    {case 0 : 
      {
	(void) Fputs( logfile ,  ", not an integer," ) ; 
	(void) Fputs( stdout ,  ", not an integer," ) ; 
      } 
      break ; 
    case 1 : 
      {
	(void) Fputs( logfile ,  ", not a string," ) ; 
	(void) Fputs( stdout ,  ", not a string," ) ; 
      } 
      break ; 
    case 2 : 
      {
	(void) Fputs( logfile ,  ", not a function," ) ; 
	(void) Fputs( stdout ,  ", not a function," ) ; 
      } 
      break ; 
    case 3 : 
    case 4 : 
      illeglliteralconfusi () ; 
      break ; 
      default: 
      unknwnliteralconfusi () ; 
      break ; 
    } 
    bstexwarnprint () ; 
  } 
} 
void poptopandprint ( ) 
{integer stklt  ; 
  stktype stktp  ; 
  poplitstk ( stklt , stktp ) ; 
  if ( ( stktp == 4 ) ) 
  {
    (void) fprintf( logfile , "%s\n",  "Empty literal" ) ; 
    (void) fprintf( stdout , "%s\n",  "Empty literal" ) ; 
  } 
  else printlit ( stklt , stktp ) ; 
} 
void popwholestack ( ) 
{while ( ( litstkptr > 0 ) ) poptopandprint () ; 
} 
void initcommandexecution ( ) 
{litstkptr = 0 ; 
  cmdstrptr = strptr ; 
} 
void checkcommandexecutio ( ) 
{if ( ( litstkptr != 0 ) ) 
  {
    {
      (void) fprintf( logfile , "%s%ld%s\n",  "ptr=" , (long)litstkptr , ", stack=" ) ; 
      (void) fprintf( stdout , "%s%ld%s\n",  "ptr=" , (long)litstkptr , ", stack=" ) ; 
    } 
    popwholestack () ; 
    {
      {
	(void) Fputs( logfile ,  "---the literal stack isn't empty" ) ; 
	(void) Fputs( stdout ,  "---the literal stack isn't empty" ) ; 
      } 
      bstexwarnprint () ; 
    } 
  } 
  if ( ( cmdstrptr != strptr ) ) 
  {
	;
#ifdef TRACE
    {
      (void) fprintf( logfile , "%s%ld%s%ld\n",  "Pointer is " , (long)strptr , " but should be " ,       (long)cmdstrptr ) ; 
      (void) fprintf( stdout , "%s%ld%s%ld\n",  "Pointer is " , (long)strptr , " but should be " ,       (long)cmdstrptr ) ; 
    } 
#endif /* TRACE */
    {
      {
	(void) Fputs( logfile ,  "Nonempty empty string stack" ) ; 
	(void) Fputs( stdout ,  "Nonempty empty string stack" ) ; 
      } 
      printconfusion () ; 
      longjmp(jmp9998,1) ; 
    } 
  } 
} 
void addpoolbufandpush ( ) 
{{
    
    if ( ( poolptr + exbuflength > poolsize ) ) 
    pooloverflow () ; 
  } 
  exbufptr = 0 ; 
  while ( ( exbufptr < exbuflength ) ) {
      
    {
      strpool [ poolptr ] = exbuf [ exbufptr ] ; 
      incr ( poolptr ) ; 
    } 
    incr ( exbufptr ) ; 
  } 
  pushlitstk ( makestring () , 1 ) ; 
} 
void zaddbufpool ( pstr ) 
strnumber pstr ; 
{pptr1 = strstart [ pstr ] ; 
  pptr2 = strstart [ pstr + 1 ] ; 
  if ( ( exbuflength + ( pptr2 - pptr1 ) > bufsize ) ) 
  bufferoverflow () ; 
  exbufptr = exbuflength ; 
  while ( ( pptr1 < pptr2 ) ) {
      
    {
      exbuf [ exbufptr ] = strpool [ pptr1 ] ; 
      incr ( exbufptr ) ; 
    } 
    incr ( pptr1 ) ; 
  } 
  exbuflength = exbufptr ; 
} 
void zaddoutpool ( pstr ) 
strnumber pstr ; 
{bufpointer breakptr  ; 
  bufpointer endptr  ; 
  pptr1 = strstart [ pstr ] ; 
  pptr2 = strstart [ pstr + 1 ] ; 
  if ( ( outbuflength + ( pptr2 - pptr1 ) > bufsize ) ) 
  {
    printoverflow () ; 
    {
      (void) fprintf( logfile , "%s%ld\n",  "output buffer size " , (long)bufsize ) ; 
      (void) fprintf( stdout , "%s%ld\n",  "output buffer size " , (long)bufsize ) ; 
    } 
    longjmp(jmp9998,1) ; 
  } 
  outbufptr = outbuflength ; 
  while ( ( pptr1 < pptr2 ) ) {
      
    outbuf [ outbufptr ] = strpool [ pptr1 ] ; 
    incr ( pptr1 ) ; 
    incr ( outbufptr ) ; 
  } 
  outbuflength = outbufptr ; 
  while ( ( outbuflength > maxprintline ) ) {
      
    endptr = outbuflength ; 
    outbufptr = maxprintline ; 
    while ( ( ( lexclass [ outbuf [ outbufptr ] ] != 1 ) && ( outbufptr >= 
    minprintline ) ) ) decr ( outbufptr ) ; 
    if ( ( outbufptr == minprintline - 1 ) ) 
    {
      outbuf [ endptr ] = outbuf [ maxprintline - 1 ] ; 
      outbuf [ maxprintline - 1 ] = 37 ; 
      outbuflength = maxprintline ; 
      breakptr = outbuflength - 1 ; 
      outputbblline () ; 
      outbuf [ maxprintline - 1 ] = outbuf [ endptr ] ; 
      outbufptr = 0 ; 
      tmpptr = breakptr ; 
      while ( ( tmpptr < endptr ) ) {
	  
	outbuf [ outbufptr ] = outbuf [ tmpptr ] ; 
	incr ( outbufptr ) ; 
	incr ( tmpptr ) ; 
      } 
      outbuflength = endptr - breakptr ; 
    } 
    else {
	
      outbuflength = outbufptr ; 
      breakptr = outbuflength + 1 ; 
      outputbblline () ; 
      outbuf [ 0 ] = 32 ; 
      outbuf [ 1 ] = 32 ; 
      outbufptr = 2 ; 
      tmpptr = breakptr ; 
      while ( ( tmpptr < endptr ) ) {
	  
	outbuf [ outbufptr ] = outbuf [ tmpptr ] ; 
	incr ( outbufptr ) ; 
	incr ( tmpptr ) ; 
      } 
      outbuflength = endptr - breakptr + 2 ; 
    } 
  } 
} 
void xequals ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  poplitstk ( poplit2 , poptyp2 ) ; 
  if ( ( poptyp1 != poptyp2 ) ) 
  {
    if ( ( ( poptyp1 != 4 ) && ( poptyp2 != 4 ) ) ) 
    {
      printstklit ( poplit1 , poptyp1 ) ; 
      {
	(void) Fputs( logfile ,  ", " ) ; 
	(void) Fputs( stdout ,  ", " ) ; 
      } 
      printstklit ( poplit2 , poptyp2 ) ; 
      printanewline () ; 
      {
	{
	  (void) Fputs( logfile ,  "---they aren't the same literal types" ) ; 
	  (void) Fputs( stdout ,  "---they aren't the same literal types" ) ; 
	} 
	bstexwarnprint () ; 
      } 
    } 
    pushlitstk ( 0 , 0 ) ; 
  } 
  else if ( ( ( poptyp1 != 0 ) && ( poptyp1 != 1 ) ) ) 
  {
    if ( ( poptyp1 != 4 ) ) 
    {
      printstklit ( poplit1 , poptyp1 ) ; 
      {
	{
	  (void) Fputs( logfile ,  ", not an integer or a string," ) ; 
	  (void) Fputs( stdout ,  ", not an integer or a string," ) ; 
	} 
	bstexwarnprint () ; 
      } 
    } 
    pushlitstk ( 0 , 0 ) ; 
  } 
  else if ( ( poptyp1 == 0 ) ) 
  if ( ( poplit2 == poplit1 ) ) 
  pushlitstk ( 1 , 0 ) ; 
  else pushlitstk ( 0 , 0 ) ; 
  else if ( ( streqstr ( poplit2 , poplit1 ) ) ) 
  pushlitstk ( 1 , 0 ) ; 
  else pushlitstk ( 0 , 0 ) ; 
} 
void xgreaterthan ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  poplitstk ( poplit2 , poptyp2 ) ; 
  if ( ( poptyp1 != 0 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 0 ) ; 
    pushlitstk ( 0 , 0 ) ; 
  } 
  else if ( ( poptyp2 != 0 ) ) 
  {
    printwrongstklit ( poplit2 , poptyp2 , 0 ) ; 
    pushlitstk ( 0 , 0 ) ; 
  } 
  else if ( ( poplit2 > poplit1 ) ) 
  pushlitstk ( 1 , 0 ) ; 
  else pushlitstk ( 0 , 0 ) ; 
} 
void xlessthan ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  poplitstk ( poplit2 , poptyp2 ) ; 
  if ( ( poptyp1 != 0 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 0 ) ; 
    pushlitstk ( 0 , 0 ) ; 
  } 
  else if ( ( poptyp2 != 0 ) ) 
  {
    printwrongstklit ( poplit2 , poptyp2 , 0 ) ; 
    pushlitstk ( 0 , 0 ) ; 
  } 
  else if ( ( poplit2 < poplit1 ) ) 
  pushlitstk ( 1 , 0 ) ; 
  else pushlitstk ( 0 , 0 ) ; 
} 
void xplus ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  poplitstk ( poplit2 , poptyp2 ) ; 
  if ( ( poptyp1 != 0 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 0 ) ; 
    pushlitstk ( 0 , 0 ) ; 
  } 
  else if ( ( poptyp2 != 0 ) ) 
  {
    printwrongstklit ( poplit2 , poptyp2 , 0 ) ; 
    pushlitstk ( 0 , 0 ) ; 
  } 
  else pushlitstk ( poplit2 + poplit1 , 0 ) ; 
} 
void xminus ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  poplitstk ( poplit2 , poptyp2 ) ; 
  if ( ( poptyp1 != 0 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 0 ) ; 
    pushlitstk ( 0 , 0 ) ; 
  } 
  else if ( ( poptyp2 != 0 ) ) 
  {
    printwrongstklit ( poplit2 , poptyp2 , 0 ) ; 
    pushlitstk ( 0 , 0 ) ; 
  } 
  else pushlitstk ( poplit2 - poplit1 , 0 ) ; 
} 
void xconcatenate ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  poplitstk ( poplit2 , poptyp2 ) ; 
  if ( ( poptyp1 != 1 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 1 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else if ( ( poptyp2 != 1 ) ) 
  {
    printwrongstklit ( poplit2 , poptyp2 , 1 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else {
      
    if ( ( poplit2 >= cmdstrptr ) ) 
    if ( ( poplit1 >= cmdstrptr ) ) 
    {
      strstart [ poplit1 ] = strstart [ poplit1 + 1 ] ; 
      {
	incr ( strptr ) ; 
	poolptr = strstart [ strptr ] ; 
      } 
      incr ( litstkptr ) ; 
    } 
    else if ( ( ( strstart [ poplit2 + 1 ] - strstart [ poplit2 ] ) == 0 ) ) 
    pushlitstk ( poplit1 , 1 ) ; 
    else {
	
      poolptr = strstart [ poplit2 + 1 ] ; 
      {
	if ( ( poolptr + ( strstart [ poplit1 + 1 ] - strstart [ poplit1 ] ) > 
	poolsize ) ) 
	pooloverflow () ; 
      } 
      spptr = strstart [ poplit1 ] ; 
      spend = strstart [ poplit1 + 1 ] ; 
      while ( ( spptr < spend ) ) {
	  
	{
	  strpool [ poolptr ] = strpool [ spptr ] ; 
	  incr ( poolptr ) ; 
	} 
	incr ( spptr ) ; 
      } 
      pushlitstk ( makestring () , 1 ) ; 
    } 
    else {
	
      if ( ( poplit1 >= cmdstrptr ) ) 
      if ( ( ( strstart [ poplit2 + 1 ] - strstart [ poplit2 ] ) == 0 ) ) 
      {
	{
	  incr ( strptr ) ; 
	  poolptr = strstart [ strptr ] ; 
	} 
	litstack [ litstkptr ] = poplit1 ; 
	incr ( litstkptr ) ; 
      } 
      else if ( ( ( strstart [ poplit1 + 1 ] - strstart [ poplit1 ] ) == 0 ) ) 
      incr ( litstkptr ) ; 
      else {
	  
	splength = ( strstart [ poplit1 + 1 ] - strstart [ poplit1 ] ) ; 
	sp2length = ( strstart [ poplit2 + 1 ] - strstart [ poplit2 ] ) ; 
	{
	  if ( ( poolptr + splength + sp2length > poolsize ) ) 
	  pooloverflow () ; 
	} 
	spptr = strstart [ poplit1 + 1 ] ; 
	spend = strstart [ poplit1 ] ; 
	spxptr1 = spptr + sp2length ; 
	while ( ( spptr > spend ) ) {
	    
	  decr ( spptr ) ; 
	  decr ( spxptr1 ) ; 
	  strpool [ spxptr1 ] = strpool [ spptr ] ; 
	} 
	spptr = strstart [ poplit2 ] ; 
	spend = strstart [ poplit2 + 1 ] ; 
	while ( ( spptr < spend ) ) {
	    
	  {
	    strpool [ poolptr ] = strpool [ spptr ] ; 
	    incr ( poolptr ) ; 
	  } 
	  incr ( spptr ) ; 
	} 
	poolptr = poolptr + splength ; 
	pushlitstk ( makestring () , 1 ) ; 
      } 
      else {
	  
	if ( ( ( strstart [ poplit1 + 1 ] - strstart [ poplit1 ] ) == 0 ) ) 
	incr ( litstkptr ) ; 
	else if ( ( ( strstart [ poplit2 + 1 ] - strstart [ poplit2 ] ) == 0 ) 
	) 
	pushlitstk ( poplit1 , 1 ) ; 
	else {
	    
	  {
	    if ( ( poolptr + ( strstart [ poplit1 + 1 ] - strstart [ poplit1 ] 
	    ) + ( strstart [ poplit2 + 1 ] - strstart [ poplit2 ] ) > poolsize 
	    ) ) 
	    pooloverflow () ; 
	  } 
	  spptr = strstart [ poplit2 ] ; 
	  spend = strstart [ poplit2 + 1 ] ; 
	  while ( ( spptr < spend ) ) {
	      
	    {
	      strpool [ poolptr ] = strpool [ spptr ] ; 
	      incr ( poolptr ) ; 
	    } 
	    incr ( spptr ) ; 
	  } 
	  spptr = strstart [ poplit1 ] ; 
	  spend = strstart [ poplit1 + 1 ] ; 
	  while ( ( spptr < spend ) ) {
	      
	    {
	      strpool [ poolptr ] = strpool [ spptr ] ; 
	      incr ( poolptr ) ; 
	    } 
	    incr ( spptr ) ; 
	  } 
	  pushlitstk ( makestring () , 1 ) ; 
	} 
      } 
    } 
  } 
} 
void xgets ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  poplitstk ( poplit2 , poptyp2 ) ; 
  if ( ( poptyp1 != 2 ) ) 
  printwrongstklit ( poplit1 , poptyp1 , 2 ) ; 
  else if ( ( ( ! messwithentries ) && ( ( fntype [ poplit1 ] == 6 ) || ( 
  fntype [ poplit1 ] == 5 ) ) ) ) 
  bstcantmesswithentri () ; 
  else switch ( ( fntype [ poplit1 ] ) ) 
  {case 5 : 
    if ( ( poptyp2 != 0 ) ) 
    printwrongstklit ( poplit2 , poptyp2 , 0 ) ; 
    else entryints [ citeptr * numentints + ilkinfo [ poplit1 ] ] = poplit2 ; 
    break ; 
  case 6 : 
    {
      if ( ( poptyp2 != 1 ) ) 
      printwrongstklit ( poplit2 , poptyp2 , 1 ) ; 
      else {
	  
	strentptr = citeptr * numentstrs + ilkinfo [ poplit1 ] ; 
	entchrptr = 0 ; 
	spptr = strstart [ poplit2 ] ; 
	spxptr1 = strstart [ poplit2 + 1 ] ; 
	if ( ( spxptr1 - spptr > entstrsize ) ) 
	{
	  {
	    bst1printstringsizee () ; 
	    {
	      (void) fprintf( logfile , "%ld%s",  (long)entstrsize , ", the entry" ) ; 
	      (void) fprintf( stdout , "%ld%s",  (long)entstrsize , ", the entry" ) ; 
	    } 
	    bst2printstringsizee () ; 
	  } 
	  spxptr1 = spptr + entstrsize ; 
	} 
	while ( ( spptr < spxptr1 ) ) {
	    
	  entrystrs [ strentptr ] [ entchrptr ] = strpool [ spptr ] ; 
	  incr ( entchrptr ) ; 
	  incr ( spptr ) ; 
	} 
	entrystrs [ strentptr ] [ entchrptr ] = 127 ; 
      } 
    } 
    break ; 
  case 7 : 
    if ( ( poptyp2 != 0 ) ) 
    printwrongstklit ( poplit2 , poptyp2 , 0 ) ; 
    else ilkinfo [ poplit1 ] = poplit2 ; 
    break ; 
  case 8 : 
    {
      if ( ( poptyp2 != 1 ) ) 
      printwrongstklit ( poplit2 , poptyp2 , 1 ) ; 
      else {
	  
	strglbptr = ilkinfo [ poplit1 ] ; 
	if ( ( poplit2 < cmdstrptr ) ) 
	glbstrptr [ strglbptr ] = poplit2 ; 
	else {
	    
	  glbstrptr [ strglbptr ] = 0 ; 
	  globchrptr = 0 ; 
	  spptr = strstart [ poplit2 ] ; 
	  spend = strstart [ poplit2 + 1 ] ; 
	  if ( ( spend - spptr > globstrsize ) ) 
	  {
	    {
	      bst1printstringsizee () ; 
	      {
		(void) fprintf( logfile , "%ld%s",  (long)globstrsize , ", the global" ) ; 
		(void) fprintf( stdout , "%ld%s",  (long)globstrsize , ", the global" ) ; 
	      } 
	      bst2printstringsizee () ; 
	    } 
	    spend = spptr + globstrsize ; 
	  } 
	  while ( ( spptr < spend ) ) {
	      
	    globalstrs [ strglbptr ] [ globchrptr ] = strpool [ spptr ] ; 
	    incr ( globchrptr ) ; 
	    incr ( spptr ) ; 
	  } 
	  glbstrend [ strglbptr ] = globchrptr ; 
	} 
      } 
    } 
    break ; 
    default: 
    {
      {
	(void) Fputs( logfile ,  "You can't assign to type " ) ; 
	(void) Fputs( stdout ,  "You can't assign to type " ) ; 
      } 
      printfnclass ( poplit1 ) ; 
      {
	{
	  (void) Fputs( logfile ,  ", a nonvariable function class" ) ; 
	  (void) Fputs( stdout ,  ", a nonvariable function class" ) ; 
	} 
	bstexwarnprint () ; 
      } 
    } 
    break ; 
  } 
} 
void xaddperiod ( ) 
{/* 15 */ poplitstk ( poplit1 , poptyp1 ) ; 
  if ( ( poptyp1 != 1 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 1 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else if ( ( ( strstart [ poplit1 + 1 ] - strstart [ poplit1 ] ) == 0 ) ) 
  pushlitstk ( snull , 1 ) ; 
  else {
      
    spptr = strstart [ poplit1 + 1 ] ; 
    spend = strstart [ poplit1 ] ; 
    while ( ( spptr > spend ) ) {
	
      decr ( spptr ) ; 
      if ( ( strpool [ spptr ] != 125 ) ) 
      goto lab15 ; 
    } 
    lab15: switch ( ( strpool [ spptr ] ) ) 
    {case 46 : 
    case 63 : 
    case 33 : 
      {
	if ( ( litstack [ litstkptr ] >= cmdstrptr ) ) 
	{
	  incr ( strptr ) ; 
	  poolptr = strstart [ strptr ] ; 
	} 
	incr ( litstkptr ) ; 
      } 
      break ; 
      default: 
      {
	if ( ( poplit1 < cmdstrptr ) ) 
	{
	  {
	    if ( ( poolptr + ( strstart [ poplit1 + 1 ] - strstart [ poplit1 ] 
	    ) + 1 > poolsize ) ) 
	    pooloverflow () ; 
	  } 
	  spptr = strstart [ poplit1 ] ; 
	  spend = strstart [ poplit1 + 1 ] ; 
	  while ( ( spptr < spend ) ) {
	      
	    {
	      strpool [ poolptr ] = strpool [ spptr ] ; 
	      incr ( poolptr ) ; 
	    } 
	    incr ( spptr ) ; 
	  } 
	} 
	else {
	    
	  poolptr = strstart [ poplit1 + 1 ] ; 
	  {
	    if ( ( poolptr + 1 > poolsize ) ) 
	    pooloverflow () ; 
	  } 
	} 
	{
	  strpool [ poolptr ] = 46 ; 
	  incr ( poolptr ) ; 
	} 
	pushlitstk ( makestring () , 1 ) ; 
      } 
      break ; 
    } 
  } 
} 
void xchangecase ( ) 
{/* 21 */ poplitstk ( poplit1 , poptyp1 ) ; 
  poplitstk ( poplit2 , poptyp2 ) ; 
  if ( ( poptyp1 != 1 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 1 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else if ( ( poptyp2 != 1 ) ) 
  {
    printwrongstklit ( poplit2 , poptyp2 , 1 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else {
      
    {
      switch ( ( strpool [ strstart [ poplit1 ] ] ) ) 
      {case 116 : 
      case 84 : 
	conversiontype = 0 ; 
	break ; 
      case 108 : 
      case 76 : 
	conversiontype = 1 ; 
	break ; 
      case 117 : 
      case 85 : 
	conversiontype = 2 ; 
	break ; 
	default: 
	conversiontype = 3 ; 
	break ; 
      } 
      if ( ( ( ( strstart [ poplit1 + 1 ] - strstart [ poplit1 ] ) != 1 ) || ( 
      conversiontype == 3 ) ) ) 
      {
	conversiontype = 3 ; 
	printapoolstr ( poplit1 ) ; 
	{
	  {
	    (void) Fputs( logfile ,  " is an illegal case-conversion string" ) ; 
	    (void) Fputs( stdout ,  " is an illegal case-conversion string" ) ; 
	  } 
	  bstexwarnprint () ; 
	} 
      } 
    } 
    exbuflength = 0 ; 
    addbufpool ( poplit2 ) ; 
    {
      bracelevel = 0 ; 
      exbufptr = 0 ; 
      while ( ( exbufptr < exbuflength ) ) {
	  
	if ( ( exbuf [ exbufptr ] == 123 ) ) 
	{
	  incr ( bracelevel ) ; 
	  if ( ( bracelevel != 1 ) ) 
	  goto lab21 ; 
	  if ( ( exbufptr + 4 > exbuflength ) ) 
	  goto lab21 ; 
	  else if ( ( exbuf [ exbufptr + 1 ] != 92 ) ) 
	  goto lab21 ; 
	  if ( ( conversiontype == 0 ) ) 
	  if ( ( exbufptr == 0 ) ) 
	  goto lab21 ; 
	  else if ( ( ( prevcolon ) && ( lexclass [ exbuf [ exbufptr - 1 ] ] 
	  == 1 ) ) ) 
	  goto lab21 ; 
	  {
	    incr ( exbufptr ) ; 
	    while ( ( ( exbufptr < exbuflength ) && ( bracelevel > 0 ) ) ) {
		
	      incr ( exbufptr ) ; 
	      exbufxptr = exbufptr ; 
	      while ( ( ( exbufptr < exbuflength ) && ( lexclass [ exbuf [ 
	      exbufptr ] ] == 2 ) ) ) incr ( exbufptr ) ; 
	      controlseqloc = strlookup ( exbuf , exbufxptr , exbufptr - 
	      exbufxptr , 14 , false ) ; 
	      if ( ( hashfound ) ) 
	      {
		switch ( ( conversiontype ) ) 
		{case 0 : 
		case 1 : 
		  switch ( ( ilkinfo [ controlseqloc ] ) ) 
		  {case 11 : 
		  case 9 : 
		  case 3 : 
		  case 5 : 
		  case 7 : 
		    lowercase ( exbuf , exbufxptr , exbufptr - exbufxptr ) ; 
		    break ; 
		    default: 
		    ; 
		    break ; 
		  } 
		  break ; 
		case 2 : 
		  switch ( ( ilkinfo [ controlseqloc ] ) ) 
		  {case 10 : 
		  case 8 : 
		  case 2 : 
		  case 4 : 
		  case 6 : 
		    uppercase ( exbuf , exbufxptr , exbufptr - exbufxptr ) ; 
		    break ; 
		  case 0 : 
		  case 1 : 
		  case 12 : 
		    {
		      uppercase ( exbuf , exbufxptr , exbufptr - exbufxptr ) ; 
		      while ( ( exbufxptr < exbufptr ) ) {
			  
			exbuf [ exbufxptr - 1 ] = exbuf [ exbufxptr ] ; 
			incr ( exbufxptr ) ; 
		      } 
		      decr ( exbufxptr ) ; 
		      while ( ( ( exbufptr < exbuflength ) && ( lexclass [ 
		      exbuf [ exbufptr ] ] == 1 ) ) ) incr ( exbufptr ) ; 
		      tmpptr = exbufptr ; 
		      while ( ( tmpptr < exbuflength ) ) {
			  
			exbuf [ tmpptr - ( exbufptr - exbufxptr ) ] = exbuf [ 
			tmpptr ] ; 
			incr ( tmpptr ) ; 
		      } 
		      exbuflength = tmpptr - ( exbufptr - exbufxptr ) ; 
		      exbufptr = exbufxptr ; 
		    } 
		    break ; 
		    default: 
		    ; 
		    break ; 
		  } 
		  break ; 
		case 3 : 
		  ; 
		  break ; 
		  default: 
		  caseconversionconfus () ; 
		  break ; 
		} 
	      } 
	      exbufxptr = exbufptr ; 
	      while ( ( ( exbufptr < exbuflength ) && ( bracelevel > 0 ) && ( 
	      exbuf [ exbufptr ] != 92 ) ) ) {
		  
		if ( ( exbuf [ exbufptr ] == 125 ) ) 
		decr ( bracelevel ) ; 
		else if ( ( exbuf [ exbufptr ] == 123 ) ) 
		incr ( bracelevel ) ; 
		incr ( exbufptr ) ; 
	      } 
	      {
		switch ( ( conversiontype ) ) 
		{case 0 : 
		case 1 : 
		  lowercase ( exbuf , exbufxptr , exbufptr - exbufxptr ) ; 
		  break ; 
		case 2 : 
		  uppercase ( exbuf , exbufxptr , exbufptr - exbufxptr ) ; 
		  break ; 
		case 3 : 
		  ; 
		  break ; 
		  default: 
		  caseconversionconfus () ; 
		  break ; 
		} 
	      } 
	    } 
	    decr ( exbufptr ) ; 
	  } 
	  lab21: prevcolon = false ; 
	} 
	else if ( ( exbuf [ exbufptr ] == 125 ) ) 
	{
	  decrbracelevel ( poplit2 ) ; 
	  prevcolon = false ; 
	} 
	else if ( ( bracelevel == 0 ) ) 
	{
	  switch ( ( conversiontype ) ) 
	  {case 0 : 
	    {
	      if ( ( exbufptr == 0 ) ) 
	      ; 
	      else if ( ( ( prevcolon ) && ( lexclass [ exbuf [ exbufptr - 1 ] 
	      ] == 1 ) ) ) 
	      ; 
	      else lowercase ( exbuf , exbufptr , 1 ) ; 
	      if ( ( exbuf [ exbufptr ] == 58 ) ) 
	      prevcolon = true ; 
	      else if ( ( lexclass [ exbuf [ exbufptr ] ] != 1 ) ) 
	      prevcolon = false ; 
	    } 
	    break ; 
	  case 1 : 
	    lowercase ( exbuf , exbufptr , 1 ) ; 
	    break ; 
	  case 2 : 
	    uppercase ( exbuf , exbufptr , 1 ) ; 
	    break ; 
	  case 3 : 
	    ; 
	    break ; 
	    default: 
	    caseconversionconfus () ; 
	    break ; 
	  } 
	} 
	incr ( exbufptr ) ; 
      } 
      checkbracelevel ( poplit2 ) ; 
    } 
    addpoolbufandpush () ; 
  } 
} 
void xchrtoint ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  if ( ( poptyp1 != 1 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 1 ) ; 
    pushlitstk ( 0 , 0 ) ; 
  } 
  else if ( ( ( strstart [ poplit1 + 1 ] - strstart [ poplit1 ] ) != 1 ) ) 
  {
    {
      (void) putc( '"' ,  logfile );
      (void) putc( '"' ,  stdout );
    } 
    printapoolstr ( poplit1 ) ; 
    {
      {
	(void) Fputs( logfile ,  "\" isn't a single character" ) ; 
	(void) Fputs( stdout ,  "\" isn't a single character" ) ; 
      } 
      bstexwarnprint () ; 
    } 
    pushlitstk ( 0 , 0 ) ; 
  } 
  else pushlitstk ( strpool [ strstart [ poplit1 ] ] , 0 ) ; 
} 
void xcite ( ) 
{if ( ( ! messwithentries ) ) 
  bstcantmesswithentri () ; 
  else pushlitstk ( citelist [ citeptr ] , 1 ) ; 
} 
void xduplicate ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  if ( ( poptyp1 != 1 ) ) 
  {
    pushlitstk ( poplit1 , poptyp1 ) ; 
    pushlitstk ( poplit1 , poptyp1 ) ; 
  } 
  else {
      
    {
      if ( ( litstack [ litstkptr ] >= cmdstrptr ) ) 
      {
	incr ( strptr ) ; 
	poolptr = strstart [ strptr ] ; 
      } 
      incr ( litstkptr ) ; 
    } 
    if ( ( poplit1 < cmdstrptr ) ) 
    pushlitstk ( poplit1 , poptyp1 ) ; 
    else {
	
      {
	if ( ( poolptr + ( strstart [ poplit1 + 1 ] - strstart [ poplit1 ] ) > 
	poolsize ) ) 
	pooloverflow () ; 
      } 
      spptr = strstart [ poplit1 ] ; 
      spend = strstart [ poplit1 + 1 ] ; 
      while ( ( spptr < spend ) ) {
	  
	{
	  strpool [ poolptr ] = strpool [ spptr ] ; 
	  incr ( poolptr ) ; 
	} 
	incr ( spptr ) ; 
      } 
      pushlitstk ( makestring () , 1 ) ; 
    } 
  } 
} 
void xempty ( ) 
{/* 10 */ poplitstk ( poplit1 , poptyp1 ) ; 
  switch ( ( poptyp1 ) ) 
  {case 1 : 
    {
      spptr = strstart [ poplit1 ] ; 
      spend = strstart [ poplit1 + 1 ] ; 
      while ( ( spptr < spend ) ) {
	  
	if ( ( lexclass [ strpool [ spptr ] ] != 1 ) ) 
	{
	  pushlitstk ( 0 , 0 ) ; 
	  goto lab10 ; 
	} 
	incr ( spptr ) ; 
      } 
      pushlitstk ( 1 , 0 ) ; 
    } 
    break ; 
  case 3 : 
    pushlitstk ( 1 , 0 ) ; 
    break ; 
  case 4 : 
    pushlitstk ( 0 , 0 ) ; 
    break ; 
    default: 
    {
      printstklit ( poplit1 , poptyp1 ) ; 
      {
	{
	  (void) Fputs( logfile ,  ", not a string or missing field," ) ; 
	  (void) Fputs( stdout ,  ", not a string or missing field," ) ; 
	} 
	bstexwarnprint () ; 
      } 
      pushlitstk ( 0 , 0 ) ; 
    } 
    break ; 
  } 
  lab10: ; 
} 
void xformatname ( ) 
{/* 16 17 52 */ poplitstk ( poplit1 , poptyp1 ) ; 
  poplitstk ( poplit2 , poptyp2 ) ; 
  poplitstk ( poplit3 , poptyp3 ) ; 
  if ( ( poptyp1 != 1 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 1 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else if ( ( poptyp2 != 0 ) ) 
  {
    printwrongstklit ( poplit2 , poptyp2 , 0 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else if ( ( poptyp3 != 1 ) ) 
  {
    printwrongstklit ( poplit3 , poptyp3 , 1 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else {
      
    exbuflength = 0 ; 
    addbufpool ( poplit3 ) ; 
    {
      exbufptr = 0 ; 
      numnames = 0 ; 
      while ( ( ( numnames < poplit2 ) && ( exbufptr < exbuflength ) ) ) {
	  
	incr ( numnames ) ; 
	exbufxptr = exbufptr ; 
	namescanforand ( poplit3 ) ; 
      } 
      if ( ( exbufptr < exbuflength ) ) 
      exbufptr = exbufptr - 4 ; 
      if ( ( numnames < poplit2 ) ) 
      {
	if ( ( poplit2 == 1 ) ) 
	{
	  (void) Fputs( logfile ,  "There is no name in \"" ) ; 
	  (void) Fputs( stdout ,  "There is no name in \"" ) ; 
	} 
	else {
	    
	  (void) fprintf( logfile , "%s%ld%s",  "There aren't " , (long)poplit2 , " names in \"" ) ; 
	  (void) fprintf( stdout , "%s%ld%s",  "There aren't " , (long)poplit2 , " names in \"" ) ; 
	} 
	printapoolstr ( poplit3 ) ; 
	{
	  {
	    (void) putc( '"' ,  logfile );
	    (void) putc( '"' ,  stdout );
	  } 
	  bstexwarnprint () ; 
	} 
      } 
    } 
    {
      {
	while ( ( ( exbufxptr < exbufptr ) && ( lexclass [ exbuf [ exbufptr ] 
	] == 1 ) && ( lexclass [ exbuf [ exbufptr ] ] == 4 ) ) ) incr ( 
	exbufxptr ) ; 
	while ( ( exbufptr > exbufxptr ) ) switch ( ( lexclass [ exbuf [ 
	exbufptr - 1 ] ] ) ) 
	{case 1 : 
	case 4 : 
	  decr ( exbufptr ) ; 
	  break ; 
	  default: 
	  if ( ( exbuf [ exbufptr - 1 ] == 44 ) ) 
	  {
	    {
	      (void) fprintf( logfile , "%s%ld%s",  "Name " , (long)poplit2 , " in \"" ) ; 
	      (void) fprintf( stdout , "%s%ld%s",  "Name " , (long)poplit2 , " in \"" ) ; 
	    } 
	    printapoolstr ( poplit3 ) ; 
	    {
	      (void) Fputs( logfile ,  "\" has a comma at the end" ) ; 
	      (void) Fputs( stdout ,  "\" has a comma at the end" ) ; 
	    } 
	    bstexwarnprint () ; 
	    decr ( exbufptr ) ; 
	  } 
	  else goto lab16 ; 
	  break ; 
	} 
	lab16: ; 
      } 
      namebfptr = 0 ; 
      numcommas = 0 ; 
      numtokens = 0 ; 
      tokenstarting = true ; 
      while ( ( exbufxptr < exbufptr ) ) switch ( ( exbuf [ exbufxptr ] ) ) 
      {case 44 : 
	{
	  if ( ( numcommas == 2 ) ) 
	  {
	    {
	      (void) fprintf( logfile , "%s%ld%s",  "Too many commas in name " , (long)poplit2 ,               " of \"" ) ; 
	      (void) fprintf( stdout , "%s%ld%s",  "Too many commas in name " , (long)poplit2 , " of \""               ) ; 
	    } 
	    printapoolstr ( poplit3 ) ; 
	    {
	      (void) putc( '"' ,  logfile );
	      (void) putc( '"' ,  stdout );
	    } 
	    bstexwarnprint () ; 
	  } 
	  else {
	      
	    incr ( numcommas ) ; 
	    if ( ( numcommas == 1 ) ) 
	    comma1 = numtokens ; 
	    else comma2 = numtokens ; 
	    namesepchar [ numtokens ] = 44 ; 
	  } 
	  incr ( exbufxptr ) ; 
	  tokenstarting = true ; 
	} 
	break ; 
      case 123 : 
	{
	  incr ( bracelevel ) ; 
	  if ( ( tokenstarting ) ) 
	  {
	    nametok [ numtokens ] = namebfptr ; 
	    incr ( numtokens ) ; 
	  } 
	  svbuffer [ namebfptr ] = exbuf [ exbufxptr ] ; 
	  incr ( namebfptr ) ; 
	  incr ( exbufxptr ) ; 
	  while ( ( ( bracelevel > 0 ) && ( exbufxptr < exbufptr ) ) ) {
	      
	    if ( ( exbuf [ exbufxptr ] == 125 ) ) 
	    decr ( bracelevel ) ; 
	    else if ( ( exbuf [ exbufxptr ] == 123 ) ) 
	    incr ( bracelevel ) ; 
	    svbuffer [ namebfptr ] = exbuf [ exbufxptr ] ; 
	    incr ( namebfptr ) ; 
	    incr ( exbufxptr ) ; 
	  } 
	  tokenstarting = false ; 
	} 
	break ; 
      case 125 : 
	{
	  if ( ( tokenstarting ) ) 
	  {
	    nametok [ numtokens ] = namebfptr ; 
	    incr ( numtokens ) ; 
	  } 
	  {
	    (void) fprintf( logfile , "%s%ld%s",  "Name " , (long)poplit2 , " of \"" ) ; 
	    (void) fprintf( stdout , "%s%ld%s",  "Name " , (long)poplit2 , " of \"" ) ; 
	  } 
	  printapoolstr ( poplit3 ) ; 
	  {
	    {
	      (void) Fputs( logfile ,  "\" isn't brace balanced" ) ; 
	      (void) Fputs( stdout ,  "\" isn't brace balanced" ) ; 
	    } 
	    bstexwarnprint () ; 
	  } 
	  incr ( exbufxptr ) ; 
	  tokenstarting = false ; 
	} 
	break ; 
	default: 
	switch ( ( lexclass [ exbuf [ exbufxptr ] ] ) ) 
	{case 1 : 
	  {
	    if ( ( ! tokenstarting ) ) 
	    namesepchar [ numtokens ] = 32 ; 
	    incr ( exbufxptr ) ; 
	    tokenstarting = true ; 
	  } 
	  break ; 
	case 4 : 
	  {
	    if ( ( ! tokenstarting ) ) 
	    namesepchar [ numtokens ] = exbuf [ exbufxptr ] ; 
	    incr ( exbufxptr ) ; 
	    tokenstarting = true ; 
	  } 
	  break ; 
	  default: 
	  {
	    if ( ( tokenstarting ) ) 
	    {
	      nametok [ numtokens ] = namebfptr ; 
	      incr ( numtokens ) ; 
	    } 
	    svbuffer [ namebfptr ] = exbuf [ exbufxptr ] ; 
	    incr ( namebfptr ) ; 
	    incr ( exbufxptr ) ; 
	    tokenstarting = false ; 
	  } 
	  break ; 
	} 
	break ; 
      } 
      nametok [ numtokens ] = namebfptr ; 
    } 
    {
      if ( ( numcommas == 0 ) ) 
      {
	firststart = 0 ; 
	lastend = numtokens ; 
	jrend = lastend ; 
	{
	  vonstart = 0 ; 
	  while ( ( vonstart < lastend - 1 ) ) {
	      
	    namebfptr = nametok [ vonstart ] ; 
	    namebfxptr = nametok [ vonstart + 1 ] ; 
	    if ( ( vontokenfound () ) ) 
	    {
	      vonnameendsandlastna () ; 
	      goto lab52 ; 
	    } 
	    incr ( vonstart ) ; 
	  } 
	  while ( ( vonstart > 0 ) ) {
	      
	    if ( ( ( lexclass [ namesepchar [ vonstart ] ] != 4 ) || ( 
	    namesepchar [ vonstart ] == 126 ) ) ) 
	    goto lab17 ; 
	    decr ( vonstart ) ; 
	  } 
	  lab17: vonend = vonstart ; 
	  lab52: firstend = vonstart ; 
	} 
      } 
      else if ( ( numcommas == 1 ) ) 
      {
	vonstart = 0 ; 
	lastend = comma1 ; 
	jrend = lastend ; 
	firststart = jrend ; 
	firstend = numtokens ; 
	vonnameendsandlastna () ; 
      } 
      else if ( ( numcommas == 2 ) ) 
      {
	vonstart = 0 ; 
	lastend = comma1 ; 
	jrend = comma2 ; 
	firststart = jrend ; 
	firstend = numtokens ; 
	vonnameendsandlastna () ; 
      } 
      else {
	  
	{
	  (void) Fputs( logfile ,  "Illegal number of comma,s" ) ; 
	  (void) Fputs( stdout ,  "Illegal number of comma,s" ) ; 
	} 
	printconfusion () ; 
	longjmp(jmp9998,1) ; 
      } 
    } 
    exbuflength = 0 ; 
    addbufpool ( poplit1 ) ; 
    figureouttheformatte () ; 
    addpoolbufandpush () ; 
  } 
} 
void xinttochr ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  if ( ( poptyp1 != 0 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 0 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else if ( ( ( poplit1 < 0 ) || ( poplit1 > 127 ) ) ) 
  {
    {
      {
	(void) fprintf( logfile , "%ld%s",  (long)poplit1 , " isn't valid ASCII" ) ; 
	(void) fprintf( stdout , "%ld%s",  (long)poplit1 , " isn't valid ASCII" ) ; 
      } 
      bstexwarnprint () ; 
    } 
    pushlitstk ( snull , 1 ) ; 
  } 
  else {
      
    {
      if ( ( poolptr + 1 > poolsize ) ) 
      pooloverflow () ; 
    } 
    {
      strpool [ poolptr ] = poplit1 ; 
      incr ( poolptr ) ; 
    } 
    pushlitstk ( makestring () , 1 ) ; 
  } 
} 
void xinttostr ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  if ( ( poptyp1 != 0 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 0 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else {
      
    inttoASCII ( poplit1 , exbuf , 0 , exbuflength ) ; 
    addpoolbufandpush () ; 
  } 
} 
void xmissing ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  if ( ( ! messwithentries ) ) 
  bstcantmesswithentri () ; 
  else if ( ( ( poptyp1 != 1 ) && ( poptyp1 != 3 ) ) ) 
  {
    if ( ( poptyp1 != 4 ) ) 
    {
      printstklit ( poplit1 , poptyp1 ) ; 
      {
	{
	  (void) Fputs( logfile ,  ", not a string or missing field," ) ; 
	  (void) Fputs( stdout ,  ", not a string or missing field," ) ; 
	} 
	bstexwarnprint () ; 
      } 
    } 
    pushlitstk ( 0 , 0 ) ; 
  } 
  else if ( ( poptyp1 == 3 ) ) 
  pushlitstk ( 1 , 0 ) ; 
  else pushlitstk ( 0 , 0 ) ; 
} 
void xnumnames ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  if ( ( poptyp1 != 1 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 1 ) ; 
    pushlitstk ( 0 , 0 ) ; 
  } 
  else {
      
    exbuflength = 0 ; 
    addbufpool ( poplit1 ) ; 
    {
      exbufptr = 0 ; 
      numnames = 0 ; 
      while ( ( exbufptr < exbuflength ) ) {
	  
	namescanforand ( poplit1 ) ; 
	incr ( numnames ) ; 
      } 
    } 
    pushlitstk ( numnames , 0 ) ; 
  } 
} 
void xpreamble ( ) 
{exbuflength = 0 ; 
  preambleptr = 0 ; 
  while ( ( preambleptr < numpreamblestrings ) ) {
      
    addbufpool ( spreamble [ preambleptr ] ) ; 
    incr ( preambleptr ) ; 
  } 
  addpoolbufandpush () ; 
} 
void xpurify ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  if ( ( poptyp1 != 1 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 1 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else {
      
    exbuflength = 0 ; 
    addbufpool ( poplit1 ) ; 
    {
      bracelevel = 0 ; 
      exbufxptr = 0 ; 
      exbufptr = 0 ; 
      while ( ( exbufptr < exbuflength ) ) {
	  
	switch ( ( lexclass [ exbuf [ exbufptr ] ] ) ) 
	{case 1 : 
	case 4 : 
	  {
	    exbuf [ exbufxptr ] = 32 ; 
	    incr ( exbufxptr ) ; 
	  } 
	  break ; 
	case 2 : 
	case 3 : 
	  {
	    exbuf [ exbufxptr ] = exbuf [ exbufptr ] ; 
	    incr ( exbufxptr ) ; 
	  } 
	  break ; 
	  default: 
	  if ( ( exbuf [ exbufptr ] == 123 ) ) 
	  {
	    incr ( bracelevel ) ; 
	    if ( ( ( bracelevel == 1 ) && ( exbufptr + 1 < exbuflength ) ) ) 
	    if ( ( exbuf [ exbufptr + 1 ] == 92 ) ) 
	    {
	      incr ( exbufptr ) ; 
	      while ( ( ( exbufptr < exbuflength ) && ( bracelevel > 0 ) ) ) {
		  
		incr ( exbufptr ) ; 
		exbufyptr = exbufptr ; 
		while ( ( ( exbufptr < exbuflength ) && ( lexclass [ exbuf [ 
		exbufptr ] ] == 2 ) ) ) incr ( exbufptr ) ; 
		controlseqloc = strlookup ( exbuf , exbufyptr , exbufptr - 
		exbufyptr , 14 , false ) ; 
		if ( ( hashfound ) ) 
		{
		  exbuf [ exbufxptr ] = exbuf [ exbufyptr ] ; 
		  incr ( exbufxptr ) ; 
		  switch ( ( ilkinfo [ controlseqloc ] ) ) 
		  {case 2 : 
		  case 3 : 
		  case 4 : 
		  case 5 : 
		  case 12 : 
		    {
		      exbuf [ exbufxptr ] = exbuf [ exbufyptr + 1 ] ; 
		      incr ( exbufxptr ) ; 
		    } 
		    break ; 
		    default: 
		    ; 
		    break ; 
		  } 
		} 
		while ( ( ( exbufptr < exbuflength ) && ( bracelevel > 0 ) && 
		( exbuf [ exbufptr ] != 92 ) ) ) {
		    
		  switch ( ( lexclass [ exbuf [ exbufptr ] ] ) ) 
		  {case 2 : 
		  case 3 : 
		    {
		      exbuf [ exbufxptr ] = exbuf [ exbufptr ] ; 
		      incr ( exbufxptr ) ; 
		    } 
		    break ; 
		    default: 
		    if ( ( exbuf [ exbufptr ] == 125 ) ) 
		    decr ( bracelevel ) ; 
		    else if ( ( exbuf [ exbufptr ] == 123 ) ) 
		    incr ( bracelevel ) ; 
		    break ; 
		  } 
		  incr ( exbufptr ) ; 
		} 
	      } 
	      decr ( exbufptr ) ; 
	    } 
	  } 
	  else if ( ( exbuf [ exbufptr ] == 125 ) ) 
	  if ( ( bracelevel > 0 ) ) 
	  decr ( bracelevel ) ; 
	  break ; 
	} 
	incr ( exbufptr ) ; 
      } 
      exbuflength = exbufxptr ; 
    } 
    addpoolbufandpush () ; 
  } 
} 
void xquote ( ) 
{{
    
    if ( ( poolptr + 1 > poolsize ) ) 
    pooloverflow () ; 
  } 
  {
    strpool [ poolptr ] = 34 ; 
    incr ( poolptr ) ; 
  } 
  pushlitstk ( makestring () , 1 ) ; 
} 
void xsubstring ( ) 
{/* 10 */ poplitstk ( poplit1 , poptyp1 ) ; 
  poplitstk ( poplit2 , poptyp2 ) ; 
  poplitstk ( poplit3 , poptyp3 ) ; 
  if ( ( poptyp1 != 0 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 0 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else if ( ( poptyp2 != 0 ) ) 
  {
    printwrongstklit ( poplit2 , poptyp2 , 0 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else if ( ( poptyp3 != 1 ) ) 
  {
    printwrongstklit ( poplit3 , poptyp3 , 1 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else {
      
    splength = ( strstart [ poplit3 + 1 ] - strstart [ poplit3 ] ) ; 
    if ( ( poplit1 >= splength ) ) 
    if ( ( ( poplit2 == 1 ) || ( poplit2 == -1 ) ) ) 
    {
      {
	if ( ( litstack [ litstkptr ] >= cmdstrptr ) ) 
	{
	  incr ( strptr ) ; 
	  poolptr = strstart [ strptr ] ; 
	} 
	incr ( litstkptr ) ; 
      } 
      goto lab10 ; 
    } 
    if ( ( ( poplit1 <= 0 ) || ( poplit2 == 0 ) || ( poplit2 > splength ) || ( 
    poplit2 < - (integer) splength ) ) ) 
    {
      pushlitstk ( snull , 1 ) ; 
      goto lab10 ; 
    } 
    else {
	
      if ( ( poplit2 > 0 ) ) 
      {
	if ( ( poplit1 > splength - ( poplit2 - 1 ) ) ) 
	poplit1 = splength - ( poplit2 - 1 ) ; 
	spptr = strstart [ poplit3 ] + ( poplit2 - 1 ) ; 
	spend = spptr + poplit1 ; 
	if ( ( poplit2 == 1 ) ) 
	if ( ( poplit3 >= cmdstrptr ) ) 
	{
	  strstart [ poplit3 + 1 ] = spend ; 
	  {
	    incr ( strptr ) ; 
	    poolptr = strstart [ strptr ] ; 
	  } 
	  incr ( litstkptr ) ; 
	  goto lab10 ; 
	} 
      } 
      else {
	  
	poplit2 = - (integer) poplit2 ; 
	if ( ( poplit1 > splength - ( poplit2 - 1 ) ) ) 
	poplit1 = splength - ( poplit2 - 1 ) ; 
	spend = strstart [ poplit3 + 1 ] - ( poplit2 - 1 ) ; 
	spptr = spend - poplit1 ; 
      } 
      while ( ( spptr < spend ) ) {
	  
	{
	  strpool [ poolptr ] = strpool [ spptr ] ; 
	  incr ( poolptr ) ; 
	} 
	incr ( spptr ) ; 
      } 
      pushlitstk ( makestring () , 1 ) ; 
    } 
  } 
  lab10: ; 
} 
void xswap ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  poplitstk ( poplit2 , poptyp2 ) ; 
  if ( ( ( poptyp1 != 1 ) || ( poplit1 < cmdstrptr ) ) ) 
  {
    pushlitstk ( poplit1 , poptyp1 ) ; 
    if ( ( ( poptyp2 == 1 ) && ( poplit2 >= cmdstrptr ) ) ) 
    {
      incr ( strptr ) ; 
      poolptr = strstart [ strptr ] ; 
    } 
    pushlitstk ( poplit2 , poptyp2 ) ; 
  } 
  else if ( ( ( poptyp2 != 1 ) || ( poplit2 < cmdstrptr ) ) ) 
  {
    {
      incr ( strptr ) ; 
      poolptr = strstart [ strptr ] ; 
    } 
    pushlitstk ( poplit1 , 1 ) ; 
    pushlitstk ( poplit2 , poptyp2 ) ; 
  } 
  else {
      
    exbuflength = 0 ; 
    addbufpool ( poplit2 ) ; 
    spptr = strstart [ poplit1 ] ; 
    spend = strstart [ poplit1 + 1 ] ; 
    while ( ( spptr < spend ) ) {
	
      {
	strpool [ poolptr ] = strpool [ spptr ] ; 
	incr ( poolptr ) ; 
      } 
      incr ( spptr ) ; 
    } 
    pushlitstk ( makestring () , 1 ) ; 
    addpoolbufandpush () ; 
  } 
} 
void xtextlength ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  if ( ( poptyp1 != 1 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 1 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else {
      
    numtextchars = 0 ; 
    {
      spptr = strstart [ poplit1 ] ; 
      spend = strstart [ poplit1 + 1 ] ; 
      spbracelevel = 0 ; 
      while ( ( spptr < spend ) ) {
	  
	incr ( spptr ) ; 
	if ( ( strpool [ spptr - 1 ] == 123 ) ) 
	{
	  incr ( spbracelevel ) ; 
	  if ( ( ( spbracelevel == 1 ) && ( spptr < spend ) ) ) 
	  if ( ( strpool [ spptr ] == 92 ) ) 
	  {
	    incr ( spptr ) ; 
	    while ( ( ( spptr < spend ) && ( spbracelevel > 0 ) ) ) {
		
	      if ( ( strpool [ spptr ] == 125 ) ) 
	      decr ( spbracelevel ) ; 
	      else if ( ( strpool [ spptr ] == 123 ) ) 
	      incr ( spbracelevel ) ; 
	      incr ( spptr ) ; 
	    } 
	    incr ( numtextchars ) ; 
	  } 
	} 
	else if ( ( strpool [ spptr - 1 ] == 125 ) ) 
	{
	  if ( ( spbracelevel > 0 ) ) 
	  decr ( spbracelevel ) ; 
	} 
	else incr ( numtextchars ) ; 
      } 
    } 
    pushlitstk ( numtextchars , 0 ) ; 
  } 
} 
void xtextprefix ( ) 
{/* 10 */ poplitstk ( poplit1 , poptyp1 ) ; 
  poplitstk ( poplit2 , poptyp2 ) ; 
  if ( ( poptyp1 != 0 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 0 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else if ( ( poptyp2 != 1 ) ) 
  {
    printwrongstklit ( poplit2 , poptyp2 , 1 ) ; 
    pushlitstk ( snull , 1 ) ; 
  } 
  else if ( ( poplit1 <= 0 ) ) 
  {
    pushlitstk ( snull , 1 ) ; 
    goto lab10 ; 
  } 
  else {
      
    spptr = strstart [ poplit2 ] ; 
    spend = strstart [ poplit2 + 1 ] ; 
    {
      numtextchars = 0 ; 
      spbracelevel = 0 ; 
      spxptr1 = spptr ; 
      while ( ( ( spxptr1 < spend ) && ( numtextchars < poplit1 ) ) ) {
	  
	incr ( spxptr1 ) ; 
	if ( ( strpool [ spxptr1 - 1 ] == 123 ) ) 
	{
	  incr ( spbracelevel ) ; 
	  if ( ( ( spbracelevel == 1 ) && ( spxptr1 < spend ) ) ) 
	  if ( ( strpool [ spxptr1 ] == 92 ) ) 
	  {
	    incr ( spxptr1 ) ; 
	    while ( ( ( spxptr1 < spend ) && ( spbracelevel > 0 ) ) ) {
		
	      if ( ( strpool [ spxptr1 ] == 125 ) ) 
	      decr ( spbracelevel ) ; 
	      else if ( ( strpool [ spxptr1 ] == 123 ) ) 
	      incr ( spbracelevel ) ; 
	      incr ( spxptr1 ) ; 
	    } 
	    incr ( numtextchars ) ; 
	  } 
	} 
	else if ( ( strpool [ spxptr1 - 1 ] == 125 ) ) 
	{
	  if ( ( spbracelevel > 0 ) ) 
	  decr ( spbracelevel ) ; 
	} 
	else incr ( numtextchars ) ; 
      } 
      spend = spxptr1 ; 
    } 
    if ( ( poplit2 >= cmdstrptr ) ) 
    poolptr = spend ; 
    else while ( ( spptr < spend ) ) {
	
      {
	strpool [ poolptr ] = strpool [ spptr ] ; 
	incr ( poolptr ) ; 
      } 
      incr ( spptr ) ; 
    } 
    while ( ( spbracelevel > 0 ) ) {
	
      {
	strpool [ poolptr ] = 125 ; 
	incr ( poolptr ) ; 
      } 
      decr ( spbracelevel ) ; 
    } 
    pushlitstk ( makestring () , 1 ) ; 
  } 
  lab10: ; 
} 
void xtype ( ) 
{if ( ( ! messwithentries ) ) 
  bstcantmesswithentri () ; 
  else if ( ( ( typelist [ citeptr ] == 5001 ) || ( typelist [ citeptr ] == 0 
  ) ) ) 
  pushlitstk ( snull , 1 ) ; 
  else pushlitstk ( hashtext [ typelist [ citeptr ] ] , 1 ) ; 
} 
void xwarning ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  if ( ( poptyp1 != 1 ) ) 
  printwrongstklit ( poplit1 , poptyp1 , 1 ) ; 
  else {
      
    {
      (void) Fputs( logfile ,  "Warning--" ) ; 
      (void) Fputs( stdout ,  "Warning--" ) ; 
    } 
    printlit ( poplit1 , poptyp1 ) ; 
    markwarning () ; 
  } 
} 
void xwidth ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  if ( ( poptyp1 != 1 ) ) 
  {
    printwrongstklit ( poplit1 , poptyp1 , 1 ) ; 
    pushlitstk ( 0 , 0 ) ; 
  } 
  else {
      
    exbuflength = 0 ; 
    addbufpool ( poplit1 ) ; 
    stringwidth = 0 ; 
    {
      bracelevel = 0 ; 
      exbufptr = 0 ; 
      while ( ( exbufptr < exbuflength ) ) {
	  
	if ( ( exbuf [ exbufptr ] == 123 ) ) 
	{
	  incr ( bracelevel ) ; 
	  if ( ( ( bracelevel == 1 ) && ( exbufptr + 1 < exbuflength ) ) ) 
	  if ( ( exbuf [ exbufptr + 1 ] == 92 ) ) 
	  {
	    incr ( exbufptr ) ; 
	    while ( ( ( exbufptr < exbuflength ) && ( bracelevel > 0 ) ) ) {
		
	      incr ( exbufptr ) ; 
	      exbufxptr = exbufptr ; 
	      while ( ( ( exbufptr < exbuflength ) && ( lexclass [ exbuf [ 
	      exbufptr ] ] == 2 ) ) ) incr ( exbufptr ) ; 
	      if ( ( ( exbufptr < exbuflength ) && ( exbufptr == exbufxptr ) ) 
	      ) 
	      incr ( exbufptr ) ; 
	      else {
		  
		controlseqloc = strlookup ( exbuf , exbufxptr , exbufptr - 
		exbufxptr , 14 , false ) ; 
		if ( ( hashfound ) ) 
		{
		  switch ( ( ilkinfo [ controlseqloc ] ) ) 
		  {case 12 : 
		    stringwidth = stringwidth + 500 ; 
		    break ; 
		  case 4 : 
		    stringwidth = stringwidth + 722 ; 
		    break ; 
		  case 2 : 
		    stringwidth = stringwidth + 778 ; 
		    break ; 
		  case 5 : 
		    stringwidth = stringwidth + 903 ; 
		    break ; 
		  case 3 : 
		    stringwidth = stringwidth + 1014 ; 
		    break ; 
		    default: 
		    stringwidth = stringwidth + charwidth [ exbuf [ exbufxptr 
		    ] ] ; 
		    break ; 
		  } 
		} 
	      } 
	      while ( ( ( exbufptr < exbuflength ) && ( lexclass [ exbuf [ 
	      exbufptr ] ] == 1 ) ) ) incr ( exbufptr ) ; 
	      while ( ( ( exbufptr < exbuflength ) && ( bracelevel > 0 ) && ( 
	      exbuf [ exbufptr ] != 92 ) ) ) {
		  
		if ( ( exbuf [ exbufptr ] == 125 ) ) 
		decr ( bracelevel ) ; 
		else if ( ( exbuf [ exbufptr ] == 123 ) ) 
		incr ( bracelevel ) ; 
		else stringwidth = stringwidth + charwidth [ exbuf [ exbufptr 
		] ] ; 
		incr ( exbufptr ) ; 
	      } 
	    } 
	    decr ( exbufptr ) ; 
	  } 
	  else stringwidth = stringwidth + charwidth [ 123 ] ; 
	  else stringwidth = stringwidth + charwidth [ 123 ] ; 
	} 
	else if ( ( exbuf [ exbufptr ] == 125 ) ) 
	{
	  decrbracelevel ( poplit1 ) ; 
	  stringwidth = stringwidth + charwidth [ 125 ] ; 
	} 
	else stringwidth = stringwidth + charwidth [ exbuf [ exbufptr ] ] ; 
	incr ( exbufptr ) ; 
      } 
      checkbracelevel ( poplit1 ) ; 
    } 
    pushlitstk ( stringwidth , 0 ) ; 
  } 
} 
void xwrite ( ) 
{poplitstk ( poplit1 , poptyp1 ) ; 
  if ( ( poptyp1 != 1 ) ) 
  printwrongstklit ( poplit1 , poptyp1 , 1 ) ; 
  else addoutpool ( poplit1 ) ; 
} 
void zexecutefn ( exfnloc ) 
hashloc exfnloc ; 
{/* 51 */ integer rpoplt1, rpoplt2  ; 
  stktype rpoptp1, rpoptp2  ; 
  wizfnloc wizptr  ; 
	;
#ifdef TRACE
  {
    (void) Fputs( logfile ,  "execute_fn `" ) ; 
  } 
  {
    outpoolstr ( logfile , hashtext [ exfnloc ] ) ; 
  } 
  {
    (void) fprintf( logfile , "%c\n",  '\'' ) ; 
  } 
#endif /* TRACE */
  switch ( ( fntype [ exfnloc ] ) ) 
  {case 0 : 
    {
	;
#ifdef STAT
      incr ( executioncount [ ilkinfo [ exfnloc ] ] ) ; 
#endif /* STAT */
      switch ( ( ilkinfo [ exfnloc ] ) ) 
      {case 0 : 
	xequals () ; 
	break ; 
      case 1 : 
	xgreaterthan () ; 
	break ; 
      case 2 : 
	xlessthan () ; 
	break ; 
      case 3 : 
	xplus () ; 
	break ; 
      case 4 : 
	xminus () ; 
	break ; 
      case 5 : 
	xconcatenate () ; 
	break ; 
      case 6 : 
	xgets () ; 
	break ; 
      case 7 : 
	xaddperiod () ; 
	break ; 
      case 8 : 
	{
	  if ( ( ! messwithentries ) ) 
	  bstcantmesswithentri () ; 
	  else if ( ( typelist [ citeptr ] == 5001 ) ) 
	  executefn ( bdefault ) ; 
	  else if ( ( typelist [ citeptr ] == 0 ) ) 
	  ; 
	  else executefn ( typelist [ citeptr ] ) ; 
	} 
	break ; 
      case 9 : 
	xchangecase () ; 
	break ; 
      case 10 : 
	xchrtoint () ; 
	break ; 
      case 11 : 
	xcite () ; 
	break ; 
      case 12 : 
	xduplicate () ; 
	break ; 
      case 13 : 
	xempty () ; 
	break ; 
      case 14 : 
	xformatname () ; 
	break ; 
      case 15 : 
	{
	  poplitstk ( poplit1 , poptyp1 ) ; 
	  poplitstk ( poplit2 , poptyp2 ) ; 
	  poplitstk ( poplit3 , poptyp3 ) ; 
	  if ( ( poptyp1 != 2 ) ) 
	  printwrongstklit ( poplit1 , poptyp1 , 2 ) ; 
	  else if ( ( poptyp2 != 2 ) ) 
	  printwrongstklit ( poplit2 , poptyp2 , 2 ) ; 
	  else if ( ( poptyp3 != 0 ) ) 
	  printwrongstklit ( poplit3 , poptyp3 , 0 ) ; 
	  else if ( ( poplit3 > 0 ) ) 
	  executefn ( poplit2 ) ; 
	  else executefn ( poplit1 ) ; 
	} 
	break ; 
      case 16 : 
	xinttochr () ; 
	break ; 
      case 17 : 
	xinttostr () ; 
	break ; 
      case 18 : 
	xmissing () ; 
	break ; 
      case 19 : 
	{
	  outputbblline () ; 
	} 
	break ; 
      case 20 : 
	xnumnames () ; 
	break ; 
      case 21 : 
	{
	  poplitstk ( poplit1 , poptyp1 ) ; 
	} 
	break ; 
      case 22 : 
	xpreamble () ; 
	break ; 
      case 23 : 
	xpurify () ; 
	break ; 
      case 24 : 
	xquote () ; 
	break ; 
      case 25 : 
	{
	  ; 
	} 
	break ; 
      case 26 : 
	{
	  popwholestack () ; 
	} 
	break ; 
      case 27 : 
	xsubstring () ; 
	break ; 
      case 28 : 
	xswap () ; 
	break ; 
      case 29 : 
	xtextlength () ; 
	break ; 
      case 30 : 
	xtextprefix () ; 
	break ; 
      case 31 : 
	{
	  poptopandprint () ; 
	} 
	break ; 
      case 32 : 
	xtype () ; 
	break ; 
      case 33 : 
	xwarning () ; 
	break ; 
      case 34 : 
	{
	  poplitstk ( rpoplt1 , rpoptp1 ) ; 
	  poplitstk ( rpoplt2 , rpoptp2 ) ; 
	  if ( ( rpoptp1 != 2 ) ) 
	  printwrongstklit ( rpoplt1 , rpoptp1 , 2 ) ; 
	  else if ( ( rpoptp2 != 2 ) ) 
	  printwrongstklit ( rpoplt2 , rpoptp2 , 2 ) ; 
	  else while ( true ) {
	      
	    executefn ( rpoplt2 ) ; 
	    poplitstk ( poplit1 , poptyp1 ) ; 
	    if ( ( poptyp1 != 0 ) ) 
	    {
	      printwrongstklit ( poplit1 , poptyp1 , 0 ) ; 
	      goto lab51 ; 
	    } 
	    else if ( ( poplit1 > 0 ) ) 
	    executefn ( rpoplt1 ) ; 
	    else goto lab51 ; 
	  } 
	  lab51: ; 
	} 
	break ; 
      case 35 : 
	xwidth () ; 
	break ; 
      case 36 : 
	xwrite () ; 
	break ; 
	default: 
	{
	  {
	    (void) Fputs( logfile ,  "Unknown built-in function" ) ; 
	    (void) Fputs( stdout ,  "Unknown built-in function" ) ; 
	  } 
	  printconfusion () ; 
	  longjmp(jmp9998,1) ; 
	} 
	break ; 
      } 
    } 
    break ; 
  case 1 : 
    {
      wizptr = ilkinfo [ exfnloc ] ; 
      while ( ( wizfunctions [ wizptr ] != 5001 ) ) {
	  
	if ( ( wizfunctions [ wizptr ] != 0 ) ) 
	executefn ( wizfunctions [ wizptr ] ) ; 
	else {
	    
	  incr ( wizptr ) ; 
	  pushlitstk ( wizfunctions [ wizptr ] , 2 ) ; 
	} 
	incr ( wizptr ) ; 
      } 
    } 
    break ; 
  case 2 : 
    pushlitstk ( ilkinfo [ exfnloc ] , 0 ) ; 
    break ; 
  case 3 : 
    pushlitstk ( hashtext [ exfnloc ] , 1 ) ; 
    break ; 
  case 4 : 
    {
      if ( ( ! messwithentries ) ) 
      bstcantmesswithentri () ; 
      else {
	  
	fieldptr = citeptr * numfields + ilkinfo [ exfnloc ] ; 
	if ( ( fieldinfo [ fieldptr ] == 0 ) ) 
	pushlitstk ( hashtext [ exfnloc ] , 3 ) ; 
	else pushlitstk ( fieldinfo [ fieldptr ] , 1 ) ; 
      } 
    } 
    break ; 
  case 5 : 
    {
      if ( ( ! messwithentries ) ) 
      bstcantmesswithentri () ; 
      else pushlitstk ( entryints [ citeptr * numentints + ilkinfo [ exfnloc ] 
      ] , 0 ) ; 
    } 
    break ; 
  case 6 : 
    {
      if ( ( ! messwithentries ) ) 
      bstcantmesswithentri () ; 
      else {
	  
	strentptr = citeptr * numentstrs + ilkinfo [ exfnloc ] ; 
	exbufptr = 0 ; 
	while ( ( entrystrs [ strentptr ] [ exbufptr ] != 127 ) ) {
	    
	  exbuf [ exbufptr ] = entrystrs [ strentptr ] [ exbufptr ] ; 
	  incr ( exbufptr ) ; 
	} 
	exbuflength = exbufptr ; 
	addpoolbufandpush () ; 
      } 
    } 
    break ; 
  case 7 : 
    pushlitstk ( ilkinfo [ exfnloc ] , 0 ) ; 
    break ; 
  case 8 : 
    {
      strglbptr = ilkinfo [ exfnloc ] ; 
      if ( ( glbstrptr [ strglbptr ] > 0 ) ) 
      pushlitstk ( glbstrptr [ strglbptr ] , 1 ) ; 
      else {
	  
	{
	  if ( ( poolptr + glbstrend [ strglbptr ] > poolsize ) ) 
	  pooloverflow () ; 
	} 
	globchrptr = 0 ; 
	while ( ( globchrptr < glbstrend [ strglbptr ] ) ) {
	    
	  {
	    strpool [ poolptr ] = globalstrs [ strglbptr ] [ globchrptr ] ; 
	    incr ( poolptr ) ; 
	  } 
	  incr ( globchrptr ) ; 
	} 
	pushlitstk ( makestring () , 1 ) ; 
      } 
    } 
    break ; 
    default: 
    unknwnfunctionclassc () ; 
    break ; 
  } 
} 
void getthetoplevelauxfil ( ) 
{/* 41 46 */ while ( true ) {
    
    if ( ( gargc > 1 ) ) 
    auxnamelength = getcmdline ( nameoffile , 1024 ) ; 
    else {
	
      (void) Fputs( stdout ,  "Please type input file name (no extension)--" ) ; 
      auxnamelength = 0 ; 
      while ( ( ! eoln ( stdin ) ) ) {
	  
	if ( ( auxnamelength == 1024 ) ) 
	{
	  readln ( stdin ) ; 
	  {
	    samtoolongfilenamepr () ; 
	    goto lab46 ; 
	  } 
	} 
	nameoffile [ auxnamelength ] = getc ( stdin ) ; 
	incr ( auxnamelength ) ; 
      } 
      if ( ( eof ( stdin ) ) ) 
      {
	(void) putc('\n',  stdout );
	(void) fprintf( stdout , "%s\n",  "Unexpected end of file on terminal---giving up!" ) 
	; 
	uexit ( 1 ) ; 
      } 
      readln ( stdin ) ; 
    } 
    {
      if ( ( ( auxnamelength + ( strstart [ sauxextension + 1 ] - strstart [ 
      sauxextension ] ) > 1024 ) || ( auxnamelength + ( strstart [ 
      slogextension + 1 ] - strstart [ slogextension ] ) > 1024 ) || ( 
      auxnamelength + ( strstart [ sbblextension + 1 ] - strstart [ 
      sbblextension ] ) > 1024 ) ) ) 
      {
	samtoolongfilenamepr () ; 
	goto lab46 ; 
      } 
      {
	namelength = auxnamelength ; 
	addextension ( sauxextension ) ; 
	auxptr = 0 ; 
	if ( ( ! aopenin ( auxfile [ auxptr ] , 0 ) ) ) 
	{
	  samwrongfilenameprin () ; 
	  goto lab46 ; 
	} 
	namelength = auxnamelength ; 
	addextension ( slogextension ) ; 
	if ( ( ! aopenout ( logfile ) ) ) 
	{
	  samwrongfilenameprin () ; 
	  goto lab46 ; 
	} 
	namelength = auxnamelength ; 
	addextension ( sbblextension ) ; 
	if ( ( ! aopenout ( bblfile ) ) ) 
	{
	  samwrongfilenameprin () ; 
	  goto lab46 ; 
	} 
      } 
      {
	namelength = auxnamelength ; 
	addextension ( sauxextension ) ; 
	nameptr = 1 ; 
	while ( ( nameptr <= namelength ) ) {
	    
	  buffer [ nameptr ] = xord [ nameoffile [ nameptr - 1 ] ] ; 
	  incr ( nameptr ) ; 
	} 
	toplevstr = hashtext [ strlookup ( buffer , 1 , auxnamelength , 0 , 
	true ) ] ; 
	auxlist [ auxptr ] = hashtext [ strlookup ( buffer , 1 , namelength , 
	3 , true ) ] ; 
	if ( ( hashfound ) ) 
	{
	;
#ifdef TRACE
	  printauxname () ; 
#endif /* TRACE */
	  {
	    {
	      (void) Fputs( logfile ,  "Already encountered auxiliary file" ) ; 
	      (void) Fputs( stdout ,  "Already encountered auxiliary file" ) ; 
	    } 
	    printconfusion () ; 
	    longjmp(jmp9998,1) ; 
	  } 
	} 
	auxlnstack [ auxptr ] = 0 ; 
      } 
      goto lab41 ; 
    } 
    lab46: gargc = 0 ; 
  } 
  lab41: ; 
} 
void auxbibdatacommand ( ) 
{/* 10 */ if ( ( bibseen ) ) 
  {
    auxerrillegalanother ( 0 ) ; 
    {
      auxerrprint () ; 
      goto lab10 ; 
    } 
  } 
  bibseen = true ; 
  while ( ( buffer [ bufptr2 ] != 125 ) ) {
      
    incr ( bufptr2 ) ; 
    if ( ( ! scan2white ( 125 , 44 ) ) ) 
    {
      auxerrnorightbracepr () ; 
      {
	auxerrprint () ; 
	goto lab10 ; 
      } 
    } 
    if ( ( lexclass [ buffer [ bufptr2 ] ] == 1 ) ) 
    {
      auxerrwhitespaceinar () ; 
      {
	auxerrprint () ; 
	goto lab10 ; 
      } 
    } 
    if ( ( ( last > bufptr2 + 1 ) && ( buffer [ bufptr2 ] == 125 ) ) ) 
    {
      auxerrstuffafterrigh () ; 
      {
	auxerrprint () ; 
	goto lab10 ; 
      } 
    } 
    {
      if ( ( bibptr == maxbibfiles ) ) 
      {
	printoverflow () ; 
	{
	  (void) fprintf( logfile , "%s%ld\n",  "number of database files " , (long)maxbibfiles ) ; 
	  (void) fprintf( stdout , "%s%ld\n",  "number of database files " , (long)maxbibfiles ) ; 
	} 
	longjmp(jmp9998,1) ; 
      } 
      biblist [ bibptr ] = hashtext [ strlookup ( buffer , bufptr1 , ( bufptr2 
      - bufptr1 ) , 6 , true ) ] ; 
      if ( ( hashfound ) ) 
      {
	{
	  (void) Fputs( logfile ,  "This database file appears more than once: " ) ; 
	  (void) Fputs( stdout ,  "This database file appears more than once: " ) ; 
	} 
	printbibname () ; 
	{
	  auxerrprint () ; 
	  goto lab10 ; 
	} 
      } 
      startname ( biblist [ bibptr ] ) ; 
      addextension ( sbibextension ) ; 
      if ( ( ! aopenin ( bibfile [ bibptr ] , 2 ) ) ) 
      {
	{
	  (void) Fputs( logfile ,  "I couldn't open database file " ) ; 
	  (void) Fputs( stdout ,  "I couldn't open database file " ) ; 
	} 
	printbibname () ; 
	{
	  auxerrprint () ; 
	  goto lab10 ; 
	} 
      } 
	;
#ifdef TRACE
      {
	outpoolstr ( logfile , biblist [ bibptr ] ) ; 
      } 
      {
	outpoolstr ( logfile , sbibextension ) ; 
      } 
      {
	(void) fprintf( logfile , "%s\n",  " is a bibdata file" ) ; 
      } 
#endif /* TRACE */
      incr ( bibptr ) ; 
    } 
  } 
  lab10: ; 
} 
void auxbibstylecommand ( ) 
{/* 10 */ if ( ( bstseen ) ) 
  {
    auxerrillegalanother ( 1 ) ; 
    {
      auxerrprint () ; 
      goto lab10 ; 
    } 
  } 
  bstseen = true ; 
  incr ( bufptr2 ) ; 
  if ( ( ! scan1white ( 125 ) ) ) 
  {
    auxerrnorightbracepr () ; 
    {
      auxerrprint () ; 
      goto lab10 ; 
    } 
  } 
  if ( ( lexclass [ buffer [ bufptr2 ] ] == 1 ) ) 
  {
    auxerrwhitespaceinar () ; 
    {
      auxerrprint () ; 
      goto lab10 ; 
    } 
  } 
  if ( ( last > bufptr2 + 1 ) ) 
  {
    auxerrstuffafterrigh () ; 
    {
      auxerrprint () ; 
      goto lab10 ; 
    } 
  } 
  {
    bststr = hashtext [ strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 
    5 , true ) ] ; 
    if ( ( hashfound ) ) 
    {
	;
#ifdef TRACE
      printbstname () ; 
#endif /* TRACE */
      {
	{
	  (void) Fputs( logfile ,  "Already encountered style file" ) ; 
	  (void) Fputs( stdout ,  "Already encountered style file" ) ; 
	} 
	printconfusion () ; 
	longjmp(jmp9998,1) ; 
      } 
    } 
    startname ( bststr ) ; 
    addextension ( sbstextension ) ; 
    if ( ( ! aopenin ( bstfile , 1 ) ) ) 
    {
      {
	(void) Fputs( logfile ,  "I couldn't open style file " ) ; 
	(void) Fputs( stdout ,  "I couldn't open style file " ) ; 
      } 
      printbstname () ; 
      bststr = 0 ; 
      {
	auxerrprint () ; 
	goto lab10 ; 
      } 
    } 
    {
      (void) Fputs( logfile ,  "The style file: " ) ; 
      (void) Fputs( stdout ,  "The style file: " ) ; 
    } 
    printbstname () ; 
  } 
  lab10: ; 
} 
void auxcitationcommand ( ) 
{/* 23 10 */ citationseen = true ; 
  while ( ( buffer [ bufptr2 ] != 125 ) ) {
      
    incr ( bufptr2 ) ; 
    if ( ( ! scan2white ( 125 , 44 ) ) ) 
    {
      auxerrnorightbracepr () ; 
      {
	auxerrprint () ; 
	goto lab10 ; 
      } 
    } 
    if ( ( lexclass [ buffer [ bufptr2 ] ] == 1 ) ) 
    {
      auxerrwhitespaceinar () ; 
      {
	auxerrprint () ; 
	goto lab10 ; 
      } 
    } 
    if ( ( ( last > bufptr2 + 1 ) && ( buffer [ bufptr2 ] == 125 ) ) ) 
    {
      auxerrstuffafterrigh () ; 
      {
	auxerrprint () ; 
	goto lab10 ; 
      } 
    } 
    {
	;
#ifdef TRACE
      {
	outtoken ( logfile ) ; 
      } 
      {
	(void) Fputs( logfile ,  " cite key encountered" ) ; 
      } 
#endif /* TRACE */
      {
	if ( ( ( bufptr2 - bufptr1 ) == 1 ) ) 
	if ( ( buffer [ bufptr1 ] == 42 ) ) 
	{
	;
#ifdef TRACE
	  {
	    (void) fprintf( logfile , "%s\n",  "---entire database to be included" ) ; 
	  } 
#endif /* TRACE */
	  if ( ( allentries ) ) 
	  {
	    {
	      (void) fprintf( logfile , "%s\n",  "Multiple inclusions of entire database" ) ; 
	      (void) fprintf( stdout , "%s\n",  "Multiple inclusions of entire database" ) ; 
	    } 
	    {
	      auxerrprint () ; 
	      goto lab10 ; 
	    } 
	  } 
	  else {
	      
	    allentries = true ; 
	    allmarker = citeptr ; 
	    goto lab23 ; 
	  } 
	} 
      } 
      tmpptr = bufptr1 ; 
      while ( ( tmpptr < bufptr2 ) ) {
	  
	exbuf [ tmpptr ] = buffer [ tmpptr ] ; 
	incr ( tmpptr ) ; 
      } 
      lowercase ( exbuf , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
      lcciteloc = strlookup ( exbuf , bufptr1 , ( bufptr2 - bufptr1 ) , 10 , 
      true ) ; 
      if ( ( hashfound ) ) 
      {
	;
#ifdef TRACE
	{
	  (void) fprintf( logfile , "%s\n",  " previously" ) ; 
	} 
#endif /* TRACE */
	dummyloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 9 , 
	false ) ; 
	if ( ( ! hashfound ) ) 
	{
	  {
	    (void) Fputs( logfile ,  "Case mismatch error between cite keys " ) ; 
	    (void) Fputs( stdout ,  "Case mismatch error between cite keys " ) ; 
	  } 
	  printatoken () ; 
	  {
	    (void) Fputs( logfile ,  " and " ) ; 
	    (void) Fputs( stdout ,  " and " ) ; 
	  } 
	  printapoolstr ( citelist [ ilkinfo [ ilkinfo [ lcciteloc ] ] ] ) ; 
	  printanewline () ; 
	  {
	    auxerrprint () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
      else {
	  
	;
#ifdef TRACE
	{
	  (void) putc('\n',  logfile );
	} 
#endif /* TRACE */
	citeloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 9 , 
	true ) ; 
	if ( ( hashfound ) ) 
	hashciteconfusion () ; 
	checkciteoverflow ( citeptr ) ; 
	citelist [ citeptr ] = hashtext [ citeloc ] ; 
	ilkinfo [ citeloc ] = citeptr ; 
	ilkinfo [ lcciteloc ] = citeloc ; 
	incr ( citeptr ) ; 
      } 
    } 
    lab23: ; 
  } 
  lab10: ; 
} 
void auxinputcommand ( ) 
{/* 10 */ boolean auxextensionok  ; 
  incr ( bufptr2 ) ; 
  if ( ( ! scan1white ( 125 ) ) ) 
  {
    auxerrnorightbracepr () ; 
    {
      auxerrprint () ; 
      goto lab10 ; 
    } 
  } 
  if ( ( lexclass [ buffer [ bufptr2 ] ] == 1 ) ) 
  {
    auxerrwhitespaceinar () ; 
    {
      auxerrprint () ; 
      goto lab10 ; 
    } 
  } 
  if ( ( last > bufptr2 + 1 ) ) 
  {
    auxerrstuffafterrigh () ; 
    {
      auxerrprint () ; 
      goto lab10 ; 
    } 
  } 
  {
    incr ( auxptr ) ; 
    if ( ( auxptr == auxstacksize ) ) 
    {
      printatoken () ; 
      {
	(void) Fputs( logfile ,  ": " ) ; 
	(void) Fputs( stdout ,  ": " ) ; 
      } 
      {
	printoverflow () ; 
	{
	  (void) fprintf( logfile , "%s%ld\n",  "auxiliary file depth " , (long)auxstacksize ) ; 
	  (void) fprintf( stdout , "%s%ld\n",  "auxiliary file depth " , (long)auxstacksize ) ; 
	} 
	longjmp(jmp9998,1) ; 
      } 
    } 
    auxextensionok = true ; 
    if ( ( ( bufptr2 - bufptr1 ) < ( strstart [ sauxextension + 1 ] - strstart 
    [ sauxextension ] ) ) ) 
    auxextensionok = false ; 
    else if ( ( ! streqbuf ( sauxextension , buffer , bufptr2 - ( strstart [ 
    sauxextension + 1 ] - strstart [ sauxextension ] ) , ( strstart [ 
    sauxextension + 1 ] - strstart [ sauxextension ] ) ) ) ) 
    auxextensionok = false ; 
    if ( ( ! auxextensionok ) ) 
    {
      printatoken () ; 
      {
	(void) Fputs( logfile ,  " has a wrong extension" ) ; 
	(void) Fputs( stdout ,  " has a wrong extension" ) ; 
      } 
      decr ( auxptr ) ; 
      {
	auxerrprint () ; 
	goto lab10 ; 
      } 
    } 
    auxlist [ auxptr ] = hashtext [ strlookup ( buffer , bufptr1 , ( bufptr2 - 
    bufptr1 ) , 3 , true ) ] ; 
    if ( ( hashfound ) ) 
    {
      {
	(void) Fputs( logfile ,  "Already encountered file " ) ; 
	(void) Fputs( stdout ,  "Already encountered file " ) ; 
      } 
      printauxname () ; 
      decr ( auxptr ) ; 
      {
	auxerrprint () ; 
	goto lab10 ; 
      } 
    } 
    {
      startname ( auxlist [ auxptr ] ) ; 
      nameptr = namelength ; 
      while ( ( nameptr <= 1024 ) ) {
	  
	nameoffile [ nameptr ] = ' ' ; 
	incr ( nameptr ) ; 
      } 
      if ( ( ! aopenin ( auxfile [ auxptr ] , 0 ) ) ) 
      {
	{
	  (void) Fputs( logfile ,  "I couldn't open auxiliary file " ) ; 
	  (void) Fputs( stdout ,  "I couldn't open auxiliary file " ) ; 
	} 
	printauxname () ; 
	decr ( auxptr ) ; 
	{
	  auxerrprint () ; 
	  goto lab10 ; 
	} 
      } 
      {
	(void) fprintf( logfile , "%s%ld%s",  "A level-" , (long)auxptr , " auxiliary file: " ) ; 
	(void) fprintf( stdout , "%s%ld%s",  "A level-" , (long)auxptr , " auxiliary file: " ) ; 
      } 
      printauxname () ; 
      auxlnstack [ auxptr ] = 0 ; 
    } 
  } 
  lab10: ; 
} 
void poptheauxstack ( ) 
{aclose ( auxfile [ auxptr ] ) ; 
  if ( ( auxptr == 0 ) ) 
  {lab31=1; return;}
  else decr ( auxptr ) ; 
} 
void getauxcommandandproc ( ) 
{/* 10 */ bufptr2 = 0 ; 
  if ( ( ! scan1 ( 123 ) ) ) 
  goto lab10 ; 
  commandnum = ilkinfo [ strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) 
  , 2 , false ) ] ; 
  if ( ( hashfound ) ) 
  switch ( ( commandnum ) ) 
  {case 0 : 
    auxbibdatacommand () ; 
    break ; 
  case 1 : 
    auxbibstylecommand () ; 
    break ; 
  case 2 : 
    auxcitationcommand () ; 
    break ; 
  case 3 : 
    auxinputcommand () ; 
    break ; 
    default: 
    {
      {
	(void) Fputs( logfile ,  "Unknown auxiliary-file command" ) ; 
	(void) Fputs( stdout ,  "Unknown auxiliary-file command" ) ; 
      } 
      printconfusion () ; 
      longjmp(jmp9998,1) ; 
    } 
    break ; 
  } 
  lab10: ; 
} 
void lastcheckforauxerror ( ) 
{numcites = citeptr ; 
  numbibfiles = bibptr ; 
  if ( ( ! citationseen ) ) 
  {
    auxend1errprint () ; 
    {
      (void) Fputs( logfile ,  "\\citation commands" ) ; 
      (void) Fputs( stdout ,  "\\citation commands" ) ; 
    } 
    auxend2errprint () ; 
  } 
  else if ( ( ( numcites == 0 ) && ( ! allentries ) ) ) 
  {
    auxend1errprint () ; 
    {
      (void) Fputs( logfile ,  "cite keys" ) ; 
      (void) Fputs( stdout ,  "cite keys" ) ; 
    } 
    auxend2errprint () ; 
  } 
  if ( ( ! bibseen ) ) 
  {
    auxend1errprint () ; 
    {
      (void) Fputs( logfile ,  "\\bibdata command" ) ; 
      (void) Fputs( stdout ,  "\\bibdata command" ) ; 
    } 
    auxend2errprint () ; 
  } 
  else if ( ( numbibfiles == 0 ) ) 
  {
    auxend1errprint () ; 
    {
      (void) Fputs( logfile ,  "database files" ) ; 
      (void) Fputs( stdout ,  "database files" ) ; 
    } 
    auxend2errprint () ; 
  } 
  if ( ( ! bstseen ) ) 
  {
    auxend1errprint () ; 
    {
      (void) Fputs( logfile ,  "\\bibstyle command" ) ; 
      (void) Fputs( stdout ,  "\\bibstyle command" ) ; 
    } 
    auxend2errprint () ; 
  } 
  else if ( ( bststr == 0 ) ) 
  {
    auxend1errprint () ; 
    {
      (void) Fputs( logfile ,  "style file" ) ; 
      (void) Fputs( stdout ,  "style file" ) ; 
    } 
    auxend2errprint () ; 
  } 
} 
void bstentrycommand ( ) 
{/* 10 */ if ( ( entryseen ) ) 
  {
    {
      (void) Fputs( logfile ,  "Illegal, another entry command" ) ; 
      (void) Fputs( stdout ,  "Illegal, another entry command" ) ; 
    } 
    {
      bsterrprintandlookfo () ; 
      goto lab10 ; 
    } 
  } 
  entryseen = true ; 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "entry" ) ; 
	  (void) Fputs( stdout ,  "entry" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    {
      if ( ( buffer [ bufptr2 ] != 123 ) ) 
      {
	bstleftbraceprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "entry" ) ; 
	    (void) Fputs( stdout ,  "entry" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
      incr ( bufptr2 ) ; 
    } 
    {
      if ( ( ! eatbstwhitespace () ) ) 
      {
	eatbstprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "entry" ) ; 
	    (void) Fputs( stdout ,  "entry" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
    while ( ( buffer [ bufptr2 ] != 125 ) ) {
	
      {
	scanidentifier ( 125 , 37 , 37 ) ; 
	if ( ( ( scanresult == 3 ) || ( scanresult == 1 ) ) ) 
	; 
	else {
	    
	  bstidprint () ; 
	  {
	    {
	      (void) Fputs( logfile ,  "entry" ) ; 
	      (void) Fputs( stdout ,  "entry" ) ; 
	    } 
	    {
	      bsterrprintandlookfo () ; 
	      goto lab10 ; 
	    } 
	  } 
	} 
      } 
      {
	;
#ifdef TRACE
	{
	  outtoken ( logfile ) ; 
	} 
	{
	  (void) fprintf( logfile , "%s\n",  " is a field" ) ; 
	} 
#endif /* TRACE */
	lowercase ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
	fnloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 11 , 
	true ) ; 
	{
	  if ( ( hashfound ) ) 
	  {
	    alreadyseenfunctionp ( fnloc ) ; 
	    goto lab10 ; 
	  } 
	} 
	fntype [ fnloc ] = 4 ; 
	ilkinfo [ fnloc ] = numfields ; 
	incr ( numfields ) ; 
      } 
      {
	if ( ( ! eatbstwhitespace () ) ) 
	{
	  eatbstprint () ; 
	  {
	    {
	      (void) Fputs( logfile ,  "entry" ) ; 
	      (void) Fputs( stdout ,  "entry" ) ; 
	    } 
	    {
	      bsterrprintandlookfo () ; 
	      goto lab10 ; 
	    } 
	  } 
	} 
      } 
    } 
    incr ( bufptr2 ) ; 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "entry" ) ; 
	  (void) Fputs( stdout ,  "entry" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  if ( ( numfields == numpredefinedfields ) ) 
  {
    {
      (void) Fputs( logfile ,  "Warning--I didn't find any fields" ) ; 
      (void) Fputs( stdout ,  "Warning--I didn't find any fields" ) ; 
    } 
    bstwarnprint () ; 
  } 
  {
    {
      if ( ( buffer [ bufptr2 ] != 123 ) ) 
      {
	bstleftbraceprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "entry" ) ; 
	    (void) Fputs( stdout ,  "entry" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
      incr ( bufptr2 ) ; 
    } 
    {
      if ( ( ! eatbstwhitespace () ) ) 
      {
	eatbstprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "entry" ) ; 
	    (void) Fputs( stdout ,  "entry" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
    while ( ( buffer [ bufptr2 ] != 125 ) ) {
	
      {
	scanidentifier ( 125 , 37 , 37 ) ; 
	if ( ( ( scanresult == 3 ) || ( scanresult == 1 ) ) ) 
	; 
	else {
	    
	  bstidprint () ; 
	  {
	    {
	      (void) Fputs( logfile ,  "entry" ) ; 
	      (void) Fputs( stdout ,  "entry" ) ; 
	    } 
	    {
	      bsterrprintandlookfo () ; 
	      goto lab10 ; 
	    } 
	  } 
	} 
      } 
      {
	;
#ifdef TRACE
	{
	  outtoken ( logfile ) ; 
	} 
	{
	  (void) fprintf( logfile , "%s\n",  " is an integer entry-variable" ) ; 
	} 
#endif /* TRACE */
	lowercase ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
	fnloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 11 , 
	true ) ; 
	{
	  if ( ( hashfound ) ) 
	  {
	    alreadyseenfunctionp ( fnloc ) ; 
	    goto lab10 ; 
	  } 
	} 
	fntype [ fnloc ] = 5 ; 
	ilkinfo [ fnloc ] = numentints ; 
	incr ( numentints ) ; 
      } 
      {
	if ( ( ! eatbstwhitespace () ) ) 
	{
	  eatbstprint () ; 
	  {
	    {
	      (void) Fputs( logfile ,  "entry" ) ; 
	      (void) Fputs( stdout ,  "entry" ) ; 
	    } 
	    {
	      bsterrprintandlookfo () ; 
	      goto lab10 ; 
	    } 
	  } 
	} 
      } 
    } 
    incr ( bufptr2 ) ; 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "entry" ) ; 
	  (void) Fputs( stdout ,  "entry" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    {
      if ( ( buffer [ bufptr2 ] != 123 ) ) 
      {
	bstleftbraceprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "entry" ) ; 
	    (void) Fputs( stdout ,  "entry" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
      incr ( bufptr2 ) ; 
    } 
    {
      if ( ( ! eatbstwhitespace () ) ) 
      {
	eatbstprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "entry" ) ; 
	    (void) Fputs( stdout ,  "entry" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
    while ( ( buffer [ bufptr2 ] != 125 ) ) {
	
      {
	scanidentifier ( 125 , 37 , 37 ) ; 
	if ( ( ( scanresult == 3 ) || ( scanresult == 1 ) ) ) 
	; 
	else {
	    
	  bstidprint () ; 
	  {
	    {
	      (void) Fputs( logfile ,  "entry" ) ; 
	      (void) Fputs( stdout ,  "entry" ) ; 
	    } 
	    {
	      bsterrprintandlookfo () ; 
	      goto lab10 ; 
	    } 
	  } 
	} 
      } 
      {
	;
#ifdef TRACE
	{
	  outtoken ( logfile ) ; 
	} 
	{
	  (void) fprintf( logfile , "%s\n",  " is a string entry-variable" ) ; 
	} 
#endif /* TRACE */
	lowercase ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
	fnloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 11 , 
	true ) ; 
	{
	  if ( ( hashfound ) ) 
	  {
	    alreadyseenfunctionp ( fnloc ) ; 
	    goto lab10 ; 
	  } 
	} 
	fntype [ fnloc ] = 6 ; 
	ilkinfo [ fnloc ] = numentstrs ; 
	incr ( numentstrs ) ; 
      } 
      {
	if ( ( ! eatbstwhitespace () ) ) 
	{
	  eatbstprint () ; 
	  {
	    {
	      (void) Fputs( logfile ,  "entry" ) ; 
	      (void) Fputs( stdout ,  "entry" ) ; 
	    } 
	    {
	      bsterrprintandlookfo () ; 
	      goto lab10 ; 
	    } 
	  } 
	} 
      } 
    } 
    incr ( bufptr2 ) ; 
  } 
  lab10: ; 
} 
boolean badargumenttoken ( ) 
{/* 10 */ register boolean Result; Result = true ; 
  lowercase ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
  fnloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 11 , false ) 
  ; 
  if ( ( ! hashfound ) ) 
  {
    printatoken () ; 
    {
      {
	(void) Fputs( logfile ,  " is an unknown function" ) ; 
	(void) Fputs( stdout ,  " is an unknown function" ) ; 
      } 
      {
	bsterrprintandlookfo () ; 
	goto lab10 ; 
      } 
    } 
  } 
  else if ( ( ( fntype [ fnloc ] != 0 ) && ( fntype [ fnloc ] != 1 ) ) ) 
  {
    printatoken () ; 
    {
      (void) Fputs( logfile ,  " has bad function type " ) ; 
      (void) Fputs( stdout ,  " has bad function type " ) ; 
    } 
    printfnclass ( fnloc ) ; 
    {
      bsterrprintandlookfo () ; 
      goto lab10 ; 
    } 
  } 
  Result = false ; 
  lab10: ; 
  return(Result) ; 
} 
void bstexecutecommand ( ) 
{/* 10 */ if ( ( ! readseen ) ) 
  {
    {
      (void) Fputs( logfile ,  "Illegal, execute command before read command" ) ; 
      (void) Fputs( stdout ,  "Illegal, execute command before read command" ) ; 
    } 
    {
      bsterrprintandlookfo () ; 
      goto lab10 ; 
    } 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "execute" ) ; 
	  (void) Fputs( stdout ,  "execute" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    if ( ( buffer [ bufptr2 ] != 123 ) ) 
    {
      bstleftbraceprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "execute" ) ; 
	  (void) Fputs( stdout ,  "execute" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
    incr ( bufptr2 ) ; 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "execute" ) ; 
	  (void) Fputs( stdout ,  "execute" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    scanidentifier ( 125 , 37 , 37 ) ; 
    if ( ( ( scanresult == 3 ) || ( scanresult == 1 ) ) ) 
    ; 
    else {
	
      bstidprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "execute" ) ; 
	  (void) Fputs( stdout ,  "execute" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
	;
#ifdef TRACE
    {
      outtoken ( logfile ) ; 
    } 
    {
      (void) fprintf( logfile , "%s\n",  " is a to be executed function" ) ; 
    } 
#endif /* TRACE */
    if ( ( badargumenttoken () ) ) 
    goto lab10 ; 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "execute" ) ; 
	  (void) Fputs( stdout ,  "execute" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    if ( ( buffer [ bufptr2 ] != 125 ) ) 
    {
      bstrightbraceprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "execute" ) ; 
	  (void) Fputs( stdout ,  "execute" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
    incr ( bufptr2 ) ; 
  } 
  {
    initcommandexecution () ; 
    messwithentries = false ; 
    executefn ( fnloc ) ; 
    checkcommandexecutio () ; 
  } 
  lab10: ; 
} 
void bstfunctioncommand ( ) 
{/* 10 */ {
    
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "function" ) ; 
	  (void) Fputs( stdout ,  "function" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    {
      if ( ( buffer [ bufptr2 ] != 123 ) ) 
      {
	bstleftbraceprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "function" ) ; 
	    (void) Fputs( stdout ,  "function" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
      incr ( bufptr2 ) ; 
    } 
    {
      if ( ( ! eatbstwhitespace () ) ) 
      {
	eatbstprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "function" ) ; 
	    (void) Fputs( stdout ,  "function" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
    {
      scanidentifier ( 125 , 37 , 37 ) ; 
      if ( ( ( scanresult == 3 ) || ( scanresult == 1 ) ) ) 
      ; 
      else {
	  
	bstidprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "function" ) ; 
	    (void) Fputs( stdout ,  "function" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
    {
	;
#ifdef TRACE
      {
	outtoken ( logfile ) ; 
      } 
      {
	(void) fprintf( logfile , "%s\n",  " is a wizard-defined function" ) ; 
      } 
#endif /* TRACE */
      lowercase ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
      wizloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 11 , 
      true ) ; 
      {
	if ( ( hashfound ) ) 
	{
	  alreadyseenfunctionp ( wizloc ) ; 
	  goto lab10 ; 
	} 
      } 
      fntype [ wizloc ] = 1 ; 
      if ( ( hashtext [ wizloc ] == sdefault ) ) 
      bdefault = wizloc ; 
    } 
    {
      if ( ( ! eatbstwhitespace () ) ) 
      {
	eatbstprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "function" ) ; 
	    (void) Fputs( stdout ,  "function" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
    {
      if ( ( buffer [ bufptr2 ] != 125 ) ) 
      {
	bstrightbraceprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "function" ) ; 
	    (void) Fputs( stdout ,  "function" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
      incr ( bufptr2 ) ; 
    } 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "function" ) ; 
	  (void) Fputs( stdout ,  "function" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    if ( ( buffer [ bufptr2 ] != 123 ) ) 
    {
      bstleftbraceprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "function" ) ; 
	  (void) Fputs( stdout ,  "function" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
    incr ( bufptr2 ) ; 
  } 
  scanfndef ( wizloc ) ; 
  lab10: ; 
} 
void bstintegerscommand ( ) 
{/* 10 */ {
    
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "integers" ) ; 
	  (void) Fputs( stdout ,  "integers" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    if ( ( buffer [ bufptr2 ] != 123 ) ) 
    {
      bstleftbraceprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "integers" ) ; 
	  (void) Fputs( stdout ,  "integers" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
    incr ( bufptr2 ) ; 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "integers" ) ; 
	  (void) Fputs( stdout ,  "integers" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  while ( ( buffer [ bufptr2 ] != 125 ) ) {
      
    {
      scanidentifier ( 125 , 37 , 37 ) ; 
      if ( ( ( scanresult == 3 ) || ( scanresult == 1 ) ) ) 
      ; 
      else {
	  
	bstidprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "integers" ) ; 
	    (void) Fputs( stdout ,  "integers" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
    {
	;
#ifdef TRACE
      {
	outtoken ( logfile ) ; 
      } 
      {
	(void) fprintf( logfile , "%s\n",  " is an integer global-variable" ) ; 
      } 
#endif /* TRACE */
      lowercase ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
      fnloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 11 , true 
      ) ; 
      {
	if ( ( hashfound ) ) 
	{
	  alreadyseenfunctionp ( fnloc ) ; 
	  goto lab10 ; 
	} 
      } 
      fntype [ fnloc ] = 7 ; 
      ilkinfo [ fnloc ] = 0 ; 
    } 
    {
      if ( ( ! eatbstwhitespace () ) ) 
      {
	eatbstprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "integers" ) ; 
	    (void) Fputs( stdout ,  "integers" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
  } 
  incr ( bufptr2 ) ; 
  lab10: ; 
} 
void bstiteratecommand ( ) 
{/* 10 */ if ( ( ! readseen ) ) 
  {
    {
      (void) Fputs( logfile ,  "Illegal, iterate command before read command" ) ; 
      (void) Fputs( stdout ,  "Illegal, iterate command before read command" ) ; 
    } 
    {
      bsterrprintandlookfo () ; 
      goto lab10 ; 
    } 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "iterate" ) ; 
	  (void) Fputs( stdout ,  "iterate" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    if ( ( buffer [ bufptr2 ] != 123 ) ) 
    {
      bstleftbraceprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "iterate" ) ; 
	  (void) Fputs( stdout ,  "iterate" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
    incr ( bufptr2 ) ; 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "iterate" ) ; 
	  (void) Fputs( stdout ,  "iterate" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    scanidentifier ( 125 , 37 , 37 ) ; 
    if ( ( ( scanresult == 3 ) || ( scanresult == 1 ) ) ) 
    ; 
    else {
	
      bstidprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "iterate" ) ; 
	  (void) Fputs( stdout ,  "iterate" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
	;
#ifdef TRACE
    {
      outtoken ( logfile ) ; 
    } 
    {
      (void) fprintf( logfile , "%s\n",  " is a to be iterated function" ) ; 
    } 
#endif /* TRACE */
    if ( ( badargumenttoken () ) ) 
    goto lab10 ; 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "iterate" ) ; 
	  (void) Fputs( stdout ,  "iterate" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    if ( ( buffer [ bufptr2 ] != 125 ) ) 
    {
      bstrightbraceprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "iterate" ) ; 
	  (void) Fputs( stdout ,  "iterate" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
    incr ( bufptr2 ) ; 
  } 
  {
    initcommandexecution () ; 
    messwithentries = true ; 
    sortciteptr = 0 ; 
    while ( ( sortciteptr < numcites ) ) {
	
      citeptr = citeinfo [ sortciteptr ] ; 
	;
#ifdef TRACE
      {
	outpoolstr ( logfile , hashtext [ fnloc ] ) ; 
      } 
      {
	(void) Fputs( logfile ,  " to be iterated on " ) ; 
      } 
      {
	outpoolstr ( logfile , citelist [ citeptr ] ) ; 
      } 
      {
	(void) putc('\n',  logfile );
      } 
#endif /* TRACE */
      executefn ( fnloc ) ; 
      checkcommandexecutio () ; 
      incr ( sortciteptr ) ; 
    } 
  } 
  lab10: ; 
} 
void bstmacrocommand ( ) 
{/* 10 */ if ( ( readseen ) ) 
  {
    {
      (void) Fputs( logfile ,  "Illegal, macro command after read command" ) ; 
      (void) Fputs( stdout ,  "Illegal, macro command after read command" ) ; 
    } 
    {
      bsterrprintandlookfo () ; 
      goto lab10 ; 
    } 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "macro" ) ; 
	  (void) Fputs( stdout ,  "macro" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    {
      if ( ( buffer [ bufptr2 ] != 123 ) ) 
      {
	bstleftbraceprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "macro" ) ; 
	    (void) Fputs( stdout ,  "macro" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
      incr ( bufptr2 ) ; 
    } 
    {
      if ( ( ! eatbstwhitespace () ) ) 
      {
	eatbstprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "macro" ) ; 
	    (void) Fputs( stdout ,  "macro" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
    {
      scanidentifier ( 125 , 37 , 37 ) ; 
      if ( ( ( scanresult == 3 ) || ( scanresult == 1 ) ) ) 
      ; 
      else {
	  
	bstidprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "macro" ) ; 
	    (void) Fputs( stdout ,  "macro" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
    {
	;
#ifdef TRACE
      {
	outtoken ( logfile ) ; 
      } 
      {
	(void) fprintf( logfile , "%s\n",  " is a macro" ) ; 
      } 
#endif /* TRACE */
      lowercase ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
      macronameloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 13 
      , true ) ; 
      if ( ( hashfound ) ) 
      {
	printatoken () ; 
	{
	  {
	    (void) Fputs( logfile ,  " is already defined as a macro" ) ; 
	    (void) Fputs( stdout ,  " is already defined as a macro" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
      ilkinfo [ macronameloc ] = hashtext [ macronameloc ] ; 
    } 
    {
      if ( ( ! eatbstwhitespace () ) ) 
      {
	eatbstprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "macro" ) ; 
	    (void) Fputs( stdout ,  "macro" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
    {
      if ( ( buffer [ bufptr2 ] != 125 ) ) 
      {
	bstrightbraceprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "macro" ) ; 
	    (void) Fputs( stdout ,  "macro" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
      incr ( bufptr2 ) ; 
    } 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "macro" ) ; 
	  (void) Fputs( stdout ,  "macro" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    {
      if ( ( buffer [ bufptr2 ] != 123 ) ) 
      {
	bstleftbraceprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "macro" ) ; 
	    (void) Fputs( stdout ,  "macro" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
      incr ( bufptr2 ) ; 
    } 
    {
      if ( ( ! eatbstwhitespace () ) ) 
      {
	eatbstprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "macro" ) ; 
	    (void) Fputs( stdout ,  "macro" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
    if ( ( buffer [ bufptr2 ] != 34 ) ) 
    {
      {
	(void) fprintf( logfile , "%s%c%s",  "A macro definition must be " , xchr [ 34 ] ,         "-delimited" ) ; 
	(void) fprintf( stdout , "%s%c%s",  "A macro definition must be " , xchr [ 34 ] ,         "-delimited" ) ; 
      } 
      {
	bsterrprintandlookfo () ; 
	goto lab10 ; 
      } 
    } 
    {
      incr ( bufptr2 ) ; 
      if ( ( ! scan1 ( 34 ) ) ) 
      {
	{
	  (void) fprintf( logfile , "%s%c%s",  "There's no `" , xchr [ 34 ] ,           "' to end macro definition" ) ; 
	  (void) fprintf( stdout , "%s%c%s",  "There's no `" , xchr [ 34 ] ,           "' to end macro definition" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
	;
#ifdef TRACE
      {
	(void) putc( '"' ,  logfile );
      } 
      {
	outtoken ( logfile ) ; 
      } 
      {
	(void) putc( '"' ,  logfile );
      } 
      {
	(void) fprintf( logfile , "%s\n",  " is a macro string" ) ; 
      } 
#endif /* TRACE */
      macrodefloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 0 , 
      true ) ; 
      fntype [ macrodefloc ] = 3 ; 
      ilkinfo [ macronameloc ] = hashtext [ macrodefloc ] ; 
      incr ( bufptr2 ) ; 
    } 
    {
      if ( ( ! eatbstwhitespace () ) ) 
      {
	eatbstprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "macro" ) ; 
	    (void) Fputs( stdout ,  "macro" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
    {
      if ( ( buffer [ bufptr2 ] != 125 ) ) 
      {
	bstrightbraceprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "macro" ) ; 
	    (void) Fputs( stdout ,  "macro" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
      incr ( bufptr2 ) ; 
    } 
  } 
  lab10: ; 
} 
void getbibcommandorentry ( ) 
{/* 22 26 15 10 */ atbibcommand = false ; 
  while ( ( ! scan1 ( 64 ) ) ) {
      
    if ( ( ! inputln ( bibfile [ bibptr ] ) ) ) 
    goto lab10 ; 
    incr ( biblinenum ) ; 
    bufptr2 = 0 ; 
  } 
  {
    if ( ( buffer [ bufptr2 ] != 64 ) ) 
    {
      {
	(void) fprintf( logfile , "%s%c%s",  "An \"" , xchr [ 64 ] , "\" disappeared" ) ; 
	(void) fprintf( stdout , "%s%c%s",  "An \"" , xchr [ 64 ] , "\" disappeared" ) ; 
      } 
      printconfusion () ; 
      longjmp(jmp9998,1) ; 
    } 
    incr ( bufptr2 ) ; 
    {
      if ( ( ! eatbibwhitespace () ) ) 
      {
	eatbibprint () ; 
	goto lab10 ; 
      } 
    } 
    scanidentifier ( 123 , 40 , 40 ) ; 
    {
      if ( ( ( scanresult == 3 ) || ( scanresult == 1 ) ) ) 
      ; 
      else {
	  
	bibidprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "an entry type" ) ; 
	    (void) Fputs( stdout ,  "an entry type" ) ; 
	  } 
	  biberrprint () ; 
	  goto lab10 ; 
	} 
      } 
    } 
	;
#ifdef TRACE
    {
      outtoken ( logfile ) ; 
    } 
    {
      (void) fprintf( logfile , "%s\n",  " is an entry type or a database-file command" ) ; 
    } 
#endif /* TRACE */
    lowercase ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
    commandnum = ilkinfo [ strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 
    ) , 12 , false ) ] ; 
    if ( ( hashfound ) ) 
    {
      atbibcommand = true ; 
      switch ( ( commandnum ) ) 
      {case 0 : 
	{
	  goto lab10 ; 
	} 
	break ; 
      case 1 : 
	{
	  if ( ( preambleptr == maxbibfiles ) ) 
	  {
	    {
	      (void) fprintf( logfile , "%s%ld%s",  "You've exceeded " , (long)maxbibfiles ,               " preamble commands" ) ; 
	      (void) fprintf( stdout , "%s%ld%s",  "You've exceeded " , (long)maxbibfiles ,               " preamble commands" ) ; 
	    } 
	    biberrprint () ; 
	    goto lab10 ; 
	  } 
	  {
	    if ( ( ! eatbibwhitespace () ) ) 
	    {
	      eatbibprint () ; 
	      goto lab10 ; 
	    } 
	  } 
	  if ( ( buffer [ bufptr2 ] == 123 ) ) 
	  rightouterdelim = 125 ; 
	  else if ( ( buffer [ bufptr2 ] == 40 ) ) 
	  rightouterdelim = 41 ; 
	  else {
	      
	    biboneoftwoprint ( 123 , 40 ) ; 
	    goto lab10 ; 
	  } 
	  incr ( bufptr2 ) ; 
	  {
	    if ( ( ! eatbibwhitespace () ) ) 
	    {
	      eatbibprint () ; 
	      goto lab10 ; 
	    } 
	  } 
	  storefield = true ; 
	  if ( ( ! scanandstorethefield () ) ) 
	  goto lab10 ; 
	  if ( ( buffer [ bufptr2 ] != rightouterdelim ) ) 
	  {
	    {
	      (void) fprintf( logfile , "%s%c%s",  "Missing \"" , xchr [ rightouterdelim ] ,               "\" in preamble command" ) ; 
	      (void) fprintf( stdout , "%s%c%s",  "Missing \"" , xchr [ rightouterdelim ] ,               "\" in preamble command" ) ; 
	    } 
	    biberrprint () ; 
	    goto lab10 ; 
	  } 
	  incr ( bufptr2 ) ; 
	  goto lab10 ; 
	} 
	break ; 
      case 2 : 
	{
	  {
	    if ( ( ! eatbibwhitespace () ) ) 
	    {
	      eatbibprint () ; 
	      goto lab10 ; 
	    } 
	  } 
	  {
	    if ( ( buffer [ bufptr2 ] == 123 ) ) 
	    rightouterdelim = 125 ; 
	    else if ( ( buffer [ bufptr2 ] == 40 ) ) 
	    rightouterdelim = 41 ; 
	    else {
		
	      biboneoftwoprint ( 123 , 40 ) ; 
	      goto lab10 ; 
	    } 
	    incr ( bufptr2 ) ; 
	    {
	      if ( ( ! eatbibwhitespace () ) ) 
	      {
		eatbibprint () ; 
		goto lab10 ; 
	      } 
	    } 
	    scanidentifier ( 61 , 61 , 61 ) ; 
	    {
	      if ( ( ( scanresult == 3 ) || ( scanresult == 1 ) ) ) 
	      ; 
	      else {
		  
		bibidprint () ; 
		{
		  {
		    (void) Fputs( logfile ,  "a string name" ) ; 
		    (void) Fputs( stdout ,  "a string name" ) ; 
		  } 
		  biberrprint () ; 
		  goto lab10 ; 
		} 
	      } 
	    } 
	    {
	;
#ifdef TRACE
	      {
		outtoken ( logfile ) ; 
	      } 
	      {
		(void) fprintf( logfile , "%s\n",  " is a database-defined macro" ) ; 
	      } 
#endif /* TRACE */
	      lowercase ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
	      curmacroloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 
	      ) , 13 , true ) ; 
	      ilkinfo [ curmacroloc ] = hashtext [ curmacroloc ] ; 
	    } 
	  } 
	  {
	    if ( ( ! eatbibwhitespace () ) ) 
	    {
	      eatbibprint () ; 
	      goto lab10 ; 
	    } 
	  } 
	  {
	    if ( ( buffer [ bufptr2 ] != 61 ) ) 
	    {
	      bibequalssignprint () ; 
	      goto lab10 ; 
	    } 
	    incr ( bufptr2 ) ; 
	    {
	      if ( ( ! eatbibwhitespace () ) ) 
	      {
		eatbibprint () ; 
		goto lab10 ; 
	      } 
	    } 
	    storefield = true ; 
	    if ( ( ! scanandstorethefield () ) ) 
	    goto lab10 ; 
	    if ( ( buffer [ bufptr2 ] != rightouterdelim ) ) 
	    {
	      {
		(void) fprintf( logfile , "%s%c%s",  "Missing \"" , xchr [ rightouterdelim ] ,                 "\" in string command" ) ; 
		(void) fprintf( stdout , "%s%c%s",  "Missing \"" , xchr [ rightouterdelim ] ,                 "\" in string command" ) ; 
	      } 
	      biberrprint () ; 
	      goto lab10 ; 
	    } 
	    incr ( bufptr2 ) ; 
	  } 
	  goto lab10 ; 
	} 
	break ; 
	default: 
	bibcmdconfusion () ; 
	break ; 
      } 
    } 
    else {
	
      entrytypeloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 11 
      , false ) ; 
      if ( ( ( ! hashfound ) || ( fntype [ entrytypeloc ] != 1 ) ) ) 
      typeexists = false ; 
      else typeexists = true ; 
    } 
  } 
  {
    if ( ( ! eatbibwhitespace () ) ) 
    {
      eatbibprint () ; 
      goto lab10 ; 
    } 
  } 
  {
    if ( ( buffer [ bufptr2 ] == 123 ) ) 
    rightouterdelim = 125 ; 
    else if ( ( buffer [ bufptr2 ] == 40 ) ) 
    rightouterdelim = 41 ; 
    else {
	
      biboneoftwoprint ( 123 , 40 ) ; 
      goto lab10 ; 
    } 
    incr ( bufptr2 ) ; 
    {
      if ( ( ! eatbibwhitespace () ) ) 
      {
	eatbibprint () ; 
	goto lab10 ; 
      } 
    } 
    if ( ( rightouterdelim == 41 ) ) 
    {
      if ( ( scan1white ( 44 ) ) ) 
      ; 
    } 
    else if ( ( scan2white ( 44 , 125 ) ) ) 
    ; 
    {
	;
#ifdef TRACE
      {
	outtoken ( logfile ) ; 
      } 
      {
	(void) fprintf( logfile , "%s\n",  " is a database key" ) ; 
      } 
#endif /* TRACE */
      tmpptr = bufptr1 ; 
      while ( ( tmpptr < bufptr2 ) ) {
	  
	exbuf [ tmpptr ] = buffer [ tmpptr ] ; 
	incr ( tmpptr ) ; 
      } 
      lowercase ( exbuf , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
      if ( ( allentries ) ) 
      lcciteloc = strlookup ( exbuf , bufptr1 , ( bufptr2 - bufptr1 ) , 10 , 
      true ) ; 
      else lcciteloc = strlookup ( exbuf , bufptr1 , ( bufptr2 - bufptr1 ) , 
      10 , false ) ; 
      if ( ( hashfound ) ) 
      {
	entryciteptr = ilkinfo [ ilkinfo [ lcciteloc ] ] ; 
	{
	  if ( ( ( ! allentries ) || ( entryciteptr < allmarker ) || ( 
	  entryciteptr >= oldnumcites ) ) ) 
	  {
	    if ( ( typelist [ entryciteptr ] == 0 ) ) 
	    {
	      {
		if ( ( ( ! allentries ) && ( entryciteptr >= oldnumcites ) ) ) 
		{
		  citeloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 
		  ) , 9 , true ) ; 
		  if ( ( ! hashfound ) ) 
		  {
		    ilkinfo [ lcciteloc ] = citeloc ; 
		    ilkinfo [ citeloc ] = entryciteptr ; 
		    citelist [ entryciteptr ] = hashtext [ citeloc ] ; 
		    hashfound = true ; 
		  } 
		} 
	      } 
	      goto lab26 ; 
	    } 
	  } 
	  else if ( ( ! entryexists [ entryciteptr ] ) ) 
	  {
	    {
	      exbufptr = 0 ; 
	      tmpptr = strstart [ citeinfo [ entryciteptr ] ] ; 
	      tmpendptr = strstart [ citeinfo [ entryciteptr ] + 1 ] ; 
	      while ( ( tmpptr < tmpendptr ) ) {
		  
		exbuf [ exbufptr ] = strpool [ tmpptr ] ; 
		incr ( exbufptr ) ; 
		incr ( tmpptr ) ; 
	      } 
	      lowercase ( exbuf , 0 , ( strstart [ citeinfo [ entryciteptr ] + 
	      1 ] - strstart [ citeinfo [ entryciteptr ] ] ) ) ; 
	      lcxciteloc = strlookup ( exbuf , 0 , ( strstart [ citeinfo [ 
	      entryciteptr ] + 1 ] - strstart [ citeinfo [ entryciteptr ] ] ) 
	      , 10 , false ) ; 
	      if ( ( ! hashfound ) ) 
	      citekeydisappearedco () ; 
	    } 
	    if ( ( lcxciteloc == lcciteloc ) ) 
	    goto lab26 ; 
	  } 
	  if ( ( typelist [ entryciteptr ] == 0 ) ) 
	  {
	    {
	      (void) Fputs( logfile ,  "The cite list is messed up" ) ; 
	      (void) Fputs( stdout ,  "The cite list is messed up" ) ; 
	    } 
	    printconfusion () ; 
	    longjmp(jmp9998,1) ; 
	  } 
	  {
	    {
	      (void) Fputs( logfile ,  "Repeated entry" ) ; 
	      (void) Fputs( stdout ,  "Repeated entry" ) ; 
	    } 
	    biberrprint () ; 
	    goto lab10 ; 
	  } 
	  lab26: ; 
	} 
      } 
      storeentry = true ; 
      if ( ( allentries ) ) 
      {
	if ( ( hashfound ) ) 
	{
	  if ( ( entryciteptr < allmarker ) ) 
	  goto lab22 ; 
	  else {
	      
	    entryexists [ entryciteptr ] = true ; 
	    citeloc = ilkinfo [ lcciteloc ] ; 
	  } 
	} 
	else {
	    
	  citeloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 9 , 
	  true ) ; 
	  if ( ( hashfound ) ) 
	  hashciteconfusion () ; 
	} 
	entryciteptr = citeptr ; 
	adddatabasecite ( citeptr ) ; 
	lab22: ; 
      } 
      else if ( ( ! hashfound ) ) 
      storeentry = false ; 
      if ( ( storeentry ) ) 
      {
	if ( ( typeexists ) ) 
	typelist [ entryciteptr ] = entrytypeloc ; 
	else {
	    
	  typelist [ entryciteptr ] = 5001 ; 
	  {
	    (void) Fputs( logfile ,  "Warning--entry type for \"" ) ; 
	    (void) Fputs( stdout ,  "Warning--entry type for \"" ) ; 
	  } 
	  printatoken () ; 
	  {
	    {
	      (void) fprintf( logfile , "%s\n",  "\" isn't style-file defined" ) ; 
	      (void) fprintf( stdout , "%s\n",  "\" isn't style-file defined" ) ; 
	    } 
	    bibwarnprint () ; 
	  } 
	} 
      } 
    } 
  } 
  {
    if ( ( ! eatbibwhitespace () ) ) 
    {
      eatbibprint () ; 
      goto lab10 ; 
    } 
  } 
  {
    while ( ( buffer [ bufptr2 ] != rightouterdelim ) ) {
	
      if ( ( buffer [ bufptr2 ] != 44 ) ) 
      {
	biboneoftwoprint ( 44 , rightouterdelim ) ; 
	goto lab10 ; 
      } 
      incr ( bufptr2 ) ; 
      {
	if ( ( ! eatbibwhitespace () ) ) 
	{
	  eatbibprint () ; 
	  goto lab10 ; 
	} 
      } 
      if ( ( buffer [ bufptr2 ] == rightouterdelim ) ) 
      goto lab15 ; 
      {
	scanidentifier ( 61 , 61 , 61 ) ; 
	{
	  if ( ( ( scanresult == 3 ) || ( scanresult == 1 ) ) ) 
	  ; 
	  else {
	      
	    bibidprint () ; 
	    {
	      {
		(void) Fputs( logfile ,  "a field name" ) ; 
		(void) Fputs( stdout ,  "a field name" ) ; 
	      } 
	      biberrprint () ; 
	      goto lab10 ; 
	    } 
	  } 
	} 
	;
#ifdef TRACE
	{
	  outtoken ( logfile ) ; 
	} 
	{
	  (void) fprintf( logfile , "%s\n",  " is a field name" ) ; 
	} 
#endif /* TRACE */
	storefield = false ; 
	if ( ( storeentry ) ) 
	{
	  lowercase ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
	  fieldnameloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) 
	  , 11 , false ) ; 
	  if ( ( hashfound ) ) 
	  if ( ( fntype [ fieldnameloc ] == 4 ) ) 
	  storefield = true ; 
	} 
	{
	  if ( ( ! eatbibwhitespace () ) ) 
	  {
	    eatbibprint () ; 
	    goto lab10 ; 
	  } 
	} 
	if ( ( buffer [ bufptr2 ] != 61 ) ) 
	{
	  bibequalssignprint () ; 
	  goto lab10 ; 
	} 
	incr ( bufptr2 ) ; 
      } 
      {
	if ( ( ! eatbibwhitespace () ) ) 
	{
	  eatbibprint () ; 
	  goto lab10 ; 
	} 
      } 
      if ( ( ! scanandstorethefield () ) ) 
      goto lab10 ; 
    } 
    lab15: incr ( bufptr2 ) ; 
  } 
  lab10: ; 
} 
void bstreadcommand ( ) 
{/* 10 */ if ( ( readseen ) ) 
  {
    {
      (void) Fputs( logfile ,  "Illegal, another read command" ) ; 
      (void) Fputs( stdout ,  "Illegal, another read command" ) ; 
    } 
    {
      bsterrprintandlookfo () ; 
      goto lab10 ; 
    } 
  } 
  readseen = true ; 
  if ( ( ! entryseen ) ) 
  {
    {
      (void) Fputs( logfile ,  "Illegal, read command before entry command" ) ; 
      (void) Fputs( stdout ,  "Illegal, read command before entry command" ) ; 
    } 
    {
      bsterrprintandlookfo () ; 
      goto lab10 ; 
    } 
  } 
  svptr1 = bufptr2 ; 
  svptr2 = last ; 
  tmpptr = svptr1 ; 
  while ( ( tmpptr < svptr2 ) ) {
      
    svbuffer [ tmpptr ] = buffer [ tmpptr ] ; 
    incr ( tmpptr ) ; 
  } 
  {
    {
      {
	checkfieldoverflow ( numfields * numcites ) ; 
	fieldptr = 0 ; 
	while ( ( fieldptr < maxfields ) ) {
	    
	  fieldinfo [ fieldptr ] = 0 ; 
	  incr ( fieldptr ) ; 
	} 
      } 
      {
	citeptr = 0 ; 
	while ( ( citeptr < maxcites ) ) {
	    
	  typelist [ citeptr ] = 0 ; 
	  citeinfo [ citeptr ] = 0 ; 
	  incr ( citeptr ) ; 
	} 
	oldnumcites = numcites ; 
	if ( ( allentries ) ) 
	{
	  citeptr = allmarker ; 
	  while ( ( citeptr < oldnumcites ) ) {
	      
	    citeinfo [ citeptr ] = citelist [ citeptr ] ; 
	    entryexists [ citeptr ] = false ; 
	    incr ( citeptr ) ; 
	  } 
	  citeptr = allmarker ; 
	} 
	else {
	    
	  citeptr = numcites ; 
	  allmarker = 0 ; 
	} 
      } 
    } 
    readperformed = true ; 
    bibptr = 0 ; 
    while ( ( bibptr < numbibfiles ) ) {
	
      {
	(void) fprintf( logfile , "%s%ld%s",  "Database file #" , (long)bibptr + 1 , ": " ) ; 
	(void) fprintf( stdout , "%s%ld%s",  "Database file #" , (long)bibptr + 1 , ": " ) ; 
      } 
      printbibname () ; 
      biblinenum = 0 ; 
      bufptr2 = last ; 
      while ( ( ! eof ( bibfile [ bibptr ] ) ) ) getbibcommandorentry () ; 
      aclose ( bibfile [ bibptr ] ) ; 
      incr ( bibptr ) ; 
    } 
    readingcompleted = true ; 
	;
#ifdef TRACE
    {
      (void) fprintf( logfile , "%s\n",  "Finished reading the database file(s)" ) ; 
    } 
#endif /* TRACE */
    {
      numcites = citeptr ; 
      numpreamblestrings = preambleptr ; 
      {
	citeptr = 0 ; 
	while ( ( citeptr < numcites ) ) {
	    
	  fieldptr = citeptr * numfields + crossrefnum ; 
	  if ( ( fieldinfo [ fieldptr ] != 0 ) ) 
	  if ( ( findcitelocsforthisc ( fieldinfo [ fieldptr ] ) ) ) 
	  {
	    citeloc = ilkinfo [ lcciteloc ] ; 
	    fieldinfo [ fieldptr ] = hashtext [ citeloc ] ; 
	    citeparentptr = ilkinfo [ citeloc ] ; 
	    fieldptr = citeptr * numfields + numpredefinedfields ; 
	    fieldendptr = fieldptr - numpredefinedfields + numfields ; 
	    fieldparentptr = citeparentptr * numfields + numpredefinedfields ; 
	    while ( ( fieldptr < fieldendptr ) ) {
		
	      if ( ( fieldinfo [ fieldptr ] == 0 ) ) 
	      fieldinfo [ fieldptr ] = fieldinfo [ fieldparentptr ] ; 
	      incr ( fieldptr ) ; 
	      incr ( fieldparentptr ) ; 
	    } 
	  } 
	  incr ( citeptr ) ; 
	} 
      } 
      {
	citeptr = 0 ; 
	while ( ( citeptr < numcites ) ) {
	    
	  fieldptr = citeptr * numfields + crossrefnum ; 
	  if ( ( fieldinfo [ fieldptr ] != 0 ) ) 
	  if ( ( ! findcitelocsforthisc ( fieldinfo [ fieldptr ] ) ) ) 
	  {
	    if ( ( citehashfound ) ) 
	    hashciteconfusion () ; 
	    nonexistentcrossrefe () ; 
	    fieldinfo [ fieldptr ] = 0 ; 
	  } 
	  else {
	      
	    if ( ( citeloc != ilkinfo [ lcciteloc ] ) ) 
	    hashciteconfusion () ; 
	    citeparentptr = ilkinfo [ citeloc ] ; 
	    if ( ( typelist [ citeparentptr ] == 0 ) ) 
	    {
	      nonexistentcrossrefe () ; 
	      fieldinfo [ fieldptr ] = 0 ; 
	    } 
	    else {
		
	      fieldparentptr = citeparentptr * numfields + crossrefnum ; 
	      if ( ( fieldinfo [ fieldparentptr ] != 0 ) ) 
	      {
		{
		  (void) Fputs( logfile ,  "Warning--you've nested cross references"                   ) ; 
		  (void) Fputs( stdout ,  "Warning--you've nested cross references" ) 
		  ; 
		} 
		badcrossreferencepri ( citelist [ citeparentptr ] ) ; 
		{
		  (void) fprintf( logfile , "%s\n",  "\", which also refers to something" ) ; 
		  (void) fprintf( stdout , "%s\n",  "\", which also refers to something" ) ; 
		} 
		markwarning () ; 
	      } 
	      if ( ( ( ! allentries ) && ( citeparentptr >= oldnumcites ) && ( 
	      citeinfo [ citeparentptr ] < mincrossrefs ) ) ) 
	      fieldinfo [ fieldptr ] = 0 ; 
	    } 
	  } 
	  incr ( citeptr ) ; 
	} 
      } 
      {
	citeptr = 0 ; 
	while ( ( citeptr < numcites ) ) {
	    
	  if ( ( typelist [ citeptr ] == 0 ) ) 
	  printmissingentry ( citelist [ citeptr ] ) ; 
	  else if ( ( ( allentries ) || ( citeptr < oldnumcites ) || ( 
	  citeinfo [ citeptr ] >= mincrossrefs ) ) ) 
	  {
	    if ( ( citeptr > citexptr ) ) 
	    {
	      citelist [ citexptr ] = citelist [ citeptr ] ; 
	      typelist [ citexptr ] = typelist [ citeptr ] ; 
	      if ( ( ! findcitelocsforthisc ( citelist [ citeptr ] ) ) ) 
	      citekeydisappearedco () ; 
	      if ( ( ( ! citehashfound ) || ( citeloc != ilkinfo [ lcciteloc ] 
	      ) ) ) 
	      hashciteconfusion () ; 
	      ilkinfo [ citeloc ] = citexptr ; 
	      fieldptr = citexptr * numfields ; 
	      fieldendptr = fieldptr + numfields ; 
	      tmpptr = citeptr * numfields ; 
	      while ( ( fieldptr < fieldendptr ) ) {
		  
		fieldinfo [ fieldptr ] = fieldinfo [ tmpptr ] ; 
		incr ( fieldptr ) ; 
		incr ( tmpptr ) ; 
	      } 
	    } 
	    incr ( citexptr ) ; 
	  } 
	  incr ( citeptr ) ; 
	} 
	numcites = citexptr ; 
	if ( ( allentries ) ) 
	{
	  citeptr = allmarker ; 
	  while ( ( citeptr < oldnumcites ) ) {
	      
	    if ( ( ! entryexists [ citeptr ] ) ) 
	    printmissingentry ( citeinfo [ citeptr ] ) ; 
	    incr ( citeptr ) ; 
	  } 
	} 
      } 
      {
	if ( ( numentints * numcites > maxentints ) ) 
	{
	  {
	    (void) fprintf( logfile , "%ld%s",  (long)numentints * numcites , ": " ) ; 
	    (void) fprintf( stdout , "%ld%s",  (long)numentints * numcites , ": " ) ; 
	  } 
	  {
	    printoverflow () ; 
	    {
	      (void) fprintf( logfile , "%s%ld\n",  "total number of integer entry-variables " ,               (long)maxentints ) ; 
	      (void) fprintf( stdout , "%s%ld\n",  "total number of integer entry-variables " ,               (long)maxentints ) ; 
	    } 
	    longjmp(jmp9998,1) ; 
	  } 
	} 
	intentptr = 0 ; 
	while ( ( intentptr < numentints * numcites ) ) {
	    
	  entryints [ intentptr ] = 0 ; 
	  incr ( intentptr ) ; 
	} 
      } 
      {
	if ( ( numentstrs * numcites > maxentstrs ) ) 
	{
	  {
	    (void) fprintf( logfile , "%ld%s",  (long)numentstrs * numcites , ": " ) ; 
	    (void) fprintf( stdout , "%ld%s",  (long)numentstrs * numcites , ": " ) ; 
	  } 
	  {
	    printoverflow () ; 
	    {
	      (void) fprintf( logfile , "%s%ld\n",  "total number of string entry-variables " ,               (long)maxentstrs ) ; 
	      (void) fprintf( stdout , "%s%ld\n",  "total number of string entry-variables " ,               (long)maxentstrs ) ; 
	    } 
	    longjmp(jmp9998,1) ; 
	  } 
	} 
	strentptr = 0 ; 
	while ( ( strentptr < numentstrs * numcites ) ) {
	    
	  entrystrs [ strentptr ] [ 0 ] = 127 ; 
	  incr ( strentptr ) ; 
	} 
      } 
      {
	citeptr = 0 ; 
	while ( ( citeptr < numcites ) ) {
	    
	  citeinfo [ citeptr ] = citeptr ; 
	  incr ( citeptr ) ; 
	} 
      } 
    } 
    readcompleted = true ; 
  } 
  bufptr2 = svptr1 ; 
  last = svptr2 ; 
  tmpptr = bufptr2 ; 
  while ( ( tmpptr < last ) ) {
      
    buffer [ tmpptr ] = svbuffer [ tmpptr ] ; 
    incr ( tmpptr ) ; 
  } 
  lab10: ; 
} 
void bstreversecommand ( ) 
{/* 10 */ if ( ( ! readseen ) ) 
  {
    {
      (void) Fputs( logfile ,  "Illegal, reverse command before read command" ) ; 
      (void) Fputs( stdout ,  "Illegal, reverse command before read command" ) ; 
    } 
    {
      bsterrprintandlookfo () ; 
      goto lab10 ; 
    } 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "reverse" ) ; 
	  (void) Fputs( stdout ,  "reverse" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    if ( ( buffer [ bufptr2 ] != 123 ) ) 
    {
      bstleftbraceprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "reverse" ) ; 
	  (void) Fputs( stdout ,  "reverse" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
    incr ( bufptr2 ) ; 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "reverse" ) ; 
	  (void) Fputs( stdout ,  "reverse" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    scanidentifier ( 125 , 37 , 37 ) ; 
    if ( ( ( scanresult == 3 ) || ( scanresult == 1 ) ) ) 
    ; 
    else {
	
      bstidprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "reverse" ) ; 
	  (void) Fputs( stdout ,  "reverse" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
	;
#ifdef TRACE
    {
      outtoken ( logfile ) ; 
    } 
    {
      (void) fprintf( logfile , "%s\n",  " is a to be iterated in reverse function" ) ; 
    } 
#endif /* TRACE */
    if ( ( badargumenttoken () ) ) 
    goto lab10 ; 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "reverse" ) ; 
	  (void) Fputs( stdout ,  "reverse" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    if ( ( buffer [ bufptr2 ] != 125 ) ) 
    {
      bstrightbraceprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "reverse" ) ; 
	  (void) Fputs( stdout ,  "reverse" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
    incr ( bufptr2 ) ; 
  } 
  {
    initcommandexecution () ; 
    messwithentries = true ; 
    if ( ( numcites > 0 ) ) 
    {
      sortciteptr = numcites ; 
      do {
	  decr ( sortciteptr ) ; 
	citeptr = citeinfo [ sortciteptr ] ; 
	;
#ifdef TRACE
	{
	  outpoolstr ( logfile , hashtext [ fnloc ] ) ; 
	} 
	{
	  (void) Fputs( logfile ,  " to be iterated in reverse on " ) ; 
	} 
	{
	  outpoolstr ( logfile , citelist [ citeptr ] ) ; 
	} 
	{
	  (void) putc('\n',  logfile );
	} 
#endif /* TRACE */
	executefn ( fnloc ) ; 
	checkcommandexecutio () ; 
      } while ( ! ( ( sortciteptr == 0 ) ) ) ; 
    } 
  } 
  lab10: ; 
} 
void bstsortcommand ( ) 
{/* 10 */ if ( ( ! readseen ) ) 
  {
    {
      (void) Fputs( logfile ,  "Illegal, sort command before read command" ) ; 
      (void) Fputs( stdout ,  "Illegal, sort command before read command" ) ; 
    } 
    {
      bsterrprintandlookfo () ; 
      goto lab10 ; 
    } 
  } 
  {
	;
#ifdef TRACE
    {
      (void) fprintf( logfile , "%s\n",  "Sorting the entries" ) ; 
    } 
#endif /* TRACE */
    if ( ( numcites > 1 ) ) 
    quicksort ( 0 , numcites - 1 ) ; 
	;
#ifdef TRACE
    {
      (void) fprintf( logfile , "%s\n",  "Done sorting" ) ; 
    } 
#endif /* TRACE */
  } 
  lab10: ; 
} 
void bststringscommand ( ) 
{/* 10 */ {
    
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "strings" ) ; 
	  (void) Fputs( stdout ,  "strings" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  {
    if ( ( buffer [ bufptr2 ] != 123 ) ) 
    {
      bstleftbraceprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "strings" ) ; 
	  (void) Fputs( stdout ,  "strings" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
    incr ( bufptr2 ) ; 
  } 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    {
      eatbstprint () ; 
      {
	{
	  (void) Fputs( logfile ,  "strings" ) ; 
	  (void) Fputs( stdout ,  "strings" ) ; 
	} 
	{
	  bsterrprintandlookfo () ; 
	  goto lab10 ; 
	} 
      } 
    } 
  } 
  while ( ( buffer [ bufptr2 ] != 125 ) ) {
      
    {
      scanidentifier ( 125 , 37 , 37 ) ; 
      if ( ( ( scanresult == 3 ) || ( scanresult == 1 ) ) ) 
      ; 
      else {
	  
	bstidprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "strings" ) ; 
	    (void) Fputs( stdout ,  "strings" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
    {
	;
#ifdef TRACE
      {
	outtoken ( logfile ) ; 
      } 
      {
	(void) fprintf( logfile , "%s\n",  " is a string global-variable" ) ; 
      } 
#endif /* TRACE */
      lowercase ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
      fnloc = strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) , 11 , true 
      ) ; 
      {
	if ( ( hashfound ) ) 
	{
	  alreadyseenfunctionp ( fnloc ) ; 
	  goto lab10 ; 
	} 
      } 
      fntype [ fnloc ] = 8 ; 
      ilkinfo [ fnloc ] = numglbstrs ; 
      if ( ( numglbstrs == 10 ) ) 
      {
	printoverflow () ; 
	{
	  (void) fprintf( logfile , "%s%ld\n",  "number of string global-variables " , (long)10 ) ; 
	  (void) fprintf( stdout , "%s%ld\n",  "number of string global-variables " , (long)10 ) ; 
	} 
	longjmp(jmp9998,1) ; 
      } 
      incr ( numglbstrs ) ; 
    } 
    {
      if ( ( ! eatbstwhitespace () ) ) 
      {
	eatbstprint () ; 
	{
	  {
	    (void) Fputs( logfile ,  "strings" ) ; 
	    (void) Fputs( stdout ,  "strings" ) ; 
	  } 
	  {
	    bsterrprintandlookfo () ; 
	    goto lab10 ; 
	  } 
	} 
      } 
    } 
  } 
  incr ( bufptr2 ) ; 
  lab10: ; 
} 
void getbstcommandandproc ( ) 
{/* 10 */ if ( ( ! scanalpha () ) ) 
  {
    {
      (void) fprintf( logfile , "%c%c%s",  '"' , xchr [ buffer [ bufptr2 ] ] ,       "\" can't start a style-file command" ) ; 
      (void) fprintf( stdout , "%c%c%s",  '"' , xchr [ buffer [ bufptr2 ] ] ,       "\" can't start a style-file command" ) ; 
    } 
    {
      bsterrprintandlookfo () ; 
      goto lab10 ; 
    } 
  } 
  lowercase ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) ) ; 
  commandnum = ilkinfo [ strlookup ( buffer , bufptr1 , ( bufptr2 - bufptr1 ) 
  , 4 , false ) ] ; 
  if ( ( ! hashfound ) ) 
  {
    printatoken () ; 
    {
      {
	(void) Fputs( logfile ,  " is an illegal style-file command" ) ; 
	(void) Fputs( stdout ,  " is an illegal style-file command" ) ; 
      } 
      {
	bsterrprintandlookfo () ; 
	goto lab10 ; 
      } 
    } 
  } 
  switch ( ( commandnum ) ) 
  {case 0 : 
    bstentrycommand () ; 
    break ; 
  case 1 : 
    bstexecutecommand () ; 
    break ; 
  case 2 : 
    bstfunctioncommand () ; 
    break ; 
  case 3 : 
    bstintegerscommand () ; 
    break ; 
  case 4 : 
    bstiteratecommand () ; 
    break ; 
  case 5 : 
    bstmacrocommand () ; 
    break ; 
  case 6 : 
    bstreadcommand () ; 
    break ; 
  case 7 : 
    bstreversecommand () ; 
    break ; 
  case 8 : 
    bstsortcommand () ; 
    break ; 
  case 9 : 
    bststringscommand () ; 
    break ; 
    default: 
    {
      {
	(void) Fputs( logfile ,  "Unknown style-file command" ) ; 
	(void) Fputs( stdout ,  "Unknown style-file command" ) ; 
      } 
      printconfusion () ; 
      longjmp(jmp9998,1) ; 
    } 
    break ; 
  } 
  lab10: ; 
} 
void initialize ( ) 
{schar i  ; 
  hashloc k  ; 
  bad = 0 ; 
  if ( ( minprintline < 3 ) ) 
  bad = 1 ; 
  if ( ( maxprintline <= minprintline ) ) 
  bad = 10 * bad + 2 ; 
  if ( ( maxprintline >= bufsize ) ) 
  bad = 10 * bad + 3 ; 
  if ( ( 4253 < 128 ) ) 
  bad = 10 * bad + 4 ; 
  if ( ( 4253 > 5000 ) ) 
  bad = 10 * bad + 5 ; 
  if ( ( 4253 >= ( 16320 ) ) ) 
  bad = 10 * bad + 6 ; 
  if ( ( maxstrings > 5000 ) ) 
  bad = 10 * bad + 7 ; 
  if ( ( maxcites > maxstrings ) ) 
  bad = 10 * bad + 8 ; 
  if ( ( entstrsize > bufsize ) ) 
  bad = 10 * bad + 9 ; 
  if ( ( globstrsize > bufsize ) ) 
  bad = 100 * bad + 11 ; 
  if ( ( 10 < 2 * 4 + 2 ) ) 
  bad = 100 * bad + 22 ; 
  if ( ( bad > 0 ) ) 
  {
    (void) fprintf( stdout , "%ld%s\n",  (long)bad , " is a bad bad" ) ; 
    uexit ( 0 ) ; 
  } 
  history = 0 ; 
  xchr [ 32 ] = ' ' ; 
  xchr [ 33 ] = '!' ; 
  xchr [ 34 ] = '"' ; 
  xchr [ 35 ] = '#' ; 
  xchr [ 36 ] = '$' ; 
  xchr [ 37 ] = '%' ; 
  xchr [ 38 ] = '&' ; 
  xchr [ 39 ] = '\'' ; 
  xchr [ 40 ] = '(' ; 
  xchr [ 41 ] = ')' ; 
  xchr [ 42 ] = '*' ; 
  xchr [ 43 ] = '+' ; 
  xchr [ 44 ] = ',' ; 
  xchr [ 45 ] = '-' ; 
  xchr [ 46 ] = '.' ; 
  xchr [ 47 ] = '/' ; 
  xchr [ 48 ] = '0' ; 
  xchr [ 49 ] = '1' ; 
  xchr [ 50 ] = '2' ; 
  xchr [ 51 ] = '3' ; 
  xchr [ 52 ] = '4' ; 
  xchr [ 53 ] = '5' ; 
  xchr [ 54 ] = '6' ; 
  xchr [ 55 ] = '7' ; 
  xchr [ 56 ] = '8' ; 
  xchr [ 57 ] = '9' ; 
  xchr [ 58 ] = ':' ; 
  xchr [ 59 ] = ';' ; 
  xchr [ 60 ] = '<' ; 
  xchr [ 61 ] = '=' ; 
  xchr [ 62 ] = '>' ; 
  xchr [ 63 ] = '?' ; 
  xchr [ 64 ] = '@' ; 
  xchr [ 65 ] = 'A' ; 
  xchr [ 66 ] = 'B' ; 
  xchr [ 67 ] = 'C' ; 
  xchr [ 68 ] = 'D' ; 
  xchr [ 69 ] = 'E' ; 
  xchr [ 70 ] = 'F' ; 
  xchr [ 71 ] = 'G' ; 
  xchr [ 72 ] = 'H' ; 
  xchr [ 73 ] = 'I' ; 
  xchr [ 74 ] = 'J' ; 
  xchr [ 75 ] = 'K' ; 
  xchr [ 76 ] = 'L' ; 
  xchr [ 77 ] = 'M' ; 
  xchr [ 78 ] = 'N' ; 
  xchr [ 79 ] = 'O' ; 
  xchr [ 80 ] = 'P' ; 
  xchr [ 81 ] = 'Q' ; 
  xchr [ 82 ] = 'R' ; 
  xchr [ 83 ] = 'S' ; 
  xchr [ 84 ] = 'T' ; 
  xchr [ 85 ] = 'U' ; 
  xchr [ 86 ] = 'V' ; 
  xchr [ 87 ] = 'W' ; 
  xchr [ 88 ] = 'X' ; 
  xchr [ 89 ] = 'Y' ; 
  xchr [ 90 ] = 'Z' ; 
  xchr [ 91 ] = '[' ; 
  xchr [ 92 ] = '\\' ; 
  xchr [ 93 ] = ']' ; 
  xchr [ 94 ] = '^' ; 
  xchr [ 95 ] = '_' ; 
  xchr [ 96 ] = '`' ; 
  xchr [ 97 ] = 'a' ; 
  xchr [ 98 ] = 'b' ; 
  xchr [ 99 ] = 'c' ; 
  xchr [ 100 ] = 'd' ; 
  xchr [ 101 ] = 'e' ; 
  xchr [ 102 ] = 'f' ; 
  xchr [ 103 ] = 'g' ; 
  xchr [ 104 ] = 'h' ; 
  xchr [ 105 ] = 'i' ; 
  xchr [ 106 ] = 'j' ; 
  xchr [ 107 ] = 'k' ; 
  xchr [ 108 ] = 'l' ; 
  xchr [ 109 ] = 'm' ; 
  xchr [ 110 ] = 'n' ; 
  xchr [ 111 ] = 'o' ; 
  xchr [ 112 ] = 'p' ; 
  xchr [ 113 ] = 'q' ; 
  xchr [ 114 ] = 'r' ; 
  xchr [ 115 ] = 's' ; 
  xchr [ 116 ] = 't' ; 
  xchr [ 117 ] = 'u' ; 
  xchr [ 118 ] = 'v' ; 
  xchr [ 119 ] = 'w' ; 
  xchr [ 120 ] = 'x' ; 
  xchr [ 121 ] = 'y' ; 
  xchr [ 122 ] = 'z' ; 
  xchr [ 123 ] = '{' ; 
  xchr [ 124 ] = '|' ; 
  xchr [ 125 ] = '}' ; 
  xchr [ 126 ] = '~' ; 
  xchr [ 0 ] = ' ' ; 
  xchr [ 127 ] = ' ' ; 
  {register integer for_end; i = 1 ; for_end = 31 ; if ( i <= for_end) do 
    xchr [ i ] = ' ' ; 
  while ( i++ < for_end ) ; } 
  xchr [ 9 ] = chr ( 9 ) ; 
  {register integer for_end; i = 0 ; for_end = 127 ; if ( i <= for_end) do 
    xord [ chr ( i ) ] = 127 ; 
  while ( i++ < for_end ) ; } 
  {register integer for_end; i = 1 ; for_end = 126 ; if ( i <= for_end) do 
    xord [ xchr [ i ] ] = i ; 
  while ( i++ < for_end ) ; } 
  {register integer for_end; i = 0 ; for_end = 127 ; if ( i <= for_end) do 
    lexclass [ i ] = 5 ; 
  while ( i++ < for_end ) ; } 
  {register integer for_end; i = 0 ; for_end = 31 ; if ( i <= for_end) do 
    lexclass [ i ] = 0 ; 
  while ( i++ < for_end ) ; } 
  lexclass [ 127 ] = 0 ; 
  lexclass [ 9 ] = 1 ; 
  lexclass [ 32 ] = 1 ; 
  lexclass [ 126 ] = 4 ; 
  lexclass [ 45 ] = 4 ; 
  {register integer for_end; i = 48 ; for_end = 57 ; if ( i <= for_end) do 
    lexclass [ i ] = 3 ; 
  while ( i++ < for_end ) ; } 
  {register integer for_end; i = 65 ; for_end = 90 ; if ( i <= for_end) do 
    lexclass [ i ] = 2 ; 
  while ( i++ < for_end ) ; } 
  {register integer for_end; i = 97 ; for_end = 122 ; if ( i <= for_end) do 
    lexclass [ i ] = 2 ; 
  while ( i++ < for_end ) ; } 
  {register integer for_end; i = 0 ; for_end = 127 ; if ( i <= for_end) do 
    idclass [ i ] = 1 ; 
  while ( i++ < for_end ) ; } 
  {register integer for_end; i = 0 ; for_end = 31 ; if ( i <= for_end) do 
    idclass [ i ] = 0 ; 
  while ( i++ < for_end ) ; } 
  idclass [ 32 ] = 0 ; 
  idclass [ 9 ] = 0 ; 
  idclass [ 34 ] = 0 ; 
  idclass [ 35 ] = 0 ; 
  idclass [ 37 ] = 0 ; 
  idclass [ 39 ] = 0 ; 
  idclass [ 40 ] = 0 ; 
  idclass [ 41 ] = 0 ; 
  idclass [ 44 ] = 0 ; 
  idclass [ 61 ] = 0 ; 
  idclass [ 123 ] = 0 ; 
  idclass [ 125 ] = 0 ; 
  {register integer for_end; i = 0 ; for_end = 127 ; if ( i <= for_end) do 
    charwidth [ i ] = 0 ; 
  while ( i++ < for_end ) ; } 
  charwidth [ 32 ] = 278 ; 
  charwidth [ 33 ] = 278 ; 
  charwidth [ 34 ] = 500 ; 
  charwidth [ 35 ] = 833 ; 
  charwidth [ 36 ] = 500 ; 
  charwidth [ 37 ] = 833 ; 
  charwidth [ 38 ] = 778 ; 
  charwidth [ 39 ] = 278 ; 
  charwidth [ 40 ] = 389 ; 
  charwidth [ 41 ] = 389 ; 
  charwidth [ 42 ] = 500 ; 
  charwidth [ 43 ] = 778 ; 
  charwidth [ 44 ] = 278 ; 
  charwidth [ 45 ] = 333 ; 
  charwidth [ 46 ] = 278 ; 
  charwidth [ 47 ] = 500 ; 
  charwidth [ 48 ] = 500 ; 
  charwidth [ 49 ] = 500 ; 
  charwidth [ 50 ] = 500 ; 
  charwidth [ 51 ] = 500 ; 
  charwidth [ 52 ] = 500 ; 
  charwidth [ 53 ] = 500 ; 
  charwidth [ 54 ] = 500 ; 
  charwidth [ 55 ] = 500 ; 
  charwidth [ 56 ] = 500 ; 
  charwidth [ 57 ] = 500 ; 
  charwidth [ 58 ] = 278 ; 
  charwidth [ 59 ] = 278 ; 
  charwidth [ 60 ] = 278 ; 
  charwidth [ 61 ] = 778 ; 
  charwidth [ 62 ] = 472 ; 
  charwidth [ 63 ] = 472 ; 
  charwidth [ 64 ] = 778 ; 
  charwidth [ 65 ] = 750 ; 
  charwidth [ 66 ] = 708 ; 
  charwidth [ 67 ] = 722 ; 
  charwidth [ 68 ] = 764 ; 
  charwidth [ 69 ] = 681 ; 
  charwidth [ 70 ] = 653 ; 
  charwidth [ 71 ] = 785 ; 
  charwidth [ 72 ] = 750 ; 
  charwidth [ 73 ] = 361 ; 
  charwidth [ 74 ] = 514 ; 
  charwidth [ 75 ] = 778 ; 
  charwidth [ 76 ] = 625 ; 
  charwidth [ 77 ] = 917 ; 
  charwidth [ 78 ] = 750 ; 
  charwidth [ 79 ] = 778 ; 
  charwidth [ 80 ] = 681 ; 
  charwidth [ 81 ] = 778 ; 
  charwidth [ 82 ] = 736 ; 
  charwidth [ 83 ] = 556 ; 
  charwidth [ 84 ] = 722 ; 
  charwidth [ 85 ] = 750 ; 
  charwidth [ 86 ] = 750 ; 
  charwidth [ 87 ] = 1028 ; 
  charwidth [ 88 ] = 750 ; 
  charwidth [ 89 ] = 750 ; 
  charwidth [ 90 ] = 611 ; 
  charwidth [ 91 ] = 278 ; 
  charwidth [ 92 ] = 500 ; 
  charwidth [ 93 ] = 278 ; 
  charwidth [ 94 ] = 500 ; 
  charwidth [ 95 ] = 278 ; 
  charwidth [ 96 ] = 278 ; 
  charwidth [ 97 ] = 500 ; 
  charwidth [ 98 ] = 556 ; 
  charwidth [ 99 ] = 444 ; 
  charwidth [ 100 ] = 556 ; 
  charwidth [ 101 ] = 444 ; 
  charwidth [ 102 ] = 306 ; 
  charwidth [ 103 ] = 500 ; 
  charwidth [ 104 ] = 556 ; 
  charwidth [ 105 ] = 278 ; 
  charwidth [ 106 ] = 306 ; 
  charwidth [ 107 ] = 528 ; 
  charwidth [ 108 ] = 278 ; 
  charwidth [ 109 ] = 833 ; 
  charwidth [ 110 ] = 556 ; 
  charwidth [ 111 ] = 500 ; 
  charwidth [ 112 ] = 556 ; 
  charwidth [ 113 ] = 528 ; 
  charwidth [ 114 ] = 392 ; 
  charwidth [ 115 ] = 394 ; 
  charwidth [ 116 ] = 389 ; 
  charwidth [ 117 ] = 556 ; 
  charwidth [ 118 ] = 528 ; 
  charwidth [ 119 ] = 722 ; 
  charwidth [ 120 ] = 528 ; 
  charwidth [ 121 ] = 528 ; 
  charwidth [ 122 ] = 444 ; 
  charwidth [ 123 ] = 500 ; 
  charwidth [ 124 ] = 1000 ; 
  charwidth [ 125 ] = 500 ; 
  charwidth [ 126 ] = 500 ; 
  {register integer for_end; k = 1 ; for_end = 5000 ; if ( k <= for_end) do 
    {
      hashnext [ k ] = 0 ; 
      hashtext [ k ] = 0 ; 
    } 
  while ( k++ < for_end ) ; } 
  hashused = 5001 ; 
  poolptr = 0 ; 
  strptr = 1 ; 
  strstart [ strptr ] = poolptr ; 
  bibptr = 0 ; 
  bibseen = false ; 
  bststr = 0 ; 
  bstseen = false ; 
  citeptr = 0 ; 
  citationseen = false ; 
  allentries = false ; 
  wizdefptr = 0 ; 
  numentints = 0 ; 
  numentstrs = 0 ; 
  numfields = 0 ; 
  strglbptr = 0 ; 
  while ( ( strglbptr < 10 ) ) {
      
    glbstrptr [ strglbptr ] = 0 ; 
    glbstrend [ strglbptr ] = 0 ; 
    incr ( strglbptr ) ; 
  } 
  numglbstrs = 0 ; 
  entryseen = false ; 
  readseen = false ; 
  readperformed = false ; 
  readingcompleted = false ; 
  readcompleted = false ; 
  implfnnum = 0 ; 
  outbuflength = 0 ; 
  predefcertainstrings () ; 
  getthetoplevelauxfil () ; 
} 
void main_body() {
    
  initialize () ; 
  {
    (void) fprintf( logfile , "%s\n",  "This is BibTeX, C Version 0.99c" ) ; 
    (void) fprintf( stdout , "%s\n",  "This is BibTeX, C Version 0.99c" ) ; 
  } 
  {
    (void) Fputs( logfile ,  "The top-level auxiliary file: " ) ; 
    (void) Fputs( stdout ,  "The top-level auxiliary file: " ) ; 
  } 
  printauxname () ; 
  while ( lab31==0 ) {
      
    incr ( auxlnstack [ auxptr ] ) ; 
    if ( ( ! inputln ( auxfile [ auxptr ] ) ) ) 
    poptheauxstack () ; 
    else getauxcommandandproc () ; 
  } 
#ifdef TRACE
  {
    (void) fprintf( logfile , "%s\n",  "Finished reading the auxiliary file(s)" ) ; 
  } 
#endif /* TRACE */
   lastcheckforauxerror () ; 
  if ( ( bststr == 0 ) ) 
  goto lab9932 ; 
  bstlinenum = 0 ; 
  bbllinenum = 1 ; 
  bufptr2 = last ; 
  if(setjmp(jmp9998)==1) goto lab9998;if(setjmp(jmp32)==0)while(true) 
  {
    if ( ( ! eatbstwhitespace () ) ) 
    break ; 
    getbstcommandandproc () ; 
  } 
   aclose ( bstfile ) ; 
  lab9932: aclose ( bblfile ) ; 
  lab9998: {
      
    if ( ( ( readperformed ) && ( ! readingcompleted ) ) ) 
    {
      {
	(void) fprintf( logfile , "%s%ld%s",  "Aborted at line " , (long)biblinenum , " of file " ) ; 
	(void) fprintf( stdout , "%s%ld%s",  "Aborted at line " , (long)biblinenum , " of file " ) ; 
      } 
      printbibname () ; 
    } 
    traceandstatprinting () ; 
    switch ( ( history ) ) 
    {case 0 : 
      ; 
      break ; 
    case 1 : 
      {
	if ( ( errcount == 1 ) ) 
	{
	  (void) fprintf( logfile , "%s\n",  "(There was 1 warning)" ) ; 
	  (void) fprintf( stdout , "%s\n",  "(There was 1 warning)" ) ; 
	} 
	else {
	    
	  (void) fprintf( logfile , "%s%ld%s\n",  "(There were " , (long)errcount , " warnings)" ) ; 
	  (void) fprintf( stdout , "%s%ld%s\n",  "(There were " , (long)errcount , " warnings)" ) ; 
	} 
      } 
      break ; 
    case 2 : 
      {
	if ( ( errcount == 1 ) ) 
	{
	  (void) fprintf( logfile , "%s\n",  "(There was 1 error message)" ) ; 
	  (void) fprintf( stdout , "%s\n",  "(There was 1 error message)" ) ; 
	} 
	else {
	    
	  (void) fprintf( logfile , "%s%ld%s\n",  "(There were " , (long)errcount , " error messages)" ) 
	  ; 
	  (void) fprintf( stdout , "%s%ld%s\n",  "(There were " , (long)errcount , " error messages)" ) 
	  ; 
	} 
      } 
      break ; 
    case 3 : 
      {
	(void) fprintf( logfile , "%s\n",  "(That was a fatal error)" ) ; 
	(void) fprintf( stdout , "%s\n",  "(That was a fatal error)" ) ; 
      } 
      break ; 
      default: 
      {
	{
	  (void) Fputs( logfile ,  "History is bunk" ) ; 
	  (void) Fputs( stdout ,  "History is bunk" ) ; 
	} 
	printconfusion () ; 
      } 
      break ; 
    } 
    aclose ( logfile ) ; 
  } 
  lab9999: ; 
} 
