#coding=utf-8
'''
this file shows a client in python use to test server

there also will have a c client which have the save functional

'''


import os
import sys
import requests
from timeit import default_timer as Time

def entry():

  url = 'http://localhost:8000'

  start = Time()

  # the server will sleep(sleep_time) to response
  resp = requests.get(url,data={"sleep_time":1})
  #resp = requests.post(url,data={"sleep_time":1}) # server not support

  end = Time()

  print('[+] request code={code} content={content} elapse={elapse}'
  .format(code=resp.status_code,content=resp.content,elapse=end-start))

if __name__ == '__main__':
    entry()
