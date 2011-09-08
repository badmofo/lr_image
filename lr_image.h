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

#ifndef __LR_IMAGE_H__
#define __LR_IMAGE_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Some functions return type LR_STATUS to indicate success/failure.
 */
#define LR_STATUS int
#define LR_OK     0
#define LR_FAIL   1

/*
 * Some file types.
 */
#define LR_FORMAT_UNKNOWN 0
#define LR_FORMAT_GIF     1
#define LR_FORMAT_JPEG    2
#define LR_FORMAT_PNG     3

/*
 * A pixel is 3 bytes in RGB24 format.
 */
typedef unsigned char* lr_pixel;


/*
 * Macros for getting at the color components of an aforementioned
 * lr_pixel.
 */
#define LR_RED(X)   (*((unsigned char*) X))
#define LR_GREEN(X) (*(((unsigned char*) X) + 1))
#define LR_BLUE(X)  (*(((unsigned char*) X) + 2))


/*
 * An image is merely a buffer full of pixels.  The only fields to pay
 * attention to here are width, height, and buffer.  The rest of the
 * fields are used by specific functions to return extra data.  You
 * can ignore them unless the a function you a getting an image back
 * from says otherwise.
 *
 * Buffer Orientation Note:
 * &buffer[0] points to the top left pixel in the image.
 * &buffer[width * height * 3] points to the bottom right pixel.
 */
typedef struct lr_image_type
{
  int width;
  int height;
  lr_pixel buffer;
  lr_pixel palette;
  char* error_msg;
} lr_image;


/*
 * A structure for holding 2D dimension info. Nifty.
 */
typedef struct lr_dimensions_type
{
  int width;
  int height;
} lr_dimensions;


/*
 * This makes a new image of the given size.  It is initialized to
 * black.  This will return NULL if the system is out of memory.
 */
lr_image* lr_allocate_image(int width, int height);

/*
 * If you are the tidy type you should free images after you are
 * done with them.
 */
void lr_free_image(lr_image* img);

/*
 * Reads an image from a file.  It expects the file to just contain
 * the raw RGB24 data, thus the need for you to supply the dimensions.
 * Returns NULL if there is a problem.  See lr_save_image_raw().
 */
lr_image* lr_read_image_raw(const char* filename, int width, int height);

/**
 * Returns one of the LR_FORMAT_* defines.
 */ 
int lr_get_image_type(const char *filepath);

/*
 * Reads a GIF, JPEG or PNG image.
 */
lr_image* lr_read_image(const char* filename);


/*
 * Reads an GIF image from a file.  Returns NULL if there is a problem.
 */
lr_image *lr_read_image_gif(const char* filename);

/*
 * Reads an PNG image from a file.  Returns NULL if there is a problem.
 */
lr_image *lr_read_image_png(const char* filename);

/*
 * Reads an image from a JPEG formatted file or from memory.  Returns
 * NULL if there is a problem.
 */
lr_image* lr_read_image_jpeg(const char* filename);
lr_image* lr_read_image_jpeg_mem(unsigned char* buffer, int size);


/* 
 * Reads an image from a JPEG formatted file, but scales the image
 * during decompression so that its width is around 200 pixels.  If
 * you are making thumbnails then you should load your images with
 * this function and then use lr_resample to shrink the returned image
 * down to it's final size.  This is much faster than simply
 * resampling the full size image.  This returns NULL if there is 
 * a problem.  This size of the image before scaling is returned 
 * via the original_size parameter.
 */
lr_image* lr_read_image_jpeg_scaled(const char* filename, 
				    lr_dimensions* original_size);
lr_image* lr_read_image_jpeg_scaled_mem(unsigned char* buffer, int size,
					lr_dimensions* original_size);

/* 
 * Saves an image in raw RGB24 format.  To load it again you'll have
 * to supply the image dimensions.  See lr_read_image_raw().  Returns
 * LR_OK if everything goes as planned, LR_FAIL otherwise.
 */
LR_STATUS lr_save_image_raw(lr_image* img, const char* filename);

/*
 * Saves an image in JPEG format using the default compression quality
 * Someone should alter this so that it accepts more JPEG compression
 * options.  Returns LR_OK on success, LR_FAIL otherwise.
 */
LR_STATUS lr_save_image_jpeg(lr_image *image, const char* filename);
LR_STATUS lr_save_image_jpeg_ex(lr_image *image, const char* filename, int quality);


