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
//#include <unistd.h>
#include <sys/stat.h>

int LR_FILTER_EDGE_DETECT3[10]  = {-1,-1,-1,-1,8,-1,-1,-1,-1, 1};
int LR_FILTER_EDGE_DETECT4[10]  = {0,-1,0,-1,4,-1,0,-1,0, 1};

int LR_5X5_GAUSSIAN_BLUE[25] = {0, 1, 2, 1, 0, 1, 4, 8, 4, 1, 2, 8, 16, 8, 2, 1, 4, 8, 4, 1, 0, 1, 2, 1, 0};

void lr_draw_rect(lr_image *img, int x, int y, int width, int height)
{
  int i, j;
  int z = 50;
  for ( i = y; i < (y + height); i++ ) {
    for ( j = x; j < (x + width); j++ ) {
      lr_set_pixel(img, j, i, z,z,z);
    }
  }
}

void lr_darken(lr_image *img, int x, int y, int param)
{
  lr_pixel p = lr_get_pixel(img, x, y);
  lr_set_pixel(img, x, y, LR_RED(p) / param, LR_GREEN(p) / param, LR_BLUE(p) / param);
}

void lr_darken_edge(lr_image *img, int param)
{
  int x, y;
  x = 0;
  for ( y = 0; y < img->height; y++ ) {
    lr_darken(img, x, y, param);
  }
  x = img->width - 1;
  for ( y = 0; y < img->height; y++ ) {
    lr_darken(img, x, y, param);
  }
  y = 0;
  for ( x = 1; x < img->width - 1; x++ ) {
    lr_darken(img, x, y, param);
  }
  y = img->height - 1;
  for ( x = 1; x < img->width - 1; x++ ) {
    lr_darken(img, x, y, param);
  }
}


int main(int argc, char* argv[])
{
  int border = 8;
  FILE *fp;
  int len;
  lr_image *scaled, *img, *thumbnail, *canvas;
  lr_dimensions scaled_dim, original_dim, bounds_dim, unscaled_dim;
  char details[25];
  int format;
  
  /*
  if ( argc != 4 ) {
    printf("\nthis program makes thumbnails.\n\n");
    printf("usage: thumb <input-jpeg-or-gif> <output-gif> <annotation>\n\n");
    exit(1);
  }
  */

  
  img = lr_read_image(argv[1]);
  if ( img == NULL ) {
    printf("\"%s\" not found or not recognized\n", argv[1]);
    exit(1);
  }

  canvas = lr_allocate_image(200, 200);

  bounds_dim.width = 125;
  bounds_dim.height = 125;
  unscaled_dim.width = img->width;
  unscaled_dim.height = img->height;

  lr_scale_dimensions(&unscaled_dim, &bounds_dim, &scaled_dim);
  scaled = lr_resample(img, scaled_dim.width, scaled_dim.height);

  lr_darken_edge(scaled, 2);

  thumbnail = lr_allocate_image(scaled->width + border * 2, scaled->height + /* 9  + */ border * 2);
  memset(thumbnail->buffer, 0xff, thumbnail->width * thumbnail->height * 3);
  lr_darken_edge(thumbnail, 2);
  lr_paste_atop(thumbnail, scaled, border, border);

  // lr_print_text(argv[3], thumbnail, border, scaled->height + border + 1);

		//(100 - strlen(argv[3]) * 6)/2, 81);

  memset(canvas->buffer, 0xCF, canvas->width * canvas->height * 3);
  lr_draw_rect(canvas, 10, 10, thumbnail->width, thumbnail->height);


  canvas = lr_convolve(LR_5X5_GAUSSIAN_BLUE, 5, canvas);
  canvas = lr_convolve(LR_5X5_GAUSSIAN_BLUE, 5, canvas);
  canvas = lr_convolve(LR_5X5_GAUSSIAN_BLUE, 5, canvas);
  //canvas = lr_apply_mask(LR_FILTER_BLUR/*, 3*/, canvas);

  lr_paste_atop(canvas, thumbnail, 5, 5);

  /*
  lr_print_text(argv[3], thumbnail, (100 - strlen(argv[3]) * 6)/2, 81);

  sprintf(details, "%dkb %dx%d", 
	  lr_get_filesize(argv[1]) / 1024,
	  original_dim.width, original_dim.height);
	  
  lr_print_text(details, thumbnail, (100 - strlen(details) * 6)/2, 91);
  */

  img = lr_crop(canvas, 1, 1, 5 * 2 + 3 + thumbnail->width , thumbnail->height + 5 * 2  + 3);

  lr_save_image_jpeg_ex(img, argv[2], 99);

  
  return 0;
}

