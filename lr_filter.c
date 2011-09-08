#include "lr_image.h"
#include <math.h>
#include <stdlib.h>


/*
 * Color constants for use with drawing functions.
 */
unsigned char LR_COLOR_RED[3]  = { 255, 0, 0 };
unsigned char LR_COLOR_BLUE[3] = { 0, 0, 255 };


/*
 * 3x3 Filters for use with lr_apply_mask().
 */
int LR_FILTER_H_LINE_DETECT[10] = {-1,-1,-1,2,2,2,-1,-1,-1, 1};
int LR_FILTER_V_LINE_DETECT[10] = {-1,2,-1,-1,2,-1,-1,2,-1, 1};
int LR_FILTER_EDGE_DETECT1[10]  = {-1,-1,-1,-1,8,-1,-1,-1,-1, 1};
int LR_FILTER_EDGE_DETECT2[10]  = {0,-1,0,-1,4,-1,0,-1,0, 1};
int LR_FILTER_BLUR[10]          = {1,1,1, 1,1,1, 1,1,1, 9};


int lr_in_image(lr_image* img, int x, int y)
{
  if ( x < 0 || y < 0 || x >= img->width || y >= img->height )
    return 0;
  else
    return 1;
}

lr_image* lr_convolve(int* mask, int mask_width, lr_image* img)
{
  int i, j, x, y, z;
  lr_image* output = lr_allocate_image(img->width, img->height);
  for ( i = 0; i < img->height; i++ ) {
    for ( j = 0; j < img->width; j++ ) {
      lr_pixel p;
      int total_r = 0, total_g = 0, total_b = 0;
      for ( y = 0; y < mask_width; y++ ) {
	for ( x = 0; x < mask_width; x++ ) {
	  int a = i + y - 1;
	  int b = j + x - 1;
	  if ( lr_in_image(img, b, a) ) {
	    lr_pixel p = lr_get_pixel(img, b, a);
	    total_g += mask[y * mask_width + x] * LR_GREEN(p);
	    total_b += mask[y * mask_width + x] * LR_BLUE(p);
	    total_r += mask[y * mask_width + x] * LR_RED(p);
	  }
	}
      }
      z = 0;
      for ( y = 0; y < mask_width; y++ ) {
	for ( x = 0; x < mask_width; x++ ) {
	  z += mask[y * mask_width + x];
	}
      }

      p = lr_get_pixel(output, j, i);
      LR_GREEN(p) = lr_clamp(total_g / z, 0, 255);
      LR_BLUE(p) = lr_clamp(total_b / z, 0, 255);
      LR_RED(p) = lr_clamp(total_r / z, 0, 255);
    }
  }
  
  return output;  
}

lr_image* lr_apply_mask(int* mask, lr_image* img)
{
  int i, j, x, y;
  lr_image* output = lr_allocate_image(img->width, img->height);
  for ( i = 0; i < img->height; i++ ) {
    for ( j = 0; j < img->width; j++ ) {
      lr_pixel p;
      int total_r = 0, total_g = 0, total_b = 0;
      for ( y = 0; y <= 2; y++ ) {
	for ( x = 0; x <= 2; x++ ) {
	  int a = i + y - 1;
	  int b = j + x - 1;
	  if ( lr_in_image(img, b, a) ) {
	    lr_pixel p = lr_get_pixel(img, b, a);
	    total_g += mask[y * 3 + x] * LR_GREEN(p);
	    total_b += mask[y * 3 + x] * LR_BLUE(p);
	    total_r += mask[y * 3 + x] * LR_RED(p);
	  }
	}
      }
      p = lr_get_pixel(output, j, i);
      LR_GREEN(p) = lr_clamp(total_g / mask[9], 0, 255);
      LR_BLUE(p) = lr_clamp(total_b / mask[9], 0, 255);
      LR_RED(p) = lr_clamp(total_r / mask[9], 0, 255);
    }
  }
  
  return output;
}

void lr_desaturate(lr_image* img)
{
  int x, y;
  for ( y = 0; y < img->height; y++ ) {
    for ( x = 0; x < img->width; x++ ) {
      lr_pixel p = lr_get_pixel(img, x, y);
      int intensity = LR_RED(p) + LR_GREEN(p) + LR_BLUE(p);
      intensity /= 3;
      LR_GREEN(p) = intensity;
      LR_RED(p) = intensity;
      LR_BLUE(p) = intensity;
    }
  }
}

