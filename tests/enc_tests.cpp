#include "utilities.hpp"

#include <cstring>
#include <iostream>

#include <gtest/gtest.h>

TEST(ENC_Test, TestRandSalt)
{
    auto s1 = rand_salt();
    auto s2 = rand_salt();
    ASSERT_NE(s1, s2);
}

TEST(ENC_Test, TestKeyDerive)
{
    auto s1 = rand_salt();

    auto key = derive_key("Hello", s1, 1);
    auto key2 = derive_key("Hello", s1, 2);
    auto key3 = derive_key("Hello", s1, 1);

    ASSERT_NE(key, key2);
    ASSERT_EQ(key, key3);
}

TEST(ENC_Test, BasicRoundTrip)
{
    auto s1 = rand_salt();

    auto key = derive_key("Hello", s1, 1);

    auto iv = rand_iv();

    std::string secret{"I am Carl the LLAMA!"};
    std::string enc_sec{"\0", secret.size()};
    std::string dec_sec{"\0", secret.size()};
    std::array<unsigned char, 12> tag;

    {
        auto rcode = gcm_encrypt((unsigned char *)secret.data(), secret.size(), nullptr, 0, key.data(), iv.data(), iv.size(), (unsigned char *)enc_sec.data(), tag.data());
        ASSERT_NE(rcode, -1);
        ASSERT_EQ(rcode, enc_sec.size());
    }

    {
        auto rcode = gcm_decrypt((unsigned char *)enc_sec.data(), enc_sec.size(), nullptr, 0, tag.data(), key.data(), iv.data(), iv.size(), (unsigned char *)dec_sec.data());
        ASSERT_NE(rcode, -1);
        ASSERT_EQ(rcode, secret.size());
    }

    ASSERT_EQ(secret, dec_sec);
}