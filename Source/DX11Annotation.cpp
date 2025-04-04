#include "DX11Annotation.h"

void DX11Annotation::Init(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& aContext)
{
	myContext = aContext;
}

void DX11Annotation::BeginAnnotation(const LPCWSTR aRegionName)
{
	HRESULT res = myContext->QueryInterface(IID_PPV_ARGS(&myAnnotation));
	if (FAILED(res) || !myAnnotation)
	{
		throw;
	}
	myAnnotation->BeginEvent(aRegionName);
}

void DX11Annotation::SetMarker(const LPCWSTR aMarkerName) const
{
	myAnnotation->SetMarker(aMarkerName);
}

void DX11Annotation::EndAnnotation() const
{
	myAnnotation->EndEvent();
}