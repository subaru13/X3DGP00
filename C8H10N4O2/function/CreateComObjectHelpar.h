#pragma once
#include <d3d11.h>
#include <string>

HRESULT load_pixel_shader(ID3D11Device*, const std::string&, ID3D11PixelShader**);
HRESULT load_vertex_shader(ID3D11Device*, const std::string&, ID3D11VertexShader**,
	ID3D11InputLayout**, const D3D11_INPUT_ELEMENT_DESC*, UINT);
HRESULT create_pixel_shader(ID3D11Device*, const std::string&, ID3D11PixelShader**);
HRESULT create_vertex_shader(ID3D11Device*, const std::string&, ID3D11VertexShader**,
	ID3D11InputLayout**, const D3D11_INPUT_ELEMENT_DESC*, UINT);
HRESULT make_dummy_texture(ID3D11Device*, ID3D11ShaderResourceView**);
HRESULT load_texture_from_file(ID3D11Device*, const wchar_t*, ID3D11ShaderResourceView**, D3D11_TEXTURE2D_DESC*);