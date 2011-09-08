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

///////////////////////////////////////////////////////////////////////////////
/// 
LR_STATUS lr_save_image_bmp(lr_image* img, const char* filename)
{
  int x, y;
  FILE* fp = fopen(filename, "wb");
  if ( fp == 0 ) {
    return LR_FAIL;
  }

  lr_put_char('B', fp);
  lr_put_char('M', fp);
  lr_put_long(54 + 3 * (img->width + 1) * img->height, fp);
  lr_put_short(0, fp);
  lr_put_short(0, fp);

  lr_put_long(54, fp);          /* HEADERSIZE */
  lr_put_long(40, fp);          /* INFOHEADERSIZE */ 
  lr_put_long(img->width, fp);  /* WIDTH */
  lr_put_long(img->height, fp); /* HEIGHT */
  lr_put_short(1, fp);          /* PLANECOUNT */
  lr_put_short(24, fp);         /* BITCOUNT */
  lr_put_long(0, fp);           /* COMPRESSION */
  lr_put_long(3 * img->width * img->height, fp); /* IMAGESIZE */
  lr_put_long(2952, fp);        /* RESOLUTION */
  lr_put_long(2952, fp);        /* RESOLUTION */
  lr_put_long(0, fp);           /* COLORTABLESIZE */
  lr_put_long(0, fp);           /* IMPORTANTCOLOR */

  for ( y = img->height - 1; y > 0; y-- ) {
    for ( x = 0; x < img->width; x++ ) {
      lr_pixel p = lr_get_pixel(img, x, y);
      lr_put_char(LR_BLUE(p), fp);
      lr_put_char(LR_GREEN(p), fp);
      lr_put_char(LR_RED(p), fp);
    }
  }
  fwrite(img->buffer, 1, img->width * img->height * 3, fp);
  fclose(fp);

  return LR_OK;
}
