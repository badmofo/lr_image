/*
 * Copyright (c) 2002.  Lucas Ryan.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name Lucas Ryan nor the names of contributors may be 
 *    used to endorse or promote products derived from this software 
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>
#include <png.h>
#include "lr_image.h"

///////////////////////////////////////////////////////////////////////////////
/// 
lr_image* lr_allocate_image(int width, int height)
{
  int buffer_size = width * height * 3;
  lr_image *img = (lr_image*) malloc(sizeof(lr_image));
  if ( img == NULL ) {
    return  NULL;
  }
  memset(img, 0, sizeof(lr_image));
  img->width = width;
  img->height = height;
  img->buffer = (lr_pixel) malloc(buffer_size);  
  return img;
}

///////////////////////////////////////////////////////////////////////////////
/// 
void lr_free_image(lr_image* img)
{
  free(img->buffer);
  free(img);
}

///////////////////////////////////////////////////////////////////////////////
/// 
lr_pixel lr_get_pixel(lr_image* img, int x, int y)
{
  return img->buffer + (y * img->width + x) * 3;
}

///////////////////////////////////////////////////////////////////////////////
/// 
void lr_set_pixel(lr_image* img, int x, int y, int r, int g, int b)
{
  lr_pixel p = lr_get_pixel(img, x, y);
  LR_RED(p) = r;
  LR_GREEN(p) = g;
  LR_BLUE(p) = b;
}

///////////////////////////////////////////////////////////////////////////////
/// 
lr_image* lr_read_image_raw(const char* filename, int width, int height)
{
  int bytes_read = 0;
  lr_image* img;
  FILE* f = fopen(filename, "rb");
  if ( f == 0 ) {
    return NULL;
  }
  img = lr_allocate_image(width, height);
  bytes_read = fread(img->buffer, 1, width * height * 3, f);
  fclose(f);
  if ( bytes_read != width * height * 3 ) {
    lr_free_image(img);
    return NULL;
  }
  
  return img;
}

///////////////////////////////////////////////////////////////////////////////
/// 
LR_STATUS lr_save_image_raw(lr_image* img, const char* filename)
{
  FILE* f = fopen(filename, "wb");
  if ( f == 0 ) {
    return LR_FAIL;
  }
  fwrite(img->buffer, 1, img->width * img->height * 3, f);
  fclose(f);
  return LR_OK;
}

#ifndef _MSC_VER

/*
static float floorf(float x)
{
  return (float)((int)(x));
}
*/

#endif


///////////////////////////////////////////////////////////////////////////////
/// This is stolen from GD ... for the time being ... the version I had was
/// buggy ... plan to rewrite soon.
///
lr_image* lr_resample(lr_image* src, int width, int height)
{
  lr_image* dst;
  int dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH;  
  int x, y;

  dst = lr_allocate_image(width, height);

  dstX = dstY = srcX = srcY = 0;
  dstW = dst->width;
  dstH = dst->height;
  srcW = src->width;
  srcH = src->height;

  for (y = dstY; (y < dstY + dstH); y++) {
    for (x = dstX; (x < dstX + dstW); x++) {
      float sy1, sy2, sx1, sx2;
      float sx, sy;
      float spixels = 0;
      float red = 0.0, green = 0.0, blue = 0.0, alpha = 0.0;
      sy1 = ((float) y - (float) dstY) * (float) srcH / (float) dstH;
      sy2 = ((float) (y + 1) - (float) dstY) * (float) srcH /
	(float) dstH;
      sy = sy1;
      do {
	float yportion;
	if (floor (sy) == floor (sy1)) {
	  yportion = 1.0 - (sy - floor (sy));
	  if (yportion > sy2 - sy1) {
	    yportion = sy2 - sy1;
	  }
	  sy = floor (sy);
	} else if (sy == floor (sy2)) {
	  yportion = sy2 - floor (sy2);
	} else {
	  yportion = 1.0;
	}
	sx1 = ((float) x - (float) dstX) * (float) srcW / dstW;
	sx2 = ((float) (x + 1) - (float) dstX) * (float) srcW / dstW;
	sx = sx1;
	do {
	  float xportion;
	  float pcontribution;
	  lr_pixel p;
	  
	  if (floor (sx) == floor (sx1)) {
	    xportion = 1.0 - (sx - floor (sx));
	    if (xportion > sx2 - sx1) {
	      xportion = sx2 - sx1;
	    }
	    sx = floor (sx);
	  } else if (sx == floor (sx2)) {
	    xportion = sx2 - floor (sx2);
	  } else {
	    xportion = 1.0;
	  }
	  pcontribution = xportion * yportion;
	  /* 2.08: previously srcX and srcY were ignored. 
	     Andrew Pattison */
	  p = lr_get_pixel(src, (int) sx + srcX, sy + srcY);
	  red += LR_RED(p) * pcontribution;
	  green += LR_GREEN(p) * pcontribution;
	  blue += LR_BLUE(p) * pcontribution;
	  spixels += xportion * yportion;
	  sx += 1.0;
	}
	while (sx < sx2);
	sy += 1.0;
      }
      while (sy < sy2);
      if (spixels != 0.0) {
	red /= spixels;
	green /= spixels;
	blue /= spixels;
      }
      /* Clamping to allow for rounding errors above */
      if (red > 255.0) {
	red = 255.0;
      }
      if (green > 255.0) {
	green = 255.0;
      }
      if (blue > 255.0) {
	blue = 255.0;
      }
      lr_set_pixel(dst, x, y,
		   (int) red,
		   (int) green,
		   (int) blue);
    }
  }

  return dst;
}


