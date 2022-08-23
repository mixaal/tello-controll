#include "decoder.h"
#include "utils.h"
#include "sdl.h"

unsigned char *frame_data = NULL;
int frame_data_width, frame_data_height;
static sync_t frame_access_lock;

typedef enum { RMODE_GREEN, RMODE_BW } render_mode_t;
static render_mode_t render_mode = RMODE_BW;


void render_green(void)
{
  render_mode = RMODE_GREEN;
}

void render_bw(void)
{
  render_mode = RMODE_BW;
}


void frame_access_gain(void)
{
	lock(&frame_access_lock);
}

void frame_access_release(void)
{
	unlock(&frame_access_lock);
}


#ifdef HAVE_LIBAVCODEC
static AVFrame *decode_frame_impl(const char *data_in, ssize_t len, ssize_t *num_consumed, bool *is_frame_available);
static AVFrame* decoder_decode_frame(void);

#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096



static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize, 
                     char *filename);

static  AVCodecContext        *context;
static  AVFrame               *frame;
static  AVFrame               *framergb;
static  AVCodec               *codec;
static  AVCodecParserContext  *parser;
  /* In the documentation example on the github master branch, the 
packet is put on the heap. This is done here to store the pointers 
to the encoded data, which must be kept around  between calls to 
parse- and decode frame. In release 11 it is put on the stack, too. 
  */
static  AVPacket              *pkt;

static void H264InitFailure(const char *str)
{
	fprintf(stderr, "%s\n", str);
	exit(-1);
}

bool decoder_is_frame_available(void)
{
	return pkt->size > 0;
}

ssize_t decoder_parse(const unsigned char* in_data, ssize_t in_size)
{
  int size = 0;
  unsigned char *data;
  ssize_t nread = av_parser_parse2(parser, context, &data, &size, 
                                in_data, in_size, 
                                0, 0, AV_NOPTS_VALUE);
  pkt->size = size;
  pkt->data = data;
  //printf("size=%d data=%p\n", size, data);
  return nread;
}

static AVFrame *decode_frame_impl(const char *data_in, ssize_t len, ssize_t *num_consumed, bool *is_frame_available)
{
  *num_consumed = decoder_parse((const unsigned char*)data_in, len);
  //fprintf(stderr, "num consumed=%ld\n", *num_consumed);
  
  *is_frame_available = decoder_is_frame_available();
  if (*is_frame_available)
  {
    AVFrame *frame = decoder_decode_frame();
    if(frame==NULL) return NULL;
    unsigned char *src_data = frame->data[0];
    int wrap = frame->linesize[0];
    int xsize = frame->width;
    int ysize = frame->height;
    //frame_access_gain();
    if(frame_data == NULL) {
	    frame_data = (unsigned char *)xmalloc(xsize*ysize*3);
            frame_data_width = xsize;
            frame_data_height = ysize;
    } else {
	    if(xsize!=frame_data_width || ysize!=frame_data_height) {
		fprintf(stderr, "resolution has changed from %dx%d -> %dx%d\n", frame_data_width, frame_data_height, xsize, ysize);
		free(frame_data);
                frame_data_width = xsize;
                frame_data_height = ysize;
	        frame_data = (unsigned char *)xmalloc(xsize*ysize*3);
	    }
    }
    

    int dst = 0;
    for (int y = 0; y< ysize; y++) {
       for (int x = 0; x< xsize; x++) {
           switch (render_mode) {
             case RMODE_BW:
               frame_data[dst++] = *(src_data + y * wrap + x);
               frame_data[dst++] = *(src_data + y * wrap + x);
               frame_data[dst++] = *(src_data + y * wrap + x);
               break;
             case RMODE_GREEN:
               frame_data[dst++] = 0; 
               frame_data[dst++] = *(src_data + y * wrap + x);
               frame_data[dst++] = 0; 
               break;
           }
       }
    }
    //frame_access_release();
    return frame;
  } else {
	  return NULL;
  }
}

static AVFrame* decoder_decode_frame(void)
{
  int got_picture = 0;
  //printf("decoder_decode_frame: frame=%p\n", frame);
  int nread = avcodec_decode_video2(context, frame, &got_picture, pkt);
  if (nread < 0 || got_picture == 0) {
    fprintf(stderr, "error decoding frame\n");
    return NULL;
  }
  return frame;
}

void decode(const char *data_in, ssize_t len)
{
  
    while (len > 0)
    {
      ssize_t num_consumed = 0;
      bool is_frame_available = False;
      
        AVFrame *frame = decode_frame_impl(data_in, len, &num_consumed, &is_frame_available);
        if (is_frame_available && frame!=NULL)
        {
          //out.append(frame);
	  pgm_save(frame->data[0], frame->linesize[0],
                 frame->width, frame->height, (char*)"x.pgm");

	  //fprintf(stderr, "Have frame frame->data=%p\n", frame->data);
	  printf("\n");

        }
        if (num_consumed < 0) {
	        fprintf(stderr, "fatal: can't continue in stream decode!\n");
		return ;
          // This case is fatal because we cannot continue to move ahead in the stream.
	 }
      
      len -= num_consumed;
      data_in += num_consumed;
    }
  
  //return out;
}


