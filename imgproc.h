#ifndef _TELLO_IMGPROC_H_
#define _TELLO_IMGPROC_H_ 1

void dark_channel(unsigned char *src, unsigned char *dark_channel, unsigned char *bright_channel, unsigned char *trans_channel, unsigned char *output, int xsize, int ysize, int mask_size) ;
void enhance_naive(unsigned char *src, unsigned char *dest, unsigned char *illum_map, int xsize, int ysize, int mask_size);

#define COLOR_MAX 255
#define COLOR_MID 128

#endif /* _TELLO_IMGPROC_H_ */
