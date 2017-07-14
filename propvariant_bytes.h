

HRESULT propvariant_from_bytes(void * addr, unsigned size, PROPVARIANT * prop)
{
    if (!(addr && size && prop)) return E_INVALIDARG;

    unsigned short tp = VT_VECTOR | VT_UI1;
    bool ok = ((prop->vt == VT_EMPTY)||(prop->vt == tp));
    if (!ok) return E_INVALIDARG;
    prop->vt = tp;
    prop->caub.pElems = (unsigned char *)addr;
    prop->caub.cElems = size;
    return S_OK;
}

HRESULT propvariant_as_bytes(PROPVARIANT * prop, void ** addr, unsigned * size)
{
    if (!(prop && addr && size)) return E_INVALIDARG;
    unsigned short tp = VT_VECTOR | VT_UI1;
    if (prop->vt != tp) return E_INVALIDARG;
    *addr = prop->caub.pElems;
    *size = (unsigned)prop->caub.cElems;
    return S_OK;
}