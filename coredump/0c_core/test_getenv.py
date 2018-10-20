#coding=utf-8

import os
import sys



def entry():
  print('[+] Test which environment variable to use')
  home = os.getenv('home')
  print('home={home}'.format(home=home))
  home = os.getenv("HOME")
  print('HOME={home}'.format(home=home))

if __name__ == '__main__':
    entry()
