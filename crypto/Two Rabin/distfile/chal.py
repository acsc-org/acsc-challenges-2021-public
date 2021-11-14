import random
from Crypto.Util.number import *
from Crypto.Util.Padding import pad

from flag import flag

p = getStrongPrime(512)
q = getStrongPrime(512)
n = p * q
B = getStrongPrime(512)

m = flag[0:len(flag)//2]
print("flag1_len =",len(m))

m1 = bytes_to_long(m)
m2 = bytes_to_long(pad(m,128))

assert m1 < n
assert m2 < n

c1 = (m1*(m1+B)) % n
c2 = (m2*(m2+B)) % n

print("n =",n)
print("B =",B)
print("c1 =",c1)
print("c2 =",c2)

# Harder!

m = flag[len(flag)//2:]
print("flag2_len =",len(m))

m1 = bytes_to_long(m)
m1 <<= ( (128-len(m))*8 )
m1 += random.SystemRandom().getrandbits( (128-len(m))*8 )

m2 = bytes_to_long(m)
m2 <<= ( (128-len(m))*8 )
m2 += random.SystemRandom().getrandbits( (128-len(m))*8 )

assert m1 < n
assert m2 < n

c1 = (m1*(m1+B)) % n
c2 = (m2*(m2+B)) % n

print("hard_c1 =",c1)
print("hard_c2 =",c2)

