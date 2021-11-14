#!/usr/bin/env python3
import deflatedecompress
import struct
import io
from base64 import b64decode
from zipfile import ZipFile

with ZipFile('../distfiles/challenge.zip') as zip:
    with open("../distfiles/challenge.zip", "rb") as f:

        flag = b""
        for i in range(6060):
            f.read(0x0C + 2 + 4)
            tmp = f.read(4)
            if not tmp:
                break
            ln = struct.unpack("<I", tmp)[0]
            print(i, ln)
            f.read(16)
            data = f.read(ln)
            deflatedecompress.Decompressor.decompress_to_bytes(
                deflatedecompress.BitInputStream(io.BytesIO(data))
            )

            with zip.open(f"{i:04d}.txt") as g:
                st = set(g.read())

            st = set(deflatedecompress.wow) - st - set([256])
            assert len(st) == 1
            flag += bytes([st.pop()])

        print(b64decode(flag))
