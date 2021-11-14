from output import *

# 1st part of the flag:
from Crypto.Util.number import *
q = pow(2,(128-flag1_len)*8)
r = bytes_to_long(bytes([128-flag1_len])*(128-flag1_len))
left = c2 - q*q*c1 - r*r - B*r
right = -q*q*B + 2*q*r + B*q
m1 = (left * inverse(right,n))%n
m1 = long_to_bytes(m1)
print(m1)

# 2nd part of the flag:
PRxy.<x,y> = PolynomialRing(Zmod(n))
PRx.<xn> = PolynomialRing(Zmod(n))
PRZZ.<xz,yz> = PolynomialRing(Zmod(n))

g1 = x*(x+B) - hard_c1
g2 = (x+y)*(x+y+B) - hard_c2

q1 = g1.change_ring(PRZZ)
q2 = g2.change_ring(PRZZ)

h = q2.resultant(q1)
h = h.univariate_polynomial()
h = h.change_ring(PRx).subs(y=xn)
h = h.monic()
diff = h.small_roots(X=2^((128-flag2_len)*8),epsilon=0.03)

print(diff)

def gcd_(a, b):
  if b == 0: return a
  return gcd_(b, a % b)

for d in diff:
  PRx.<x> = PolynomialRing(Zmod(n))
  y = d
  g1 = x*(x+B) - hard_c1
  g2 = (x+y)*(x+y+B) - hard_c2
  q,r = gcd_(g1,g2).coefficients()
  m2 = -q * r^(-1)
  print(m1+long_to_bytes(m2))
  print(m1+long_to_bytes(-m2))