///////////////////////////////////////////////////////////////////////////////
// 
lr_image* lr_minify(lr_image* src, int factor)
{
  int x, y, i, j, xx, yy;
  int r, g, b;
  int pixels = 1 << factor;
  lr_image* dst = lr_allocate_image(src->width >> factor, src->height >> factor);
  
  for ( y = 0; y < dst->height; y++ ) {
    for ( x = 0; x < dst->width; x++ ) {
      r = g = b = 0;
      xx = x << factor;
      yy = y << factor;
      for ( i = 0; i < pixels; i++ ) {
	for ( j = 0; j < pixels; j++ ) {
	  lr_pixel p = lr_get_pixel(src, xx + j, yy + i);
	  r += LR_RED(p);
	  g += LR_GREEN(p);
	  b += LR_BLUE(p);
	}
      }

      r = lr_min(255, r >> (factor << 1));
      g = lr_min(255, g >> (factor << 1));
      b = lr_min(255, b >> (factor << 1));
      lr_set_pixel(dst, x, y, r, g, b);      
    }
  }

  return dst;
}

///////////////////////////////////////////////////////////////////////////////
// 
lr_image* lr_maxify(lr_image* src, int factor)
{
  int x, y, i, j, xx, yy;

  lr_image* dst = lr_allocate_image(src->width << factor, 
				    src->height << factor);

  int b = 1 << factor;

  for ( y = 0; y < src->height; y++ ) {
    for ( x = 0; x < src->width; x++ ) {
      lr_pixel p1 = lr_get_pixel(src, x, y);
      xx = x << factor;
      yy = y << factor;
      for ( i = 0; i < b; i++ ) {
	for ( j = 0; j < b; j++ ) {
	  lr_pixel p2 = lr_get_pixel(dst, xx + j, yy + i);
	  memcpy(p2, p1, 3);
	}
      }
    }
  }

  return dst;
}

