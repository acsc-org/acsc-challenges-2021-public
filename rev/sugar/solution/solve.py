#!/usr/bin/env python3
import binascii
import uuid

from Crypto.Cipher import AES

KEY         = binascii.unhexlify("a186282314bb20353fea9fb3b09ef6cd")
IV          = b'\x00' * 16
DISKGUID    = uuid.UUID('644b505a-2ad7-4b3d-a40a-a0fa8e32d35d').bytes_le

if __name__ == '__main__':
    aes = AES.new(KEY, AES.MODE_CBC, IV)
    print('ACSC{' + format(binascii.hexlify(aes.encrypt(DISKGUID)).decode('ascii')) + '}')
