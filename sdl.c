#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sdl.h"
#include "utils.h"

static SDL_Window *win = NULL;
static SDL_Renderer *ren = NULL;

static int screenWidth = -1;
static int screenHeight = -1;

static Uint8 getalpha(SDL_Surface *surface, int x, int y) ;

#define min(a,b) ((a)>(b)) ? (b) : (a)
#define max(a,b) ((a)>(b)) ? (a) : (b)

void sdl_setup(const char *title, int w, int h)
{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0){
		fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
		exit(-1);
	}

        if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0)
         {
           printf("Error initializing SDL_mixer: %s\n", Mix_GetError());
            exit(-1);
          }
   
        int result;
        int audio_flags = MIX_INIT_OGG;
        if (audio_flags!= (result = Mix_Init(audio_flags))) {
           printf("Could not initialize mixer (result: %d).\n", result);
           printf("Mix_Init: %s\n", Mix_GetError());
           exit(-1);
        }

	// load support for the JPG and PNG image formats
	int flags=IMG_INIT_JPG|IMG_INIT_PNG;
	int initted=IMG_Init(flags);
	if((initted&flags) != flags) {
    		printf("IMG_Init: Failed to init required jpg and png support!\n");
    		printf("IMG_Init: %s\n", IMG_GetError());
                exit(-1);
	}	

        win = SDL_CreateWindow(title, 100, 100, w, h, SDL_WINDOW_SHOWN);
        if (win == NULL){
	    fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
	    SDL_Quit();
	    exit(-1);
        }

        screenWidth = w;
        screenHeight = h;
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL){
		SDL_DestroyWindow(win);
	        fprintf(stderr, "SDL_CreateRenderer Error: %s\n" , SDL_GetError());
	        SDL_Quit();
                exit(-1);
        }
}

void sdl_update(render_func_t render_func, kbd_func_t keyboard_down, kbd_func_t keyboard_up)
{
SDL_Event e;
bool quit = 0;
while (!quit){
    while (SDL_PollEvent(&e)){
        if (e.type == SDL_QUIT){
            quit = 1;
        }
        if (e.type == SDL_KEYDOWN){
              keyboard_down(e.key.keysym.sym);
        }
        if (e.type == SDL_KEYUP){
              keyboard_up(e.key.keysym.sym);
        }
        if (e.type == SDL_MOUSEBUTTONDOWN){
            quit = 1;
        }
    }

    SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(ren);
    render_func();
    SDL_RenderPresent(ren);
  }
}

void sdl_display(void)
{
    SDL_RenderPresent(ren);
}

void sdl_clear(void)
{
  SDL_RenderClear(ren);
}

void sdl_draw_texture(sprite_t *sprite, int x, int y, double angle, double scaleX, double scaleY, float sprite_alpha)
{
SDL_Rect dst;
        if(!sprite->show) return; 
	sprite->x = dst.x = x;
	sprite->y = dst.y = y;
        dst.w = sprite->width;
        dst.h = sprite->height;
        dst.w = (int)floor(dst.w*scaleX);
        dst.h = (int)floor(dst.h*scaleY);
	//SDL_RenderCopy(ren, tex, NULL, &dst);
        if(sprite_alpha<0) sprite_alpha=0;
        if(sprite_alpha>1) sprite_alpha=1;
        if(sprite_alpha>=0 && sprite_alpha<1.0) {
          SDL_SetTextureAlphaMod(sprite->tex, 255.0f * sprite_alpha);
        }
        SDL_RenderCopyEx( ren, sprite->tex, NULL, &dst, angle, NULL, SDL_FLIP_NONE );
}

void sdl_set_texture(sprite_t *sprite, SDL_Texture *tex)
{
   sprite -> tex = tex;
}

