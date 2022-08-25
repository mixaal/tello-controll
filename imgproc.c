#include <stdlib.h>
#include "imgproc.h"


void dark_channel(unsigned char *src, unsigned char *dark_channel, unsigned char *bright_channel, unsigned char *diff_channel, int xsize, int ysize, int mask_size) 
{
    int ms = mask_size / 2;
    int off;
    for (int y = 0; y< ysize; y++) {
      for (int x = 0; x< xsize; x++) {
            int _min_val = 1000;
            int _max_val = -1;
            for(int mx=x-ms; mx<x+ms; mx++) {
              for(int my=y-ms; my<y+ms; my++) {
                  if(mx<0 || mx>=xsize) continue;
                  if(my<0 || my>=ysize) continue;
                  off = 3*(xsize * my + mx);
                  int r = src[off];
                  int g = src[off + 1];
                  int b = src[off + 2];
                  if(r<_min_val) _min_val = r;
                  if(g<_min_val) _min_val = g;
                  if(b<_min_val) _min_val = b;

                  if(r>_max_val) _max_val = r;
                  if(g>_max_val) _max_val = g;
                  if(b>_max_val) _max_val = b;
               }
            } 
	    off = 3*(xsize * y + x);
            if(dark_channel!=NULL) {
              dark_channel[off] = _min_val;
              dark_channel[off + 1] = _min_val;
              dark_channel[off + 2] = _min_val;
            }
            if(bright_channel!=NULL) {
              bright_channel[off] = _max_val;
              bright_channel[off + 1] = _max_val;
              bright_channel[off + 2] = _max_val;
            }
            if(diff_channel!=NULL) {
              diff_channel[off] = _max_val - _min_val;
              diff_channel[off + 1] = _max_val - _min_val;
              diff_channel[off + 2] = _max_val - _max_val;
            }
      }
    }
}


void enhance_naive(unsigned char *src, unsigned char *dest, unsigned char *illum_map, int xsize, int ysize, int mask_size)
{
    int off;
    for (int y = 0; y< ysize; y++) {
      for (int x = 0; x< xsize; x++) {
	    off = 3*(xsize * y + x);
  
            int r = src[off];
            int g = src[off + 1];
            int b = src[off + 2];

            int _max = r;
            if(g>_max) _max=g;
            if(b>_max) _max=b;

            if(illum_map!=NULL) {
              illum_map[off] = _max;
              illum_map[off + 1] = _max;
              illum_map[off + 2] = _max;
            }
            if(_max<1) _max=1;
            float _fm = _max / 255.0f;
            if(dest!=NULL) {
              r /= _fm;
              g /= _fm;
              b /= _fm;
              if(r<0) r=0;
              if(g<0) g=0;
              if(b<0) b=0;
              if(r>255) r=255;
              if(g>255) g=255;
              if(b>255) b=255;
              dest[off] = r ;
              dest[off + 1] = g ;
              dest[off + 2] = b ;
            }
      }
    }
}

