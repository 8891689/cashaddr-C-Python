# Author: 8891689
# Assisted in creation: ChatGPT / deepseek
import argparse

CHARSET = "qpzry9x8gf2tvdw0s3jn54khce6mua7l"

def _polymod(values):
    """Correct CashAddr checksum polynomial modulo operation implementation"""
    c = 1
    for d in values:
        c0 = (c >> 35)
        c = ((c & 0x07ffffffff) << 5) ^ d
        if c0 & 0x01:
            c ^= 0x98f2bc8e61
        if c0 & 0x02:
            c ^= 0x79b76d99e2
        if c0 & 0x04:
            c ^= 0xf33e5fb3c4
        if c0 & 0x08:
            c ^= 0xae2eabe2a8
        if c0 & 0x10:
            c ^= 0x1e4f43e470
    return c ^ 1

def _calculate_checksum(prefix, payload):
    """Calculate CashAddr checksum"""
    prefix_values = [ord(c) & 0x1f for c in prefix.lower()]
    data = prefix_values + [0] + payload + [0]*8
    poly = _polymod(data)
    checksum = []
    for i in range(7, -1, -1):
        checksum.append((poly >> (5*i)) & 0x1f)
    return checksum

def _pack_5bit(data_bytes):
    """Convert byte array to list of 5-bit values (with padding)"""
    buffer = 0
    buffer_bits = 0
    packed = []
    for byte in data_bytes:
        buffer = (buffer << 8) | byte
        buffer_bits += 8
        while buffer_bits >= 5:
            buffer_bits -= 5
            packed.append((buffer >> buffer_bits) & 0x1f)
            buffer &= (1 << buffer_bits) - 1
    if buffer_bits > 0:
        packed.append((buffer << (5 - buffer_bits)) & 0x1f)
    return packed

def _unpack_5bit(packed):
    """Convert list of 5-bit values to byte array (remove padding)"""
    buffer = 0
    buffer_bits = 0
    data = []
    for value in packed:
        buffer = (buffer << 5) | value
        buffer_bits += 5
        while buffer_bits >= 8:
            buffer_bits -= 8
            data.append((buffer >> buffer_bits) & 0xff)
            buffer &= (1 << buffer_bits) - 1
    return bytes(data)

def decode_cashaddr(address):
    """Decode CashAddr address, returns prefix, version, type, and hash160, also returns intermediate debug information"""
    if ':' in address:
        prefix, base32_part = address.split(':')
    else:
        prefix = 'bitcoincash'
        base32_part = address

    print(f"[Debug] Prefix: {prefix}")
    print(f"[Debug] Base32 Part: {base32_part}")

    # Validate Base32 characters
    for char in base32_part:
        if char not in CHARSET:
            raise ValueError(f"Invalid character '{char}'")

    # Convert to 5-bit array
    packed = [CHARSET.index(c) for c in base32_part.lower()]
    print(f"[Debug] 5-bit Array (packed): {packed}")

    # Split payload and checksum
    if len(packed) < 8:
        raise ValueError("Address is too short")
    payload_packed = packed[:-8]
    checksum = packed[-8:]
    print(f"[Debug] Payload (payload_packed): {payload_packed}")
    print(f"[Debug] Extracted Checksum: {checksum}")

    # Verify checksum
    data = [ord(c) & 0x1f for c in prefix.lower()] + [0] + payload_packed + checksum
    polymod_result = _polymod(data)
    print(f"[Debug] Polynomial Modulo Result: {polymod_result}")
    if polymod_result != 0:
        raise ValueError("Invalid checksum")

    # Convert to byte array
    payload_bytes = _unpack_5bit(payload_packed)
    print(f"[Debug] Unpacked Payload Bytes: {list(payload_bytes)}")
    if len(payload_bytes) < 1:
        raise ValueError("Invalid payload")

    # Parse version byte
    version_byte = payload_bytes[0]
    version = version_byte & 0x07  # Low 3 bits are version
    type_bits = (version_byte >> 3) & 0x1f  # High 5 bits are type
    print(f"[Debug] Version Byte: {version_byte}, Version: {version}, Type Bits: {type_bits}")

    # Determine address type
    if type_bits == 0:
        addr_type = "P2PKH"
    elif type_bits == 1:
        addr_type = "P2SH"
    else:
        addr_type = "Unknown Type"

    # Extract hash160
    hash160 = payload_bytes[1:21]
    if len(hash160) != 20:
        raise ValueError("Hash160 length is incorrect")

    return {
        "prefix": prefix,
        "version": version,
        "type": addr_type,
        "hash160": hash160.hex(),
        "debug": {
            "packed": packed,
            "payload_packed": payload_packed,
            "checksum_extracted": checksum,
            "polymod_result": polymod_result,
            "payload_bytes": list(payload_bytes)
        }
    }

def encode_cashaddr(prefix, version, addr_type, hash160):
    """Encode CashAddr address, also output debug information
    :param prefix: Address prefix (e.g., bitcoincash)
    :param version: Version number (0-7)
    :param addr_type: Address type (P2PKH/P2SH)
    :param hash160: 20-byte hash160 as a hexadecimal string
    :return: Encoded CashAddr address
    """
    # Determine type identifier
    if addr_type == "P2PKH":
        type_bits = 0
    elif addr_type == "P2SH":
        type_bits = 1
    else:
        raise ValueError("Unsupported address type")

    # Construct version byte
    version_byte = (type_bits << 3) | (version & 0x07)
    hash_bytes = bytes.fromhex(hash160)
    payload_bytes = bytes([version_byte]) + hash_bytes
    print(f"[Debug] Version Byte during Encoding: {version_byte}")
    print(f"[Debug] payload_bytes during Encoding: {list(payload_bytes)}")

    # Convert to 5-bit array
    payload_packed = _pack_5bit(payload_bytes)
    print(f"[Debug] Converted to 5-bit array during Encoding: {payload_packed}")

    # Calculate checksum
    checksum = _calculate_checksum(prefix, payload_packed)
    print(f"[Debug] Calculated Checksum: {checksum}")
    full_payload = payload_packed + checksum
    print(f"[Debug] Full 5-bit array (payload + checksum): {full_payload}")

    # Generate Base32 string
    base32_part = ''.join([CHARSET[v] for v in full_payload])
    return f"{prefix}:{base32_part}"

def main():
    parser = argparse.ArgumentParser(description="Bitcoin Cash address decoding and encoding tool")
    parser.add_argument('address', help="Bitcoin Cash address")
    args = parser.parse_args()

    try:
        # Decode address
        decoded = decode_cashaddr(args.address)
        print("\nDecoding Result:")
        print(f"Prefix: {decoded['prefix']}")
        print(f"Version: {decoded['version']}")
        print(f"Type: {decoded['type']}")
        print(f"Hash160: {decoded['hash160']}")

        print("\nDebug Information:")
        for key, value in decoded['debug'].items():
            print(f"{key}: {value}")

        # Re-encode for verification
        encoded = encode_cashaddr(
            decoded['prefix'],
            decoded['version'],
            decoded['type'],
            decoded['hash160']
        )
        print("\nRe-encoded Address:")
        print(encoded)

    except ValueError as e:
        print(f"Error: {str(e)}")

if __name__ == "__main__":
    main()
