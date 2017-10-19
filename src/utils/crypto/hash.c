/*****************************************************************************
 * Copyright (C) Queen's University Belfast, ECIT, 2016                      *
 *                                                                           *
 * This file is part of libsafecrypto.                                       *
 *                                                                           *
 * This file is subject to the terms and conditions defined in the file      *
 * 'LICENSE', which is part of this source code package.                     *
 *****************************************************************************/

/*
 * Git commit information:
 *   Author: $SC_AUTHOR$
 *   Date:   $SC_DATE$
 *   Branch: $SC_BRANCH$
 *   Id:     $SC_IDENT$
 */

#include "hash.h"
#include "sha3/tinysha3.h"
#include "sha2/sha2_safecrypto.h"
#include "blake2/blake2_safecrypto.h"
#include "whirlpool/whirlpool.h"



utils_crypto_hash_t * utils_crypto_hash_create(crypto_hash_e type)
{
    utils_crypto_hash_t *crypto_hash = PRNG_MALLOC(sizeof(utils_crypto_hash_t));

    switch (type)
    {
#ifdef ENABLE_SHA3
        case CRYPTO_HASH_SHA3_512:
        {
            crypto_hash->type   = CRYPTO_HASH_SHA3_512;
            crypto_hash->length = 64;
            crypto_hash->init   = tinysha3_init;
            crypto_hash->update = tinysha3_update;
            crypto_hash->final  = tinysha3_final;
            crypto_hash->ctx    = PRNG_MALLOC(sizeof(sha3_ctx_t));
        } break;

        case CRYPTO_HASH_SHA3_384:
        {
            crypto_hash->type   = CRYPTO_HASH_SHA3_384;
            crypto_hash->length = 48;
            crypto_hash->init   = tinysha3_init;
            crypto_hash->update = tinysha3_update;
            crypto_hash->final  = tinysha3_final;
            crypto_hash->ctx    = PRNG_MALLOC(sizeof(sha3_ctx_t));
        } break;

        case CRYPTO_HASH_SHA3_256:
        {
            crypto_hash->type   = CRYPTO_HASH_SHA3_256;
            crypto_hash->length = 32;
            crypto_hash->init   = tinysha3_init;
            crypto_hash->update = tinysha3_update;
            crypto_hash->final  = tinysha3_final;
            crypto_hash->ctx    = PRNG_MALLOC(sizeof(sha3_ctx_t));
        } break;

        case CRYPTO_HASH_SHA3_224:
        {
            crypto_hash->type   = CRYPTO_HASH_SHA3_256;
            crypto_hash->length = 28;
            crypto_hash->init   = tinysha3_init;
            crypto_hash->update = tinysha3_update;
            crypto_hash->final  = tinysha3_final;
            crypto_hash->ctx    = PRNG_MALLOC(sizeof(sha3_ctx_t));
        } break;
#endif

#ifdef ENABLE_SHA2
        case CRYPTO_HASH_SHA2_512:
        {
            crypto_hash->type   = CRYPTO_HASH_SHA2_512;
            crypto_hash->length = 64;
            crypto_hash->init   = sc_sha2_init;
            crypto_hash->update = sc_sha2_update;
            crypto_hash->final  = sc_sha2_final;
            crypto_hash->ctx    = PRNG_MALLOC(sizeof(sha2_ctx));
        } break;

        case CRYPTO_HASH_SHA2_384:
        {
            crypto_hash->type   = CRYPTO_HASH_SHA2_384;
            crypto_hash->length = 48;
            crypto_hash->init   = sc_sha2_init;
            crypto_hash->update = sc_sha2_update;
            crypto_hash->final  = sc_sha2_final;
            crypto_hash->ctx    = PRNG_MALLOC(sizeof(sha2_ctx));
        } break;

        case CRYPTO_HASH_SHA2_256:
        {
            crypto_hash->type   = CRYPTO_HASH_SHA2_256;
            crypto_hash->length = 32;
            crypto_hash->init   = sc_sha2_init;
            crypto_hash->update = sc_sha2_update;
            crypto_hash->final  = sc_sha2_final;
            crypto_hash->ctx    = PRNG_MALLOC(sizeof(sha2_ctx));
        } break;

        case CRYPTO_HASH_SHA2_224:
        {
            crypto_hash->type   = CRYPTO_HASH_SHA2_224;
            crypto_hash->length = 28;
            crypto_hash->init   = sc_sha2_init;
            crypto_hash->update = sc_sha2_update;
            crypto_hash->final  = sc_sha2_final;
            crypto_hash->ctx    = PRNG_MALLOC(sizeof(sha2_ctx));
        } break;
#endif

#ifdef ENABLE_BLAKE2
        case CRYPTO_HASH_BLAKE2_512:
        {
            crypto_hash->type   = CRYPTO_HASH_BLAKE2_512;
            crypto_hash->length = 64;
            crypto_hash->init   = sc_blake2b_init;
            crypto_hash->update = sc_blake2b_update;
            crypto_hash->final  = sc_blake2b_final;
            crypto_hash->ctx    = PRNG_MALLOC(sizeof(blake2b_state));
        } break;

        case CRYPTO_HASH_BLAKE2_384:
        {
            crypto_hash->type   = CRYPTO_HASH_BLAKE2_384;
            crypto_hash->length = 48;
            crypto_hash->init   = sc_blake2b_init;
            crypto_hash->update = sc_blake2b_update;
            crypto_hash->final  = sc_blake2b_final;
            crypto_hash->ctx    = PRNG_MALLOC(sizeof(blake2b_state));
        } break;

        case CRYPTO_HASH_BLAKE2_256:
        {
            crypto_hash->type   = CRYPTO_HASH_BLAKE2_256;
            crypto_hash->length = 32;
            crypto_hash->init   = sc_blake2b_init;
            crypto_hash->update = sc_blake2b_update;
            crypto_hash->final  = sc_blake2b_final;
            crypto_hash->ctx    = PRNG_MALLOC(sizeof(blake2b_state));
        } break;

        case CRYPTO_HASH_BLAKE2_224:
        {
            crypto_hash->type   = CRYPTO_HASH_BLAKE2_224;
            crypto_hash->length = 28;
            crypto_hash->init   = sc_blake2b_init;
            crypto_hash->update = sc_blake2b_update;
            crypto_hash->final  = sc_blake2b_final;
            crypto_hash->ctx    = PRNG_MALLOC(sizeof(blake2b_state));
        } break;
#endif

#ifdef ENABLE_WHIRLPOOL
        case CRYPTO_HASH_WHIRLPOOL_512:
        {
            crypto_hash->type   = CRYPTO_HASH_WHIRLPOOL_512;
            crypto_hash->length = 64;
            crypto_hash->init   = whirlpool_init;
            crypto_hash->update = whirlpool_update;
            crypto_hash->final  = whirlpool_final;
            crypto_hash->ctx    = PRNG_MALLOC(sizeof(whirlpool_ctx));
        } break;
#endif

        default:
        {
            PRNG_FREE(crypto_hash, sizeof(utils_crypto_hash_t));
            crypto_hash = NULL;
        };
    }

    return crypto_hash;
}

