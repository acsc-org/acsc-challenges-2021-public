#!/usr/bin/env python3

import base64
import json
import os
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
from secret import hidden_username, flag

key = os.urandom(16)
iv1 = os.urandom(16)
iv2 = os.urandom(16)


def encrypt(msg):
    aes1 = AES.new(key, AES.MODE_CBC, iv1)
    aes2 = AES.new(key, AES.MODE_CBC, iv2)
    enc = aes2.encrypt(aes1.encrypt(pad(msg, 16)))
    return iv1 + iv2 + enc


def decrypt(msg):
    iv1, iv2, enc = msg[:16], msg[16:32], msg[32:]
    aes1 = AES.new(key, AES.MODE_CBC, iv1)
    aes2 = AES.new(key, AES.MODE_CBC, iv2)
    msg = unpad(aes1.decrypt(aes2.decrypt(enc)), 16)
    return msg


def create_user():
    username = input("Your username: ")
    if username:
        data = {"username": username, "is_admin": False}
    else:
        # Default token
        data = {"username": hidden_username, "is_admin": True}
    token = encrypt(json.dumps(data).encode())
    print("Your token: ")
    print(base64.b64encode(token).decode())


def login():
    username = input("Your username: ")
    token = input("Your token: ").encode()
    try:
        data_raw = decrypt(base64.b64decode(token))
    except:
        print("Failed to login! Check your token again")
        return None

    try:
        data = json.loads(data_raw.decode())
    except:
        print("Failed to login! Your token is malformed")
        return None

    if "username" not in data or data["username"] != username:
        print("Failed to login! Check your username again")
        return None

    return data


def none_menu():
    print("1. Create user")
    print("2. Log in")
    print("3. Exit")

    try:
        inp = int(input("> "))
    except ValueError:
        print("Wrong choice!")
        return None

    if inp == 1:
        create_user()
        return None
    elif inp == 2:
        return login()
    elif inp == 3:
        exit(0)
    else:
        print("Wrong choice!")
        return None


def user_menu(user):
    print("1. Show flag")
    print("2. Log out")
    print("3. Exit")

    try:
        inp = int(input("> "))
    except ValueError:
        print("Wrong choice!")
        return None

    if inp == 1:
        if "is_admin" in user and user["is_admin"]:
            print(flag)
        else:
            print("No.")
        return user
    elif inp == 2:
        return None
    elif inp == 3:
        exit(0)
    else:
        print("Wrong choice!")
        return None


def main():
    user = None

    print("Welcome to CBCBC flag sharing service!")
    print("You can get the flag free!")
    print("This is super-duper safe from padding oracle attacks,")
    print("because it's using CBC twice!")
    print("=====================================================")

    while True:
        if user:
            user = user_menu(user)
        else:
            user = none_menu()


if __name__ == "__main__":
    main()
