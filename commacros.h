
#ifndef COM_MACROS_H_
#define COM_MACROS_H_

//////////////////////////////////////////////////////////////////////////
// Com Ole

#include <unknwn.h>
#include <assert.h>

#ifdef WIN32
#include <Windows.h>
#endif

class UnknownImp : public IUnknown
{
  UnknownImp(const UnknownImp &);
  UnknownImp & operator = (const UnknownImp &);
  //
public:
  UnknownImp() : m_ref(0) { ; }
  //
protected:
  volatile unsigned long m_ref;
  //

};

inline
void
lock_increment(volatile unsigned long  * ref)
{
#ifdef WIN32
  InterlockedIncrement(ref);
#else
  __sync_add_and_fetch(ref, 1);
#endif
}

inline
void
lock_decrement(volatile unsigned long  * ref)
{
#ifdef WIN32
  InterlockedDecrement(ref);
#else
  __sync_sub_and_fetch(ref, 1);
#endif
}


#define ADDREF		STDMETHOD_(unsigned long, AddRef)() { lock_increment(&m_ref); return m_ref; }
#define RELEASE		STDMETHOD_(unsigned long, Release)() { assert(0 < m_ref); lock_decrement(&m_ref);unsigned long ref = m_ref; if (0 == ref){ delete this; } return ref; }
#define IBEGIN		STDMETHOD(QueryInterface)( REFIID iid, void ** ppv ) { *ppv = NULL;
#define IUNKNOWN			if (IID_IUnknown == iid) { *ppv = static_cast<IUnknown *>(static_cast<UnknownImp *>(this)); this->AddRef(); return S_OK; }
#define I(Ix)				if (IID_##Ix == iid) { *ppv = static_cast<Ix*>(this); this->AddRef(); return S_OK; }
#define IEND				return E_NOINTERFACE; }

#define UNKNOWNIMP1(I1) ADDREF RELEASE IBEGIN IUNKNOWN I(I1) IEND
#define UNKNOWNIMP2(I1, I2) ADDREF RELEASE IBEGIN IUNKNOWN I(I1) I(I2) IEND
#define UNKNOWNIMP3(I1, I2, I3) ADDREF RELEASE IBEGIN IUNKNOWN I(I1) I(I2) I(I3) IEND
#define UNKNOWNIMP4(I1, I2, I3, I4) ADDREF RELEASE IBEGIN IUNKNOWN I(I1) I(I2) I(I3) I(I4) IEND

#define IID_PPV_ARG(Type , Expr )  IID_##Type, \
	reinterpret_cast<void**>(static_cast<Type **>(Expr))

//////////////////////////////////////////////////////////////////////////

#endif //COM_MACROS_H_