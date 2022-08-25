#ifndef _TELLO_IMGPROC_H_
#define _TELLO_IMGPROC_H_ 1

void dark_channel(unsigned char *src, unsigned char *dark_channel, unsigned char *bright_channel, unsigned char *diff_channel, int xsize, int ysize, int mask_size) ;
void enhance_naive(unsigned char *src, unsigned char *dest, unsigned char *illum_map, int xsize, int ysize, int mask_size);

#endif /* _TELLO_IMGPROC_H_ */
