#include "verify.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>

/*************************************************
* Name:        PQCLEAN_KYBER512_CLEAN_verify
*
* Description: Compare two arrays for equality in constant time.
*
* Arguments:   const uint8_t *a: pointer to first byte array
*              const uint8_t *b: pointer to second byte array
*              size_t len:       length of the byte arrays
*
* Returns 0 if the byte arrays are equal, 1 otherwise
**************************************************/
int PQCLEAN_KYBER512_CLEAN_verify(const uint8_t *a, const uint8_t *b, size_t len) {
    size_t i;
    uint8_t r = 0, s = 0;
    char filename[256];
    struct timeval tv;
    struct tm *tm_info;
    int mismatch_count = 0;

    // Perform the XOR operation and count mismatches
    for (i = 0; i < len; i++) {
        if (a[i] != b[i]) {
            mismatch_count++;
        }
        r |= a[i] ^ b[i];
    }

    // printf("Verify!\n");

    // Get current time with microseconds
    gettimeofday(&tv, NULL);
    time_t now = tv.tv_sec;
    tm_info = localtime(&now);

    // Generate unique filename based on mismatch count and microsecond timestamp
    if (mismatch_count == 0) {
        snprintf(filename, sizeof(filename), "stored_verify_ciphertext/success_%ld_%06ld.csv", tv.tv_sec, tv.tv_usec);
    } else {
        snprintf(filename, sizeof(filename), "stored_verify_ciphertext/fail_%d_%ld_%06ld.csv", mismatch_count, tv.tv_sec, tv.tv_usec);
    }

    // Open the CSV file for writing
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    // Write CSV headers
    fprintf(file, "Index,A,B,R\n");

    // Write the results to the CSV file
    for (i = 0; i < len; i++) {
        s = a[i] ^ b[i];
        fprintf(file, "%zu,%u,%u,%u\n", i, a[i], b[i], s);
    }

    // Close the CSV file
    fclose(file);

    return (-(uint64_t)r) >> 63;
}

// int PQCLEAN_KYBER512_CLEAN_verify(const uint8_t *a, const uint8_t *b, size_t len) {
//     size_t i;
//     uint8_t r = 0;

//     for (i = 0; i < len; i++) {
//         r |= a[i] ^ b[i];
//     }

//     return (-(uint64_t)r) >> 63;
// }

/*************************************************
* Name:        PQCLEAN_KYBER512_CLEAN_cmov
*
* Description: Copy len bytes from x to r if b is 1;
*              don't modify x if b is 0. Requires b to be in {0,1};
*              assumes two's complement representation of negative integers.
*              Runs in constant time.
*
* Arguments:   uint8_t *r:       pointer to output byte array
*              const uint8_t *x: pointer to input byte array
*              size_t len:       Amount of bytes to be copied
*              uint8_t b:        Condition bit; has to be in {0,1}
**************************************************/
void PQCLEAN_KYBER512_CLEAN_cmov(uint8_t *r, const uint8_t *x, size_t len, uint8_t b) {
    size_t i;

    b = -b;
    for (i = 0; i < len; i++) {
        r[i] ^= b & (r[i] ^ x[i]);
    }
}
