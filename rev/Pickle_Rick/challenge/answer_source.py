import pickle
import random
import struct
from banner import banner


def object_to_pickle(arg):
    if type(arg) == tuple:
        ret = b"("
        for v in arg:
            ret += object_to_pickle(v)
        ret += b"t"
        return ret
    elif type(arg) == int:
        return b"J" + struct.pack("<i", arg)
    elif type(arg) == str:
        return b'S"' + arg.encode() + b'"\n'
    elif type(arg) == bytes:
        ln = struct.pack("<I", len(arg))
        return b"B" + ln + arg
    elif arg is None:
        return b"N"


def function_to_code_object_args(func):
    code_object_args = (
        func.__code__.co_argcount,
        func.__code__.co_kwonlyargcount,
        func.__code__.co_posonlyargcount,
        func.__code__.co_nlocals,
        func.__code__.co_stacksize,
        func.__code__.co_flags,
        func.__code__.co_code,
        func.__code__.co_consts,
        func.__code__.co_names,
        func.__code__.co_varnames,
        "something_suspicious.py",  # co_filename,
        func.__code__.co_name,
        func.__code__.co_firstlineno,
        func.__code__.co_lnotab,
        func.__code__.co_freevars,
        func.__code__.co_cellvars,
    )

    return object_to_pickle(code_object_args)


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


flag = b"YEAH!I'm_pickle-RICK!"
sbox = list(range(256))
random.shuffle(sbox)

mixed = mix(flag)
subed = [sbox[v] for v in mixed]

search_pickle = function_to_code_object_args(search)
mix_pickle = function_to_code_object_args(mix)


def construct_tree(depth, index):
    if depth == 8:
        return b"J" + struct.pack("<i", sbox[index]) + b"\x85"

    flip = (depth + 1) % 2
    left_subtree = construct_tree(depth + 1, index + (flip << depth))
    right_subtree = construct_tree(depth + 1, index + ((1 - flip) << depth))

    return left_subtree + right_subtree + b"\x86"


tree = construct_tree(0, 0)


def amazing_function(a, b, c=None):
    if type(b) == int:
        return a[b]
    else:
        return (
            f"CORRECT! The flag is: ACSC{{{c.decode('ascii')}}}" if a == b else "WRONG!"
        )


rick_says = b"YEAH!I'm_pickle-RICK!"
assert len(rick_says) == 21

pickle_rick = (
    # Print banner
    b"cbuiltins\nprint\n"
    + b"T"
    + len(banner).to_bytes(4, "little")
    + banner
    + b"\x85"
    + b"R"
    +
    # Print input
    b"cbuiltins\nprint\n"
    + b'S"Pickle Rick says:"\n'
    + b"\x85"
    + b"R"
    + b"cbuiltins\nprint\n"
    + b"c__main__\nrick_says\n"
    + b"\x85"
    + b"R"
    + b"cbuiltins\nprint\n"
    + b'S"The flag machine says:"\n'
    + b"\x85"
    + b"R"
    +
    # Memoize tree in memo[0]
    tree
    + b"\x94"
    + b"0"
    +
    # Memoize search code object in memo[1]
    b"cbuiltins\ntype\n"
    + b"c__main__\namazing_function\n"
    + b"\x85R"
    + b"cbuiltins\ntype\n"
    + b"cbuiltins\ngetattr\n"
    + b"c__main__\namazing_function\n"
    + b'S"__code__"\n'
    + b"\x86R\x85R"
    + search_pickle
    + b"R"
    + b"}"
    + b"\x86R"
    + b"\x94"
    + b"0"
    +
    # Memozie mixed input in memo[2]
    b"cbuiltins\ntype\n"
    + b"c__main__\namazing_function\n"
    + b"\x85R"
    + b"cbuiltins\ntype\n"
    + b"cbuiltins\ngetattr\n"
    + b"c__main__\namazing_function\n"
    + b'S"__code__"\n'
    + b"\x86R\x85R"
    + mix_pickle
    + b"R"
    + b"}"
    + b"\x86R"
    + b"c__main__\nrick_says\n"
    + b"\x85R"
    + b"\x94"
    + b"0"
)

# Prepare print + compare
pickle_rick += b"cbuiltins\nprint\n" + b"c__main__\namazing_function\n"

# Push (search(tree, mixed[i]) for i in range(21))
pickle_rick += b"("

for i in range(21):
    pickle_rick += (
        # Search tree
        b"g1\ng0\n"
        + b"c__main__\namazing_function\n"
        + b"g2\n"
        + b"J"
        + struct.pack("<i", i)
        + b"\x86R"
        + b"\x86R"
    )

pickle_rick += b"t"

# Push subed
pickle_rick += b"("

for v in subed:
    pickle_rick += b"J" + struct.pack("<i", v)

pickle_rick += b"t"

pickle_rick += b"c__main__\nrick_says\n"

pickle_rick += (
    # Compare string and get output
    b"\x87R"
    +
    # Print output
    b"\x85R"
    +
    # End
    b"."
)

with open("rick.pickle", "wb") as f:
    f.write(pickle_rick)

print(pickle.loads(pickle_rick))
