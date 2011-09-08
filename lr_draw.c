void lr_draw_horizontal(lr_image* img, int y, const unsigned char* color)
{
  for ( int i = 0; i < img->width; i++ ) {
    lr_pixel p = lr_get_pixel(img, i, y);
    LR_GREEN(p) = LR_GREEN(color);;
    LR_RED(p) = LR_RED(color);
    LR_BLUE(p) = LR_BLUE(color);
  }
}

void lr_draw_vertical(lr_image* img, int x, const unsigned char* color)
{
  for ( int i = 0; i < img->height; i++ ) {
    lr_pixel p = lr_get_pixel(img, x, i);
    LR_GREEN(p) = LR_GREEN(color);;
    LR_RED(p) = LR_RED(color);
    LR_BLUE(p) = LR_BLUE(color);
  }
}
