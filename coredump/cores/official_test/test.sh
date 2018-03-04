
corepath=/proc/sys/kernel/core_pattern

gcc $PWD/core_pattern_pipe_test.c -o core_pattern_pipe_test
gcc $PWD/make_crash.c -g -O0 -o crash

echo "original core_pattern is:" `cat $corepath` ", the limit is :" `ulimit -c`

echo "|$PWD/core_pattern_pipe_test %p UID=%u GID=%g sig=%s" | sudo tee $corepath
ulimit -c unlimited

echo "now core_pattern is:" `cat $corepath` ", the limit is :" `ulimit -c`

rm -f ./core.info
./crash
less ./core.info