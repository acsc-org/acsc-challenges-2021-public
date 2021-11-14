cmd = (b"echo 'There are a lot of Capture The Flag (CTF) competitions in "
       b"our days, some of them have excelent tasks, but in most cases "
       b"they're forgotten just after the CTF finished. We decided to make"
       b" some kind of CTF archive and of course, it'll be too boring to "
       b"have just an archive, so we made a place, where you can get some "
       b"another CTF-related info - current overall Capture The Flag team "
       b"rating, per-team statistics etc'")


flag_cmd = b"cat flag;echo 'A" 
b1 = b"G(NBB>35,YCC@P/4"
b2 = b"0+1781J3)R2;=G>H"
b1 = b';OJG>4I;PK=0:VGG'
b2 = b'.JNSTW@--U71/OX6'

cmd = flag_cmd + b1 + b2 + cmd[48:]

from pwn import *

r = remote('localhost', 10217)

print(r.recvrepeat(0.5))
r.sendline(b'S')
print(r.recvrepeat(0.5))
r.sendline(cmd)
print(r.recvrepeat(0.5))
r.sendline(b'E')
r.interactive()
