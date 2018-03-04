

docker run -it \
  --ulimit core=-1 \
  --privileged \
  -v $PWD:/docker_host \
  centos:7_make_gcc_gdb bash -c "cd /docker_host;/usr/bin/bash"

