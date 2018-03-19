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
import time

curpath = os.path.realpath(__file__)
curpath = os.path.dirname(curpath)


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
    return fullpath


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
    return fullpath


def tick_called(msg):
    '''
    用来记录被调用 测试脚本时使用
    '''
    called = os.path.join(curpath, 'called')
    with open(called, 'ab+') as fw:
        fw.write('{t} called msg={m}\n'.format(t=datetime.datetime.now(), m=msg))


def core_read(**kwargs):
    '''
    path_corepattern
    '''
    with open(kwargs.get('path_corepattern', ''), 'rb') as fr:
        c = fr.read()
        return c


def core_set(**kwargs):
    '''
    path_corepattern
    '''
    path_corepattern = kwargs.get('path_corepattern', '')
    print('[+] Before register the content of {f} ={c}'
        .format(f=path_corepattern, c=core_read(**kwargs)))
    towrite = '|/usr/bin/python {f} %p %u %h %e'.format(f=os.path.realpath(__file__))
    with open(path_corepattern, 'wb') as fw:
        fw.write(towrite)
    print('[+] After register the content of {f} ={c}'
        .format(f=path_corepattern, c=core_read(**kwargs)))


def core_restore(**kwargs):
    '''
    path_corepattern
    '''
    path_corepattern = kwargs.get('path_corepattern', '')
    print('[+] Before restore the content of {f} ={c}'
        .format(f=path_corepattern, c=core_read(**kwargs)))
    towrite = 'core'
    with open(path_corepattern, 'wb') as fw:
        fw.write(towrite)
    print('[+] After restore the content of {f} ={c}'
        .format(f=path_corepattern, c=core_read(**kwargs)))

def core_generate(**kwargs):
    '''
    core_pid
    core_uid
    core_hostname
    core_execname
    core_saved_path
    core_input

    :return fullpath_core
    '''
    now = time.time() * 1000
    now = int(now)
    filename = 'core_time-{t}_pid-{pid}_uid-{uid}_host-{hostname}_name-{execname}'.format(
        t=now, pid=kwargs.get('core_pid', ''), uid=kwargs.get('core_uid', ''),
        hostname=kwargs.get('core_hostname', ''), execname=kwargs.get('core_execname', '')
    )
    saved = kwargs.get('core_saved_path', None) or curpath
    fullpath = os.path.join(saved, filename)
    return redirect_to_gzfile(fullpath, kwargs.get('core_input', sys.stdin))


def entry():
    fullpath_core_pattern = '/proc/sys/kernel/core_pattern'

    function_args = {
        'path_corepattern': fullpath_core_pattern,

    }

    parser = argparse.ArgumentParser(description='Use for take over the core by pipe',
                                     version='1.0')
    parser.add_argument('--set', action='store_true',
                        help='Register this python file in {f} for core dump pipe'.format(f=fullpath_core_pattern))
    parser.add_argument('--restore', action='store_true',
                        help='Restore the file {f}'.format(f=fullpath_core_pattern))

    parser.add_argument('--testcore', action='store_true',
                        help='Test core generate')

    args, unknownargs = parser.parse_known_args()

    if args.restore:
        core_restore(**function_args)
    elif args.set:
        core_set(**function_args)

    elif args.testcore:
        fp = open(os.path.join(curpath, 'core_test'), 'rb')
        function_args.update({
            'core_pid': 1, 'core_uid': 2,
            'core_hostname': 3,
            'core_execname': 4,
            'core_saved_path': curpath,
            'core_input': fp,
        })
        v = core_generate(**function_args)
        fp.close()
        print('[+] Generate core {f}'.format(f=v))
    elif len(unknownargs) > 3:
        function_args.update({
            'core_pid': unknownargs[0], 'core_uid': unknownargs[1],
            'core_hostname': unknownargs[2],
            'core_execname': unknownargs[3],
            'core_input': sys.stdin,
        })
        core_generate(**function_args)
    else:
        parser.print_help()


if __name__ == '__main__':
    sy = sys.version_info
    if not (sy.major >= 2 and sy.minor >= 7):
        raise ValueError('only support Python version up 2.7.x')
    entry()
