#include "ASCIIFont.h"

std::unique_ptr<ASCII> ASCII::instance;

ASCII::Handle ASCII::addFont(ID3D11Device* device, const wchar_t* file_name, FLOAT2 size_per_char)
{
	assert(device && "The device is invalid.");
	for (const auto& d : datas)
	{
		if (d.file_name == file_name)return -1;
	}

	Handle result = -1;

	if (file_name)
	{
		FILE* fp = nullptr;
		_wfopen_s(&fp, file_name, L"rb");
		if (fp != nullptr)
		{
			fclose(fp);
			datas.emplace_back();
			auto& it = datas.rbegin();
			it->spr = std::make_shared<SpriteBatch>(device, 2048, file_name);
			it->file_name = file_name;
			it->size_per_char = size_per_char;
			result = static_cast<int>(datas.size() - 1);
		}
	}
	return result;
}

FLOAT2 ASCII::print(ID3D11DeviceContext* immediate_context, ASCII::Handle handle, std::string str, FLOAT2 pos, FLOAT2 scale, FLOAT4 color)
{
	if (str.empty())return pos;
	if (datas.empty())return pos;
	if (scale.x * scale.y == 0.0f)return pos;
	if (color.w <= 0.0f)return pos;
	if (handle >= datas.size() || handle < 0)return pos;

	const SprData& choice = datas.at(handle);

	choice.spr->begin(immediate_context, NULL);

	FLOAT2 tex_size = choice.spr->getTextrueSize() / choice.size_per_char;
	FLOAT2 size = choice.size_per_char * scale;

	FLOAT2 carriage = pos;
	for (const char c : str)
	{
		switch (c)
		{
		case '\n':
			carriage.x = pos.x;
			carriage.y += size.y;
			break;
		case ' ':
			carriage.x += size.x;
			break;
		case '\r':
			carriage.x = pos.x;
			break;
		case '\t':
			carriage.x += size.x * 4.0f;
			break;
		default:
			choice.spr->render(immediate_context,
				carriage,
				size,
				{ tex_size.x * (c & 0x0F), tex_size.y * (c >> 4) },
				tex_size, 0, color);
			carriage.x += size.x;
			break;
		}
	}

	choice.spr->end(immediate_context);

	return carriage + size;
}

ASCII::~ASCII()
{
	for (auto& d : datas)
	{
		d.spr.reset();
	}
	datas.clear();
}

bool ASCII::createInstance()
{
	if (instance == nullptr)
	{
		instance = std::make_unique<ASCII>();
		return true;
	}
	return false;
}

bool ASCII::destroyInstance()
{
	if (instance != nullptr)
	{
		instance.reset();
		instance = nullptr;
		return true;
	}
	return false;
}