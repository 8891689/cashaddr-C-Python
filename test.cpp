#include "cashaddr.h"
#include <stdio.h>
#include <string.h>

int main() {
    CashAddrResult result;
    char encoded_address[256];
    int ret;

    printf("--- Decode Test ---\n");
    const char *address_to_decode = "bitcoincash:qp63uahgrxged4z5jswyt5dn5v3lzsem6cy4spdc2h";
    printf("Decoding address: %s\n", address_to_decode);
    ret = decode_cashaddr(address_to_decode, &result);
    if (ret == 0) {
        printf("Decode successful:\n");
        printf("  Prefix: %s\n", result.prefix);
        printf("  Version: %d\n", result.version);
        printf("  Type: %s\n", result.type);
        printf("  Hash160: %s\n", result.hash160);
    } else {
        printf("Decode failed, error code: %d\n", ret);
    }
    printf("\n");

    printf("--- Encode Test ---\n");
    const char *prefix_encode = "bitcoincash";
    int version_encode = 0;
    const char *type_encode = "P2PKH";
    const char *hash160_encode = "751e76e8199196d454941c45d1b3a323f1433bd6";
    printf("Encoding parameters:\n");
    printf("  Prefix: %s\n", prefix_encode);
    printf("  Version: %d\n", version_encode);
    printf("  Type: %s\n", type_encode);
    printf("  Hash160: %s\n", hash160_encode);

    ret = encode_cashaddr(prefix_encode, version_encode, type_encode, hash160_encode, encoded_address, sizeof(encoded_address));
    if (ret == 0) {
        printf("Encode successful:\n");
        printf("  Encoded address: %s\n", encoded_address);
    } else {
        printf("Encode failed, error code: %d\n", ret);
    }
    printf("\n");

    printf("--- Error Decode Test ---\n");
    const char *invalid_address = "bitcoincash:qr6m7j9njldzl0qd7x5zx2fkucvmvn0casrccampoa"; // Intentionally modify the last character to make the checksum error
    printf("Decoding error address: %s\n", invalid_address);
    ret = decode_cashaddr(invalid_address, &result);
    if (ret == 0) {
        printf("Decode successful (Expected to fail!):\n"); // Theoretically, it should not succeed here
    } else {
        printf("Decode failed (Expected to succeed), error code: %d\n", ret);
        printf("  Error message should indicate invalid checksum.\n");
    }

    return 0;
}
