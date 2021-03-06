#pragma once
#include "Singleton.h"
#include <vector>

namespace Horizon
{
	class Scene;
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		void AddScene(Scene* pScene);
		void AddActiveScene(Scene* pScene);
		void RemoveScene(Scene* pScene);
		void SetActiveScene(Scene* pScene);
		Scene* GetActiveScene() const;
		void NextScene();
		//void PreviousScene();
		void Initialize();
		void PostInitialize();
		void FixedUpdate();
		void Update();
		void LateUpdate();
		void Render();
	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		~SceneManager();

		std::vector<Scene*> m_pScenes;
		bool m_Initialized = false;
		Scene* m_pActiveScene = nullptr;
		Scene* m_pNewActiveScene = nullptr;
	};
}
