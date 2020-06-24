#ifdef WIN32

#include <Windows.h>
#include <ostream>

#include "fmem.h"

template <typename char_type> struct ostreambuf : public std::basic_streambuf<char_type, std::char_traits<char_type> > {
    ostreambuf(char_type *buffer, std::streamsize bufferLength)
    {
        this->setp(buffer, buffer + bufferLength);
    }
};

template <class STREAM> struct STDIOAdapter {
    static FILE *yield(STREAM *stream)
    {
        assert(stream != NULL);

        static cookie_io_functions_t Cookies = {
            .read = NULL,
            .write = cookieWrite,
            .seek = NULL,
            .close = cookieClose
        };

        return fopencookie(stream, "w", Cookies);
    }

    ssize_t static cookieWrite(void *cookie, const char *buf, size_t size)
    {
        if (cookie == NULL)
            return -1;

        STREAM *writer = static_cast<STREAM *>(cookie);

        writer->write(buf, size);

        return size;
    }

    int static cookieClose(void *cookie)
    {
        return EOF;
    }
}; // STDIOAdapter

#ifdef __cplusplus
extern "C"
#endif
    int
    fmemopen3(FILE **self, void *ptr, size_t size, const char *mode)
{
    // https://stackoverflow.com/questions/109449/getting-a-file-from-a-stdfstream/33612982#33612982
    ostreambuf<char> buf((char *)ptr, size);
    std::ostream os(&buf);
    return STDIOAdapter<std::ostream>::yield(&os);
}


#endif // WIN32
