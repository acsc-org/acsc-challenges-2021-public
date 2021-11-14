import os
import string
from Crypto.Cipher import AES, ARC4, DES

BLOCK = 16


def bxor(a, b):
  res = [c1 ^ c2 for (c1, c2) in zip(a, b)]
  return bytes(res)


def block_hash(data):
  data = AES.new(data, AES.MODE_ECB).encrypt(b"\x00" * AES.block_size)
  data = ARC4.new(data).encrypt(b"\x00" * DES.key_size)
  data = DES.new(data, DES.MODE_ECB).encrypt(b"\x00" * DES.block_size)
  return data[:-2]


def hash(data):
  length = len(data)
  if length % BLOCK != 0:
    pad_len = BLOCK - length % BLOCK
    data += bytes([pad_len] * pad_len)
    length += pad_len
  block_cnt = length // BLOCK
  blocks = [data[i * BLOCK:(i + 1) * BLOCK] for i in range(block_cnt)]
  res = b"\x00" * BLOCK
  for block in blocks:
    res = bxor(res, block_hash(block))
  return res


def check(cmd, new_cmd):
  if len(cmd) != len(new_cmd):
    return False
  if hash(cmd) != hash(new_cmd):
    return False
  for c in new_cmd:
    if chr(c) not in string.printable:
      return False
  return True


cmd = (b"echo 'There are a lot of Capture The Flag (CTF) competitions in "
       b"our days, some of them have excelent tasks, but in most cases "
       b"they're forgotten just after the CTF finished. We decided to make"
       b" some kind of CTF archive and of course, it'll be too boring to "
       b"have just an archive, so we made a place, where you can get some "
       b"another CTF-related info - current overall Capture The Flag team "
       b"rating, per-team statistics etc'")


def menu():
  print("[S]tore command")
  print("[E]xecute command")
  print("[F]iles")
  print("[L]eave")
  return input("> ")


while True:
  choice = menu()
  if choice[0] == "S":
    new_cmd = input().encode()
    if check(cmd, new_cmd):
      cmd = new_cmd
    else:
      print("Oops!")
      exit(1)
  elif choice[0] == "E":
    os.system(cmd)
  elif choice[0] == "F":
    os.system(b"ls")
  elif choice[0] == "L":
    break
  else:
    print("Command Unsupported")
    exit(1)
