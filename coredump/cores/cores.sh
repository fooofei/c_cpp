
corepath=/proc/sys/kernel/core_pattern

viewcore()
{
  echo "core_pattern is:" `cat $corepath` ", the limit is :" `ulimit -c`
}

setcore()
{
  viewcore
  echo "-----------------------------------------"
  echo "|/home/test_samba_share/2temp/take_core.py %t %p %u %e" | sudo tee $corepath
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