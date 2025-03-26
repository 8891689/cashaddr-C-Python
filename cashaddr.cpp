/*Author: 8891689
 * Assisted in creation: ChatGPT / deepseek
 */
#include "cashaddr.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Base32 character set used in CashAddr */
static const char *CHARSET = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

/* Internal function declarations */
static uint64_t _polymod(const int *values, size_t count);
static void _calculate_checksum(const char *prefix, const int *payload, int payload_len, int checksum[8]);
static int _pack_5bit(const unsigned char *data, int data_len, int *out, int max_out);
static int _unpack_5bit(const int *data, int data_len, unsigned char *out, int max_out);
static int hexchar2int(char c);
static int hexstr2bytes(const char *hex, unsigned char *out, int max_out);

/* Polynomial modulo operation for checksum calculation */
static uint64_t _polymod(const int *values, size_t count) {
    uint64_t c = 1;
    for (size_t i = 0; i < count; i++) {
        int d = values[i];
        int c0 = (int)(c >> 35);
        c = ((c & 0x07ffffffffULL) << 5) ^ d;
        if (c0 & 0x01) c ^= 0x98f2bc8e61ULL;
        if (c0 & 0x02) c ^= 0x79b76d99e2ULL;
        if (c0 & 0x04) c ^= 0xf33e5fb3c4ULL;
        if (c0 & 0x08) c ^= 0xae2eabe2a8ULL;
        if (c0 & 0x10) c ^= 0x1e4f43e470ULL;
    }
    return c ^ 1;
}

