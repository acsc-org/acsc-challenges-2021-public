# /usr/bin/env python3
import pickle
import sys

# Check version >= 3.9
if sys.version_info[0] != 3 or sys.version_info[1] < 9:
    print("Check your Python version!")
    exit(0)

# This function is truly amazing, so do not fix it!
def amazing_function(a, b, c=None):
    if type(b) == int:
        return a[b]
    else:
        return (
            f"CORRECT! The flag is: ACSC{{{c.decode('ascii')}}}" if a == b else "WRONG!"
        )


with open("rick.pickle", "rb") as f:
    pickle_rick = f.read()

rick_says = b"Wubba lubba dub-dub!!"  # What is the right input here?
assert type(rick_says) == bytes and len(rick_says) == 21
pickle.loads(pickle_rick)
