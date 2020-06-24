#ifndef WHERE_AM_I_H
#define WHERE_AM_I_H

/*

Write code in the library or executable.
In library:
    get_library_fullpath() get the library fullpath,
    get_executable_fullpath() get the executable which load the library fullpath.

if error return -1, if succeed return 0.

stackoverflow:
    https://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
    https://stackoverflow.com/questions/933850/how-do-i-find-the-location-of-the-executable-in-c

*/

// only for Windows
int get_library_fullpath_w(wchar_t **, size_t *);
int get_executable_fullpath_w(wchar_t ** ,size_t *);

/* string is utf-8 encoding, Windows, posix */
int get_library_fullpath(char ** , size_t  *);
int get_executable_fullpath(char ** , size_t *);

#endif //WHERE_AM_I_H
