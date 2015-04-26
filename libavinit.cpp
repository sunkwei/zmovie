#include "libavinit.h"
extern "C" {
#   include <libavformat/avformat.h>
}

libavInit::libavInit()
{
    av_register_all();
    avcodec_register_all();
    avformat_network_init();
}

static libavInit _inited;
