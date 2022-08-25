#include <stdlib.h>
#include <stdio.h>
#include "imgproc.h"


void dark_channel(unsigned char *src, unsigned char *dark_channel, unsigned char *bright_channel, unsigned char *trans_channel, int xsize, int ysize, int mask_size) 
{
    int ms = mask_size / 2;
    int off;
    float  Ac = 20;
    float minV = -Ac / (255.0f - Ac);

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
            if(trans_channel!=NULL) {
              float tx = (float)(_max_val - Ac) / (255.0f -Ac);
              tx -= minV;
              tx /= (1-minV);
              tx *=255;
              trans_channel[off] = tx;
              trans_channel[off + 1] = tx;
              trans_channel[off + 2] = tx;
            }
      }
    }
}


void enhance_naive(unsigned char *src, unsigned char *dest, unsigned char *illum_map, int xsize, int ysize, int mask_size)
{
    int off;
    int max_I = -1;
    int min_I = 1000;

    for (int y = 0; y< ysize; y++) {
      for (int x = 0; x< xsize; x++) {
	    off = 3*(xsize * y + x);
  
            int r = src[off];
            int g = src[off + 1];
            int b = src[off + 2];
            int I = 0.3f * r + 0.6f * g + 0.1f* b;
	    if(max_I<I) max_I = I;
	    if(min_I>I) min_I = I;

            int _max = r;
            if(g>_max) _max=g;
            if(b>_max) _max=b;

            if(illum_map!=NULL) {
              illum_map[off] = _max;
              illum_map[off + 1] = _max;
              illum_map[off + 2] = _max;
            }
      }
    }


     // map min_r -> 0, max_r -> 255.0f
     // new_r  = (255.0f / (max_r - minr)) * (r - minr)
     
     int Id = max_I - min_I;
  
     printf("Id=%d max_I=%d min_I=%d\n", Id, max_I, min_I);
     for (int y = 0; y< ysize; y++) {
      for (int x = 0; x< xsize; x++) {
	    off = 3*(xsize * y + x);
            int r = src[off];
            int g = src[off + 1];
            int b = src[off + 2];
            int I = 0.3f * r + 0.6f * g + 0.1f* b;
            if(Id!=0) I = (255.0f / Id) * (I-min_I);

            if(dest!=NULL) {
              dest [off] = I;
              dest[off + 1] = I;
              dest[off + 2] = I;
            }

      }
    }

}

