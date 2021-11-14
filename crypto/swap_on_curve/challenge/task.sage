from params import p, a, b, flag, y

x = int.from_bytes(flag, "big")

assert 0 < x < p
assert 0 < y < p
assert x != y

EC = EllipticCurve(GF(p), [a, b])

assert EC(x,y)
assert EC(y,x)

print("p = {}".format(p))
print("a = {}".format(a))
print("b = {}".format(b))
