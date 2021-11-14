import binascii
import sys

from Crypto.Cipher import AES

NONCE = binascii.unhexlify(''.join("""
70 e1 97 39 ec 85 d7 01 df a4 00 00 
""".split()))

MAC = binascii.unhexlify(''.join("""
a7 8f 71 1c 37 2f 4a 1e 69 09 44 c4 8e c6 e4 f9 
""".split()))

FVEK = binascii.unhexlify(''.join("""
61 09 1c 95 fd a2 dd 20-0b 74 30 9a dd 7d 78 25 
1b 95 61 38 fa b3 b8 15-13 ac 5a 84 80 e3 fb a4 
3c 95 5f 9d 7f 23 fc 22-9d b4 00 86 
""".split()).replace('-', ''))

if __name__ == '__main__':
    key = binascii.unhexlify(sys.argv[1])

    if len(key) != 0x20:
        print('Wrong!')
        exit(1)

    cipher = AES.new(key, AES.MODE_CCM, nonce=NONCE)
    try:
        plaintext = cipher.decrypt_and_verify(FVEK, MAC)
    except ValueError as e:
        if 'MAC check failed' in str(e):
            print('Wrong!')
            exit(1)
        else:
            raise e
    else:
        flag = 'ACSC{{{}}}'.format(
            binascii.hexlify(key).decode('ascii')
        )
        print(f'Correct!')
        print(f'The flag is "{flag}".')
        exit(0)
