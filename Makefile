OBJS =  lr_image.o \
	lr_filter.o \
	lr_pyramid.o \
	lr_text.o \
	lr_util.o \
	lr_bmp.o \
	lr_quantize.o \
	lr_jpeg.o \
	lr_gif.o \
	lr_png.o \
	gd_lzw_out.o \
	wu.o

APPS = thumb block equalize

CFLAGS = -O3
#CFLAGS = -g

LDLIBS = -lm -ljpeg -lpng -lpopt

#####################################################################

apps: $(APPS)

lr_image.a: $(OBJS)
	ar rcs lr_image.a $(OBJS)

clean:
	rm -f *.o $(APPS) lr_image.a

thumb:    lr_image.a thumb.c
equalize: lr_image.a equalize.c
block:    lr_image.a block.c

tarball:
	tar -cvzf lr_image_0.1.tgz *.c *.h README LICENSE Makefile

#####################################################################