sprite_t sdl_load_texture(const char *path)
{
  sprite_t sprite;
  SDL_Surface *bmp = IMG_Load(path);
  if (bmp == NULL){
        if(ren!=NULL) SDL_DestroyRenderer(ren);
	if(win!=NULL) SDL_DestroyWindow(win);
	fprintf(stderr, "SDL_LoadBMP Error: %s\n", SDL_GetError());
	SDL_Quit();
	exit(-1);
  }

  int bpp = bmp->format->BytesPerPixel;
  printf("%s bpp=%d\n", path, bpp);
  SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
  if (tex == NULL){
        if(ren!=NULL) SDL_DestroyRenderer(ren);
	if(win!=NULL) SDL_DestroyWindow(win);
	fprintf(stderr,  "SDL_CreateTextureFromSurface Error: %s",SDL_GetError());
	SDL_Quit();
	exit(-1);
  }

  SDL_QueryTexture(tex, NULL, NULL, &sprite.width, &sprite.height);
  Uint8 *mask = (Uint8 *)xmalloc(sprite.width * sprite.height * sizeof(Uint8));
  for(int i=0; i<sprite.width; i++) 
    for(int j=0; j<sprite.height; j++)  {
      mask[i+j*sprite.width] = getalpha(bmp, i, j);
    }
  SDL_FreeSurface(bmp);
  sprite.collision_mask =  mask;
  sprite.tex = tex;
  sprite.x = sprite.y = 0;
  sprite.show = 1;
  sprite.enable_collision = 1;
  return sprite;
}

void sdl_enable_collision(sprite_t *s)
{
  s->enable_collision = 1;
}


void sdl_disable_collision(sprite_t *s)
{
  s->enable_collision = 0;
}

bool sdl_is_collision(sprite_t *s1, sprite_t *s2, float scale1, float scale2)
{
    if(!s1->enable_collision || !s2->enable_collision) return 0;
    Uint8 *mask1 = s1 -> collision_mask;
    Uint8 *mask2 = s2 -> collision_mask;
    int w1 = s1->width * scale1;
    int w2 = s2->width * scale2;
    int h1 = s1->height * scale1;
    int h2 = s2->height * scale2;

    int top = max(s1->y, s2->y);
    int bottom = min(s1->y+h1, s2->y+h2);
    int left = max(s1->x, s2->x);
    int right = min(s1->x+w1, s2->x+w2);

    int i1, i2;
    float d1 = 1.0/scale1;
    float d2 = 1.0/scale2;
    int  x1, y1, x2, y2;
    for (int y=top; y<bottom; y++) {
      for (int x=left; x<right; x++) {
          y1 = (y-s1->y) * d1;
          y2 = (y-s2->y) * d2;

          x1 = (x-s1->x) * d1;
          x2 = (x-s2->x) * d2;

          i1 = y1  * s1->width + x1;
          i2 = y2  * s2->width + x2;
          if(mask1[i1]>128 && mask2[i2]>128) return 1;
      }
    }
    return 0;
}

void sdl_draw_horiz_gradient(int x, int y, int w, int h, int src_r, int src_g, int src_b, int dst_r, int dst_g, int dst_b, int count, int percentage)
{
    int dx = w/count;
    float dr = dst_r - src_r;
    float dg = dst_g - src_g;
    float db = dst_b - src_b;
    
    dr /= (float)count;
    dg /= (float)count;
    db /= (float)count;

    float r = src_r, g = src_g, b=src_b;
    int p_stop = count * percentage / 100;
    int cnt = 0;
    for(int xx=x; xx<x+w; xx+=dx) {
       //printf("xx=%d dx=%d cnt=%d p_stop=%d\n", xx, dx, cnt, p_stop);
       if(cnt++ > p_stop) return;
       sdl_draw_rect(xx, y, dx, h, r, g, b);
       r+=dr;
       g+=dg;
       b+=db;
    }
}


/**
 * Create texture for continous streaming.
 */
SDL_Texture *create_texture(int w, int h)
{
	SDL_Texture* tex = SDL_CreateTexture
        (
        ren,
        //SDL_PIXELFORMAT_IYUV,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        w, h
        );
    if( !tex ) {
        fprintf(stderr, "Can't create texture: %s\n", SDL_GetError() );
	exit(-1);
    }
    return tex;
}

static SDL_Rect scale_aspect( int src_w, int src_h, int dst_w, int dst_h )
{
    SDL_Rect ret;
    ret.w = dst_w;
    ret.h = dst_h;

    double srcRatio = (double)src_w / src_h;
    double dstRatio = (double)dst_w / dst_h;
    if( srcRatio > dstRatio )
        ret.h = dst_w / srcRatio;
    else
        ret.w = dst_h * srcRatio;

    ret.x = (dst_w - ret.w) / 2;
    ret.y = (dst_h - ret.h) / 2;

    return ret;
}