///////////////////////////////////////////////////////////////////////////////
/// 
void lr_nn_resample(lr_image* src, lr_image* dst)
{
  int x, y;
  float yy, xx;
  float width_scale, height_scale;

  width_scale = (float) src->width / (float) dst->width;
  height_scale = (float) src->height / (float) dst->height;

  for ( y = 0; y < dst->height; y++ ) {
    for ( x = 0; x < dst->width; x++ ) {

      lr_pixel p;

      xx = x * width_scale;
      yy = y * height_scale;

      p  = lr_get_pixel(src, xx, yy);            
      lr_set_pixel(dst, x, y, LR_RED(p), LR_GREEN(p), LR_BLUE(p));
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
/// 
lr_image* lr_crop(lr_image* img, int left, int top, int right, int bottom)
{
  int x, y;
  lr_image* cropped = lr_allocate_image(right - left, bottom - top);
  
  for ( y = top; y < bottom; y++ ) {
    for ( x = left; x < right; x++ ) {
      memcpy(lr_get_pixel(cropped, x - left, y - top),
	     lr_get_pixel(img, x, y), 
	     3);
    }
  }
  
  return cropped;
}

///////////////////////////////////////////////////////////////////////////////
/// 
void lr_paste_atop(lr_image* bottom, lr_image* top, int x, int y)
{
  int i, j;
  for ( i = 0; i < top->height; i++ ) {
    for ( j = 0; j < top->width; j++ ) {
      lr_pixel p;
      p  = lr_get_pixel(top, j, i);
      lr_set_pixel(bottom, x + j, y + i, LR_RED(p), LR_GREEN(p), LR_BLUE(p));
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
/// 
void lr_scale_dimensions(lr_dimensions *original,
			 lr_dimensions *bounds,
			 lr_dimensions *scaled)
{
  *scaled = *original;
  if ( original->width > bounds->width ) {
    scaled->width = bounds->width;
    scaled->height = (original->height * bounds->width) / original->width;
  } 
  if ( scaled->height > bounds->height ) {
    scaled->height = bounds->height;
    scaled->width = (original->width * bounds->height) / original->height;
  }
}

///////////////////////////////////////////////////////////////////////////////
/// 
lr_image *lr_montage(lr_image** images, 
		     int across, int down, 
		     int border, 
		     lr_dimensions *bounds)
{
  int i;
  lr_image *montage, *thumb;
  int thumb_width = (bounds->width - (across + 1) * border) / across;
  int thumb_height = (bounds->height - (down + 1) * border) / down;

  montage = lr_allocate_image(thumb_width * across + (across + 1) * border,
			      thumb_height * down + (down + 1) * border);
  
  memset(montage->buffer, 0, montage->width * montage->height * 3);

  for ( i = 0; i < (across * down); i++ ) {
    if ( images[i] ) {
      thumb = lr_resample(images[i], thumb_width, thumb_height);
      lr_paste_atop(montage, thumb, 
		    (i % across) * thumb_width + ((i % across) + 1) * border, 
		    (i / across) * thumb_height + ((i / across) + 1) * border);
      lr_free_image(thumb);
    }
  }

  return montage;
}

///////////////////////////////////////////////////////////////////////////////
/// 
void lr_reverse_image(lr_pixel pixels, int width, int height)
{
  int i = 0;
  int offset = 0;
  int buffer_length = width * height * 3;
  lr_pixel line;

  for ( i = 0; i < buffer_length / 2; i++ ) {
    unsigned char tmp = pixels[i];
    pixels[i] = pixels[buffer_length - 1 - i];
    pixels[buffer_length - 1 - i] = tmp;
  }
  
  line = pixels;
  for ( i = 0; i < height; i++ ) {
    int j;
    for ( j = 0; j < width / 2; j++ ) {
      int tmp;
      lr_pixel left, right;
      left = line + ((width - 1 - j) * 3);
      right = line + (j * 3);
      memcpy(&tmp, right, 3);
      memcpy(right, left, 3);
      memcpy(left, &tmp, 3);
    }
    line += (width * 3);
  }
}

int lr_get_image_type(const char *filepath)
{
  int bytes_read = 0;
  unsigned char header[16];
  FILE *fp = fopen(filepath, "rb");
  if ( !fp ) {
    return LR_FORMAT_UNKNOWN;
  }
  memset(header, 0, 16);
  bytes_read = fread(header, 1, 16, fp);
  fclose(fp);

  if ( header[0] == 0xff && header[1] == 0xd8 ) {
    return LR_FORMAT_JPEG;
  } else if ( header[0] == 0x47 && header[1] == 0x49 ) {
    return LR_FORMAT_GIF;
  } else if ( png_sig_cmp(header, 0, 16) == 0 ) {
    return LR_FORMAT_PNG;
  }

  return LR_FORMAT_UNKNOWN;
}


lr_image *lr_read_image(const char *filepath)
{
  int format = lr_get_image_type(filepath);
  if ( format == LR_FORMAT_JPEG ) {
    return lr_read_image_jpeg(filepath);
  } else if ( format == LR_FORMAT_GIF ) {
    return lr_read_image_gif(filepath);
  } else if ( format == LR_FORMAT_PNG ) {
    return lr_read_image_png(filepath); 
  } else {
      printf("unknown format\n");
  }
  return NULL;
}