/*
 * Saves an image in GIF format using the LZW compression ... lookout
 * for Unisys' legal team!  Returns LR_OK on success, LR_FAIL
 * otherwise.  WARNING: this particular function might leak memory.
 */
LR_STATUS lr_save_image_gif(lr_image *img, const char* filename);

/*
 * Saves an image in 24-bit BMP format.  Returns LR_OK on success,
 * LR_FAIL otherwise.
 */
LR_STATUS lr_save_image_bmp(lr_image* img, const char* filename);

/*
 * Saves an image in 24-bit PNG format.  Return LR_OK on success,
 * LR_FAIL otherwise.
 */
LR_STATUS lr_save_image_png(lr_image *image, const char* filename);

/*
 * Returns a reference to a pixel in an image.
 */
lr_pixel lr_get_pixel(lr_image* img, int x, int y);

/*
 * Sets the color components of a pixel in an image.
 */
void lr_set_pixel(lr_image* img, int x, int y, int r, int g, int b);

/**
 * Returns a cropped version of the input image.  The boundries are included 
 * in the cropped version.
 */
lr_image* lr_crop(lr_image* img, int left, int top, int right, int bottom);

/*
 * Resamples an image.  The destination should be a blank image
 * allocated to the desired size.  Resampling is different from
 * strait-up rescaling in that resampled minimizations don't look all
 * jaggy.  If you are looking to make quick and dirty thumbnails that 
 * look like ass See lr_cheesy_resample().
 */
lr_image* lr_resample(lr_image* src, int width, int height);

/*
 * Suppose you want to scale an image down so that it fits within a
 * bounding box and you wish to preserve it's aspect ratio.  Wouldn't
 * it be nice if someone wrote a function to do the math for you.  Gee
 * whiz.
 */
void lr_scale_dimensions(lr_dimensions *original,
			 lr_dimensions *bounds,
			 lr_dimensions *scaled);

/* 
 * Alters the bottom image so that the top image appears atop it at
 * the given position.  This function doesn't perform clipping so make
 * sure the top image fits completely within the bottom one or you
 * will find yourself a world of shit.
 */
void lr_paste_atop(lr_image* bottom, lr_image* top, int x, int y);


/*
 * The following functions are for printing text.  I'm guessing you'll
 * want to use the more high level of the two lr_print_text.  It
 * prints text all on one line (i.e. '\n' isn't supported).  There is
 * only one font and it's characters all are 6x10.
 */
void lr_print_letter(int c, lr_image *img, int x, int y);
void lr_print_text(const char* str, lr_image *img, int x, int y);

/* 
 * Ignore these functions.  You probably don't need to use em anyway.
 */
lr_image *lr_minify(lr_image* src, int factor);
lr_image *lr_maxify(lr_image* src, int factor);
void lr_nn_resample(lr_image* src, lr_image* dst);
void lr_reverse_image(lr_pixel pixels, int width, int height);
lr_image *lr_montage(lr_image** images, 
		     int across, int down, 
		     int border, 
		     lr_dimensions *bounds);


/*
 * Channel extraction.
 */
void lr_extract_channel(lr_image* img, 
			int channel_number, 
			unsigned char* output_buffer);

void lr_extract_channel_as_float(lr_image* img, 
				 int channel_number, 
				 float* output_buffer);

/*
 * Rather exotic functions 
 */
void lr_desaturate(lr_image* img);
lr_image* lr_remove_borders(lr_image* img, float threshold, float search_depth);
void lr_equalize(lr_image *img);

 


/*
 * Self explainatory utility functions.
 */
float lr_min(float a, float b);
float lr_max(float a, float b);
int lr_clamp(int value, int min, int max);
void lr_put_char(int c, FILE *fp);
void lr_put_short(unsigned short s, FILE *fp);
void lr_put_long(unsigned long l, FILE *fp);
int lr_get_filesize(const char* filename);


/*
 * Color constants for use with drawing functions.
 */
extern unsigned char LR_COLOR_RED[3];
extern unsigned char LR_COLOR_BLUE[3];


/*
 * 3x3 Filters for use with lr_apply_mask().
 */
extern int LR_FILTER_H_LINE_DETECT[10];
extern int LR_FILTER_V_LINE_DETECT[10];
extern int LR_FILTER_EDGE_DETECT1[10];
extern int LR_FILTER_EDGE_DETECT2[10];
extern int LR_FILTER_BLUR[10];

#ifdef __cplusplus
}
#endif

#endif // __LR_IMAGE_H__
