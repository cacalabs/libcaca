/* What will I use my programming skill for? */

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "caca.h"

/* ref($sprite) eq 'HASH' && $sprite->{__address__} */
void *
address_of(SV *sprite)
{
  /* make sure sprite is a hashref */
  if (SvTYPE(SvRV(sprite)) != SVt_PVHV) {
    return NULL;
  }
  return (struct caca_sprite *)
    SvIV(*hv_fetch((HV *) SvRV(sprite), "__address__", 11, 0));
}

/* turn a perl array of numbers into a c array */
void *
c_array(SV *p_array)
{
}

MODULE = Term::Caca   PACKAGE = Term::Caca

# -==[- Basic functions -]==--------------------------------------------------

void
_init()
  CODE:
    caca_init();

void
_set_delay(usec)
    unsigned int usec
  CODE:
    caca_set_delay(usec);

unsigned int
_get_feature(feature)
    unsigned int feature
  CODE:
    RETVAL = caca_get_feature(feature);
  OUTPUT:
    RETVAL

void
_set_feature(feature)
    unsigned int feature
  CODE:
    caca_set_feature(feature);

const char *
_get_feature_name(feature)
    unsigned int feature
  CODE:
    RETVAL = caca_get_feature_name(feature);
  OUTPUT:
    RETVAL

unsigned int
_get_rendertime()
  CODE:
    RETVAL = caca_get_rendertime();
  OUTPUT:
    RETVAL

unsigned int
_get_width()
  CODE:
    RETVAL = caca_get_width();
  OUTPUT:
    RETVAL

unsigned int
_get_height()
  CODE:
    RETVAL = caca_get_height();
  OUTPUT:
    RETVAL

int
_set_window_title(title)
    const char *title
  CODE:
    RETVAL = caca_set_window_title(title);
  OUTPUT:
    RETVAL

unsigned int
_get_window_width()
  CODE:
    RETVAL = caca_get_window_width();
  OUTPUT:
    RETVAL

unsigned int
_get_window_height()
  CODE:
    RETVAL = caca_get_window_height();
  OUTPUT:
    RETVAL

void
_refresh()
  CODE:
    caca_refresh();

void
_end()
  CODE:
    caca_end();

# -==[- Event handling -]==---------------------------------------------------

unsigned int
_get_event(event_mask)
    unsigned int event_mask
  CODE:
    RETVAL = caca_get_event(event_mask);
  OUTPUT:
    RETVAL

unsigned int
_get_mouse_x()
  CODE:
    RETVAL = caca_get_mouse_x();
  OUTPUT:
    RETVAL

unsigned int
_get_mouse_y()
  CODE:
    RETVAL = caca_get_mouse_y();
  OUTPUT:
    RETVAL

unsigned int
_wait_event(event_mask)
    unsigned int event_mask
  CODE:
    RETVAL = caca_wait_event(event_mask);
  OUTPUT:
    RETVAL

# -==[- Character printing -]==-----------------------------------------------

void
_set_color(fgcolor, bgcolor)
    unsigned int fgcolor;
    unsigned int bgcolor;
  CODE:
    caca_set_color(fgcolor, bgcolor);

unsigned int
_get_fg_color()
  CODE:
    RETVAL = caca_get_fg_color();
  OUTPUT:
    RETVAL

unsigned int
_get_bg_color()
  CODE:
    RETVAL = caca_get_bg_color();
  OUTPUT:
    RETVAL

const char *
_get_color_name(color)
    unsigned int color
  CODE:
    RETVAL = caca_get_color_name(color);
  OUTPUT:
    RETVAL

void
_putchar(x, y, c)
    int  x;
    int  y;
    char c;
  CODE:
    caca_putchar(x, y, c);

void
_putstr(x, y, s)
    int        x;
    int        y;
    const char *s;
  CODE:
    caca_putstr(x, y, s);

# skip caca_printf for now.
# handle va_args on perl side.

void
_clear()
  CODE:
    caca_clear();

# -==[- Primitives drawing -]==-----------------------------------------------

void
_draw_line(x1, y1, x2, y2, c)
    int x1;
    int y1;
    int x2;
    int y2;
    char c;
  CODE:
    caca_draw_line(x1, y1, x2, y2, c);

void
_draw_polyline(x, y, n, c)
    SV *x;
    SV *y;
    int n;
    char c;
  INIT:
    int *xc;
    int *yc;
    int i;
    /* make sure x and y are perl arrayrefs */
    if ( (SvTYPE(SvRV(x)) != SVt_PVAV)
      || (SvTYPE(SvRV(y)) != SVt_PVAV) )
    {
      XSRETURN_UNDEF;
    }

    /* create a C int array out of x and y */
    xc = (int *) malloc((n+1) * sizeof(int *));
    if (!xc) {
      XSRETURN_UNDEF;
    }
    yc = (int *) malloc((n+1) * sizeof(int *));
    if (!yc) {
      XSRETURN_UNDEF;
    }
    for (i = 0; i <= n; i++) {
      SV **integer;

      integer = av_fetch((AV *) SvRV(x), i, 0);
      if (integer) {
        xc[i] = SvIV(*integer);
      } else {
        xc[i] = 0;
      }

      integer = av_fetch((AV *) SvRV(y), i, 0);
      if (integer) {
        yc[i] = SvIV(*integer);
      } else {
        yc[i] = 0;
      }
    }
  CODE:
    caca_draw_polyline(xc, yc, n, c);
    free(yc);
    free(xc);

