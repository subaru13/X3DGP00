#pragma once
#include "../FrameworkConfig.h"
#include <map>
#include <Windows.h>
#include <memory>

class KeyManager final
{
public:
	struct KeyObject
	{
		short l;
		short n;
	};
private:
	std::map<char, std::shared_ptr<KeyObject>> keys;
	KeyManager() = default;
public:

	static KeyManager* instance()
	{
		static KeyManager ins;
		return &ins;
	}

	void update()
	{
		if (keys.empty())return;
		for (auto& it = keys.begin(); it != keys.end();)
		{
			if (it->second.use_count() > 1)
			{
				it->second->l = it->second->n;
				it->second->n = ::GetKeyState(static_cast<int>(it->first));
				++it;
			}
			else
			{
				it = keys.erase(it);
			}
		}
	}

	std::shared_ptr<KeyObject>& addKey(char key)
	{
		auto it = keys.find(key);
		if (it == keys.end())
		{
			it = keys.emplace(key, std::make_shared<KeyObject>()).first;
		}
		return it->second;
	}

	~KeyManager()
	{
		keys.clear();
	}
};


class Key final
{
private:
	Key& operator=(Key&) = delete;
	const char _key;
	std::shared_ptr<KeyManager::KeyObject> _obj;
	short _trg()const { return (~_obj->l) & _obj->n; }
	short _rel()const { return _obj->l & (~_obj->n); }
public:
	Key(Key& k) :_key(k._key), _obj(k._obj) {}
	Key(char key) :_key(key), _obj(KeyManager::instance()->addKey(_key)) {}

	//キーが押されたタイミングでtrue
	bool down()const
	{
		return _trg() & 0x80;
	}
	//キーが押され続けている間true
	bool hold()const
	{
		return _obj->n & 0x80;
	}
	//キーが離されたタイミングでtrue
	bool up()const
	{
		return _rel() & 0x80;
	}

	const char& getLabel()const { return _key; }

	~Key()
	{
		_obj.reset();
	}
};

class Mouse final
{
private:
	POINT pos;
	Key l_button;
	Key r_button;
	Key c_button;
	Mouse() :l_button(MK_LBUTTON), r_button(MK_RBUTTON), c_button(VK_MBUTTON), pos() {}
public:
	void update(HWND hwnd)
	{
		GetCursorPos(&pos);
		ScreenToClient(hwnd, &pos);
		RECT rc;
		GetClientRect(hwnd, &rc);
		float screen_w = (float)(rc.right - rc.left);
		float screen_h = (float)(rc.bottom - rc.top);
		pos.x = static_cast<LONG>((float)pos.x * ((float)SCREEN_WIDTH / screen_w));
		pos.y = static_cast<LONG>((float)pos.y * ((float)SCREEN_HEIGHT / screen_h));
	}

	const POINT& getPos()const { return pos; }
	const Key& getL()const { return l_button; }
	const Key& getR()const { return r_button; }
	const Key& getC()const { return c_button; }

	static Mouse* instance()
	{
		static Mouse ins;
		return &ins;
	}
};