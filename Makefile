# Copyright (c) 2002.  Lucas Ryan.  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name Lucas Ryan nor the names of contributors may be 
#    used to endorse or promote products derived from this software 
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
# COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#

#####################################################################

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

APPS = flip thumb block wang equalize

#CFLAGS = -O3 -fexpensive-optimizations
CFLAGS = -g

LDLIBS = -lm -ljpeg -lpng

#####################################################################

lr_image.a: $(OBJS)
	ar rcs lr_image.a $(OBJS)

clean:
	rm *.o $(APPS) lr_image.a *~

apps: $(APPS)

thumb:    lr_image.a thumb.c
	gcc -o thumb thumb.c lr_image.a -lpopt -ljpeg -lm -lpng
equalize: lr_image.a equalize.c
wang:     lr_image.a wang.c
flip:     lr_image.a flip.c
equalize: lr_image.a equalize.c
block:    lr_image.a block.c
png8:    lr_image.a png8.c

tarball:
	tar -cvzf lr_image_0.1.tgz *.c *.h README LICENSE Makefile

#####################################################################