void
_draw_thin_line(x1, y1, x2, y2)
    int x1;
    int y1;
    int x2;
    int y2;
  CODE:
    caca_draw_thin_line(x1, y1, x2, y2);

void
_draw_thin_polyline(x, y, n)
    SV  *x;
    SV  *y;
    int n;
  INIT:
    int *xc;
    int *yc;
    int i;
    /* make sure x and y are perl arrayrefs */
    if ( (SvTYPE(SvRV(x)) != SVt_PVAV)
      || (SvTYPE(SvRV(y)) != SVt_PVAV) )
    {
      XSRETURN_UNDEF;
    }

    /* create a C int array out of x and y */
    xc = (int *) malloc((n+1) * sizeof(int *));
    if (!xc) {
      XSRETURN_UNDEF;
    }
    yc = (int *) malloc((n+1) * sizeof(int *));
    if (!yc) {
      XSRETURN_UNDEF;
    }
    for (i = 0; i <= n; i++) {
      SV **integer;

      integer = av_fetch((AV *) SvRV(x), i, 0);
      if (integer) {
        xc[i] = SvIV(*integer);
      } else {
        xc[i] = 0;
      }

      integer = av_fetch((AV *) SvRV(y), i, 0);
      if (integer) {
        yc[i] = SvIV(*integer);
      } else {
        yc[i] = 0;
      }
    }
  CODE:
    caca_draw_thin_polyline(xc, yc, n);
    free(yc);
    free(xc);

void
_draw_circle(x, y, r, c)
    int  x;
    int  y;
    int  r;
    char c;
  CODE:
    caca_draw_circle(x, y, r, c);

void
_draw_ellipse(x0, y0, a, b, c)
    int  x0;
    int  y0;
    int  a;
    int  b;
    char c;
  CODE:
    caca_draw_ellipse(x0, y0, a, b, c);

void
_draw_thin_ellipse(x0, y0, a, b)
    int x0;
    int y0;
    int a;
    int b;
  CODE:
    caca_draw_thin_ellipse(x0, y0, a, b);

void
_fill_ellipse(x0, y0, a, b, c)
    int  x0;
    int  y0;
    int  a;
    int  b;
    char c;
  CODE:
    caca_fill_ellipse(x0, y0, a, b, c);

void
_draw_box(x0, y0, x1, y1, c)
    int  x0;
    int  y0;
    int  x1;
    int  y1;
    char c;
  CODE:
    caca_draw_box(x0, y0, x1, y1, c);

void
_draw_thin_box(x0, y0, x1, y1)
    int x0;
    int y0;
    int x1;
    int y1;
  CODE:
    caca_thin_box(x0, y0, x1, y1);

void
_fill_box(x0, y0, x1, y1, c)
    int  x0;
    int  y0;
    int  x1;
    int  y1;
    char c;
  CODE:
    caca_fill_box(x0, y0, x1, y1, c);

void
_draw_triangle(x0, y0, x1, y1, x2, y2, c)
    int  x0;
    int  y0;
    int  x1;
    int  y1;
    int  x2;
    int  y2;
    char c;
  CODE:
    caca_draw_triangle(x0, y0, x1, y1, x2, y2, c);

void
_draw_thin_triangle(x0, y0, x1, y1, x2, y2)
    int x0;
    int y0;
    int x1;
    int y1;
    int x2;
    int y2;
  CODE:
    caca_draw_thin_triangle(x0, y0, x1, y1, x2, y2);

void
_fill_triangle(x0, y0, x1, y1, x2, y2, c)
    int  x0;
    int  y0;
    int  x1;
    int  y1;
    int  x2;
    int  y2;
    char c;
  CODE:
    caca_fill_triangle(x0, y0, x1, y1, x2, y2, c);

# -==[- Mathematical functions -]==-------------------------------------------

int
_rand(min, max)
    int min;
    int max;
  CODE:
    RETVAL = caca_rand(min, max);
  OUTPUT:
    RETVAL

unsigned int
_sqrt(n)
    unsigned int n;
  CODE:
    RETVAL = caca_sqrt(n);
  OUTPUT:
    RETVAL

# -==[- Sprite handling -]==-

SV *
_load_sprite(file)
    const char *file
  INIT:
    struct caca_sprite  *c_sprite;
    HV                  *sprite;
  CODE:
    if (!file) {
      XSRETURN_UNDEF;
    }
    c_sprite = caca_load_sprite(file);
    if (!c_sprite) {
      XSRETURN_UNDEF;
    } else {
      sprite = (HV *) sv_2mortal((SV *) newHV());
      if (!sprite) {
        XSRETURN_UNDEF;
      }
      hv_store(sprite, "__address__", 11, newSViv((int) c_sprite), 0);
      RETVAL = newRV((SV *) sprite);
    }
  OUTPUT:
    RETVAL