void decoder_init(void)
{
  avcodec_register_all();

  codec = avcodec_find_decoder(AV_CODEC_ID_H264);
  if (!codec)
    H264InitFailure("cannot find decoder");
  
  context = avcodec_alloc_context3(codec);
  if (!context)
    H264InitFailure("cannot allocate context");

  if(codec->capabilities & AV_CODEC_CAP_TRUNCATED) {
    context->flags |= AV_CODEC_FLAG_TRUNCATED;
  }  

  int err = avcodec_open2(context, codec, NULL);
  if (err < 0)
    H264InitFailure("cannot open context");

  parser = av_parser_init(AV_CODEC_ID_H264);
  if (!parser)
    H264InitFailure("cannot init parser");
  
  frame = av_frame_alloc();
  if (!frame)
    H264InitFailure("cannot allocate frame");


  framergb = av_frame_alloc();
  if (!framergb)
    H264InitFailure("cannot allocate rgb frame");

#if 1
  pkt = (AVPacket *)xmalloc(sizeof(*pkt));
  if (!pkt)
    H264InitFailure("cannot allocate packet");
  av_init_packet(pkt);
#endif

}


void decoder_destroy(void)
{
  av_parser_close(parser);
  avcodec_close(context);
  av_free(context);
  av_frame_free(&frame);
  av_frame_free(&framergb);
#if 1
  free(pkt);
#endif
}


/*
 * Video decoding example
 */
static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
                     char *filename)
{
    FILE *f;
    int i;
    //printf("pgm_save: wrap=%d xsize=%d ysize=%d\n", wrap, xsize, ysize);
    f = fopen(filename,"w");
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}


#endif /* HAVE_LIBAVCODEC */

#if 0
#ifdef HAVE_LIBAVIF


void decode(const char *data_in, ssize_t len) 
{
   avifRGBImage rgb;
    memset(&rgb, 0, sizeof(rgb));
  avifDecoder * decoder = avifDecoderCreate();
  avifResult result = avifDecoderSetIOMemory(decoder, (unsigned char *)data_in, len);
    if (result != AVIF_RESULT_OK) {
        perror("avifDecoderSetIOMemory()");
        goto cleanup;
    }

    result = avifDecoderParse(decoder);
#if 0
    if (result != AVIF_RESULT_OK) {
        fprintf(stderr, "Failed to decode image: %s\n", avifResultToString(result));
        goto cleanup;
    }
#endif

    // Now available:
    // * All decoder->image information other than pixel data:
    //   * width, height, depth
    //   * transformations (pasp, clap, irot, imir)
    //   * color profile (icc, CICP)
    //   * metadata (Exif, XMP)
    // * decoder->alphaPresent
    // * number of total images in the AVIF (decoder->imageCount)
    // * overall image sequence timing (including per-frame timing with avifDecoderNthImageTiming())

    int xsize = decoder->image->width;
    int ysize = decoder->image->height;
    printf("Parsed AVIF: %ux%u (%ubpc)\n", decoder->image->width, decoder->image->height, decoder->image->depth);

    while (avifDecoderNextImage(decoder) == AVIF_RESULT_OK) {
        // Now available (for this frame):
        // * All decoder->image YUV pixel data (yuvFormat, yuvPlanes, yuvRange, yuvChromaSamplePosition, yuvRowBytes)
        // * decoder->image alpha data (alphaPlane, alphaRowBytes)
        // * this frame's sequence timing

        avifRGBImageSetDefaults(&rgb, decoder->image);
        // Override YUV(A)->RGB(A) defaults here: depth, format, chromaUpsampling, ignoreAlpha, alphaPremultiplied, libYUVUsage, etc

        // Alternative: set rgb.pixels and rgb.rowBytes yourself, which should match your chosen rgb.format
        // Be sure to use uint16_t* instead of uint8_t* for rgb.pixels/rgb.rowBytes if (rgb.depth > 8)
        avifRGBImageAllocatePixels(&rgb);

        if (avifImageYUVToRGB(decoder->image, &rgb) != AVIF_RESULT_OK) {
            fprintf(stderr, "Conversion from YUV failed\n");
            goto cleanup;
        }

        // Now available:
        // * RGB(A) pixel data (rgb.pixels, rgb.rowBytes)

        if (rgb.depth > 8) {
            fprintf(stderr, "only 8bit depth supported, current depth: %d\n", rgb.depth);
            goto cleanup;
        }
        uint8_t * firstPixel = rgb.pixels;
        printf(" * First pixel: RGBA(%u,%u,%u,%u)\n", firstPixel[0], firstPixel[1], firstPixel[2], firstPixel[3]);

        unsigned char *src_data = firstPixel;
        //frame_access_gain();
        if(frame_data == NULL) {
	    frame_data = (unsigned char *)xmalloc(xsize*ysize*3);
            frame_data_width = xsize;
            frame_data_height = ysize;
       } else {
	    if(xsize!=frame_data_width || ysize!=frame_data_height) {
		fprintf(stderr, "resolution has changed from %dx%d -> %dx%d\n", frame_data_width, frame_data_height, xsize, ysize);
		free(frame_data);
                frame_data_width = xsize;
                frame_data_height = ysize;
	        frame_data = (unsigned char *)xmalloc(xsize*ysize*3);
	    }
       } 
       
    int dst = 0;
    for (int y = 0; y< ysize; y++) {
       for (int x = 0; x< xsize; x++) {
           frame_data[dst++] = *(src_data + y * xsize + x);
           frame_data[dst++] = *(src_data + y * xsize + x);
           frame_data[dst++] = *(src_data + y * xsize + x);
       }
    }
    //frame_access_release();

 
    }

cleanup:
    avifRGBImageFreePixels(&rgb); // Only use in conjunction with avifRGBImageAllocatePixels()
    avifDecoderDestroy(decoder);
}


void decoder_destroy(void) 
{
}


void decoder_init(void)
{
}
#endif /* HAVE_LIBAVIF */
#endif




