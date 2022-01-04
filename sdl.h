#ifndef __NN_SDL_H__
#define __NN_SDL_H__ 1

#include "utils.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

typedef void (*render_func_t)(void) ;
typedef void (*kbd_func_t)(SDL_Keycode key) ;

typedef struct {
  Uint8 *collision_mask;
  SDL_Texture *tex;
  int width, height, x, y;
  bool show;
  bool enable_collision;
} sprite_t;

void sdl_setup(const char *title, int w, int h);
void sdl_quit(void);
void sdl_display(void);
void sdl_update(render_func_t render_func, kbd_func_t keyboard_down, kbd_func_t keyboard_up);
void sdl_clear(void);
void sdl_draw_texture(sprite_t *sprite, int x, int y, double angle, double scaleX, double scaleY, float sprite_alpha);
sprite_t sdl_load_texture(const char *path);
void sdl_enable_collision(sprite_t *s);
void sdl_disable_collision(sprite_t *s);
bool sdl_is_collision(sprite_t *s1, sprite_t *s2, float scale1, float scale2);
void sdl_set_texture(sprite_t *sprite, SDL_Texture *tex);
void sdl_circle(int32_t centreX, int32_t centreY, int32_t radius, int r, int g, int b);
void sdl_line(int x1, int y1, int x2, int y2, int r, int g, int b);
void sdl_draw_horiz_gradient(int x, int y, int w, int h, int src_r, int src_g, int src_b, int dst_r, int dst_g, int dst_b, int count, int percentage);
void sdl_draw_rect(int x, int y, int w, int h, int r, int g, int b);
Mix_Chunk *sdl_load_sfx(const char *path);
void sdl_play_sfx(Mix_Chunk *chunk);
void sdl_play_music(const char *path);
void sdl_render_texture(SDL_Texture *tex, unsigned char *data, int src_w, int src_h, int dst_w, int dst_h);
SDL_Texture *create_texture(int w, int h);
void sdl_battery_status(int power);
void sdl_wifi_signal_strength(int strength);
void sdl_draw_box(int x, int y, int w, int h, int r, int g, int b);
#endif /* __NN_SDL_H__ */
