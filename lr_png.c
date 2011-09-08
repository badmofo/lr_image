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
#include <string.h>
#include <png.h>
#include "lr_image.h"


static void
png_cexcept_error (png_structp png_ptr, png_const_charp msg)
{
  fprintf (stderr, "libpng error: %s\n", msg);
}




lr_image* lr_read_image_png(const char *filename)
{
    lr_image *img;

    char *data;
    size_t i;
    FILE *file;
#define PNG_SIG_SIZE 8
    unsigned char png_sig[PNG_SIG_SIZE];
    int sig_bytes;
    png_struct *png;
    png_info *info;
    png_uint_32 png_width, png_height;
    int depth, color_type, interlace;
    unsigned int pixel_size;
    png_byte **row_pointers;

    file = fopen (filename, "rb");
    if (file == NULL) {
        printf("can't open file\n");
        return NULL;
    }

    sig_bytes = fread (png_sig, 1, PNG_SIG_SIZE, file);
    if (png_check_sig (png_sig, sig_bytes) == 0) {
        printf("not a png\n");
        fclose (file);
        return NULL;
    }

    png = png_create_read_struct (PNG_LIBPNG_VER_STRING,
                                  NULL,
                                  NULL,
                                  NULL);
    info = png_create_info_struct (png);
    png_init_io (png, file);
    png_set_sig_bytes (png, sig_bytes);
    png_read_info (png, info);
    png_get_IHDR (png, info,
                  &png_width, &png_height, &depth,
                  &color_type, &interlace, NULL, NULL);

    /* convert palette/gray image to rgb */
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb (png);

    /* expand gray bit depth if needed */
    if (color_type == PNG_COLOR_TYPE_GRAY && depth < 8)
        png_set_gray_1_2_4_to_8 (png);
    /* transform transparency to alpha */
    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha (png);

    if (depth == 16)
        png_set_strip_16 (png);

    if (depth < 8)
        png_set_packing (png);

    /* convert grayscale to RGB */
    if (color_type == PNG_COLOR_TYPE_GRAY
        || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb (png);

    if (interlace != PNG_INTERLACE_NONE)
        png_set_interlace_handling (png);

    png_set_bgr (png);
    png_set_filler (png, 0xff, PNG_FILLER_AFTER);

    // png_set_read_user_transform_fn (png, premultiply_data);

    png_read_update_info (png, info);

    pixel_size = 4;
    data = malloc (png_width * png_height * pixel_size);
    img = lr_allocate_image(png->width, png->height);

    row_pointers = malloc (png_height * sizeof(char *));
    for (i=0; i < png_height; i++) 
        row_pointers[i] = (png_byte *) (*data + i * png_width * pixel_size);

    png_read_image (png, row_pointers);
    png_read_end (png, info);

    for (i=0; i < png_height; i++) {
        int j;
        for (j=0;j < png_width; j++) {
            memcpy(img->buffer + (i*img->width + j) * 3, row_pointers[i]+j*4, 3);
        }
    }

    free (row_pointers);
    fclose (file);
    

    png_destroy_read_struct (&png, &info, NULL);
    free(data);
    free(row_pointers);

    return img;
}



LR_STATUS
lr_save_image_png (lr_image * image, const char *filename)
{
  const int ciBitDepth = 8;
  const int ciChannels = 3;
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  static FILE *pfFile;
  png_uint_32 ulRowBytes;
  static png_byte **ppbRowPointers = NULL;
  int i;

  // open the PNG output file

  if ( !filename ) {
    return LR_FAIL;
  }
  
  if (!(pfFile = fopen (filename, "wb")))
    return LR_FAIL;

  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL,
				     (png_error_ptr) png_cexcept_error,
				     (png_error_ptr) NULL);
  if (!png_ptr) {
    fclose (pfFile);
    return LR_FAIL;
  }
  
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    fclose(pfFile);
    png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
    return LR_FAIL;
  }

  png_init_io(png_ptr, pfFile);

  // we're going to write a very simple 3x8 bit RGB image

  png_set_IHDR (png_ptr, info_ptr, image->width, image->height, ciBitDepth,
		PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  // write the file header information

  png_write_info (png_ptr, info_ptr);


  // row_bytes is the width x number of channels

  ulRowBytes = image->width * ciChannels;

  // we can allocate memory for an array of row-pointers

  if ((ppbRowPointers =
       (png_bytepp) malloc (image->height * sizeof (png_bytep))) == NULL)
    {
      fprintf (stderr, "out of memory");
    }

  // set the individual row-pointers to point at the correct offsets

  for (i = 0; i < image->height; i++) {
    ppbRowPointers[i] = image->buffer + i * ulRowBytes;
  }

  // write out the entire image data in one call

  png_write_image (png_ptr, ppbRowPointers);

  // write the additional chunks to the PNG file (not really needed)

  png_write_end (png_ptr, info_ptr);

  // and we're done

  free (ppbRowPointers);
  ppbRowPointers = NULL;

  // clean up after the write, and free any memory allocated

  png_destroy_write_struct (&png_ptr, &info_ptr);

  fclose (pfFile);

  return LR_OK;
}


LR_STATUS
lr_save_image_png8 (lr_image *image, const char *filename)
{
  const int ciBitDepth = 8;
  const int ciChannels = 3;
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  static FILE *pfFile;
  png_uint_32 ulRowBytes;
  static png_byte **ppbRowPointers = NULL;
  int i;
  lr_image *quantized = NULL;

  // open the PNG output file

  if ( !filename ) {
    return LR_FAIL;
  }
  
  if (!(pfFile = fopen (filename, "wb")))
    return LR_FAIL;

  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL,
				     (png_error_ptr) png_cexcept_error,
				     (png_error_ptr) NULL);
  if (!png_ptr) {
    fclose (pfFile);
    return LR_FAIL;
  }
  
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    fclose(pfFile);
    png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
    return LR_FAIL;
  }


  quantized = lr_wu_quantize_8bit(image);


  png_init_io(png_ptr, pfFile);
  png_set_IHDR (png_ptr, info_ptr, image->width, image->height, 8,
		PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
  png_set_PLTE(png_ptr, info_ptr, (png_colorp) quantized->palette, 256);

  // write the file header information
  png_write_info(png_ptr, info_ptr);


  // row_bytes is the width x number of channels
  ulRowBytes = image->width * 1;

  // we can allocate memory for an array of row-pointers
  if ((ppbRowPointers =
       (png_bytepp) malloc (image->height * sizeof (png_bytep))) == NULL)
    {
      fprintf (stderr, "out of memory");
    }

  // set the individual row-pointers to point at the correct offsets
  for (i = 0; i < image->height; i++) {
    ppbRowPointers[i] = quantized->buffer + i * ulRowBytes;
  }

  // write out the entire image data in one call
  png_write_image (png_ptr, ppbRowPointers);

  // write the additional chunks to the PNG file (not really needed)
  png_write_end (png_ptr, info_ptr);

  // and we're done
  free (ppbRowPointers);
  ppbRowPointers = NULL;

  // clean up after the write, and free any memory allocated
  png_destroy_write_struct (&png_ptr, &info_ptr);

  fclose (pfFile);

  return LR_OK;
}
