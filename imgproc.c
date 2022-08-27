#include <stdlib.h>
#include <stdio.h>
#include "imgproc.h"


void dark_channel(unsigned char *src, unsigned char *dark_channel, unsigned char *bright_channel, unsigned char *trans_channel, unsigned char *output, int xsize, int ysize, int mask_size) 
{
    int ms = mask_size / 2;
    int off;
    //float  Ac = 170.0f;
    //float minV = -Ac / (255.0f - Ac);

    int global_max = -1;
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
                    float I = 0.3f*r + 0.6f*g +0.1f*b;
                    if(global_max<I) global_max = I;
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
#if 0
                Ac = global_max * 0.9f;
                float tx = (float)(_max_val - Ac) / (255.0f -Ac);
                tx -= minV;
                tx /= (1-minV);
                tx *=255;
#endif
                trans_channel[off] = _max_val - _min_val;
                trans_channel[off + 1] = _max_val - _min_val;
                trans_channel[off + 2] = _max_val - _min_val;
            }
            if(output!=NULL) {
                int R = src[off];
                int G = src[off + 1];
                int B = src[off + 2];
                int diff = _max_val - _min_val;
                float c=80.0f;
                float br=COLOR_MAX - _max_val;
                br *= 0.4f;
                float F = (259 * (c + COLOR_MAX)) / (COLOR_MAX * ( 259 - c ));
                float JR = R-COLOR_MID;
                float JG = G-COLOR_MID;
                float JB = B-COLOR_MID;
                JR = F * JR + COLOR_MID + br;
                JG = F * JG + COLOR_MID + br;
                JB = F * JB + COLOR_MID + br;
                if(JR>255) JR=255;
                if(JG>255) JG=255;
                if(JB>255) JB=255;
                if(JR<0) JR=0;
                if(JG<0) JG=0;
                if(JB<0) JB=0;
                //     printf("JR=%d JG=%d JB=%d\n", JR, JG, JB);
                output[off]   = (diff<COLOR_MID) ? JR : R;
                output[off+1] = (diff<COLOR_MID) ? JG : G;
                output[off+2] = (diff<COLOR_MID) ? JB : B;
#if 0
                if(diff<COLOR_MID) {
                output[off]   = 0;
                output[off+1] = 0;
                output[off+2] = 255;
                }
#endif
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

