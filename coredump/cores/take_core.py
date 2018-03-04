#!/usr/bin/env python2
#coding=utf-8
'''

###
 这个文件用来接管 控制 coredump 的存储

### 需手动
  echo "|/usr/bin/python /docker_host/1.py" > /proc/sys/kernel/core_pattern

### sys.stdin 如果没有数据 返回 ''，不是 None

###
  容器里对 core 对操作会持久化到 image 里
  预期在 container 中做的修改 如果不 commit ，下次从相同到 image 进入 container
  core 应该还是未修改的， 在 macOS 测试却是修改过的

### https://favoorr.github.io/2017/02/10/learn-gdb-from-brendan/

### 长时间被困在 core_pattern 设置了后，无法生成 core

'''
import os
import sys
import datetime
import io

curpath = os.path.realpath(__file__)
curpath = os.path.dirname(curpath)

def redirect_to_file(fullpath):
  if os.path.exists(fullpath):
    os.remove(fullpath)

  with open(fullpath,'wb') as fw:
    while 1:
      c = sys.stdin.read(1024)
      if not c:
        break
      fw.write(c)

def redirect_to_gzfile(fullpath):
  pass

def entry():
  now = datetime.datetime.now()
  now = now.strftime('%s')
  filename = 'core_time-{t}_pid-{pid}_uid-{uid}_host-{hostname}_name-{execname}'.format(
    t=now,pid=sys.argv[1],uid=sys.argv[2],
    hostname=
  )
  fullpath =os.path.join(curpath,filename)
  if os.path.exists(fullpath):
    os.remove(fullpath)
  with open(fullpath,'wb') as fw:
     while 0:
      c = sys.stdin.read(4)
      if not c:
        break
      fw.write(c)


if __name__ == '__main__':
  sy = sys.version_info
  if not (sy.major>=2 and sy.minor >= 7):
    raise  ValueError('only support Python version up 2.7.x')
  entry()
