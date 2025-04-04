//--------------------------------------------------------------------------------------
// File: DirectXTexEXR.h
//
// DirectXTex Auxilary functions for using the OpenEXR library
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//--------------------------------------------------------------------------------------

#pragma once
#include "DirectXTex.h"

namespace DirectX
{
	HRESULT __cdecl GetMetadataFromEXRFile(
		_In_z_ const wchar_t* szFile,
		_Out_ TexMetadata& metadata);

	// The data is always loaded as R16G16B16A16_FLOAT.
	// For the load functions, the metadata parameter can be nullptr as this information is also available in the returned ScratchImage.
	//This is a simple loading example.

	//		auto image = std::make_unique<ScratchImage>();
	//		HRESULT hr = LoadFromEXRFile( L"flowers.exr", nullptr, *image );
	//		if ( FAILED(hr) )
	//			// error

	HRESULT __cdecl LoadFromEXRFile(
		_In_z_ const wchar_t* szFile,
		_Out_opt_ TexMetadata* metadata, _Out_ ScratchImage& image);


	//Storing an image.

	//		const Image* img = image->GetImage(0,0,0);
	//		assert( img );
	//		HRESULT hr = SaveToEXRFile( *img, L"NEW_IMAGE.EXR" );
	//		if ( FAILED(hr) )
	//		  // error

	HRESULT __cdecl SaveToEXRFile(_In_ const Image& image, _In_z_ const wchar_t* szFile);
}