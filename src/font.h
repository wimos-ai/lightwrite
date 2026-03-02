#ifndef FONT_H
#define FONT_H

#ifdef __cplusplus
extern "C"{
#endif


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct _Font_Data_ {
	const char *text;

	SDL_Texture *texture;
	SDL_Rect rect;

	int font_w;
	int font_h;
} Font_Data;

Font_Data prepare_string(TTF_Font *font, SDL_Renderer *renderer, int x, int y,
    const char *string, SDL_Color color);

#ifdef __cplusplus
}
#endif


#endif // !FONT_H
