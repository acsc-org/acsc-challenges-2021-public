#!/usr/bin/env python3
from base64 import b64encode
import string
import random
import zlib
import struct

with open("flag.txt", "rb") as f:
    flag = f.read()

flag = b64encode(flag)

chunks, offset = [], 0


def gen_random_message(char):
    chars = list((string.ascii_letters + string.digits + "+/").encode())
    chars.remove(char)
    additional = random.choice(chars)
    chars.remove(additional)

    while True:
        length = random.randint(100, 200) * 4
        msg = bytes([random.choice(chars) for _ in range(length)])
        if set(msg) == set(chars):
            break

    return msg, additional


for idx, c in enumerate(flag):
    random_message, additional = gen_random_message(c)

    bitstream = "1"  # End
    bitstream += "01"  # Type 2
    bitstream += "0" * 5  # hlit == 0
    bitstream += "0" * 5  # hdist == 0

    # 16, 17, 18, 0, 8, 7, 9, 6
    bitstream += "0010"  # hclen == 4
    bitstream += "000" * 3  # [16] == [17] == [18] == 0
    bitstream += "100"  # [0] == 1
    bitstream += "000" * 3  # [8] == [7] == [9] == 0
    bitstream += "100"  # [6] == 1

    char_set = set((string.ascii_letters + string.digits + "+/").encode())
    char_symbol_map, cnt = dict(), 0
    for i in range(258):
        if (i in char_set and additional != i) or i == 256:
            bitstream += "1"  # length 6 object
            char_symbol_map[i] = f"{cnt:06b}"
            cnt += 1
        else:
            bitstream += "0"  # None

    for r in random_message:
        bitstream += char_symbol_map[r]

    bitstream += "1" * 6  # End of stream
    bitstream += "0" * ((-len(bitstream)) % 8)

    bytestream = []
    for i in range(0, len(bitstream), 8):
        bytestream.append(int(bitstream[i : i + 8][::-1], 2))
    bytestream = bytes(bytestream)

    filename = f"{idx:04d}.txt"
    data_chunk = bytes.fromhex("504b0304 14000400 0800 00000000")
    data_chunk += struct.pack("<I", zlib.crc32(random_message))
    data_chunk += struct.pack("<I", len(bytestream))
    data_chunk += struct.pack("<I", len(random_message))
    data_chunk += struct.pack("<H", len(filename))
    data_chunk += b"\0\0"
    data_chunk += filename.encode()
    data_chunk += bytestream

    dir_chunk = bytes.fromhex("504b0102 140314000400 0800 00000000")
    dir_chunk += struct.pack("<I", zlib.crc32(random_message))
    dir_chunk += struct.pack("<I", len(bytestream))
    dir_chunk += struct.pack("<I", len(random_message))
    dir_chunk += struct.pack("<H", len(filename))
    dir_chunk += b"\0\0"
    dir_chunk += b"\0\0"
    dir_chunk += b"\0\0"
    dir_chunk += b"\0\0"
    dir_chunk += b"\x20\0\0\0"
    dir_chunk += struct.pack("<I", offset)
    dir_chunk += filename.encode()

    offset += len(data_chunk)

    chunks.append((data_chunk, dir_chunk))

f = open("challenge.zip", "wb")

size_dir = 0
for chunk, _ in chunks:
    f.write(chunk)
for _, chunk in chunks:
    size_dir += len(chunk)
    f.write(chunk)

eocd_chunk = bytes.fromhex("504b0506 0000 0000")
eocd_chunk += struct.pack("<H", len(flag))
eocd_chunk += struct.pack("<H", len(flag))
eocd_chunk += struct.pack("<I", size_dir)
eocd_chunk += struct.pack("<I", offset)
eocd_chunk += b"\0\0"
f.write(eocd_chunk)

f.close()