int
_get_sprite_frames(sprite)
    SV *sprite
  INIT:
    struct caca_sprite *c_sprite;
    c_sprite = address_of(sprite);
    if (!c_sprite) {
      XSRETURN_UNDEF;
    }
  CODE:
    RETVAL = caca_get_sprite_frames(c_sprite);
  OUTPUT:
    RETVAL

int
_get_sprite_width(sprite, f)
    SV  *sprite;
    int f;
  INIT:
    struct caca_sprite *c_sprite;
    c_sprite = address_of(sprite);
    if (!c_sprite) {
      XSRETURN_UNDEF;
    }
  CODE:
    RETVAL = caca_get_sprite_width(c_sprite, f);
  OUTPUT:
    RETVAL

int
_get_sprite_height(sprite, f)
    SV  *sprite;
    int f;
  INIT:
    struct caca_sprite *c_sprite;
    c_sprite = address_of(sprite);
    if (!c_sprite) {
      XSRETURN_UNDEF;
    }
  CODE:
    RETVAL = caca_get_sprite_height(c_sprite, f);
  OUTPUT:
    RETVAL

int
_get_sprite_dx(sprite, f)
    SV  *sprite;
    int f;
  INIT:
    struct caca_sprite *c_sprite;
    c_sprite = address_of(sprite);
    if (!c_sprite) {
      XSRETURN_UNDEF;
    }
  CODE:
    RETVAL = caca_get_sprite_dx(c_sprite, f);
  OUTPUT:
    RETVAL

int
_get_sprite_dy(sprite, f)
    SV  *sprite;
    int f;
  INIT:
    struct caca_sprite *c_sprite;
    c_sprite = address_of(sprite);
    if (!c_sprite) {
      XSRETURN_UNDEF;
    }
  CODE:
    RETVAL = caca_get_sprite_dy(c_sprite, f);
  OUTPUT:
    RETVAL

void
_draw_sprite(x, y, sprite, f)
    int x;
    int y;
    SV *sprite;
    int f;
  INIT:
    struct caca_sprite *c_sprite;
    c_sprite = address_of(sprite);
    if (!c_sprite) {
      XSRETURN_UNDEF;
    }
  CODE:
    caca_draw_sprite(x, y, c_sprite, f);

void
_free_sprite(sprite)
    SV *sprite;
  INIT:
    struct caca_sprite *c_sprite;
    c_sprite = address_of(sprite);
    if (!c_sprite) {
      XSRETURN_UNDEF;
    }
  CODE:
    caca_free_sprite(c_sprite);

# -==[- Bitmap handling -]==--------------------------------------------------

SV *
_create_bitmap(bpp, w, h, pitch, rmask, gmask, bmask, amask)
    unsigned int bpp;
    unsigned int w;
    unsigned int h;
    unsigned int pitch;
    unsigned int rmask;
    unsigned int gmask;
    unsigned int bmask;
    unsigned int amask;
  INIT:
    struct caca_bitmap *c_bitmap;
    HV                 *bitmap;
  CODE:
    c_bitmap =
      caca_create_bitmap(bpp, w, h, pitch, rmask, gmask, bmask, amask);
    if (!c_bitmap) {
      XSRETURN_UNDEF;
    } else {
      bitmap = (HV *) sv_2mortal((SV *) newHV());
      if (!bitmap) {
        XSRETURN_UNDEF;
      }
      hv_store(bitmap, "__address__", 11, newSViv((int) c_bitmap), 0);
      hv_store(bitmap, "__bpp__",      7, newSViv((int) bpp     ), 0);
      RETVAL = newRV((SV *) bitmap);
    }
  OUTPUT:
    RETVAL

void
_set_bitmap_palette(bitmap, red, green, blue, alpha)
    SV *bitmap;
    SV *red;
    SV *green;
    SV *blue;
    SV *alpha;
  INIT:
    struct caca_bitmap *c_bitmap;
    unsigned int *c_red;
    unsigned int *c_green;
    unsigned int *c_blue;
    unsigned int *c_alpha;

    c_bitmap = address_of(bitmap);
    if (!c_bitmap) {
      XSRETURN_UNDEF;
    }
    /* TODO: perl array to c array */
    c_red   = c_array(red);
    c_green = c_array(green);
    c_blue  = c_array(blue);
    c_alpha = c_array(alpha);
  CODE:
    caca_set_bitmap_palette(c_bitmap, c_red, c_green, c_blue, c_alpha);

void
_draw_bitmap(x1, y1, x2, y2, bitmap, pixels)
    int x1;
    int y1;
    int x2;
    int y2;
    SV *bitmap;
    SV *pixels;
  INIT:
    /* TODO: implement Tie::Scalar::Pointer for pixel support */
  CODE:

void
_free_bitmap(bitmap)
    SV *bitmap;
  INIT:
    struct caca_bitmap *c_bitmap;
    c_bitmap = address_of(bitmap);
    if (!c_bitmap) {
      XSRETURN_UNDEF;
    }
  CODE:
    caca_free_bitmap(c_bitmap);

# vim:sw=2 sts=2 expandtab