float lr_get_intensity(lr_image* img, int x, int y)
{
  lr_pixel p = lr_get_pixel(img, x, y);
  return (float) (LR_RED(p) + LR_GREEN(p) + LR_BLUE(p)) / (3.0 * 255.0);
}


void lr_vertical_edge_detect(lr_image* img)
{
  int i, j;
  for ( i = 0; i < img->width; i++ ) {
    int last_intensity = 0;
    for ( j = 0; j < img->height; j++ ) {
      lr_pixel p = lr_get_pixel(img, i, j);
      int intensity = LR_GREEN(p) + LR_BLUE(p) + LR_RED(p);
      intensity /= 3;
      LR_GREEN(p) = abs(last_intensity - intensity);
      LR_BLUE(p) = abs(last_intensity - intensity);
      LR_RED(p) = abs(last_intensity - intensity);
      last_intensity = intensity;
    }
  }
}

void lr_horizontal_edge_detect(lr_image* img)
{
  int i, j;
  for ( i = 0; i < img->height; i++ ) {
    int last_intensity = 0;
    for ( j = 0; j < img->width; j++ ) {
      lr_pixel p = lr_get_pixel(img, j, i);
      int intensity = LR_GREEN(p) + LR_BLUE(p) + LR_RED(p);
      intensity /= 3;
      LR_GREEN(p) = abs(last_intensity - intensity);
      LR_BLUE(p) = abs(last_intensity - intensity);
      LR_RED(p) = abs(last_intensity - intensity);
      last_intensity = intensity;
    }
  }
}


void lr_adjust_intensity(lr_image* img, float multipler)
{
  int i, j;
  for ( i = 0; i < img->height; i++ ) {
    for ( j = 0; j < img->width; j++ ) {
      lr_pixel p = lr_get_pixel(img, j, i);
      LR_GREEN(p) = LR_GREEN(p) * multipler;
      LR_BLUE(p) = LR_BLUE(p) * multipler;
      LR_RED(p) = LR_RED(p) * multipler;
    }
  }
}

// threshold and search depth are both 0...1
lr_image* lr_remove_borders(lr_image* img, float threshold, float search_depth)
{
  int top, bottom, left, right, x, y;
  lr_image *e, *h, *v;
  e = lr_apply_mask(LR_FILTER_EDGE_DETECT1, img);
  h = lr_apply_mask(LR_FILTER_H_LINE_DETECT, e);
  v = lr_apply_mask(LR_FILTER_V_LINE_DETECT, e);
  
  top = left = 0;
  bottom = img->height - 1; 
  right = img->width - 1;
  
  for ( y = 0; y < h->height * search_depth; y++ ) {
    float total = 0.0;
    for ( x = 0; x < h->width; x++ ) 
      total += lr_get_intensity(h, x, y);
    if ( total > (h->width * threshold) ) 
      top = y;
  }
  
  for ( y = h->height - (h->height * search_depth); y < h->height; y++ ) {
    float total = 0.0;
    for ( x = 0; x < h->width; x++ ) 
      total += lr_get_intensity(h, x, y);
    if ( total > (h->width * threshold) ) {
      bottom = y;
      break;
    }
  }
  
  for ( x = 0; x < v->width * search_depth; x++ ) {
    float total = 0.0;
    for ( y = 0; y < v->height; y++ ) 
      total += lr_get_intensity(v, x, y);
    if ( total > (v->height * threshold) ) 
      left = x;
  }
  
  for ( x = v->width - (v->width * search_depth); x < v->width; x++ ) {
    float total = 0.0;
    for ( y = 0; y < v->height; y++ ) 
      total += lr_get_intensity(v, x, y);
    if ( total > (v->height * threshold) ) {
      right = x;
      break;
    }
  }
  
  if ( top ) top++;
  if ( left ) left++; 
  if ( bottom < img->height - 1 ) bottom--;
  if ( right < img->width - 1 ) right--;
  
  lr_free_image(e);
  lr_free_image(h);
  lr_free_image(v);

  return lr_crop(img, left, top, right + 1, bottom + 1);
}

/**
 * output_buffer should be size img->width * img->height
 */
void lr_extract_channel(lr_image* img, 
			int channel_number, 
			unsigned char* output_buffer)
{
  int x, y;
  for ( y = 0; y < img->height; y++ ) {
    for ( x = 0; x < img->width; x++ ) {
      lr_pixel p = lr_get_pixel(img, x, y);
      output_buffer[y * img->width + x] = p[channel_number];
    }
  }
}


