

// string copy standard
// If the function succeeds, the return value is the length of the string received by buffer, in string_type. 
//    This value does not include the size of the terminating null character.
// If the function fails because buffer is too small to hold the string plus the terminating null character, 
//    the return value is the required buffer size. This value includes the size of the terminating null character.
template<typename string_type>
HRESULT string_copy_out(const string_type & s, typename string_type::pointer buffer, unsigned * size)
{
    if (!size) return E_INVALIDARG;

    unsigned in_size = *size;
    unsigned len = (unsigned)s.size();
    if (buffer)
    {
        if (!(len < in_size))
        {
            *size = len + 1;
            return E_OUTOFMEMORY;
        }
        string_type::traits_type::copy(buffer, s.c_str(), len);
        buffer[len] = 0;
        *size = len;
    }
    else
    {
        *size = len + 1;
    }

    return S_OK;
}
