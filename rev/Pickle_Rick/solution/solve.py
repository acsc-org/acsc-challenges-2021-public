# /usr/bin/env python3.9
import pickle
import dis
import types
import numpy as np

with open("../distfiles/rick.pickle", "rb") as f:
    pickle_rick = f.read()

# You can get this by reversing with pickletools
tree = pickle.loads(pickle_rick[0x435D:0x4A5C] + b".")

# Recover serach function by reading bytecodes
code = pickle.loads(pickle_rick[0x4AD6:0x4BD9] + b".")
code = types.CodeType(*code)
print(dis.dis(code))


def search(a, b):
    c = 0
    while True:
        try:
            a0, a1 = a
            if b % 2 == c:
                a = a1
                b //= 2
                c = 1 - c
            else:
                a = a0
                b //= 2
                c = 1 - c
        except:
            return a[0]


# Recover mix function by reading bytecodes
code = pickle.loads(pickle_rick[0x4C57:0x4D9D] + b".")
code = types.CodeType(*code)
print(dis.dis(code))


def mix(a):
    ln = a.__len__()
    arr = []
    i = 0
    while i < ln:
        s, j = 0, 0
        while j < ln:
            s += (j + 1) * a[(i + j) % ln]
            j += 1
        s %= 257
        assert s < 256
        arr.append(s)
        i += 1
    return arr


# Recover the result
result = pickle.loads(pickle_rick[0x5197:0x5202] + b".")
print(result)

# Reverse the search function
rev = [None for _ in range(256)]
for i in range(256):
    v = search(tree, i)
    rev[v] = i

before_search = [rev[v] for v in result]

# Reverse the mix function
# sage: Matrix(GF(257), [[(j - i) % 21 + 1 for j in range(21)] for i in range(21)]).inverse()
# There are several ways to get the inverse of the matrix
mat_inverse = np.array(
    [
        (216, 57, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8),
        (8, 216, 57, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8),
        (8, 8, 216, 57, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8),
        (8, 8, 8, 216, 57, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8),
        (8, 8, 8, 8, 216, 57, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8),
        (8, 8, 8, 8, 8, 216, 57, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8),
        (8, 8, 8, 8, 8, 8, 216, 57, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8),
        (8, 8, 8, 8, 8, 8, 8, 216, 57, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8),
        (8, 8, 8, 8, 8, 8, 8, 8, 216, 57, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8),
        (8, 8, 8, 8, 8, 8, 8, 8, 8, 216, 57, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8),
        (8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 216, 57, 8, 8, 8, 8, 8, 8, 8, 8, 8),
        (8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 216, 57, 8, 8, 8, 8, 8, 8, 8, 8),
        (8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 216, 57, 8, 8, 8, 8, 8, 8, 8),
        (8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 216, 57, 8, 8, 8, 8, 8, 8),
        (8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 216, 57, 8, 8, 8, 8, 8),
        (8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 216, 57, 8, 8, 8, 8),
        (8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 216, 57, 8, 8, 8),
        (8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 216, 57, 8, 8),
        (8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 216, 57, 8),
        (8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 216, 57),
        (57, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 216),
    ]
)

# mat = np.array([[(j - i) % 21 + 1 for j in range(21)] for i in range(21)])
# print(mat.dot(mat_inverse) % 257)

ans = bytes([v for v in mat_inverse.dot(np.array(before_search)) % 257])
print(ans)
