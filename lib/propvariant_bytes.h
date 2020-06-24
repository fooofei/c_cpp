/* This file shows how to use PROPVARIANT type VT_VECTOR to pass addr and size */

HRESULT propvariant_from_bytes(void *addr, unsigned size, PROPVARIANT *prop);

HRESULT propvariant_as_bytes(PROPVARIANT *prop, void **addr, unsigned *size);
