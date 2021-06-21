#include "SceneManager.h"

bool SceneBase::ChangeScene(const SceneName & name)
{
	return s_manager ? s_manager->ChangeScene(name) : false;
}


bool SceneManager::ChangeScene(const SceneName & name)
{
	if (!SceneDatas.empty() && name != NowScene)
	{
		SceneList::iterator it = SceneDatas.find(name);
		if (it != SceneDatas.end())
		{
			NextScene = name;
			return true;
		}
	}
	return false;
}

void SceneManager::edit(ID3D11Device* device, float elapsed_time)
{
	if (!SceneDatas.empty())
	{
		if (NextScene != NowScene)
		{
			SceneList::iterator it = SceneDatas.find(NowScene);
			if (it != SceneDatas.end() && it->second)it->second->uninitialize();
			NowScene = NextScene;
			it = SceneDatas.find(NowScene);
			if (it != SceneDatas.end() && it->second)it->second->initialize(device);
		}
		SceneList::iterator it = SceneDatas.find(NowScene);
		if (it != SceneDatas.end() && it->second)it->second->update(elapsed_time);
	}
}
void SceneManager::render(ID3D11DeviceContext* immediate_context, float elapsed_time)
{
	if (!SceneDatas.empty())
	{
		SceneList::iterator it = SceneDatas.find(NowScene);
		if (it != SceneDatas.end() && it->second)it->second->render(immediate_context,elapsed_time);
	}
}
void SceneManager::release()
{
	if (!SceneDatas.empty())
	{
		for (auto data : SceneDatas)
		{
			if (data.second)
			{
				data.second->uninitialize();
				data.second->s_manager = nullptr;
				delete data.second;
			}
		}
		SceneDatas.clear();
	}
}

