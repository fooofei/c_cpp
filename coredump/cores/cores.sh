
corepath=/proc/sys/kernel/core_pattern

viewcore()
{
  echo "core_pattern is:" `cat $corepath` ", the limit is :" `ulimit -c`
}

setcore()
{
  viewcore
  echo "-----------------------------------------"
  pypath=$(readlink -f ./take_core.py)
  echo "|/usr/bin/python2 $pypath %p %u %h %e" | sudo tee $corepath
  ulimit -c unlimited
  echo "-----------------------------------------"
  viewcore
}


restorecore()
{
  viewcore
  echo "core" | sudo tee $corepath
  viewcore
}

# from http://man7.org/linux/man-pages/man5/core.5.html
setsystemdcore()
{
  viewcore
  echo "-----------------------------------------"
  echo "|/usr/lib/systemd/systemd-coredump %P %u %g %s %t %c %e" | sudo tee $corepath
  ulimit -c unlimited
  echo "-----------------------------------------"
  viewcore
}