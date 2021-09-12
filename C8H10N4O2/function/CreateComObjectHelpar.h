#pragma once
#include <d3d11.h>
#include <string>

HRESULT loadPixelShader(ID3D11Device*, const std::string&, ID3D11PixelShader**);
HRESULT loadVertexShader(ID3D11Device*, const std::string&, ID3D11VertexShader**,
	ID3D11InputLayout**, const D3D11_INPUT_ELEMENT_DESC*, UINT);
HRESULT loadGeometryShader(ID3D11Device*, const std::string&, ID3D11GeometryShader**);
HRESULT loadDomainShader(ID3D11Device*, const std::string&, ID3D11DomainShader**);
HRESULT loadHullShader(ID3D11Device*, const std::string&, ID3D11HullShader**);
HRESULT loadVertexShader(ID3D11Device*, const std::string&, ID3D11VertexShader**);
HRESULT createPixelShader(ID3D11Device*, const std::string&, ID3D11PixelShader**);
HRESULT createVertexShader(ID3D11Device*, const std::string&, ID3D11VertexShader**,
	ID3D11InputLayout**, const D3D11_INPUT_ELEMENT_DESC*, UINT);
HRESULT createVertexShader(ID3D11Device*, const std::string&, ID3D11VertexShader**);
HRESULT makeDummyTexture(ID3D11Device*, ID3D11ShaderResourceView**);
HRESULT loadTextureFromFile(ID3D11Device*, const wchar_t*, ID3D11ShaderResourceView**, D3D11_TEXTURE2D_DESC*);