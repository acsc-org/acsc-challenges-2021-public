import secrets
p = random_prime(1 << 512)

flag = b"ACSC{have_you_already_read_the_swap<-->swap?}"
flag += secrets.token_bytes(64 - len(flag))
x = int.from_bytes(flag, "big")
y = randint(2, p)

assert 0 < x < p
assert 0 < y < p
assert x != y

F.<a> = GF(p)[]

f = (y^3 - x^3) + a*(y - x) - (x^2 - y^2)
a = int(f.roots()[0][0])

F.<b> = GF(p)[]
f1 = x^3 + a*x + b - y^2
f2 = y^3 + a*y + b - x^2

b1 = int(f1.roots()[0][0])
b2 = int(f2.roots()[0][0])
assert b1 == b2
b = b1

EC = EllipticCurve(GF(p), [a, b1])
assert EC(x, y)
assert EC(y, x)

print("p = {}".format(p))
print("a = {}".format(a))
print("b = {}".format(b))