void sdl_render_texture(SDL_Texture *tex, unsigned char *data, int src_w, int src_h, int dst_w, int dst_h)
{
 SDL_UpdateTexture(tex, NULL, data, src_w * SDL_BYTESPERPIXEL(SDL_PIXELFORMAT_RGB24) );
 //printf("redner_tex sw=%d sh=%d dw=%d dh=%d\n", src_w, src_h, dst_w, dst_h);
 SDL_Rect s = scale_aspect(src_w, src_h, dst_w, dst_h );
 //printf("s=[%d %d %d %d]\n", s.x, s.y, s.w, s.h);
 SDL_SetRenderDrawColor( ren, 255, 0, 0, 255 );
 SDL_RenderFillRect( ren, &s );
 SDL_RenderCopy( ren, tex, NULL, &s );
}

void sdl_line(int x1, int y1, int x2, int y2, int r, int g, int b)
{
  SDL_SetRenderDrawColor(ren, r, g, b, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(ren, x1, y1, x2, y2);
}
void sdl_circle(int32_t centreX, int32_t centreY, int32_t radius, int r, int g, int b)
{

   const int32_t diameter = (radius * 2);

   int32_t x = (radius - 1);
   int32_t y = 0;
   int32_t tx = 1;
   int32_t ty = 1;
   int32_t error = (tx - diameter);

  SDL_SetRenderDrawColor(ren, r, g, b, SDL_ALPHA_OPAQUE);
   while (x >= y)
   {
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(ren, centreX + x, centreY - y);
      SDL_RenderDrawPoint(ren, centreX + x, centreY + y);
      SDL_RenderDrawPoint(ren, centreX - x, centreY - y);
      SDL_RenderDrawPoint(ren, centreX - x, centreY + y);
      SDL_RenderDrawPoint(ren, centreX + y, centreY - x);
      SDL_RenderDrawPoint(ren, centreX + y, centreY + x);
      SDL_RenderDrawPoint(ren, centreX - y, centreY - x);
      SDL_RenderDrawPoint(ren, centreX - y, centreY + x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}

void sdl_draw_rect(int x, int y, int w, int h, int r, int g, int b)
{
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;
  SDL_SetRenderDrawColor(ren, r, g, b, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(ren, &rect);
} 

Mix_Chunk *sdl_load_sfx(const char *path)
{
  Mix_Chunk *chunk = Mix_LoadWAV(path);

  if(chunk==NULL) {
    printf("Mix_Init: %s\n", Mix_GetError());
    exit(-1);
 }
  return chunk;
}

void sdl_play_sfx(Mix_Chunk *chunk)
{
 if(chunk!=NULL) Mix_PlayChannel(-1, chunk, 0);
}

void sdl_play_music(const char *path)
{
  Mix_Music *music = Mix_LoadMUS(path); 
  if(music==NULL) {
    printf("Mix_Init: %s\n", Mix_GetError());
    exit(-1);
 }

  Mix_PlayMusic(music, -1);
}

void sdl_battery_status(int power)
{
	if(power<0) power = 0;
	if(power>100) power = 100;
	int w=110, h=40;
	if(screenWidth<800 && screenHeight<600) {
		w = 50; h= 20;
	}

	sdl_draw_rect(10, 10, w, h, 0, 0, 255);
	int r = 255;
	int g=  0;
	int b = 0;
	if(power>30) {
		r = g= 255;
		b = 0;
	}
	if(power>60) {
		r = b= 0;
		g= 255;
	}
	int ex = power * (w-10)/100;
	sdl_draw_rect(15, 15, ex, h-10, r, g, b);
	sdl_draw_rect(10+w, 10+h/4, h/5, h/2, 0, 0, 255);
}


void sdl_quit(void)
{
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  IMG_Quit();
  SDL_Quit();
}


static Uint8 getalpha(SDL_Surface *surface, int x, int y) 
{
    int bpp = surface->format->BytesPerPixel;
    float Iv;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
    case 1:
        return *p;

    case 2:
        return p[0];

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            Iv =  0.3f*p[0] + 0.6f*p[1] +  0.1f*p[2];
        else
            Iv =  0.1f*p[0] + 0.6f*p[1] + 0.3f*p[2];
        return (Uint8)floor(Iv);

   case 4:
        return p[3];

   default:
        return 0;       /* shouldn't happen, but avoids warnings */
      }
}

#if 0
static Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;


    switch (bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
            break;

    case 4:
            return *(Uint32 *)p;
            break;

        default:
            return 0;       /* shouldn't happen, but avoids warnings */
      }
}
#endif
