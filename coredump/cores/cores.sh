
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
  echo "|$pypath %t %p %u %e" | sudo tee $corepath
  #echo "|$pypath %t %p %u %e" > $corepath
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
