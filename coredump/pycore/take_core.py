#!/usr/bin/env python
# coding=utf-8
'''

###
 这个文件用来接管 控制 coredump 的存储

### 需手动
  echo "|/usr/bin/python /docker_host/1.py" > /proc/sys/kernel/core_pattern

### sys.stdin 如果没有数据 返回 ''，不是 None

###
  容器里对 core 对操作会持久化到 image 里
  预期在 container 中对 /proc/sys/kernel/core_pattern 做的修改 如果不 commit ，
  下次从相同到 image 进入 container core 应该还是未修改的，
  在 macOS 测试却是修改过的

###
  core dump 的小说明
  https://favoorr.github.io/2017/02/10/learn-gdb-from-brendan/

'''
import os
import sys
import datetime
import io
import contextlib
import gzip
import argparse

curpath = os.path.realpath(__file__)
curpath = os.path.dirname(curpath)
fullpath_core_pattern = '/proc/sys/kernel/core_pattern'


def redirect_to_file(fullpath, inputs):
  '''
  Write input to file, not compress
  '''
  if os.path.exists(fullpath):
    os.remove(fullpath)

  with open(fullpath, 'wb') as fw:
    while 1:
      c = inputs.read(1024)
      if not c:
        break
      fw.write(c)


def redirect_to_gzfile(fullpath, inputs):
  '''
  Write input to file, with compress
  '''
  if os.path.exists(fullpath):
    os.remove(fullpath)
  if not fullpath.endswith('.gz'):
    fullpath = fullpath + '.gz'
  if os.path.exists(fullpath):
    os.remove(fullpath)

  with gzip.open(fullpath, 'wb', compresslevel=9) as fw:
    with contextlib.closing(io.BufferedWriter(fw)) as fww:
      while 1:
        c = inputs.read(1024)
        if not c:
          break
        fww.write(c)


def tick_called():
  '''
  用来记录被调用 测试脚本时使用
  '''
  called = os.path.join(curpath, 'called')
  with open(called, 'ab+') as fw:
    fw.write('{t} called\n'.format(t=datetime.datetime.now()))


def core_generate(args_others):
  inputs = sys.stdin
  # core_pid=sys.argv[1]
  # core_uid=sys.argv[2]
  # core_hostname=sys.argv[3]
  # core_execname=sys.argv[4]
  core_pid = args_others[0]
  core_uid = args_others[1]
  core_hostname = args_others[2]
  core_execname = args_others[3]

  now = datetime.datetime.now()
  now = now.strftime('%s')
  filename = 'core_time-{t}_pid-{pid}_uid-{uid}_host-{hostname}_name-{execname}'.format(
    t=now, pid=core_pid, uid=core_uid,
    hostname=core_hostname, execname=core_execname
  )
  fullpath = os.path.join(curpath, filename)
  redirect_to_gzfile(fullpath, inputs)


def core_read():
  with open(fullpath_core_pattern, 'rb') as fr:
    c = fr.read()
    return c


def core_set():
  print('[+] Before register the content of {f}={c}'.format(f=fullpath_core_pattern, c=core_read()))
  towrite = '|/usr/bin/python {f} %p %u %h %e'.format(f=os.path.realpath(__file__))
  with open(fullpath_core_pattern, 'wb') as fw:
    fw.write(towrite)
  print('[+] After register the content of {f}={c}'.format(f=fullpath_core_pattern, c=core_read()))


def core_restore():
  print('[+] Before restore the content of {f}={c}'.format(f=fullpath_core_pattern, c=core_read()))
  towrite = 'core'
  with open(fullpath_core_pattern, 'wb') as fw:
    fw.write(towrite)
  print('[+] After restore the content of {f}={c}'.format(f=fullpath_core_pattern, c=core_read()))


def entry():
  parser = argparse.ArgumentParser(description='Use for take over the core by pipe',
                                   version='1.0')
  parser.add_argument('--set', action='store_true',
                      help='Register this python file in {f} for core dump'.format(f=fullpath_core_pattern))
  parser.add_argument('--restore', action='store_true',
                      help='Restore the file {f}'.format(f=fullpath_core_pattern))
  args, unknownargs = parser.parse_known_args()

  if args.restore:
    core_restore()
  elif args.set:
    core_set()
  else:
    core_generate(unknownargs)


if __name__ == '__main__':
  sy = sys.version_info
  if not (sy.major >= 2 and sy.minor >= 7):
    raise ValueError('only support Python version up 2.7.x')
  entry()
