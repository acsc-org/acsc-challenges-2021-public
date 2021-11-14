import ast

with open("./distfiles/output.txt") as f:
    p = ast.literal_eval(f.readline().strip().split(" = ")[1])
    a = ast.literal_eval(f.readline().strip().split(" = ")[1])
    b = ast.literal_eval(f.readline().strip().split(" = ")[1])

F.<x, y> = ZZ[]

f = x^3 + a*x + b - y^2
g = y^3 + a*y + b - x^2

for root in f.resultant(g, y).univariate_polynomial().change_ring(GF(p)).roots():
    flag = int(root[0]).to_bytes(100, "big")
    if b"ACSC{" in flag:
        print(flag[:flag.find(b"}") + 1].strip(b"\0").decode())

