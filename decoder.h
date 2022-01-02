#ifndef __TELLO_DECODER_H__
#define __TELLO_DECODER_H__ 1

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>

#ifdef __cplusplus
}
#endif

void decoder_init(void);
void decoder_destroy(void);
bool decoder_is_frame_available(void);
AVFrame* decoder_decode_frame(void);
ssize_t decoder_parse(const unsigned char* in_data, ssize_t in_size);
AVFrame *decode_frame_impl(const char *data_in, ssize_t len, ssize_t *num_consumed, bool *is_frame_available);
void decode(const char *data_in, ssize_t len);

void frame_access_gain(void);
void frame_access_release(void);

#endif /* __TELLO_DECODER_H__ */
