

#ifndef HEX_STR_CONVERSION_H_
#define HEX_STR_CONVERSION_H_

#include <string>


/* 6162636465666768414243444546 -> abcdefghABCDEF */
int
hex2str(const std::string & hex, std::string & dst);


/* abcdefghABCDEF -> 6162636465666768414243444546 */
int
str2hex(const std::string & str, std::string & dst);



#endif //HEX_STR_CONVERSION_H_
