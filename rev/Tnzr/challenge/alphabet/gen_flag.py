#!/usr/bin/env python3

import numpy as np
from PIL import Image
import glob


alphabet = {}
flag = "ACSC{WELCOM3_T0_TH3_ACSC_W3_N33D_U}"    
target = []
rand_key = [np.random.randint(0,256,size=(15,15)) for _ in range(len(flag))]

def gen_flag():
    for i in glob.glob("*.png"):
        pic = Image.open(i).convert("L")
        pic_n = np.array(pic)
        for row, a_row in enumerate(pic_n):
            for col, value in enumerate(a_row):
                if value > 0:
                    pic_n[row][col] = np.random.randint(1,3) 
        alphabet[i[:i.find(".png")]] = pic_n 

    for idx, flag_byte in enumerate(flag):
        target.append(np.dot(alphabet[flag_byte],rand_key[idx]))

def check_flag_vanilla():
    img = np.zeros([15,15*len(flag)],dtype=np.uint8)
    img.fill(0) 
    img = Image.fromarray(img)
    for idx in range(len(flag)):
        t = np.dot(target[idx],np.linalg.inv(rand_key[idx]))
        t = np.dot(t,70)
        img.paste(Image.fromarray(t),(15*idx,0))
    img.show()

def check_flag_tile():
    black = Image.open("assets/black.png")
    green = Image.open("assets/green.png")
    purple = Image.open("assets/purple.png")
    pic_n = np.array(purple)
    tile_name = [black, green, purple]
    img = np.zeros([48*15,48*15*len(flag),4],dtype=np.uint8)
    img.fill(0) 
    img = Image.fromarray(img)
    z = []
    for idx in range(len(flag)):
        t = np.dot(target[idx],np.linalg.inv(rand_key[idx]))
        for row, a_row in enumerate(t):
            for col, value in enumerate(a_row):
                img.paste(tile_name[int(value+0.5)],(col*48+(idx*15*48),row*48))

    img.show()
    img.save("solve.png")

def write_data():
    with open("flag.h",'w') as f:
        dic={"[": "{", "]":"}" , "(": None, ")": None}
        dic = { ord(k): dic[k]  for k in dic}
        f.write(f"int rand_key[{len(rand_key)}][15][15] = ")
        f.write( str(rand_key).translate(dic).replace("array","")+";\n")
        f.write(f"int target[{len(target)}][15][15] = ")
        f.write( str(target).translate(dic).replace("array","")+";\n")
    canvas = [alphabet[flag_byte] for flag_byte in flag]
    with open('solve.npy', 'wb') as f:
        np.save(f, canvas)
    with open("solve.h",'w') as f:
        dic={"[": "{", "]":"}" , "(": None, ")": None}
        dic = { ord(k): dic[k]  for k in dic}
        f.write(f"int canvas[{len(rand_key)}][15][15] = ")
        f.write( str(canvas).translate(dic).replace("array","").replace(", dtype=uint8",'')+";\n")

if __name__ == "__main__":
    print("generating flag...")
    gen_flag()
    check_flag_vanilla()
    check_flag_tile()
    write_data()
