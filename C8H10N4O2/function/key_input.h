#pragma once
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
	std::map<char, std::shared_ptr<KeyObject>> Keys;
	KeyManager() = default;
public:

	static KeyManager* instance()
	{
		static KeyManager ins;
		return &ins;
	}

	void Update()
	{
		if (Keys.empty())return;
		for (auto& it = Keys.begin(); it != Keys.end();)
		{
			if (it->second.use_count() > 1)
			{
				it->second->l = it->second->n;
				it->second->n = ::GetKeyState(static_cast<int>(it->first));
				++it;
			}
			else
			{
				it = Keys.erase(it);
			}
		}
	}

	std::shared_ptr<KeyObject>& Add(char key)
	{
		auto it = Keys.find(key);
		if (it == Keys.end())
		{
			it = Keys.emplace(key, std::make_shared<KeyObject>()).first;
		}
		return it->second;
	}

	~KeyManager()
	{
		Keys.clear();
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
	Key(char key) :_key(key), _obj(KeyManager::instance()->Add(_key)) {}

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
