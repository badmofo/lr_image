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

#include "lr_image.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

///////////////////////////////////////////////////////////////////////////////
/// 
lr_image *lr_allocate_image_8bit(int width, int height)
{
  int buffer_size = width * height;
  lr_image *img
    = (lr_image*) malloc(sizeof(lr_image));
  if ( img == NULL ) {
    printf("allocation error");
    exit(1);
  }
  memset(img, 0, sizeof(lr_image));
  img->width = width;
  img->height = height;
  img->buffer = (lr_pixel) malloc(buffer_size);  
  img->palette = (lr_pixel) malloc(256 * 3);
  memset(img->palette, 0, 256 * 3);
  return img;
}

///////////////////////////////////////////////////////////////////////////////
/// 
lr_set_pixel_8bit(lr_image* img, int x, int y, unsigned char color_index)
{
  *(img->buffer + (img->width * y + x)) = color_index;
}

///////////////////////////////////////////////////////////////////////////////
/// 
unsigned char* GetWuPicture();
unsigned char* GetWuPalette();

///////////////////////////////////////////////////////////////////////////////
/// 
lr_image *lr_wu_quantize_8bit(lr_image* img)
{
  int x, y;
  lr_image *quantized;
  quantized = lr_allocate_image_8bit(img->width, img->height);
  for ( x = 0; x < img->width; x++ ) {
    for ( y = 0; y < img->height; y++ ) {
      lr_pixel p = lr_get_pixel(img, x, y);
      lr_set_pixel(img, x, y, LR_BLUE(p), LR_GREEN(p), LR_RED(p));
    }
  }

  wuReduce(img->buffer, 256, img->width * img->height, quantized->buffer);
  //  memcpy(quantized->buffer, GetWuPicture(), img->width * img->height);

  memcpy(quantized->palette, GetWuPalette(), 3 * 256);
  return quantized;
}

