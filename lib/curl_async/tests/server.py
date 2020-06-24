#coding=utf-8
'''
this file shows build a simple server for test curl async

'''

import os
import sys
import SimpleHTTPServer
import SocketServer
import urlparse
import time

class Myhandler(SimpleHTTPServer.SimpleHTTPRequestHandler,object):
  '''
  only get the sleep number from client

  SimpleHTTPRequestHandler's base class is not devide from object,
  we cannot use super(Myhandler,self).do_GET() to call super method

  on Windows erron, not have on linux
  AttributeError: class SimpleHTTPRequestHandler has no attribute 'do_POST'

  '''
  def do_GET(self):

    # when put the server on linux, the path will be two
    # one is /?sleep_time=1 and one is /?sleep_time=1/
    # the second is not our expect
    # when put the server on Windows, the path is only one.
    # https://stackoverflow.com/questions/14984401/simplehttpserver-and-socketserver
    # print twice
    path = self.path
    #path = path.rstrip('/')
    #query_components = urlparse.parse_qs(urlparse.urlparse(path).query)
    #tsleep = query_components.get('sleep_time',[0])
    # when use Content-Type: application/x-www-form-urlencoded
    # the path is not work
    data = self.rfile.read(int(self.headers['Content-Length']))
    # data is 'sleep_time=1'
    print('[+] GET data={data}'.format(data=data))
    tsleep = data.split('=')[1]
    time.sleep(int(tsleep[0]))
    SimpleHTTPServer.SimpleHTTPRequestHandler.do_GET(self)


def entry():

  host_this = ('', 8000)

  while 1:
    try:
      # SocketServer.TCPServer is for single connection
      httpd = SocketServer.ThreadingTCPServer(host_this, Myhandler)
      break
    except Exception as er:
      print(er)

  print('[+] already build server at {host}'.format(host=host_this))

  httpd.serve_forever()


if __name__ == '__main__':
    entry()
