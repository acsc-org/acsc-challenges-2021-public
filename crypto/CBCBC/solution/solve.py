#!/usr/bin/env python3

import base64
import json
import sys

from pwn import *
from Crypto.Util.Padding import pad, unpad

if len(sys.argv) != 3:
    r = process(["python3", "chal.py"])
else:
    r = remote(sys.argv[1], sys.argv[2])

r.sendlineafter('> ', '1')
r.sendafter('username: ', '\n')
r.recvuntil('token: \n')
enc_token = base64.b64decode(r.recvuntil('\n')[:-1])
recovered = bytearray(len(enc_token) - 32)

for block in range(len(enc_token) // 16 - 2):
    to_send = bytearray(enc_token)
    for i in range(16):
        for j in range(i):
            to_send[-33 - 16 * block - j] = enc_token[-33 - 16 * block - j] ^ recovered[-1 - block * 16 - j] ^ (i + 1)

        for v in range(1, 256):
            to_send[-33 - 16 * block - i] = enc_token[-33 - 16 * block - i] ^ v

            r.sendlineafter('> ', '2')
            r.sendafter('username: ', '\n')
            r.sendlineafter('token: ', base64.b64encode(to_send[:len(to_send) - 16 * block]))
            if b'malformed' in r.recvuntil('\n'):
                recovered[-1 - block * 16 - i] = v ^ (i + 1)
                break
        else:
            recovered[-1 - block * 16 - i] = (i + 1)

        print(recovered)

hidden_username = json.loads(unpad(recovered, 16).decode())['username']

r.sendlineafter('> ', '2')
r.sendlineafter('username: ', hidden_username)
r.sendlineafter('token: ', base64.b64encode(enc_token))

r.sendlineafter('> ', '1')
flag = r.recvuntil('\n').strip().decode()

print(flag)
