#coding=utf-8

import os
import sys



def entry():
  home = os.getenv('home')
  print('home={home}'.format(home=home))

if __name__ == '__main__':
    entry()