/* Calculate CashAddr checksum */
static void _calculate_checksum(const char *prefix, const int *payload, int payload_len, int checksum[8]) {
    int prefix_len = (int)strlen(prefix);
    int data_len = prefix_len + 1 + payload_len + 8;
    int *data = (int *)malloc(data_len * sizeof(int));
    if (!data) {
        //fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    // Convert prefix: take the lower 5 bits of each character
    for (int i = 0; i < prefix_len; i++) {
        data[i] = tolower(prefix[i]) & 0x1f;
    }
    data[prefix_len] = 0;
    // Copy payload
    for (int i = 0; i < payload_len; i++) {
        data[prefix_len + 1 + i] = payload[i];
    }
    // The subsequent 8 zeros are already initialized to 0 (explicitly assign 0 here)
    for (int i = prefix_len + 1 + payload_len; i < data_len; i++) {
        data[i] = 0;
    }
    uint64_t poly = _polymod(data, data_len);
    free(data);
    for (int i = 0; i < 8; i++) {
        checksum[i] = (poly >> (5 * (7 - i))) & 0x1f;
    }
}

/* Convert byte array to 5-bit array, return the length of the 5-bit array */
static int _pack_5bit(const unsigned char *data, int data_len, int *out, int max_out) {
    int buffer = 0, bits = 0;
    int count = 0;
    for (int i = 0; i < data_len; i++) {
        buffer = (buffer << 8) | data[i];
        bits += 8;
        while (bits >= 5) {
            bits -= 5;
            if (count >= max_out) return -1;
            out[count++] = (buffer >> bits) & 0x1f;
            buffer &= (1 << bits) - 1;
        }
    }
    if (bits > 0) {
        if (count >= max_out) return -1;
        out[count++] = (buffer << (5 - bits)) & 0x1f;
    }
    return count;
}

/* Restore 5-bit array to byte array, return the number of restored bytes */
static int _unpack_5bit(const int *data, int data_len, unsigned char *out, int max_out) {
    int buffer = 0, bits = 0;
    int count = 0;
    for (int i = 0; i < data_len; i++) {
        buffer = (buffer << 5) | data[i];
        bits += 5;
        while (bits >= 8) {
            bits -= 8;
            if (count >= max_out) return -1;
            out[count++] = (buffer >> bits) & 0xff;
            buffer &= (1 << bits) - 1;
        }
    }
    return count;
}

/* Decode CashAddr, write the result to the result structure, return 0 for success */
int decode_cashaddr(const char *address, CashAddrResult *result) {
    char prefix[32] = {0};
    const char *base32_part = NULL;
    const char *colon = strchr(address, ':');
    if (colon) {
        size_t pre_len = colon - address;
        if (pre_len >= sizeof(prefix)) {
            fprintf(stderr, "Prefix is too long\n");
            return -1;
        }
        strncpy(prefix, address, pre_len);
        prefix[pre_len] = '\0';
        base32_part = colon + 1;
    } else {
        strcpy(prefix, "bitcoincash");
        base32_part = address;
    }
    //printf("[Debug] Prefix: %s\n", prefix);
    //printf("[Debug] Base32 part: %s\n", base32_part);

    int base32_len = (int)strlen(base32_part);
    int *packed = (int *)malloc(base32_len * sizeof(int));
    if (!packed) {
        //fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }
    // Convert base32 characters to 5-bit array
    for (int i = 0; i < base32_len; i++) {
        char ch = tolower(base32_part[i]);
        const char *p = strchr(CHARSET, ch);
        if (!p) {
            fprintf(stderr, "Invalid character '%c'\n", ch);
            free(packed);
            return -1;
        }
        packed[i] = (int)(p - CHARSET);
    }
    //printf("[Debug] 5-bit array (packed): [");
    for (int i = 0; i < base32_len; i++) {
        //printf("%d%s", packed[i], i == base32_len - 1 ? "" : ", ");
    }
    //printf("]\n");

    if (base32_len < 8) {
        fprintf(stderr, "Address is too short\n");
        free(packed);
        return -1;
    }
    int payload_len = base32_len - 8;
    int *payload_packed = packed; // The first payload_len numbers are the payload
    //int *checksum_extracted = packed + payload_len;
    //printf("[Debug] Payload (payload_packed): [");
    for (int i = 0; i < payload_len; i++) {
        //printf("%d%s", payload_packed[i], i == payload_len - 1 ? "" : ", ");
    }
    //printf("]\n");
    //printf("[Debug] Extracted checksum: [");
    for (int i = 0; i < 8; i++) {
        //printf("%d%s", checksum_extracted[i], i == 7 ? "" : ", ");
    }
    //printf("]\n");

    // Construct data array for checksum calculation
    int prefix_len = (int)strlen(prefix);
    int data_len = prefix_len + 1 + base32_len;
    int *data = (int *)malloc(data_len * sizeof(int));
    if (!data) {
        //fprintf(stderr, "Memory allocation failed\n");
        free(packed);
        return -1;
    }
    for (int i = 0; i < prefix_len; i++) {
        data[i] = tolower(prefix[i]) & 0x1f;
    }
    data[prefix_len] = 0;
    memcpy(data + prefix_len + 1, packed, base32_len * sizeof(int));
    uint64_t poly = _polymod(data, data_len);
    free(data);
    //printf("[Debug] Polynomial modulo result: %llu\n", (unsigned long long)poly);
    if (poly != 0) {
        fprintf(stderr, "Invalid checksum\n");
        free(packed);
        return -1;
    }

    // Unpack payload (convert to byte array)
    unsigned char payload_bytes[100] = {0};
    int payload_bytes_len = _unpack_5bit(payload_packed, payload_len, payload_bytes, sizeof(payload_bytes));
    if (payload_bytes_len < 1) {
        fprintf(stderr, "Invalid payload\n");
        free(packed);
        return -1;
    }
    //printf("[Debug] Unpacked payload bytes: [");
    for (int i = 0; i < payload_bytes_len; i++) {
        //printf("%d%s", payload_bytes[i], i == payload_bytes_len - 1 ? "" : ", ");
    }
    //printf("]\n");

    // Parse version byte
    int version_byte = payload_bytes[0];
    int version = version_byte & 0x07;
    int type_bits = (version_byte >> 3) & 0x1f;
    //printf("[Debug] Version byte: %d, Version: %d, Type bits: %d\n", version_byte, version, type_bits);

    char addr_type[16] = {0};
    if (type_bits == 0) {
        strcpy(addr_type, "P2PKH");
    } else if (type_bits == 1) {
        strcpy(addr_type, "P2SH");
    } else {
        strcpy(addr_type, "Unknown Type");
    }

    // Extract hash160: payload_bytes[1..20]
    if (payload_bytes_len < 21) {
        fprintf(stderr, "Payload length is insufficient\n");
        free(packed);
        return -1;
    }
    char hash160_hex[41] = {0};
    for (int i = 0; i < 20; i++) {
        sprintf(hash160_hex + i * 2, "%02x", payload_bytes[i+1]);
    }

    /* Fill in the return result */
    strncpy(result->prefix, prefix, sizeof(result->prefix)-1);
    result->version = version;
    strncpy(result->type, addr_type, sizeof(result->type)-1);
    strncpy(result->hash160, hash160_hex, sizeof(result->hash160)-1);

    free(packed);
    return 0;
}

/* Convert a hexadecimal character to a number */
static int hexchar2int(char c) {
    if ('0' <= c && c <= '9')
        return c - '0';
    c = tolower(c);
    if ('a' <= c && c <= 'f')
        return c - 'a' + 10;
    return -1;
}

/* Convert hexadecimal string to byte array, return the number of bytes */
static int hexstr2bytes(const char *hex, unsigned char *out, int max_out) {
    int len = (int)strlen(hex);
    if (len % 2 != 0) return -1;
    int bytes = len / 2;
    if (bytes > max_out) return -1;
    for (int i = 0; i < bytes; i++) {
        int hi = hexchar2int(hex[2*i]);
        int lo = hexchar2int(hex[2*i+1]);
        if (hi < 0 || lo < 0) return -1;
        out[i] = (unsigned char)((hi << 4) | lo);
    }
    return bytes;
}

/* Encode CashAddr, write the result to out_address */
int encode_cashaddr(const char *prefix, int version, const char *type, const char *hash160,
                    char *out_address, size_t out_size) {
    int type_bits = -1;
    if (strcmp(type, "P2PKH") == 0) {
        type_bits = 0;
    } else if (strcmp(type, "P2SH") == 0) {
        type_bits = 1;
    } else {
        fprintf(stderr, "Unsupported address type\n");
        return -1;
    }
    int version_byte = (type_bits << 3) | (version & 0x07);
    unsigned char hash_bytes[20];
    if (hexstr2bytes(hash160, hash_bytes, sizeof(hash_bytes)) != 20) {
        fprintf(stderr, "Invalid hash160\n");
        return -1;
    }
    unsigned char payload_bytes[21];
    payload_bytes[0] = (unsigned char)version_byte;
    memcpy(payload_bytes + 1, hash_bytes, 20);
    //printf("[Debug] Version byte during encoding: %d\n", version_byte);
    //printf("[Debug] payload_bytes during encoding: [");
    for (int i = 0; i < 21; i++) {
        //printf("%d%s", payload_bytes[i], i == 20 ? "" : ", ");
    }
    //printf("]\n");

    int payload_packed[100] = {0};
    int payload_packed_len = _pack_5bit(payload_bytes, 21, payload_packed, 100);
    if (payload_packed_len < 0) {
        fprintf(stderr, "pack_5bit failed\n");
        return -1;
    }
    //printf("[Debug] Converted to 5-bit array during encoding: [");
    for (int i = 0; i < payload_packed_len; i++) {
        //printf("%d%s", payload_packed[i], i == payload_packed_len - 1 ? "" : ", ");
    }
    //printf("]\n");

    int checksum[8] = {0};
    _calculate_checksum(prefix, payload_packed, payload_packed_len, checksum);
    //printf("[Debug] Calculated checksum: [");
    for (int i = 0; i < 8; i++) {
        //printf("%d%s", checksum[i], i == 7 ? "" : ", ");
    }
    //printf("]\n");

    int full_payload[120] = {0};
    memcpy(full_payload, payload_packed, payload_packed_len * sizeof(int));
    memcpy(full_payload + payload_packed_len, checksum, 8 * sizeof(int));
    //printf("[Debug] Full 5-bit array (payload + checksum): [");
    for (int i = 0; i < payload_packed_len + 8; i++) {
        //printf("%d%s", full_payload[i], i == payload_packed_len+8 - 1 ? "" : ", ");
    }
    //printf("]\n");

    char base32_part[256] = {0};
    int pos = 0;
    for (int i = 0; i < payload_packed_len + 8; i++) {
        if (pos >= (int)sizeof(base32_part)-1) break;
        base32_part[pos++] = CHARSET[ full_payload[i] ];
    }
    base32_part[pos] = '\0';

    /* Concatenate prefix and Base32 part */
    int n = snprintf(out_address, out_size, "%s:%s", prefix, base32_part);
    if (n < 0 || (size_t)n >= out_size) {
        fprintf(stderr, "Output buffer is too small\n");
        return -1;
    }
    return 0;
}
