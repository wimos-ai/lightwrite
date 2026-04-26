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

namespace enc
{

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

    using salt_t = std::array<unsigned char, 16>;
    using tag_t = std::array<unsigned char, 16>;
    using iv_t = std::array<unsigned char, 12>;
    using key_t = std::array<unsigned char, 32>;

    inline std::optional<std::pair<std::string, tag_t>>
    gcm_encrypt(std::string_view plaintext,
                std::string_view aad,
                const key_t &key,
                const iv_t &iv)
    {
        std::string ciphertext;
        ciphertext.resize(plaintext.size());

        tag_t tag{};

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
                const tag_t &tag,
                const key_t &key,
                const iv_t &iv)
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

    key_t derive_key(std::string_view password, const salt_t &salt, int iterations = 100000);

    salt_t rand_salt();

    iv_t rand_iv();
};

SDL_Color get_color_negitive(SDL_Color other);

TTF_Font *get_default_font(int font_sz);

TTF_Font *font_from_buffer(const char *buff, size_t bufflen, int font_sz);

// Returns the widest string and its rendered width
std::pair<const char *, int> get_widest_str(TTF_Font *font, std::span<const char *> strs);

std::string getOpenSSLError();

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