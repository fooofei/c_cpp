
# bsd 可以生成 core 没找到 PIPE 功能
# bsd 上的 linux Docker Container 仅仅在 core pattern 是 core 的时候能写
#  其他时候都不能

# HOST-linux + Docker-linux 测试成功
# HOST-macOS + Docker-linux 还没学会

docker rm temp

  #--ulimit core=-1 \
  #--privileged \

: '
docker run -it \
  --name="temp" \
  --privileged \
  -v $PWD:/docker_host \
  centos:7_make_gcc_gdb bash -c "cd /docker_host;/usr/bin/bash"
'

# 不需要权限

docker run -it \
  -v $PWD:/docker_host \
  --name="temp" \
  euleros:compile zsh -c "cd /docker_host;zsh"

