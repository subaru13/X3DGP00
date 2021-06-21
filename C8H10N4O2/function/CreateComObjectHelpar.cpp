#include "CreateComObjectHelpar.h"
#include "misc.h"
#include <map>
#include <wrl.h>
#include <Shlwapi.h>
#include <d3dcompiler.h>
#include <assert.h>
#include <filesystem>


#ifdef _MSC_VER
#pragma comment(lib, "d3dcompiler")
#endif

#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/DDSTextureLoader.h>


#define TINYEXR_IMPLEMENTATION
#undef max
#undef min


using namespace Microsoft::WRL;

namespace
{
    std::map<std::string, ComPtr<ID3D11PixelShader>>   ps_cache;
    struct VsCacheData
    {
        ComPtr<ID3D11VertexShader>    shader;
        ComPtr<ID3D11InputLayout>     layout;
        VsCacheData(ID3D11VertexShader* s, ID3D11InputLayout* l)
            :shader(s), layout(l) {}
    };
    std::map<std::string, VsCacheData>   vs_cache;
    std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>> srv_cache;
}

HRESULT load_pixel_shader(ID3D11Device* device, const std::string& cso_name, ID3D11PixelShader** pixel_shader)
{
    auto it = ps_cache.find(cso_name);
    if (it != ps_cache.end())
    {
        *pixel_shader = it->second.Get();
        (*pixel_shader)->AddRef();
        return S_OK;
    }

    FILE* fp = nullptr;
    fopen_s(&fp, cso_name.c_str(), "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long cso_sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixel_shader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    ps_cache.insert(std::make_pair(cso_name, *pixel_shader));

    return hr;
}

HRESULT load_vertex_shader(ID3D11Device* device, const std::string& cso_name,
    ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout,
    const D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
{
    auto it = vs_cache.find(cso_name);
    if (it != vs_cache.end())
    {
        *vertex_shader = it->second.shader.Get();
        (*vertex_shader)->AddRef();
        *input_layout = it->second.layout.Get();
        (*input_layout)->AddRef();
        return S_OK;
    }

    FILE* fp = nullptr;
    fopen_s(&fp, cso_name.c_str(), "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long cso_sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    hr = device->CreateInputLayout(input_element_desc, num_elements, cso_data.get(), cso_sz, input_layout);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    vs_cache.insert(std::make_pair(cso_name, VsCacheData(*vertex_shader, *input_layout)));

    return hr;
}

HRESULT create_pixel_shader(ID3D11Device* device, const std::string& shader, ID3D11PixelShader** pixel_shader)
{
    assert(device && "The device is invalid.");
    HRESULT hr = S_OK;

    auto it = ps_cache.find(shader);

    if (it != ps_cache.end())
    {
        (*pixel_shader) = it->second.Get();
        (*pixel_shader)->AddRef();
    }
    else
    {
        DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
        flags |= D3DCOMPILE_DEBUG;
        flags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
        ComPtr<ID3DBlob> compiled_shader_blob;
        ComPtr<ID3DBlob> error_message_blob;
        hr = D3DCompile(shader.c_str(), shader.length(), 0, 0, 0, "main", "ps_5_0",
            flags, 0, &compiled_shader_blob, &error_message_blob);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        hr = device->CreatePixelShader(compiled_shader_blob->GetBufferPointer(), compiled_shader_blob->GetBufferSize(), 0, pixel_shader);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        ps_cache.insert(std::make_pair(shader, *pixel_shader));
    }

    return hr;
}

HRESULT create_vertex_shader(ID3D11Device* device, const std::string& shader,
    ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout,
    const D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
{
    assert(device && "The device is invalid.");
    HRESULT hr = S_OK;
    auto it = vs_cache.find(shader);

    if (it != vs_cache.end())
    {
        *vertex_shader = it->second.shader.Get();
        (*vertex_shader)->AddRef();
        *input_layout = it->second.layout.Get();
        (*input_layout)->AddRef();
        return S_OK;
    }
    else
    {
        DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
        flags |= D3DCOMPILE_DEBUG;
        flags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
        ComPtr<ID3DBlob> compiled_shader_blob;
        ComPtr<ID3DBlob> error_message_blob;
        hr = D3DCompile(shader.c_str(), shader.length(), 0, 0, 0, "main", "vs_5_0",
            flags, 0, &compiled_shader_blob, &error_message_blob);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        hr = device->CreateVertexShader(compiled_shader_blob->GetBufferPointer(),
            compiled_shader_blob->GetBufferSize(), 0, vertex_shader);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        hr = device->CreateInputLayout
        (
            input_element_desc, num_elements,
            compiled_shader_blob->GetBufferPointer(),
            compiled_shader_blob->GetBufferSize(),
            input_layout);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        vs_cache.insert(std::make_pair(shader, VsCacheData(*vertex_shader, *input_layout)));
    }

    return hr;
}

HRESULT make_dummy_texture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view)
{
    assert(device && "The device is invalid.");
    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC texture2d_desc = {};
    texture2d_desc.Width = 1;
    texture2d_desc.Height = 1;
    texture2d_desc.MipLevels = 1;
    texture2d_desc.ArraySize = 1;
    texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture2d_desc.SampleDesc.Count = 1;
    texture2d_desc.SampleDesc.Quality = 0;
    texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
    texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texture2d_desc.CPUAccessFlags = 0;
    texture2d_desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA subresource_data = {};
    u_int color = 0xFFFFFFFF;
    subresource_data.pSysMem = &color;
    subresource_data.SysMemPitch = 4;
    subresource_data.SysMemSlicePitch = 4;
    ComPtr<ID3D11Texture2D> texture2d;
    hr = device->CreateTexture2D(&texture2d_desc, &subresource_data, &texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
    shader_resource_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shader_resource_view_desc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(texture2d.Get(), &shader_resource_view_desc, shader_resource_view);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    return hr;
}

HRESULT load_texture_from_file(ID3D11Device* device,const wchar_t* file_name, 
    ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc)
{
    assert(device && "The device is invalid.");
    HRESULT hr = S_OK;

    std::filesystem::path file_pass = file_name;

    file_pass.replace_extension("json");
    if (!std::filesystem::exists(file_pass))
    {

    }


    ComPtr<ID3D11Resource> resource;
    auto it = srv_cache.find(file_name);
    if (it != srv_cache.end())
    {
        *shader_resource_view = it->second.Get();
        (*shader_resource_view)->AddRef();
    }
    else
    {
        std::wstring extension = PathFindExtensionW(file_name);
        if (extension == L".dds")
        {
            hr = DirectX::CreateDDSTextureFromFile(device, file_name, &resource, shader_resource_view);
        }
        else
        {
            hr = DirectX::CreateWICTextureFromFile(device, file_name, &resource, shader_resource_view);
        }
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        srv_cache.insert(std::make_pair(file_name, *shader_resource_view));
    }

    if (resource == nullptr)
    {
        (*shader_resource_view)->GetResource(&resource);
    }

    ComPtr<ID3D11Texture2D> texture2d;
    hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    texture2d->GetDesc(texture2d_desc);

    return hr;
}

void ClearComObjectCache()
{
    ps_cache.clear();
    vs_cache.clear();
    srv_cache.clear();
}