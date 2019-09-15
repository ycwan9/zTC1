#! /bin/env python3

import os
import glob
import binascii
import gzip
try:
    io = __import__("io").BytesIO
except:
    io = __import__("StringIO").StringIO

for fn in glob.glob('*.html'):
    s = open(fn, 'rb').read()
    dat = io()
    with gzip.GzipFile(fileobj=dat, mode="w") as f:
        f.write(s)
    dat = dat.getvalue()
    try:
        s = ','.join(["0x%02x" % c for c in dat])
    except:
        s = ','.join(["0x"+binascii.hexlify(c) for c in dat])

    fn = fn.replace('.', '_')
    print("const unsigned char %s[0x%x] = {%s};" % (fn, len(dat), s))

