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
#include <jpeglib.h>
#include "lr_image.h"
#include <setjmp.h>




/*
 * ERROR HANDLING:
 *
 * The JPEG library's standard error handler (jerror.c) is divided into
 * several "methods" which you can override individually.  This lets you
 * adjust the behavior without duplicating a lot of code, which you might
 * have to update with each future release.
 *
 * Our example here shows how to override the "error_exit" method so that
 * control is returned to the library's caller when a fatal error occurs,
 * rather than calling exit() as the standard error_exit method does.
 *
 * We use C's setjmp/longjmp facility to return control.  This means that the
 * routine which calls the JPEG library must first execute a setjmp() call to
 * establish the return point.  We want the replacement error_exit to do a
 * longjmp().  But we need to make the setjmp buffer accessible to the
 * error_exit routine.  To do this, we make a private extension of the
 * standard JPEG error handler object.  (If we were using C++, we'd say we
 * were making a subclass of the regular error handler.)
 *
 * Here's the extended error handler struct:
 */

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  //(*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}







///////////////////////////////////////////////////////////////////////////////
/// 
LR_STATUS lr_save_image_jpeg_ex(lr_image *image, const char* filename, int quality)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  int i;
  unsigned char* line;
  
  FILE* f = fopen(filename, "wb");
  if ( f == 0 ) {
    return LR_FAIL;
  }

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, f);

  cinfo.image_width = image->width;
  cinfo.image_height = image->height;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;
  // what does this flag mean?
  cinfo.optimize_coding = 1;

  jpeg_set_defaults(&cinfo);
  if ( quality > 0 ) 
	  jpeg_set_quality(&cinfo, quality, TRUE);
  jpeg_start_compress(&cinfo, TRUE);

  for ( i = 0; i < image->height; i++ ) {
    line = image->buffer + (image->width * i * 3);
    jpeg_write_scanlines(&cinfo, &line, 1);
  }

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  
  fflush(f);
  fclose(f);

  return LR_OK;
}

///////////////////////////////////////////////////////////////////////////////
/// 
LR_STATUS lr_save_image_jpeg(lr_image *image, const char* filename)
{
  return lr_save_image_jpeg_ex(image, filename, -1);
}


///////////////////////////////////////////////////////////////////////////////
/// 
lr_image* lr_read_image_jpeg_scaled(const char* filename, 
				    lr_dimensions* original_size)
{

  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  int row_stride, offset;
  lr_image *img;
  unsigned char *line;
  
  FILE* f = fopen(filename, "rb");
  if ( f == 0 ) {
    return NULL;
  }

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, f);
  jpeg_read_header(&cinfo, TRUE);

  cinfo.out_color_space = JCS_RGB;

  //printf ("%d\n", cinfo.image_width);

  if ( cinfo.image_width / 8 > 200 ) {
    cinfo.scale_denom = 8;
  } else if ( cinfo.image_width / 4 > 200 ) {
    cinfo.scale_denom = 4;
  } else if ( cinfo.image_width / 2 > 200 ) {
    cinfo.scale_denom = 2;
  } else {
    cinfo.scale_denom = 1;
  }

  jpeg_start_decompress(&cinfo);

  img = lr_allocate_image(cinfo.output_width, cinfo.output_height);
  
  if ( original_size ) {
    original_size->width = cinfo.image_width;
    original_size->height = cinfo.image_height;
  }

  //printf("image is %d x %d %d\n", cinfo.output_width, cinfo.output_height,
  //cinfo.output_components);

  row_stride = cinfo.output_width * cinfo.output_components;
  offset = 0;
  while ( cinfo.output_scanline < cinfo.output_height ) {
    line = img->buffer + offset;
    jpeg_read_scanlines(&cinfo, &line, 1);
    offset += row_stride;
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  fclose(f);

  return img;
}


///////////////////////////////////////////////////////////////////////////////
/// 
lr_image* lr_read_image_jpeg(const char* filename)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  int row_stride, offset;
  lr_image *img;
  unsigned char *line;
  
  FILE* f = fopen(filename, "rb");
  if ( f == 0 ) {
    return NULL;
  }

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, f);
  jpeg_read_header(&cinfo, TRUE);

  cinfo.out_color_space = JCS_RGB;

  jpeg_start_decompress(&cinfo);

  img = lr_allocate_image(cinfo.output_width, cinfo.output_height);

  row_stride = cinfo.output_width * cinfo.output_components;
  offset = 0;
  while ( cinfo.output_scanline < cinfo.output_height ) {
    line = img->buffer + offset;
    jpeg_read_scanlines(&cinfo, &line, 1);
    offset += row_stride;
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  fclose(f);

  return img;
}


