#!/usr/bin/python3

from time import sleep
from sys import stdin, stdout, exit
from socket import *

LIMIT = 4096

buf = b''
while True:
  s = stdin.buffer.readline()
  buf += s

  if len(buf) > LIMIT:
    print('You are too greedy')
    exit(0)

  if s == b'\n':
    break

p = socket(AF_INET, SOCK_STREAM)
p.connect(("localhost", 11452))
p.sendall(buf)

sleep(2)

p.setblocking(False)
res = b''
try:
  while True:
    s = p.recv(1024)
    if not s:
      break
    res += s
except:
  pass

stdout.buffer.write(res)
