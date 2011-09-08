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
#include "lr_letters.h"
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
/// 
static int* get_letter(int c)
{
  if ( c >= 32 && c <= 126 ) {
    return letters[c - 32];
  } else if ( c >= 161 && c <= 255 ) {
    return letters[c - 66];
  } else {
    return letters[0];
  }
}

///////////////////////////////////////////////////////////////////////////////
/// 
void lr_print_letter(int c, lr_image *img, int x, int y)
{
  int i, j;
  int* buffer;
  buffer = get_letter(c);
  for ( i = 0; i < 10; i++ ) {
    for ( j = 0; j < 6; j++ ) {
      if ( buffer[i] & (1 << j) ) {
	if ( (x + j) < img->width && (y + i) < img->height ) {
	  lr_set_pixel(img, x + j, y + i, 0, 0, 0);
	}
      }
    }
  } 
}

///////////////////////////////////////////////////////////////////////////////
/// 
void lr_print_text(const char* str, lr_image *img, int x, int y)
{
  int i;
  for ( i = 0; i < strlen(str); i++ ) {
    lr_print_letter(str[i], img, x + 6 * i, y);
  }
}