/*
 * jdatasrc.c
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains decompression data source routines for the case of
 * reading JPEG data from a file (or any stdio stream).  While these routines
 * are sufficient for most applications, some will want to use a different
 * source manager.
 * IMPORTANT: we assume that fread() will correctly transcribe an array of
 * JOCTETs from 8-bit-wide elements on external storage.  If char is wider
 * than 8 bits on your machine, you may need to do some tweaking.
 */

/* this is not a core library module, so it doesn't define JPEG_INTERNALS */
//#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"


/* Expanded data source object for stdio input */

typedef struct {
  struct jpeg_source_mgr pub;   /* public fields */

  char * infile;                /* source stream */
  unsigned int pos;
  unsigned int len;
  JOCTET * buffer;              /* start of buffer */
  boolean start_of_file;        /* have we gotten any data yet? */
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;

#define INPUT_BUF_SIZE  4096    /* choose an efficiently fread'able size */


/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */

METHODDEF(void)
init_source (j_decompress_ptr cinfo)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  /* We reset the empty-input-file flag for each image,
   * but we don't clear the input buffer.
   * This is correct behavior for reading a series of images from one source.
   */
  src->start_of_file = TRUE;
  src->pos=0;
}


/*
 * Fill the input buffer --- called whenever buffer is emptied.
 *
 * In typical applications, this should read fresh data into the buffer
 * (ignoring the current state of next_input_byte & bytes_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been reloaded.  It is not necessary to
 * fill the buffer entirely, only to obtain at least one more byte.
 *
 * There is no such thing as an EOF return.  If the end of the file has been
 * reached, the routine has a choice of ERREXIT() or inserting fake data into
 * the buffer.  In most cases, generating a warning message and inserting a
 * fake EOI marker is the best course of action --- this will allow the
 * decompressor to output however much of the image is there.  However,
 * the resulting error message is misleading if the real problem is an empty
 * input file, so we handle that case specially.
 *
 * In applications that need to be able to suspend compression due to input
 * not being available yet, a FALSE return indicates that no more data can be
 * obtained right now, but more may be forthcoming later.  In this situation,
 * the decompressor will return to its caller (with an indication of the
 * number of scanlines it has read, if any).  The application should resume
 * decompression after it has loaded more data into the input buffer.  Note
 * that there are substantial restrictions on the use of suspension --- see
 * the documentation.
 *
 * When suspending, the decompressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_input_byte & bytes_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point must be rescanned after resumption, so move it to
 * the front of the buffer rather than discarding it.
 */

METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr cinfo)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;
  size_t nbytes;

  //nbytes = JFREAD(src->infile, src->buffer, INPUT_BUF_SIZE);
  if (src->pos+INPUT_BUF_SIZE>src->len) nbytes=src->len-src->pos;
  else nbytes=INPUT_BUF_SIZE;

  memcpy(src->buffer, &src->infile[src->pos], nbytes);
  src->pos+=nbytes;

  if (nbytes <= 0) {
    if (src->start_of_file)     /* Treat empty input file as fatal error */
      ERREXIT(cinfo, JERR_INPUT_EMPTY);
      
    WARNMS(cinfo, JWRN_JPEG_EOF);
    /* Insert a fake EOI marker */
    src->buffer[0] = (JOCTET) 0xFF;
    src->buffer[1] = (JOCTET) JPEG_EOI;
    nbytes = 2;
  }

  src->pub.next_input_byte = src->buffer;
  src->pub.bytes_in_buffer = nbytes;
  src->start_of_file = FALSE;

  return TRUE;
}


/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 * Writers of suspendable-input applications must note that skip_input_data
 * is not granted the right to give a suspension return.  If the skip extends
 * beyond the data currently in the buffer, the buffer can be marked empty so
 * that the next read will cause a fill_input_buffer call that can suspend.
 * Arranging for additional bytes to be discarded before reloading the input
 * buffer is the application writer's problem.
 */

METHODDEF(void)
skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  /* Just a dumb implementation for now.  Could use fseek() except
   * it doesn't work on pipes.  Not clear that being smart is worth
   * any trouble anyway --- large skips are infrequent.
   */
  if (num_bytes > 0) {
    while (num_bytes > (long) src->pub.bytes_in_buffer) {
      num_bytes -= (long) src->pub.bytes_in_buffer;
      (void) fill_input_buffer(cinfo);
      /* note we assume that fill_input_buffer will never return FALSE,
       * so suspension need not be handled.
       */
    }
    src->pub.next_input_byte += (size_t) num_bytes;
    src->pub.bytes_in_buffer -= (size_t) num_bytes;
  }
}


