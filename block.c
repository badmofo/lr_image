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
#include <stdlib.h>
#include "lr_image.h"

#define THUMBNAIL_WIDTH  100
#define THUMBNAIL_HEIGHT 100
#define BLOCK_ACROSS     6
#define BLOCK_DOWN       3
#define BLOCK_PADDING    2

int main(int argc, char *argv[])
{
  int across, down;
  int width, height;
  int num_thumbnails;
  lr_image *block;
  int i, j, n;

  if ( argc < 3 ) {
    printf("usage: block <output-jpeg> <input-gif-1> ... <input-gif-n>\n");
    exit(1);
  }

  num_thumbnails = argc - 2;
  down = (num_thumbnails + BLOCK_ACROSS - 1) / BLOCK_ACROSS;
  if ( num_thumbnails < BLOCK_ACROSS ) {
    across = num_thumbnails;
  } else {
    across = BLOCK_ACROSS;
  }

  width = across * THUMBNAIL_WIDTH + (across - 1) * BLOCK_PADDING;
  height = down * THUMBNAIL_HEIGHT + (down - 1) * BLOCK_PADDING;
  block = lr_allocate_image(width, height);
  memset(block->buffer, 0xff, width * height * 3);
  
  n = 0;
  for ( i = 0; i < BLOCK_DOWN && n < num_thumbnails; i++ ) {
    for ( j = 0; j < BLOCK_ACROSS && n < num_thumbnails; j++, n++ ) {
      FILE *fp = NULL; 
      lr_image *thumbnail = lr_read_image(argv[2 + n]);
      if ( thumbnail == NULL ) {
	continue;
      }

      lr_paste_atop(block, thumbnail, j * 102, i * 102);
      lr_free_image(thumbnail);
    }
  }
  
  if ( lr_save_image_jpeg(block, argv[1]) != LR_OK ) {
    printf("error writing \"%s\"\n", argv[1]);
    exit(1);
  }

  return 0;
}
