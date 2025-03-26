/*Author: 8891689
 * Assisted in creation: ChatGPT / deepseek
 */
#ifndef CASHADDR_H
#define CASHADDR_H

#include <stddef.h> // size_t

#ifdef __cplusplus
extern "C" {
#endif

/* CashAddr decoding result structure */
typedef struct CashAddrResult {
    char prefix[32];     // Prefix
    int version;         // Version
    char type[16];       // Address type (P2PKH, P2SH, Unknown Type)
    char hash160[41];    // Hexadecimal string representation of Hash160
} CashAddrResult;

/* Decode CashAddr address, write the result to the result structure, return 0 for success, non-zero for failure */
int decode_cashaddr(const char *address, CashAddrResult *result);

/* Encode CashAddr address, write the result to out_address, return 0 for success, non-zero for failure */
int encode_cashaddr(const char *prefix, int version, const char *type, const char *hash160,
                    char *out_address, size_t out_size);

#ifdef __cplusplus
}
#endif

#endif // CASHADDR_H
