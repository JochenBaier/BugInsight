// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <array>
#include <cassert>

#include <Windows.h>

#include "utest.h"
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/types.h>

#include "utest.h"

extern size_t buginsight_internal_allocated_bytes(const char* p_function);


struct openssl_tests
{
};

UTEST_F_SETUP(openssl_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(openssl_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(openssl_tests, Basic_exit_0)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));

  int ret=OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_CIPHERS | OPENSSL_INIT_ADD_ALL_DIGESTS, NULL);
  assert(ret == 1);
  ret=OPENSSL_init_ssl(OPENSSL_INIT_SSL_DEFAULT, NULL);
  assert(ret == 1);

  size_t allocted_bytes=buginsight_internal_allocated_bytes("malloc");
  ASSERT_EQ(allocted_bytes, 58676);

  BIO* bio=BIO_new(BIO_s_mem());

  allocted_bytes=buginsight_internal_allocated_bytes("malloc");
  ASSERT_EQ(allocted_bytes, 58884);

  BIO_free(bio);

  allocted_bytes=buginsight_internal_allocated_bytes("malloc");
  ASSERT_EQ(allocted_bytes, 58676);

  SSL_CTX* ctx=SSL_CTX_new(TLS_method());
  ASSERT_TRUE(ctx);

  allocted_bytes=buginsight_internal_allocated_bytes("malloc");
  ASSERT_EQ(allocted_bytes, 254444);

  SSL_CTX_free(ctx);

  allocted_bytes=buginsight_internal_allocated_bytes("malloc");
  ASSERT_EQ(allocted_bytes, 247986);

  OPENSSL_cleanup();

  allocted_bytes=buginsight_internal_allocated_bytes("malloc");
  ASSERT_EQ(allocted_bytes, 0);
}
