#include "utilities.hpp"

#include <cstring>
#include <iostream>

#include <gtest/gtest.h>

TEST(ENC_Test, TestRandSalt)
{
    auto s1 = enc::rand_salt();
    auto s2 = enc::rand_salt();
    ASSERT_NE(s1, s2);
}

TEST(ENC_Test, TestKeyDerive)
{
    auto s1 = enc::rand_salt();

    auto key = enc::derive_key("Hello", s1, 1);
    auto key2 = enc::derive_key("Hello", s1, 2);
    auto key3 = enc::derive_key("Hello", s1, 1);

    ASSERT_NE(key, key2);
    ASSERT_EQ(key, key3);
}

TEST(ENC_Test, BasicRoundTrip)
{
    auto s1 = enc::rand_salt();

    auto key = enc::derive_key("Hello", s1, 1);

    auto iv = enc::rand_iv();

    std::string secret{"I am Carl the LLAMA!"};
    std::string enc_sec{"\0", secret.size()};
    std::string dec_sec{"\0", secret.size()};
    enc::tag_t tag;

    {
        auto rcode = enc::gcm_encrypt((unsigned char *)secret.data(), secret.size(), nullptr, 0, key.data(), iv.data(), iv.size(), (unsigned char *)enc_sec.data(), tag.data());
        ASSERT_NE(rcode, -1);
        ASSERT_EQ(rcode, enc_sec.size());
    }

    {
        auto rcode = enc::gcm_decrypt((unsigned char *)enc_sec.data(), enc_sec.size(), nullptr, 0, tag.data(), key.data(), iv.data(), iv.size(), (unsigned char *)dec_sec.data());
        ASSERT_NE(rcode, -1);
        ASSERT_EQ(rcode, secret.size());
    }

    ASSERT_EQ(secret, dec_sec);
}

TEST(ENC_Test, TagFailure)
{
    auto s1 = enc::rand_salt();

    auto key = enc::derive_key("Hello", s1, 1);

    auto iv = enc::rand_iv();

    std::string secret{"I am Carl the LLAMA!"};
    std::string enc_sec{"\0", secret.size()};
    std::string dec_sec{"\0", secret.size()};
    enc::tag_t tag;

    {
        auto rcode = enc::gcm_encrypt((unsigned char *)secret.data(), secret.size(), nullptr, 0, key.data(), iv.data(), iv.size(), (unsigned char *)enc_sec.data(), tag.data());
        ASSERT_NE(rcode, -1);
        ASSERT_EQ(rcode, enc_sec.size());
    }

    tag[0] ^= 9;

    {
        auto rcode = enc::gcm_decrypt((unsigned char *)enc_sec.data(), enc_sec.size(), nullptr, 0, tag.data(), key.data(), iv.data(), iv.size(), (unsigned char *)dec_sec.data());
        ASSERT_EQ(rcode, -1);
    }

    tag[0] ^= 9; // XOR self reverses: ((a ^ b) ^ b) == a

    tag.back() ^= 10;
    {
        auto rcode = enc::gcm_decrypt((unsigned char *)enc_sec.data(), enc_sec.size(), nullptr, 0, tag.data(), key.data(), iv.data(), iv.size(), (unsigned char *)dec_sec.data());
        ASSERT_EQ(rcode, -1);
    }
}

TEST(ENC_Test, ImprovedAPI)
{
    auto s1 = enc::rand_salt();

    auto key = enc::derive_key("Hello", s1, 1);

    auto iv = enc::rand_iv();

    auto sec = "I like trains (Everyone knows)";

    auto ev = enc::gcm_encrypt(sec, {}, key, iv);

    ASSERT_TRUE(ev.has_value());

    LOG_INFO("HELLO");

    auto dv = enc::gcm_decrypt(ev.value().first, {}, ev.value().second, key, iv);

    ASSERT_TRUE(dv.has_value());

    ASSERT_EQ(std::string_view(sec), std::string_view(dv.value()));
}