/*
 * An additional method that can be provided by data source modules is the
 * resync_to_restart method for error recovery in the presence of RST markers.
 * For the moment, this source module just uses the default resync method
 * provided by the JPEG library.  That method assumes that no backtracking
 * is possible.
 */


/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

METHODDEF(void)
term_source (j_decompress_ptr cinfo)
{
  /* no work necessary here */
}


/*
 * Prepare for input from a stdio stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing decompression.
 */

GLOBAL(void)
jpeg_mem_src2 (j_decompress_ptr cinfo, char * infile, unsigned int len)
{
  my_src_ptr src;

  /* The source object and input buffer are made permanent so that a series
   * of JPEG images can be read from the same file by calling jpeg_stdio_src
   * only before the first one.  (If we discarded the buffer at the end of
   * one image, we'd likely lose the start of the next one.)
   * This makes it unsafe to use this manager and a different source
   * manager serially with the same JPEG object.  Caveat programmer.
   */
  if (cinfo->src == NULL) {     /* first time for this JPEG object? */
    cinfo->src = (struct jpeg_source_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                  sizeof(my_source_mgr));
    src = (my_src_ptr) cinfo->src;
    src->buffer = (JOCTET *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                  INPUT_BUF_SIZE * sizeof(JOCTET));
  }

  src = (my_src_ptr) cinfo->src;
  src->pub.init_source = init_source;
  src->pub.fill_input_buffer = fill_input_buffer;
  src->pub.skip_input_data = skip_input_data;
  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = term_source;
  src->infile = infile;
  src->len = len;
  src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
  src->pub.next_input_byte = NULL; /* until buffer loaded */
}




lr_image* lr_read_image_jpeg_mem(unsigned char* buffer, int size)
{

  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  int row_stride, offset;
  lr_image *img;
  unsigned char *line;
  

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_mem_src2(&cinfo, buffer, size);
  jpeg_read_header(&cinfo, TRUE);

  cinfo.out_color_space = JCS_RGB;

  jpeg_start_decompress(&cinfo);

  img = lr_allocate_image(cinfo.output_width, cinfo.output_height);

  row_stride = cinfo.output_width * cinfo.output_components;
  offset = 0;
  while ( cinfo.output_scanline < cinfo.output_height ) {
    line = img->buffer + offset;
    jpeg_read_scanlines(&cinfo, &line, 1);
    offset += row_stride;
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  return img;
}




///////////////////////////////////////////////////////////////////////////////
/// 




/* never show any warnings */
METHODDEF(void)
my_emit_message (j_common_ptr cinfo, int msg_level)
{
}






lr_image* lr_read_image_jpeg_scaled_mem(unsigned char* buffer, int size,
					lr_dimensions* original_size)
{

  struct jpeg_decompress_struct cinfo;
  struct my_error_mgr jerr;
  int row_stride, offset;
  lr_image *img;
  unsigned char *line;
  
  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  jerr.pub.emit_message = my_emit_message;
  /* Establish the setjmp return context for my_error_exit to use. */
  if ( setjmp(jerr.setjmp_buffer) ) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
    return NULL;
  }

  jpeg_create_decompress(&cinfo);
  jpeg_mem_src2(&cinfo, buffer, size);
  jpeg_read_header(&cinfo, TRUE);



  cinfo.out_color_space = JCS_RGB;

  if ( cinfo.image_width / 8 > 200 ) {
    cinfo.scale_denom = 8;
  } else if ( cinfo.image_width / 4 > 200 ) {
    cinfo.scale_denom = 4;
  } else if ( cinfo.image_width / 2 > 200 ) {
    cinfo.scale_denom = 2;
  } else {
    cinfo.scale_denom = 1;
  }

  jpeg_start_decompress(&cinfo);

  img = lr_allocate_image(cinfo.output_width, cinfo.output_height);
  
  if ( original_size ) {
    original_size->width = cinfo.image_width;
    original_size->height = cinfo.image_height;
  }

  row_stride = cinfo.output_width * cinfo.output_components;
  offset = 0;
  while ( cinfo.output_scanline < cinfo.output_height ) {
    line = img->buffer + offset;
    jpeg_read_scanlines(&cinfo, &line, 1);
    offset += row_stride;
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  return img;
}


