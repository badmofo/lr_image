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
#include <string.h>
#include <stdlib.h>
#include "lr_image.h"
#include <unistd.h>
#include <sys/stat.h>
#include <popt.h>

static float floorf(float x)
{
  return (float)((int)(x));
}

static float ceilf(float x)
{
  float floor_x = floorf(x);
  if ( (x - floor_x) >= 0.5 ) {
    return floor_x + 1.0;
  } else {
    return floor_x;
  }
}

void elipsify(char* destination, int destination_len, const char *source)
{
  int source_len = strlen(source);
  
  if ( source_len <= (destination_len - 1) ) {
    memcpy(destination, source, source_len + 1); 
  } else {
    int right, left;
    left = floorf((destination_len - 4.0) / 2.0);
    right = ceilf((destination_len - 4.0) / 2.0);
    memset(destination, 0, destination_len);
    memcpy(destination, source, left);
    memcpy(destination + left, "...", 3);
    memcpy(destination + left + 3, source + strlen(source) - right, right);
  }
}

void write_thumbnail(lr_image *img, 
		     char *annotation,
		     lr_dimensions *original_dim, 
		     int original_filesize,
		     int width, int height, 
		     char *outfile)
{
  lr_image *scaled, *thumbnail;
  lr_dimensions scaled_dim, bounds_dim, unscaled_dim;
  char details[25];
  char elipsified_annotation[1024];
  int annotation_height = 0;

  bounds_dim.width = width;
  bounds_dim.height = height;

  if ( annotation ) {
    annotation_height = 20;
    bounds_dim.height -= annotation_height;
  }

  unscaled_dim.width = img->width;
  unscaled_dim.height = img->height;

  lr_scale_dimensions(&unscaled_dim, &bounds_dim, &scaled_dim);
  scaled = lr_resample(img, scaled_dim.width, scaled_dim.height);
  thumbnail = lr_allocate_image(width, height);
  memset(thumbnail->buffer, 0xff, width * height * 3);
  lr_paste_atop(thumbnail, scaled, (width - scaled->width) / 2,
		((height - annotation_height) - scaled->height) / 2);

  if ( annotation ) {
    elipsify(elipsified_annotation, (width - 4)/6 + 1, annotation);

    lr_print_text(elipsified_annotation, 
		  thumbnail, 
		  (width - strlen(elipsified_annotation) * 6)/2, 
		  (height - annotation_height + 1));
    
    sprintf(details, "%dkb %dx%d", 
	    original_filesize / 1024,
	    original_dim->width, original_dim->height);

    lr_print_text(details, thumbnail, (width - strlen(details) * 6)/2, (height - (annotation_height/2) + 1));
  }
 
  lr_save_image_gif(thumbnail, outfile);

  lr_free_image(scaled);
  lr_free_image(thumbnail);
}

int main(int argc, char* argv[])
{
  FILE *fp;
  lr_image *img;
  lr_dimensions original_dim;
  int format, filesize;
  char *input_str = NULL, *annotation_str = NULL, *bigthumb_str = NULL, *smallthumb_str = NULL;
  
  struct poptOption optionsTable[] = {
    { "input", 'i', POPT_ARG_STRING, &input_str, 0,
      "jpeg or gif input file", },
    { "annotation", 'a', POPT_ARG_STRING, &annotation_str, 0,
      "thumbnail annotation" },
    { "bigthumb", 'b', POPT_ARG_STRING, &bigthumb_str, 0,
      "200x200 GIF thumbnail output file" },
    { "smallthumb", 's', POPT_ARG_STRING, &smallthumb_str, 0,
      "100x100 GIF thumbnail output file" },
    POPT_AUTOHELP
    { NULL, 0, 0, NULL, 0 }
  };

  poptContext optCon = poptGetContext(NULL, argc, (const char**)argv, optionsTable, 0);
  while ( poptGetNextOpt(optCon) > 0 ) ;

  if ( !input_str || (!smallthumb_str && !bigthumb_str) ) {
    poptPrintHelp(optCon, stderr, 0);
    exit(1);
  }
  
  img = NULL;
  format = lr_get_image_type(input_str);
  if ( format == LR_FORMAT_JPEG ) {
    img = lr_read_image_jpeg_scaled(input_str, &original_dim);
  } else if ( format == LR_FORMAT_GIF ) {
    img = lr_read_image_gif(input_str);
    original_dim.width = img->width;
    original_dim.height = img->height;
  } 

  if ( img == NULL ) {
    printf("\"%s\" not found or not recognized\n", input_str);
    exit(1);
  }

  filesize = lr_get_filesize(input_str);

  if ( smallthumb_str ) {
    write_thumbnail(img, annotation_str, &original_dim, filesize, 100, 100, smallthumb_str);
  }
  if ( bigthumb_str ) {
    write_thumbnail(img, annotation_str, &original_dim, filesize, 200, 200, bigthumb_str);
  }

  lr_free_image(img);

  return 0;
}

