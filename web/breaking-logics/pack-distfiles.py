import os
from zipfile import ZipFile
fake = 'fake_flag.jsp'
dst = 'distfiles/breaking-logics-dist-file.zip'
if os.path.exists(dst):
    os.remove(dst)
zipObj = ZipFile(dst, 'w')
for a,b,c in os.walk('challenge'):
    for f in c:
        f = '%s/%s' % (a,f)
        if 'flag' in f:
            zipObj.write(fake, f)
            print '[*] add fake flag ->', fake
        else:
            zipObj.write(f)
            print f



zipObj.close()
