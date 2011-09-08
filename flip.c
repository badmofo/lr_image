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

#include <stdio.h>
#include "lr_image.h"
#include <unistd.h>
#include <sys/stat.h>

void lr_swap_pixels(lr_image* img, int x1, int y1, int x2, int y2)
{
  char buffer[3];  
  memcpy(buffer, lr_get_pixel(img, x1, y1), 3);
  memcpy(lr_get_pixel(img, x1, y1), lr_get_pixel(img, x2, y2), 3);
  memcpy(lr_get_pixel(img, x2, y2), buffer, 3); 
}

lr_flip_image(lr_image* img)
{
  int x, y;
  char buffer[3];

  for ( y = 0; y < (img->height / 2); y++ ) {
    for ( x = 0; x < img->width; x++ ) {		     
      int xx = img->width - x - 1;
      int yy = img->height - y - 1;
      lr_swap_pixels(img, x, y, xx, yy);
    }
  }
  if ( img->height % 2 == 1 ) {
    y = (img->height / 2);
    for ( x = 0; x < (img->width / 2); x++ ) {
      int xx = img->width - x - 1;
      lr_swap_pixels(img, x, y, xx, y);
    }
  }
}

lr_flip_region(lr_image* img, int x, int y, int w, int h)
{
  int i, j, x1, y1, x2, y2;
  char buffer[3];

  for ( i = 0; i < (h / 2); i++ ) {
    for ( j = 0; j < w; j++ ) {
      x1 = x + j;
      y1 = y + i;
      x2 = x + (w - j) - 1;
      y2 = y + (h - i) - 1;
      lr_swap_pixels(img, x1, y1, x2, y2);
    }
  }
  
  if ( h % 2 ) {
    i = h / 2;
    y = y + i;
    for ( j = 0; j < (w / 2); j++ ) {
      x1 = x + j;
      x2 = x + (w - j) - 1;
      lr_swap_pixels(img, x1, y, x2, y);
    }
  }
}

void lr_fix_montage(lr_image* img)
{
  int i, j, x, y;
  for (  i = 0; i < 3; i++ ) {
    for (  j = 0; j < 3; j++ ) {
      int w, h;
      w = (img->width - 4 * 3) / 3;
      h = (img->height - 4 * 3) / 3;
      x = w * i + (i + 1) * 3;
      y = h * j + (j + 1) * 3;
      printf("%d %d %d %d\n", x, y, w, h);
      lr_flip_region(img, x, y, w, h);
    }
  }
}

int main(int argc, char* argv[])
{
  FILE *fp;
  int len;
  lr_image *original;
  
  original = lr_read_image(argv[1]);
  // lr_flip_image(original);
  lr_save_image_bmp(original, argv[2]);

  return 0;
}
