#ifndef INCLUDE_SCENE_MANAGER
#define INCLUDE_SCENE_MANAGER
#include <map>
#include <string>
#include <d3d11.h>
#include <memory>

typedef std::string SceneName;
class SceneBase;
class SceneManager;

/*****************************************************************************************
	The base class of the scene.
	Please inherit this for the scene to be registered in the manager.
	The base of the scene has the manager's address.
	But you can't access it.
	If you want to change the scene,
	please use the function that changes from the manager's address.
	Pass the pointer of ID3D11Device to the default constructor.
	Therefore, the first argument of the inherited
	constructor must be a pointer to ID3D11Device.
*****************************************************************************************/
class SceneBase
{
private:
	friend SceneManager;
	SceneManager* s_manager;
public:
	SceneBase(ID3D11Device*) :s_manager(nullptr) {}

	/*******************************************************
		Cannot be overridden.
		Change the scene.
		Please enter the registered name of
		the scene you want to change.
	*******************************************************/
	virtual bool changeScene(const SceneName&) final;

	/*******************************************************
		It is the initialization process of the scene.
		Runs only once when the scene changes.
		Please do the processing that you want
		to do only once throughout
		the whole with the constructor.
		The manager provides a pointer to
		the ID3D11Device registered with the manager.
	*******************************************************/
	virtual void initialize(ID3D11Device*) = 0;

	/*******************************************************
		It is a scene update process.
		Called every frame.
		The manager will provide the elapsed time.
	*******************************************************/
	virtual void update(float) = 0;

	/*******************************************************
		It is a scene draw process.
		Called every frame.
		The manager provides the elapsed time
		and the context.
	*******************************************************/
	virtual void render(ID3D11DeviceContext*, float) = 0;

	/*******************************************************
		It is the end processing of the scene.
		Runs only once when changing scenes.
		Please create as needed.
	*******************************************************/
	virtual void uninitialize() {}

	virtual ~SceneBase() {}
};

/*****************************************************************************************
	This class registers and manages scenes that inherit from SceneBase.
	The first scene to be executed is the first registered scene.
*****************************************************************************************/
class SceneManager final
{
private://Declaration
	typedef	SceneBase* Scene;
	typedef	std::map<SceneName, Scene>	SceneList;
	friend bool SceneBase::changeScene(const SceneName& name);
private://Variables
	SceneList								SceneDatas;
	SceneName								NowScene;
	SceneName								NextScene;
private://Useful
	SceneManager() :SceneDatas(), NowScene("scene is empty"), NextScene() {}
public:

	/*******************************************************
		Change the scene.
		Please enter the registered name of
		the scene you want to change.
	*******************************************************/
	bool changeScene(const SceneName& name);

	static SceneManager* getInstance()
	{
		static SceneManager instance;
		return &instance;
	}

	/*******************************************************
		Register the scene with the manager.
		Please pass the type of the scene to be
		registered and the registered name as arguments.
		The second argument is a variable length template
		argument that can be passed to the constructor
		of the scene to be registered
		by entering 0 or more arguments.
	*******************************************************/
	template<class T>
	bool AddScene(ID3D11Device* device, const SceneName& name)
	{
		static_assert(std::is_base_of<SceneBase, T>::value == true, "This type cannot be used.");
		SceneList::iterator it = SceneDatas.find(name);
		if (it != SceneDatas.end())
		{
			return false;
		}
		Scene scene = new T(device);
		scene->s_manager = this;
		SceneDatas.insert(std::make_pair(name, scene));
		if (NextScene.empty())NextScene = name;
		return true;
	}

	/*******************************************************
		It is a function that
		initializes + uninitializes + updates.
		It also decides whether to initialize.
		Pass the elapsed time from the previous
		frame as an argument.
		If not passed, 1 / 60th of a second
		will be used as the elapsed time.
	*******************************************************/
	void edit(ID3D11Device* device, float elapsed_time = static_cast<float>(1.0f / 60.0f));

	/*******************************************************
		Draw the scene data.
		Pass the elapsed time from the previous
		frame as an argument.
		If not passed, 1 / 60th of a second
		will be used as the elapsed time.
	*******************************************************/
	void render(ID3D11DeviceContext* immediate_context, float elapsed_time = static_cast<float>(1.0f / 60.0f));

	/*******************************************************
		The registered scene will be released.
	*******************************************************/
	void release();

	/*******************************************************
		The destructor releases the registered scene.
	*******************************************************/
	~SceneManager() { release(); }
};

/// <summary>
/// シーンマネージャにシーンを追加します。
/// </summary>
/// <typeparam name="T">追加するシーンの型(SceneBaseを継承していること)</typeparam>
/// <param name="device">有効なデバイス</param>
/// <param name="name">登録するシーンの名前</param>
/// <returns>登録できた場合true</returns>
template<class T>
inline bool AddScene(ID3D11Device* device, const SceneName& name)
{
	return SceneManager::getInstance()->AddScene<T>(device, name);
}

/// <summary>
/// シーンを変更します
/// </summary>
/// <param name="name">変更先のシーンの名前</param>
/// <returns>変更できた場合true</returns>
inline bool ChangeScene(const SceneName& name)
{
	return SceneManager::getInstance()->changeScene(name);
}

#endif
