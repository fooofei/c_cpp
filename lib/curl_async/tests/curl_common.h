#pragma once


// url is may be change with the server
// sleep_time is the GET 's  "Content-Type: application/x-www-form-urlencoded"  param

int make_curl_block(const char *url, int sleep_time, CURL **outcurl);
int make_curl_noblock(const char *url, int sleep_time, CURL **outurl);
