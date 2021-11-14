#!/usr/bin/env python3

import numpy as np
from PIL import Image
from flag_data import *


img = np.zeros([15,15*35],dtype=np.uint8)
img.fill(0) 
img = Image.fromarray(img)
for idx in range(35):
    t = np.dot(target[idx],np.linalg.inv(rand_key[idx]))
    t = np.dot(t,70)
    img.paste(Image.fromarray(t),(15*idx,0))
img.show()
