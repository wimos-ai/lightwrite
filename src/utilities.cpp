#include "utilities.hpp"
#include "logger.hpp"

#include <array>
#include <cstring>

#include <openssl/rand.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

SDL_Color get_color_negitive(SDL_Color other)
{
    return SDL_Color{255 - other.r, 255 - other.g, 255 - other.b, other.a};
}

RasterizedTextInfo::RasterizedTextInfo(TTF_Font *font, SDL_Renderer *renderer, const char *text, SDL_Color color)
{

    LOG_TTF_ERROR(TTF_SizeUTF8(font, text, &this->w, &this->h));
    SDL_Surface *title_s = TTF_RenderUTF8_Blended(font, text, color);
    if (!title_s)
    {
        LOG_ERROR("TTF_RenderUTF8_Blended: %s", TTF_GetError());
    }

    this->tx = SDL_CreateTextureFromSurface(renderer, title_s);
    if (!tx)
    {
        LOG_ERROR("SDL_CreateTextureFromSurface: %s", TTF_GetError());
    }

    SDL_FreeSurface(title_s);
}

RasterizedTextInfo::RasterizedTextInfo(TTF_Font *font, SDL_Renderer *renderer, std::string_view sv, SDL_Color color) : RasterizedTextInfo(font, renderer, RasterizedTextInfo::sv_fwd_helper(sv), color)
{
}

RasterizedTextInfo::~RasterizedTextInfo()
{
    SDL_DestroyTexture(this->tx);
}

void RasterizedTextInfo::render(SDL_Renderer *renderer, int x, int y)
{
    SDL_Rect r{x, y, this->w, this->h};
    LOG_SDL_ERROR(SDL_RenderCopy(renderer, this->tx, nullptr, &r));
}

const char *RasterizedTextInfo::sv_fwd_helper(std::string_view sv)
{
    // HERE BE DRAGONS!
    static thread_local char *chars;
    static thread_local size_t char_sz;

    if (chars == nullptr)
    {
        chars = (char *)std::malloc(sv.size() + 1);
        char_sz = sv.size() + 1;
    }

    if (char_sz < sv.size() + 1)
    {
        std::memset(chars, 0, char_sz);
        char_sz = sv.size() + 1;
        chars = (char *)std::realloc(chars, char_sz);
    }
    memcpy(chars, sv.data(), sv.size());
    chars[sv.size()] = 0;
    return chars;
}

// From: https://wiki.openssl.org/index.php/EVP_Authenticated_Encryption_and_Decryption#Authenticated_Decryption_using_GCM_mode
int gcm_encrypt(unsigned char *plaintext, int plaintext_len,
                unsigned char *aad, int aad_len,
                unsigned char *key,
                unsigned char *iv, int iv_len,
                unsigned char *ciphertext,
                unsigned char *tag)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
    {
        LOG_ERROR("OPENSSL: EVP_CIPHER_CTX_new");
        return -1;
    }

    /* Initialise the encryption operation. */
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
    {
        LOG_ERROR("OPENSSL: EVP_EncryptInit_ex");
        return -1;
    }

    /*
     * Set IV length if default 12 bytes (96 bits) is not appropriate
     */
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL))
    {
        LOG_ERROR("OPENSSL: EVP_CIPHER_CTX_ctrl");
        return -1;
    }

    /* Initialise key and IV */
    if (1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv))
    {
        LOG_ERROR("OPENSSL: EVP_EncryptInit_ex");
        return -1;
    }

    /*
     * Provide any AAD data. This can be called zero or more times as
     * required
     */
    if (1 != EVP_EncryptUpdate(ctx, NULL, &len, aad, aad_len))
    {
        LOG_ERROR("OPENSSL: EVP_EncryptUpdate");
        return -1;
    }

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    {
        LOG_ERROR("OPENSSL: EVP_EncryptUpdate");
        return -1;
    }
    ciphertext_len = len;

    /*
     * Finalise the encryption. Normally ciphertext bytes may be written at
     * this stage, but this does not occur in GCM mode
     */
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
    {
        LOG_ERROR("OPENSSL: EVP_EncryptFinal_ex");
        return -1;
    }
    ciphertext_len += len;

    /* Get the tag */
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag))
    {
        LOG_ERROR("OPENSSL: EVP_CIPHER_CTX_ctrl");
        return -1;
    }

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

// From: https://wiki.openssl.org/index.php/EVP_Authenticated_Encryption_and_Decryption#Authenticated_Decryption_using_GCM_mode
int gcm_decrypt(unsigned char *ciphertext, int ciphertext_len,
                unsigned char *aad, int aad_len,
                unsigned char *tag,
                unsigned char *key,
                unsigned char *iv, int iv_len,
                unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    int ret;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
    {
        LOG_ERROR("OPENSSL: EVP_CIPHER_CTX_new");
        return -1;
    }

    /* Initialise the decryption operation. */
    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
    {
        LOG_ERROR("OPENSSL: EVP_DecryptInit_ex");
        return -1;
    }

    /* Set IV length. Not necessary if this is 12 bytes (96 bits) */
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL))
    {
        LOG_ERROR("OPENSSL: EVP_CIPHER_CTX_ctrl");
        return -1;
    }

    /* Initialise key and IV */
    if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv))
    {
        LOG_ERROR("OPENSSL: EVP_DecryptInit_ex");
        return -1;
    }

    /*
     * Provide any AAD data. This can be called zero or more times as
     * required
     */
    if (!EVP_DecryptUpdate(ctx, NULL, &len, aad, aad_len))
    {
        LOG_ERROR("OPENSSL: EVP_DecryptUpdate");
        return -1;
    }

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    if (!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    {
        LOG_ERROR("OPENSSL: EVP_DecryptUpdate");
        return -1;
    }
    plaintext_len = len;

    /* Set expected tag value. Works in OpenSSL 1.0.1d and later */
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag))
    {
        LOG_ERROR("OPENSSL: EVP_CIPHER_CTX_ctrl");
        return -1;
    }

    /*
     * Finalise the decryption. A positive return value indicates success,
     * anything else is a failure - the plaintext is not trustworthy.
     */
    ret = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    if (ret > 0)
    {
        /* Success */
        plaintext_len += len;
        return plaintext_len;
    }
    else
    {
        /* Verify failed */
        return -1;
    }
}

std::array<unsigned char, 16> rand_salt()
{
    std::array<unsigned char, 16> arr;
    if (RAND_bytes(arr.data(), arr.size()) != 1)
    {
        LOG_FATAL("OPENSSL: RAND_bytes");
        exit(-1);
    }
    return arr;
}

std::array<unsigned char, 12> rand_iv()
{
    std::array<unsigned char, 12> arr;
    if (RAND_bytes(arr.data(), arr.size()) != 1)
    {
        LOG_FATAL("OPENSSL: RAND_bytes");
        exit(-1);
    }
    return arr;
}

std::array<unsigned char, 32> derive_key(std::string_view password, std::array<unsigned char, 16> &salt, int iterations )
{

    std::array<unsigned char, 32> key;

    if (PKCS5_PBKDF2_HMAC(password.data(), password.size(),
                          salt.data(), salt.size(),
                          iterations,
                          EVP_sha256(), // Digest Algorithm
                          key.size(), key.data()) != 1)
    {
        LOG_FATAL("OPENSSL: PKCS5_PBKDF2_HMAC");
        exit(-1);
    }
    return key;
}