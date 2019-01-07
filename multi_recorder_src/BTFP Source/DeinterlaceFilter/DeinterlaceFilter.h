#include <initguid.h>
#include "DeinterlaceFilter_i.h"

class CDeinterlaceFilter : public CTransInPlaceFilter
{
	public:
    static CUnknown* WINAPI CreateInstance(LPUNKNOWN punk, HRESULT* phr);

    DECLARE_IUNKNOWN;

private:
    // Constructor - just calls the base class constructor
    CDeinterlaceFilter(TCHAR* tszName, LPUNKNOWN punk, HRESULT* phr)
		: CTransInPlaceFilter(tszName, punk, CLSID_DeinterlaceFilter, phr)
    {}

    // Overrides the PURE virtual Transform of CTransInPlaceFilter base class.
    // This is where the "real work" is done by altering *pSample.
    // We do the Null transform by leaving it alone.
    HRESULT Transform(IMediaSample* pSample);

    // We accept any input type.  We'd return S_FALSE for any we didn't like.
    HRESULT CheckInputType(const CMediaType* mtIn);

private:
	BITMAPINFOHEADER* GetBMIHeader(const AM_MEDIA_TYPE* pamt);
};