#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string_view>
#include <vector>
#include <array>

int gcm_decrypt(unsigned char *ciphertext, int ciphertext_len,
                unsigned char *aad, int aad_len,
                unsigned char *tag,
                unsigned char *key,
                unsigned char *iv, int iv_len,
                unsigned char *plaintext);

int gcm_encrypt(unsigned char *plaintext, int plaintext_len,
                unsigned char *aad, int aad_len,
                unsigned char *key,
                unsigned char *iv, int iv_len,
                unsigned char *ciphertext,
                unsigned char *tag);

std::array<unsigned char, 32> derive_key(std::string_view password, std::array<unsigned char, 16> &salt, int iterations = 100000);

std::array<unsigned char, 16> rand_salt();

std::array<unsigned char, 12> rand_iv();

SDL_Color get_color_negitive(SDL_Color other);

class RasterizedTextInfo
{
public:
    RasterizedTextInfo(TTF_Font *font, SDL_Renderer *renderer, const char *text, SDL_Color color);
    RasterizedTextInfo(TTF_Font *font, SDL_Renderer *renderer, std::string_view sv, SDL_Color color);
    ~RasterizedTextInfo();

public:
    void render(SDL_Renderer *renderer, int x, int y);

public:
    SDL_Texture *tx;
    int w;
    int h;

public:
    static const char *sv_fwd_helper(std::string_view sv);
};