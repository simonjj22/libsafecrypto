/*****************************************************************************
 * Copyright (C) Queen's University Belfast, ECIT, 2017                      *
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

#include <stdlib.h>
#include "safecrypto.h"
#include "safecrypto_debug.h"
#include "utils/crypto/hash.h"
#include "utils/crypto/prng.h"
#include "utils/threading/threading.h"

#include <string.h>


#define MIN_PARAM_SET 0
#define MAX_PARAM_SET 3
#define MAX_ITER      4096

#define USE_FIXED_BUFFERS     0
#if USE_FIXED_BUFFERS == 1
#define FIXED_BUFFER_SIZE     1200
#else
#define FIXED_BUFFER_SIZE     0
#endif


void show_progress(char *msg, int32_t count, int32_t max)
{
    int i;
    int barWidth = 50;
    double progress = (double) count / max;

    printf("%-20s [", msg);
    int pos = barWidth * progress;
    for (i = 0; i < barWidth; ++i) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }
    printf("] %4d/%4d (%3d %%)\r", count, max, (int)(progress * 100.0f));
    if (count == max) printf("\n");
    fflush(stdout);
}

int main(void)
{
    safecrypto_t *sc = NULL;

#ifdef DISABLE_SIGNATURES
    UINT32 flags[2] = {SC_FLAG_NONE, SC_FLAG_NONE};

    sc = safecrypto_create(SC_SCHEME_SIG_DILITHIUM, 0, flags);
    if (NULL != sc) {
        fprintf(stderr, "ERROR! safecrypto_create() succeeded but the scheme has been disabled\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
#else
    SINT32 i, j;
    UINT8 message[8192];
    size_t length = 64;
    size_t siglen = 0, pubkeylen = 0, privkeylen = 0;
#if USE_FIXED_BUFFERS
    UINT8 *fixed_buffer = malloc(FIXED_BUFFER_SIZE);
    UINT8 *sig = fixed_buffer, *pubkey = fixed_buffer, *privkey = fixed_buffer;
#else
    UINT8 *sig = NULL, *pubkey, *privkey;
#endif
    prng_ctx_t *prng_ctx = prng_create(SC_ENTROPY_RANDOM,
        SC_PRNG_AES_CTR_DRBG,
        SC_PRNG_THREADING_NONE, 0x00100000);
    prng_init(prng_ctx, NULL, 0);
    if (NULL == prng_ctx) {
        fprintf(stderr, "ERROR! Could not create prng_ctx_t\n");
        goto error_return;
    }

#ifdef USE_HUFFMAN_STATIC_ENTROPY
    UINT32 flags[2] = {SC_FLAG_0_ENTROPY_HUFFMAN, SC_FLAG_NONE};
    sc_entropy_type_e coding = SC_ENTROPY_HUFFMAN_STATIC;
#else
#ifdef USE_BAC_ENTROPY
    UINT32 flags[2] = {SC_FLAG_0_ENTROPY_BAC, SC_FLAG_NONE};
    sc_entropy_type_e coding = SC_ENTROPY_BAC;
#else
    UINT32 flags[2] = {SC_FLAG_NONE, SC_FLAG_NONE};
    sc_entropy_type_e coding = SC_ENTROPY_NONE;
#endif
#endif

    flags[0] |= SC_FLAG_MORE;
    flags[1]  = SC_FLAG_1_CSPRNG_AES_CTR_DRBG;

    SC_TIMER_INSTANCE(keygen_timer);
    SC_TIMER_INSTANCE(sign_timer);
    SC_TIMER_INSTANCE(verify_timer);
    SC_TIMER_CREATE(keygen_timer);
    SC_TIMER_CREATE(sign_timer);
    SC_TIMER_CREATE(verify_timer);

    char disp_msg[128];
    snprintf(disp_msg, 128, "%-20s", "Sign/Verify Test");

    for (i=MIN_PARAM_SET; i<=MAX_PARAM_SET; i++) {

        SC_TIMER_RESET(keygen_timer);
        SC_TIMER_RESET(sign_timer);
        SC_TIMER_RESET(verify_timer);

        printf("Message length: %6d bytes\n", (int)length);

        // Create a SAFEcrypto object
        sc = safecrypto_create(SC_SCHEME_SIG_DILITHIUM_G, i, flags);

        for (j=0; j<MAX_ITER; j++) {

            // Create a key pair
            SC_TIMER_START(keygen_timer);
            if (SC_FUNC_SUCCESS != safecrypto_keygen(sc)) {
                fprintf(stderr, "ERROR! safecrypto_keygen() failed\n");
                goto error_return;
            }
            SC_TIMER_STOP(keygen_timer);

            safecrypto_set_key_coding(sc, coding, coding);
            pubkeylen = FIXED_BUFFER_SIZE;
            if (SC_FUNC_SUCCESS != safecrypto_public_key_encode(sc, &pubkey, &pubkeylen)) {
                fprintf(stderr, "ERROR! safecrypto_public_key_encode() failed\n");
                goto error_return;
            }
            privkeylen = FIXED_BUFFER_SIZE;
            if (SC_FUNC_SUCCESS != safecrypto_private_key_encode(sc, &privkey, &privkeylen)) {
                fprintf(stderr, "ERROR! safecrypto_private_key_encode() failed\n");
                goto error_return;
            }

#if 0
            // Free all resources for the given SAFEcrypto object
            if (SC_FUNC_SUCCESS != safecrypto_destroy(sc)) {
                return EXIT_FAILURE;
            }

            // Create a SAFEcrypto object and load the encoded keys
            sc = safecrypto_create(SC_SCHEME_SIG_DILITHIUM_G, i, flags);
            if (SC_FUNC_SUCCESS != safecrypto_public_key_load(sc, pubkey, pubkeylen)) {
                fprintf(stderr, "ERROR! safecrypto_public_key_load() failed\n");
                goto error_return;
            }
            if (SC_FUNC_SUCCESS != safecrypto_private_key_load(sc, privkey, privkeylen)) {
                fprintf(stderr, "ERROR! safecrypto_private_key_load() failed\n");
                goto error_return;
            }
#if USE_FIXED_BUFFERS == 0
            free(pubkey);
            free(privkey);
#endif
            safecrypto_set_key_coding(sc, coding, coding);
            pubkeylen = FIXED_BUFFER_SIZE;
            if (SC_FUNC_SUCCESS != safecrypto_public_key_encode(sc, &pubkey, &pubkeylen)) {
                fprintf(stderr, "ERROR! safecrypto_public_key_encode() failed\n");
                goto error_return;
            }
            privkeylen = FIXED_BUFFER_SIZE;
            if (SC_FUNC_SUCCESS != safecrypto_private_key_encode(sc, &privkey, &privkeylen)) {
                fprintf(stderr, "ERROR! safecrypto_private_key_encode() failed\n");
                goto error_return;
            }
#endif
#if USE_FIXED_BUFFERS == 0
            free(pubkey);
            free(privkey);
#endif

            // Generate a random message
            prng_mem(prng_ctx, message, length);

            // Generate a signature for that message
            SC_TIMER_START(sign_timer);
            siglen = FIXED_BUFFER_SIZE;
            if (SC_FUNC_SUCCESS != safecrypto_sign(sc, message, length, &sig, &siglen)) {
                goto error_return;
            }
            SC_TIMER_STOP(sign_timer);

            SC_TIMER_START(verify_timer);
            // Invert a bit of the message digest to cause verification to fail
            if ((j & 0x3) == 3) {
                sig[j%(siglen-1)] ^= 1 << (j % 8);

                // Verify the signature using the public key
                if (SC_FUNC_SUCCESS == safecrypto_verify(sc, message, length, sig, siglen)) {
                    fprintf(stderr, "ERROR! Signature verified even though it was corrupt (j=%d)\n", j);
                    for (i=0; i<siglen; i++) {
                        if ((i&0x0F) == 0) fprintf(stderr, "\n  ");
                        fprintf(stderr, "%4d ", sig[i]);
                    }
                    fprintf(stderr, "\n");
                    goto error_return;
                }
            }
            else if ((j & 0x3) == 2) {
                message[j%64] ^= 1 << (j % 8);

                // Verify the signature using the public key
                if (SC_FUNC_SUCCESS == safecrypto_verify(sc, message, length, sig, siglen)) {
                    fprintf(stderr, "ERROR! Signature verified even though the message was corrupt\n");
                    goto error_return;
                }
            }
            else {
                // Verify the signature using the public key
                if (SC_FUNC_SUCCESS != safecrypto_verify(sc, message, length, sig, siglen)) {
                    fprintf(stderr, "ERROR! Signature NOT verified\n");
                    goto error_return;
                }
            }
            SC_TIMER_STOP(verify_timer);

#if USE_FIXED_BUFFERS
#else
            free(sig);
            sig = NULL;
#endif

            if ((j & 0x1F) == 0x1F) show_progress(disp_msg, j, MAX_ITER);
        }

        show_progress(disp_msg, MAX_ITER, MAX_ITER);

        const char *stats = safecrypto_processing_stats(sc);
        printf("%s", stats);

        double elapsed = SC_TIMER_GET_ELAPSED(keygen_timer);
        printf("KeyGen time: %f (%f per sec)\n", elapsed, (double)MAX_ITER / elapsed);
        elapsed = SC_TIMER_GET_ELAPSED(sign_timer);
        printf("Sign time:   %f (%f per sec)\n", elapsed, (double)MAX_ITER / elapsed);
        elapsed = SC_TIMER_GET_ELAPSED(verify_timer);
        printf("Verify time: %f (%f per sec)\n\n", elapsed, (double)MAX_ITER / elapsed);

        // Free all resources for the given SAFEcrypto object
        if (SC_FUNC_SUCCESS != safecrypto_destroy(sc)) {
            return EXIT_FAILURE;
        }
    }

#if USE_FIXED_BUFFERS
    if (fixed_buffer) free(fixed_buffer);
#else
    if (sig) free(sig);
#endif
    SC_TIMER_DESTROY(keygen_timer);
    SC_TIMER_DESTROY(sign_timer);
    SC_TIMER_DESTROY(verify_timer);
    prng_destroy(prng_ctx);
    return EXIT_SUCCESS;

error_return:
    if (sc) {
        UINT32 error;
        const char *file;
        SINT32 line;
        while (SC_OK != (error = safecrypto_err_get_error_line(sc, &file, &line))) {
            printf("ERROR: %08X, %s, line %d\n", error, file, line);
        }
    }
#if USE_FIXED_BUFFERS
    if (fixed_buffer) free(fixed_buffer);
#else
    if (sig) free(sig);
#endif
    SC_TIMER_DESTROY(keygen_timer);
    SC_TIMER_DESTROY(sign_timer);
    SC_TIMER_DESTROY(verify_timer);
    prng_destroy(prng_ctx);
    safecrypto_destroy(sc);
    return EXIT_FAILURE;
#endif
}


