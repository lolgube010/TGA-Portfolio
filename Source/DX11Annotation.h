#pragma once
//#include <d3d11.h>
#include <d3d11_1.h>
#include <wrl/client.h>

class DX11Annotation
{
public:
	DX11Annotation() = default;
	void Init(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& aContext);
	void BeginAnnotation(LPCWSTR aRegionName);
	void SetMarker(LPCWSTR aMarkerName) const;
	void EndAnnotation() const;

private:
	Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> myAnnotation;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> myContext;
};
