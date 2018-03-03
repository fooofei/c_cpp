
#ifndef PROPER_COREDUMP_H
#define PROPER_COREDUMP_H


// only for posix
// If no core dump generated before, then generated core dump
//   file at given coredump_gen_path, if detected generated 
//   core dump files, then not generated core dump files again.
// 效果: 生成辅助文件 ~/hw_gzip_stdin.sh(或者/hw_gzip_stdin.sh)
//       生成 core dump 文件
// 
int set_coredump(const char * coredump_gen_path);

void set_core_debug_mode();


#endif //PROPER_COREDUMP_H
