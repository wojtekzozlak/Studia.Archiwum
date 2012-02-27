
#define PI 3.1415926535897932384

typedef struct {
          float x, y;
        } point;

void PSOpenFile ( const char *filename, unsigned int dpi,
                  int xa, int ya, int xb, int yb );
void PSCloseFile ( void );

void PSSetGray ( float gray );
void PSSetRGB ( float red, float green, float blue );
void PSSetLineWidth ( float w );

void PSDrawLine ( float x1, float y1, float x2, float y2 );
void PSSetClipRect ( float w, float h, float x, float y );
void PSDrawRect ( float w, float h, float x, float y );
void PSFillRect ( float w, float h, float x, float y );

void PSDrawPolyline ( int n, const point *p, char closed );
void PSFillPolygon ( int n, const point *p );
void PSDrawCircle ( float x, float y, float r );
void PSFillCircle ( float x, float y, float r );
void PSDrawArc ( float x, float y, float r, float a0, float a1 );
void PSWriteCommand ( char *command );

void PSGSave ( void );
void PSGRestore ( void );

void PSBeginDict ( int n );
void PSEndDict ( void );

