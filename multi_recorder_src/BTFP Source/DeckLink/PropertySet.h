#pragma once

class KsPropertySetImpl
	: public IDispatchImpl<IKsPropertySet, &IID_IKsPropertySet>
	, public CUnknown
{
public:
	DECLARE_IUNKNOWN;

private:
	void *m_pStream;

public:
	KsPropertySetImpl(void *pStream)
		: CUnknown(NAME("KsPropertySetImpl"), NULL/*pStream->GetOwner()*/)
	{  
		m_pStream = pStream;  
	}

	~KsPropertySetImpl()
	{
		m_pStream  = NULL;
	}

	STDMETHODIMP QuerySupported(REFGUID guidPropSet, DWORD dwPropID, DWORD *pTypeSupport) 
	{
		if (guidPropSet != AMPROPSETID_Pin)
			return E_PROP_SET_UNSUPPORTED;

		if (dwPropID != AMPROPERTY_PIN_CATEGORY)
			return E_PROP_ID_UNSUPPORTED;

		if (pTypeSupport) // We support getting this property, but not setting it.  
			*pTypeSupport = KSPROPERTY_SUPPORT_GET; 
		return S_OK;
	}

	// Set: Cannot set any properties.
	STDMETHODIMP Set(REFGUID guidPropSet, 
		DWORD dwID,
		void *pInstanceData, 
		DWORD cbInstanceData, 
		void *pPropData, 
		DWORD cbPropData)
	{
		return E_NOTIMPL;
	}

	// Get: Return the pin category (our only property). 
	STDMETHODIMP Get(REFGUID guidPropSet,    // Which property set.
		DWORD dwPropID,         // Which property in that set.
		void *pInstanceData,    // Instance data (ignore).
		DWORD cbInstanceData,   // Size of the instance data (ignore).
		void *pPropData,        // Buffer to receive the property data.
		DWORD cbPropData,       // Size of the buffer.
		DWORD *pcbReturned)     // Return the size of the property.            
	{
		if (guidPropSet != AMPROPSETID_Pin) 
			return E_PROP_SET_UNSUPPORTED;

		if (dwPropID != AMPROPERTY_PIN_CATEGORY)
			return E_PROP_ID_UNSUPPORTED;

		if (pPropData == NULL && pcbReturned == NULL)
			return E_POINTER;

		if (pcbReturned)
			*pcbReturned = sizeof(GUID);

		if (pPropData == NULL)  // Caller just wants to know the size.
			return S_OK;

		if (cbPropData < sizeof(GUID)) // The buffer is too small.
			return E_UNEXPECTED;

		*(GUID *)pPropData = PIN_CATEGORY_CAPTURE;

		return S_OK;
	}
};