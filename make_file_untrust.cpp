#define STRICT
#include <windows.h>
#include <atlbase.h>
#include <urlmon.h>
#include <stdlib.h>


HRESULT set_untrust(const wchar_t *p)
{
    HRESULT hr;
    CComPtr<IInternetSecurityManager> spism;
    spism.CoCreateInstance(CLSID_InternetSecurityManager);

    DWORD dwZone;
    hr = spism->MapUrlToZone(p, &dwZone, MUTZ_ISFILE | MUTZ_DONT_UNESCAPE);

    if (SUCCEEDED(hr)) {
        printf("Zone is %d\n", dwZone);
    } else {
        printf("Couldn't get zone\n");
    }
    return hr;
}

void MarkFileUnsafe(const WCHAR *wFileName)
{
    HRESULT hr;
    IZoneIdentifier *pizone = NULL;
    IPersistFile *piper = NULL;

    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        printf("[-] Failed CoInitializeEx - pEnroll [%x]\n", hr);
        goto error;
    }
    hr = CoCreateInstance(CLSID_PersistentZoneIdentifier, NULL, CLSCTX_INPROC_SERVER, IID_IZoneIdentifier,
        (void **)&pizone);
    if (FAILED(hr)) {
        printf("[-] Failed CoCreateInstance - pEnroll [%x]\n", hr);
        goto error;
    }
    if (pizone->SetId(URLZONE_INTERNET) != S_OK) {
        printf("[-] SetId failed\n");
        goto error;
    }
    hr = pizone->QueryInterface(IID_IPersistFile, (void **)&piper);
    if (FAILED(hr)) {
        printf("[-] QueryInterface failed\n");
        goto error;
    }
    hr = piper->Save(wFileName, TRUE);
    if (FAILED(hr)) {
        printf("[-] Failed Save\n");
        goto error;
    }
error:
    if (pizone != NULL)
        pizone->Release();
    if (piper != NULL)
        piper->Release();
    CoUninitialize();
}

// 我自己的 有点乱 用的时候再改
void MakeFileUnsafe2(void)
{
    CoInitialize(NULL);
    CComPtr<IZoneIdentifier> spzi;
    DWORD dwZone;
    HRESULT hr;

    hr = spzi.CoCreateInstance(CLSID_PersistentZoneIdentifier);

    //     hr =CoCreateInstance(CLSID_PersistentZoneIdentifier,
    //         NULL,
    //         CLSCTX_INPROC_SERVER,
    //         IID_IZoneIdentifier,
    //         (void **)&spzi);

    // URLZONE_LOCAL_MACHINE;


    DWORD dwDestZone = URLZONE_INTERNET;

    const WCHAR *p = L"C:\\Users\\fei\\Desktop\\1.docx";

    // MarkFileUnsafe(p);


    hr = spzi->SetId(dwDestZone);
    hr = CComQIPtr<IPersistFile>(spzi)->Save(p, TRUE);


    hr = CComQIPtr<IPersistFile>(spzi)->Load(p, STGM_READ);

    if (SUCCEEDED(spzi->GetId(&dwZone))) {
        printf("Zone identifier is %d\n", dwZone);
    } else {
        printf("Couldn't get zone identifier (perhaps there isn't one)\n");
    }


    CoUninitialize();
}


int main()
{
    return 0;
}
