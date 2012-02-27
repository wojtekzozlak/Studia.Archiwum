
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "postscript.h"

static FILE *f;
static long written;

void ps_BeginDict ( int n )
{
  fprintf ( f, "%d dict begin\n", n );
} /*ps_BeginDict*/

void ps_EndDict ()
{
  fprintf ( f, "end\n" );
} /*ps_EndDict*/

void PSOpenFile ( const char *filename, unsigned int dpi,
                  int xa, int ya, int xb, int yb )
{
  f = fopen ( filename, "w");
  if (f == NULL)
  {
    printf ( "cannot create file %s\n", filename );
    exit ( 1 );
  }
  fprintf(f, "%%!PS-Adobe-2.0\n");
  fprintf(f, "%%%%BoundingBox: %d %d %d %d\n", xa, ya, xb, yb );
  fprintf(f, "%% %s\n", filename);
  fprintf(f, "gsave\n");
  fprintf(f, "20 dict begin\n");
  fprintf(f, "/resolution %u def\n", dpi);
  fprintf(f, "72 resolution div dup scale\n");
} /*PSOpenFile*/

void PSCloseFile ()
{
  fprintf(f, "end\n");
  fprintf(f, "grestore\n");
  fprintf(f, "showpage\n");
  fprintf(f, "%%\n");
  fclose ( f );
} /*PSCloseFile*/

void PSSet_Gray ( float gray )
{
  fprintf ( f, "%5.3f setgray\n", gray );
} /*PSSetGray*/

void PSSetRGB ( float red, float green, float blue )
{
  fprintf ( f, "%5.3f %5.3f %5.3f setrgbcolor\n", red, green, blue );
} /*PSSetRGB*/

void PSSetLineWidth ( float w )
{
  fprintf ( f, "%5.3f setlinewidth\n", w );
} /*PSSetLineWidth*/

void PSDrawLine ( float x1, float y1, float x2, float y2 )
{
  fprintf ( f, "newpath %5.3f %5.3f moveto %5.3f %5.3f lineto stroke\n",
	    x1, y1, x2, y2 );
} /*PSDrawLine*/

void PSSetClipRect ( float w, float h, float x, float y )
{
  fprintf ( f,
    "newpath %5.3f %5.3f moveto %5.3f %5.3f lineto %5.3f %5.3f lineto "
    "%5.3f %5.3f lineto closepath clip\n",
    x, y, x + w, y, x + w, y + h, x, y + h );
} /*PSSetClipRect*/

void PSDrawRect ( float w, float h, float x, float y )
{
  fprintf ( f,
    "newpath %5.3f %5.3f moveto %5.3f %5.3f lineto %5.3f %5.3f lineto "
    "%5.3f %5.3f lineto closepath stroke\n",
    x, y, x + w, y, x + w, y + h, x, y + h );
} /*PSDrawRect*/

void PSFillRect ( float w, float h, float x, float y )
{
  fprintf ( f,
    "newpath %5.3f %5.3f moveto %5.3f %5.3f lineto %5.3f %5.3f lineto "
    "%5.3f %5.3f lineto closepath fill\n",
    x, y, x + w, y, x + w, y + h, x, y + h );
} /*PSFillRect*/

static void PSOutputPath ( int n, const point *p )
{
  int i;

  fprintf ( f, "%5.3f %5.3f newpath moveto\n", p[0].x, p[0].y );
  for ( i = 1; i < n; i++ )
    fprintf ( f, "%5.3f %5.3f lineto\n", p[i].x, p[i].y );
} /*PSOutputPath*/

void PSDrawPolyline ( int n, const point *p, char closed )
{
  if ( n < 2 )
    return;
  PSOutputPath ( n, p );
  if ( closed )
    fprintf ( f, " closepath" );
  fprintf ( f, " stroke\n" );
} /*PSDrawPolyline*/

void PSFillPolygon ( int n, const point *p )
{
  if ( n < 2 )
    return;
  PSOutputPath ( n, p );
  fprintf ( f, "closepath fill\n" );
} /*PSFillPolygon*/

void PSDrawCircle ( float x, float y, float r )
{
  fprintf ( f, "newpath %5.3f %5.3f %5.3f 0 360 arc stroke\n", x, y, r );
} /*PSDrawCircle*/

void PSFillCircle ( float x, float y, float r )
{
  fprintf ( f, "newpath %5.3f %5.3f %5.3f 0 360 arc fill\n", x, y, r );
} /*PSFillCircle*/

void PSDrawArc ( float x, float y, float r, float a0, float a1 )
{
  fprintf ( f, "newpath %5.3f %5.3f %5.3f %5.3f %5.3f arc stroke\n",
            x, y, r, 180.0/PI*a0, 180.0/PI*a1 );
} /*PSDrawArc*/

void PSWriteCommand ( char *command )
{
  fprintf ( f, "%s\n", command );
} /*PSWriteCommand*/

void PSGSave ()
{
  fprintf ( f, "gsave\n" );
} /*PSGSave*/

void PSGRestore ()
{
  fprintf ( f, "grestore\n" );
} /*PSGRestore*/


