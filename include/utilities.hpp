#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string_view>
#include <vector>
#include <array>
#include <span>
#include <optional>
#include <string>
#include <string_view>

#include "logger.hpp"

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

inline std::optional<std::pair<std::string, std::array<unsigned char, 16>>>
gcm_encrypt(std::string_view plaintext,
            std::string_view aad,
            const std::array<unsigned char, 32> &key,
            const std::array<unsigned char, 12> &iv)
{
    std::string ciphertext;
    ciphertext.resize(plaintext.size());

    std::array<unsigned char, 16> tag{};

    int len = gcm_encrypt(
        reinterpret_cast<unsigned char *>(const_cast<char *>(plaintext.data())),
        static_cast<int>(plaintext.size()),
        reinterpret_cast<unsigned char *>(const_cast<char *>(aad.data())),
        static_cast<int>(aad.size()),
        const_cast<unsigned char *>(key.data()),
        const_cast<unsigned char *>(iv.data()),
        static_cast<int>(iv.size()),
        reinterpret_cast<unsigned char *>(ciphertext.data()),
        tag.data());

    if (len < 0)
        return std::nullopt;

    ciphertext.resize(len);
    return std::make_optional(std::make_pair(std::move(ciphertext), tag));
}

inline std::optional<std::string>
gcm_decrypt(std::string_view ciphertext,
            std::string_view aad,
            const std::array<unsigned char, 16> &tag,
            const std::array<unsigned char, 32> &key,
            const std::array<unsigned char, 12> &iv)
{
    std::string plaintext;
    plaintext.resize(ciphertext.size());

    int len = gcm_decrypt(
        reinterpret_cast<unsigned char *>(const_cast<char *>(ciphertext.data())),
        static_cast<int>(ciphertext.size()),
        reinterpret_cast<unsigned char *>(const_cast<char *>(aad.data())),
        static_cast<int>(aad.size()),
        const_cast<unsigned char *>(tag.data()),
        const_cast<unsigned char *>(key.data()),
        const_cast<unsigned char *>(iv.data()),
        static_cast<int>(iv.size()),
        reinterpret_cast<unsigned char *>(plaintext.data()));

    if (len < 0)
        return std::nullopt;

    plaintext.resize(len);
    return std::make_optional(std::move(plaintext));
}

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