
docker run -it \
  --ulimit core=-1 \
  --privileged \
  -v /Users/hujianfei/PycharmProjects/untitled2:/docker_host \
  centos:7_make_gcc_gdb bash -c "cd /docker_host/cores;/usr/bin/bash"
