#ifndef ZIFIMAGE_H
#define ZIFIMAGE_H

extern "C" {
#   include <libavcodec/avcodec.h>
}

typedef struct zifImage
{
    PixelFormat fmt_type;
    int width;
    int height;
    unsigned char *data[4];
    int stride[4];

    double stamp;

    void *internal_ptr;
} zifImage;

#endif // ZIFIMAGE_H