// normalizes the values in the channel to [0...1] 
void lr_extract_channel_as_float(lr_image* img, 
				 int channel_number, 
				 float* output_buffer)
{
  int x, y;
  for ( y = 0; y < img->height; y++ ) {
    for ( x = 0; x < img->width; x++ ) {
      lr_pixel p = lr_get_pixel(img, x, y);
      output_buffer[y * img->width + x] = p[channel_number] / 255.0;
    }
  }
}



/////////////////////////////////////////////////////////////////////////

#define NONE   0
#define WEAK   1
#define STRONG 2

typedef struct MyFilterData {
  int					strength;
  int					intensity;
  int					antibanding;
  unsigned long       inHistogram[256];	// Input histogram
  unsigned long		outHistogram[256];	// Output histogram
  unsigned long		LUT[256];			// Lookup table derived from Histogram[]
  unsigned long		jran, ia, ic, im;
  long				inHistoMax;
  long				outHistoMax;
  int					disable;
} MyFilterData;

///////////////////////////////////////////////////////////////////////////////
/// 
static int lr_max_int(int a, int b)
{
  return ( a > b ) ? a : b;
}



///////////////////////////////////////////////////////////////////////////

void lr_equalize(lr_image *img)
{
  MyFilterData* mfd;
  int x, y;
  unsigned int r, g, b, luma, m;
  int luthi, lutlo, lut;
  int oluma;
  int* luminance;
  int width = img->width;
  int height = img->height;

  mfd = (MyFilterData*) malloc(sizeof(MyFilterData));

  luminance = malloc(width * height * sizeof(int));


  mfd->strength = 200;
  mfd->intensity = 210;
  mfd->antibanding = NONE;
  mfd->ia = 4096UL;
  mfd->ic = 150889UL;
  mfd->im = 714025UL;
  mfd->disable = 1;

  
  /* Calculate and store the luminance and calculate the global histogram
     based on the luminance. */
  //src = fa->src.data;
  for (x = 0; x < 256; x++) mfd->inHistogram[x] = 0;
  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      lr_pixel p = lr_get_pixel(img, x, y);
      r = LR_RED(p);
      g = LR_GREEN(p);
      b = LR_BLUE(p);
      luma = (55 * r + 182 * g + 19 * b) >> 8;
      luminance[y * width + x] = luma;
      mfd->inHistogram[luma]++;
    }
  }
  
  /* Calculate the lookup table. */
  mfd->LUT[0] = mfd->inHistogram[0];
  /* Accumulate. */
  for (x = 1; x < 256; x++) {
    mfd->LUT[x] = mfd->LUT[x-1] + mfd->inHistogram[x];
  }
  /* Normalize. */
  for (x = 0; x < 256; x++) {
    mfd->LUT[x] = (mfd->LUT[x] * mfd->intensity) / (height * width);
  }
  
  /* Adjust the LUT based on the selected strength. This is an alpha
     mix of the calculated LUT and a linear LUT with gain 1. */
  for (x = 0; x < 256; x++) {
    mfd->LUT[x] = (mfd->strength * mfd->LUT[x]) / 255 +
      ((255 - mfd->strength) * x) / 255;
  }
  
  /* Output the equalized frame. */
  for (x = 0; x < 256; x++) mfd->outHistogram[x] = 0;
  //src = fa->src.data;
  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      luma = luminance[y * width + x];
      if (luma == 0) {
	lr_pixel p = lr_get_pixel(img, x, y);
	LR_RED(p) = 
	LR_GREEN(p) =
	  LR_BLUE(p) = 0;

	//src[x] = 0;
	mfd->outHistogram[0]++;
      } else {
	lr_pixel p = lr_get_pixel(img, x, y);
	lut = mfd->LUT[luma];
	r = LR_RED(p);
	g = LR_GREEN(p);
	b = LR_BLUE(p);
	if (((m = lr_max_int(r, lr_max_int(g, b))) * lut) / luma > 255) {
	  r = (r * 255) / m; 
	  g = (g * 255) / m; 
	  b = (b * 255) / m; 
	} else {
	  r = (r * lut) / luma;
	  g = (g * lut) / luma;
	  b = (b * lut) / luma;
	}
	LR_RED(p) = r;
	LR_GREEN(p) = g;
	LR_BLUE(p) = b;
	oluma = (55 * r + 182 * g + 19 * b) >> 8;
	mfd->outHistogram[oluma]++;
      }
    }
  }

  free(mfd);
  free(luminance);
}




