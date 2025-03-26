# cashaddr-C++-Python
This is a C++ and Python library designed for encoding and decoding Bitcoin Cash (BCH) addresses in the CashAddr format. 
```
g++ -o test test.cpp cashaddr.cpp
```
```
./test
--- Decode Test ---
Decoding address: bitcoincash:qp63uahgrxged4z5jswyt5dn5v3lzsem6cy4spdc2h
Decode successful:
  Prefix: bitcoincash
  Version: 0
  Type: P2PKH
  Hash160: 751e76e8199196d454941c45d1b3a323f1433bd6

--- Encode Test ---
Encoding parameters:
  Prefix: bitcoincash
  Version: 0
  Type: P2PKH
  Hash160: 751e76e8199196d454941c45d1b3a323f1433bd6
Encode successful:
  Encoded address: bitcoincash:qp63uahgrxged4z5jswyt5dn5v3lzsem6cy4spdc2h

--- Error Decode Test ---
Decoding error address: bitcoincash:qr6m7j9njldzl0qd7x5zx2fkucvmvn0casrccampoa
Invalid character 'o'
Decode failed (Expected to succeed), error code: -1
  Error message should indicate invalid checksum.
```
```
python3 cash_address_converter.py qp63uahgrxged4z5jswyt5dn5v3lzsem6cy4spdc2h
[Debug] Prefix: bitcoincash
[Debug] Base32 Part: qp63uahgrxged4z5jswyt5dn5v3lzsem6cy4spdc2h
[Debug] 5-bit Array (packed): [0, 1, 26, 17, 28, 29, 23, 8, 3, 6, 8, 25, 13, 21, 2, 20, 18, 16, 14, 4, 11, 20, 13, 19, 20, 12, 17, 31, 2, 16, 25, 27, 26, 24, 4, 21, 16, 1, 13, 24, 10, 23]
[Debug] Payload (payload_packed): [0, 1, 26, 17, 28, 29, 23, 8, 3, 6, 8, 25, 13, 21, 2, 20, 18, 16, 14, 4, 11, 20, 13, 19, 20, 12, 17, 31, 2, 16, 25, 27, 26, 24]
[Debug] Extracted Checksum: [4, 21, 16, 1, 13, 24, 10, 23]
[Debug] Polynomial Modulo Result: 0
[Debug] Unpacked Payload Bytes: [0, 117, 30, 118, 232, 25, 145, 150, 212, 84, 148, 28, 69, 209, 179, 163, 35, 241, 67, 59, 214]
[Debug] Version Byte: 0, Version: 0, Type Bits: 0

Decoding Result:
Prefix: bitcoincash
Version: 0
Type: P2PKH
Hash160: 751e76e8199196d454941c45d1b3a323f1433bd6

Debug Information:
packed: [0, 1, 26, 17, 28, 29, 23, 8, 3, 6, 8, 25, 13, 21, 2, 20, 18, 16, 14, 4, 11, 20, 13, 19, 20, 12, 17, 31, 2, 16, 25, 27, 26, 24, 4, 21, 16, 1, 13, 24, 10, 23]
payload_packed: [0, 1, 26, 17, 28, 29, 23, 8, 3, 6, 8, 25, 13, 21, 2, 20, 18, 16, 14, 4, 11, 20, 13, 19, 20, 12, 17, 31, 2, 16, 25, 27, 26, 24]
checksum_extracted: [4, 21, 16, 1, 13, 24, 10, 23]
polymod_result: 0
payload_bytes: [0, 117, 30, 118, 232, 25, 145, 150, 212, 84, 148, 28, 69, 209, 179, 163, 35, 241, 67, 59, 214]
[Debug] Version Byte during Encoding: 0
[Debug] payload_bytes during Encoding: [0, 117, 30, 118, 232, 25, 145, 150, 212, 84, 148, 28, 69, 209, 179, 163, 35, 241, 67, 59, 214]
[Debug] Converted to 5-bit array during Encoding: [0, 1, 26, 17, 28, 29, 23, 8, 3, 6, 8, 25, 13, 21, 2, 20, 18, 16, 14, 4, 11, 20, 13, 19, 20, 12, 17, 31, 2, 16, 25, 27, 26, 24]
[Debug] Calculated Checksum: [4, 21, 16, 1, 13, 24, 10, 23]
[Debug] Full 5-bit array (payload + checksum): [0, 1, 26, 17, 28, 29, 23, 8, 3, 6, 8, 25, 13, 21, 2, 20, 18, 16, 14, 4, 11, 20, 13, 19, 20, 12, 17, 31, 2, 16, 25, 27, 26, 24, 4, 21, 16, 1, 13, 24, 10, 23]

Re-encoded Address:
bitcoincash:qp63uahgrxged4z5jswyt5dn5v3lzsem6cy4spdc2h
```
# Sponsorship
If this project has been helpful to you, please consider sponsoring. Your support is greatly appreciated. Thank you!
```
BTC: bc1qt3nh2e6gjsfkfacnkglt5uqghzvlrr6jahyj2k
ETH: 0xD6503e5994bF46052338a9286Bc43bC1c3811Fa1
DOGE: DTszb9cPALbG9ESNJMFJt4ECqWGRCgucky
TRX: TAHUmjyzg7B3Nndv264zWYUhQ9HUmX4Xu4
```
