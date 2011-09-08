#include "lr_image.h"


///////////////////////////////////////////////////////////////////////////////
/// 
int lr_abs_diff(int a, int b)
{
  int d = a - b + 127;
  if ( d < 0 || d > 255 ) printf("blah\n");
  return d;
}

///////////////////////////////////////////////////////////////////////////////
/// 
lr_image* lr_diff(lr_image* alpha, lr_image* beta)
{
  lr_image *output;
  int x, y;

  if ( alpha->width != beta->width || alpha->height != beta->height ) {
    return NULL;
  }

  output = lr_allocate_image(alpha->width, alpha->height);

  for ( y = 0; y < alpha->height; y++ ) {
    for ( x = 0; x < alpha->width; x++ ) {
      lr_pixel p_a = lr_get_pixel(alpha, x, y);
      lr_pixel p_b = lr_get_pixel(beta, x, y);
      lr_pixel p_o = lr_get_pixel(output, x, y);
      LR_RED(p_o) = lr_abs_diff(LR_RED(p_a), LR_RED(p_b));
      LR_GREEN(p_o) = lr_abs_diff(LR_GREEN(p_a), LR_GREEN(p_b));
      LR_BLUE(p_o) = lr_abs_diff(LR_BLUE(p_a), LR_BLUE(p_b));
    }
  }

  return output;
}

///////////////////////////////////////////////////////////////////////////////
/// 
lr_image* gen_pyramid(lr_image *img, int levels, int r)
{
  lr_image *scaled, *pyramid;

  /*

  scaled = lr_allocate_image(r, r);
  lr_resample(img, scaled);
  lr_desaturate(scaled);
  lr_image* pyramid = lr_allocate_image(r * levels, r * 3);
  lr_paste_atop(pyramid, scaled, 0, 0);
  lr_paste_atop(pyramid, scaled, 0, r);

  lr_image* last = scaled;
  for ( int i = 1; i < levels; i++ ) {
    scaled = lr_minify(scaled, 1);
    lr_image* upsampled = lr_maxify(scaled, 1);
    lr_image* diff = lr_diff(last, upsampled);
    upsampled = lr_maxify(upsampled, i - 1);
    lr_paste_atop(pyramid, scaled, r * i, 0);
    lr_paste_atop(pyramid, upsampled, r * i, r);
    lr_paste_atop(pyramid, diff, r * i, r << 1);
    last = scaled;
  }
  */
  return pyramid;
}