SINT32 utils_crypto_hash_destroy(utils_crypto_hash_t *hash)
{
    if (NULL == hash) {
        return PRNG_FUNC_FAILURE;
    }

    switch (hash->type)
    {
#ifdef ENABLE_WHIRLPOOL
        case CRYPTO_HASH_WHIRLPOOL_512:
        {
            PRNG_FREE(hash->ctx, sizeof(whirlpool_ctx));
        } break;
#endif

#ifdef ENABLE_SHA3
        case CRYPTO_HASH_SHA3_512:
        case CRYPTO_HASH_SHA3_384:
        case CRYPTO_HASH_SHA3_256:
        case CRYPTO_HASH_SHA3_224:
        {
            PRNG_FREE(hash->ctx, sizeof(sha3_ctx_t));
        } break;
#endif

#ifdef ENABLE_SHA2
        case CRYPTO_HASH_SHA2_512:
        case CRYPTO_HASH_SHA2_384:
        case CRYPTO_HASH_SHA2_256:
        case CRYPTO_HASH_SHA2_224:
        {
            PRNG_FREE(hash->ctx, sizeof(sha2_ctx));
        } break;
#endif

#ifdef ENABLE_BLAKE2
        case CRYPTO_HASH_BLAKE2_512:
        case CRYPTO_HASH_BLAKE2_384:
        case CRYPTO_HASH_BLAKE2_256:
        case CRYPTO_HASH_BLAKE2_224:
        {
            PRNG_FREE(hash->ctx, sizeof(blake2b_state));
        } break;
#endif

        default:
        {
            return PRNG_FUNC_FAILURE;
        };
    }

    PRNG_FREE(hash, sizeof(utils_crypto_hash_t));

    return PRNG_FUNC_SUCCESS;
}

SINT32 hash_init(utils_crypto_hash_t *c)
{
    if (NULL == c) {
        return PRNG_FUNC_FAILURE;
    }
    return c->init(c->ctx, c->length);
}

SINT32 hash_update(utils_crypto_hash_t *c, const void *data, size_t len)
{
    if (NULL == c || NULL == data) {
        return PRNG_FUNC_FAILURE;
    }
    if (0 == len) {
        return PRNG_FUNC_SUCCESS;
    }
    return c->update(c->ctx, data, len);
}

SINT32 hash_final(utils_crypto_hash_t *c, void *md)
{
    if (NULL == c || NULL == md) {
        return PRNG_FUNC_FAILURE;
    }
    return c->final(c->ctx, md);
}
