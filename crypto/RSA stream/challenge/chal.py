import gmpy2
from Crypto.Util.number import long_to_bytes, bytes_to_long, getStrongPrime, inverse
from Crypto.Util.Padding import pad

from flag import m
#m = b"ACSC{<REDACTED>}" # flag!

f = open("chal.py","rb").read() # I'll encrypt myself!
print("len:",len(f))
p = getStrongPrime(1024)
q = getStrongPrime(1024)

n = p * q
e = 0x10001
print("n =",n)
print("e =",e)
print("# flag length:",len(m))
m = pad(m, 255)
m = bytes_to_long(m)

assert m < n
stream = pow(m,e,n)
cipher = b""

for a in range(0,len(f),256):
  q = f[a:a+256]
  if len(q) < 256:q = pad(q, 256)
  q = bytes_to_long(q)
  c = stream ^ q
  cipher += long_to_bytes(c,256)
  e = gmpy2.next_prime(e)
  stream = pow(m,e,n)

open("chal.enc","wb").write(cipher)

