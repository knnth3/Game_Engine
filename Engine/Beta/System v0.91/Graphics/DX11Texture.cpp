#include "DX11Texture.h"
#include <DirectXTex\DirectXTex.h>

static ID3D11SamplerState* m_samplerState = nullptr;

using namespace DirectX;

Graphics::DX11Texture::DX11Texture(const LPCWSTR filepath, ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_bIsValid = false;
	m_texture = nullptr;
	m_samplerState = nullptr;
	m_filepath = filepath;
	m_device = device;
	m_context = context;
	if (!m_samplerState)
		CreateSamplerSate();

	Initialize();
}

bool Graphics::DX11Texture::IsValid()
{
	return m_bIsValid;
}

void Graphics::DX11Texture::SetAsActive()
{
	if (!m_samplerState)
		CreateSamplerSate();
	m_context->PSSetShaderResources(0, 1, &m_texture);
}

Graphics::DX11Texture::~DX11Texture()
{
	//Not sure on how to go about deleting textures
	//if they are active
	//m_context->PSSetShaderResources(0, 0, nullptr);
	//m_context->PSSetSamplers(0, 0, nullptr);
	CLOSE_COM_PTR(m_texture);
	CLOSE_COM_PTR(m_samplerState);
}

void Graphics::DX11Texture::Initialize()
{
	wchar_t ext[_MAX_EXT];
	wchar_t fname[_MAX_FNAME];
	errno_t err = _wsplitpath_s(m_filepath, nullptr, 0, nullptr, 0, fname, _MAX_FNAME, ext, _MAX_EXT);
	HRESULT result;
	ScratchImage srcImage;
	if (_wcsicmp(ext, L".dds") == 0)
	{
		result = LoadFromDDSFile(m_filepath, DDS_FLAGS_NONE, nullptr, srcImage);
		CheckSuccess(result);
	}
	if (SUCCEEDED(result))
	{
		ScratchImage secondary;
		ScratchImage finalImage;
		result = CreateShaderResourceView(m_device, srcImage.GetImages(), srcImage.GetImageCount(), srcImage.GetMetadata(), &m_texture);
		CheckSuccess(result);
		m_bIsValid = true;
	}
}

void Graphics::DX11Texture::CreateSamplerSate()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT result = m_device->CreateSamplerState(&sampDesc, &m_samplerState);
	CheckSuccess(result);
	m_context->PSSetSamplers(0, 1, &m_samplerState);
}
