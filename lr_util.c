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
#include <stdio.h>
#include <math.h>
#include <sys/stat.h>

///////////////////////////////////////////////////////////////////////////////
/// 
int lr_get_filesize(const char* filename)
{
  struct stat file_info;
  stat(filename, &file_info);
  return file_info.st_size;
}

///////////////////////////////////////////////////////////////////////////////
/// 
int lr_clamp(int value, int min, int max)
{
  if ( value > max ) return max;
  if ( value < min ) return min;
  return value;
}

///////////////////////////////////////////////////////////////////////////////
/// 
float lr_min(float a, float b)
{
  return ( a < b ) ? a : b;
}

///////////////////////////////////////////////////////////////////////////////
/// 
float lr_max(float a, float b)
{
  return ( a > b ) ? a : b;
}

///////////////////////////////////////////////////////////////////////////////
/// 
void lr_put_char(int c, FILE *fp)
{
  fputc(c, fp);
}

///////////////////////////////////////////////////////////////////////////////
/// 
void lr_put_short(unsigned short s, FILE *fp)
{
  fputc((char)((s >> 0) & 0xff), fp);
  fputc((char)((s >> 8) & 0xff), fp);
}

///////////////////////////////////////////////////////////////////////////////
/// 
void lr_put_long(unsigned long l, FILE *fp)
{
  fputc((char)((l >>  0) & 0xff), fp);
  fputc((char)((l >>  8) & 0xff), fp);
  fputc((char)((l >> 16) & 0xff), fp);
  fputc((char)((l >> 24) & 0xff), fp);
}
