#include "GamePCH.h"
#include "CooperativeLevel.h"

#include "CubeHandleComponent.h"

#include <GameObject.h>
#include <TextComponent.h>
#include <ScoreComponent.h>
#include <HealthComponent.h>
#include <SpriteComponent.h>
#include <TextureComponent.h>
#include <TransformComponent.h>
#include <ScoreDisplayObserver.h>
#include <HealthDisplayObserver.h>
#include <ScoreDisplayComponent.h>
#include <HealthDisplayComponent.h>
#include <TimedFunctionComponent.h>

#include <timer.h>
#include "Command.h"
#include <InputManager.h>
#include <TriggerManager.h>
#include <ResourceManager.h>
#include <SoundSystemServiceLocator.h>

#include "LevelReader.h"
#include "MainMenuScene.h"

CooperativeLevel::CooperativeLevel(int level, int player1Score, int player1Lives, int player2Score, int player2Lives) : Scene("SinglePlayerScene"),
m_Level{ level },
m_Player1Score{ player1Score },
m_Player1Lives{ player1Lives },
m_Player2Score{ player2Score },
m_Player2Lives{ player2Lives },
m_LevelCompleted{},
m_SwitchToNewLevel{}
{

}

void CooperativeLevel::Initialize()
{
	LevelReader levelReader{};
	const std::wstring fileName = L"CooperativeLevel" + std::to_wstring(m_Level) + L".json";
	levelReader.ParseLevel(fileName);

	for (Horizon::GameObject* const pPrefab : levelReader.GetGameObjects())
	{
		Add(pPrefab);
	}

	const std::vector<Horizon::GameObject*> pCubes = GetGameObjects("Cube");

	for (Horizon::GameObject* const pCube : pCubes)
		m_pCubeHandles.push_back(pCube->GetComponent<CubeHandleComponent>());

	{
		Horizon::GameObject* const pCubesSwitch = new Horizon::GameObject();

		m_pTimedFunction = new Horizon::TimedFunctionComponent(pCubesSwitch, true, 0.1f);

		m_pTimedFunction->SetTimerFunction([this](float totalTime)
			{
				for (CubeHandleComponent* const pCubeHandleComponent : m_pCubeHandles)
				{
					pCubeHandleComponent->SwitchSprite();
				}

				if (totalTime > 4.f)
					m_SwitchToNewLevel = true;
			});

		pCubesSwitch->AddComponent(m_pTimedFunction);
		Add(pCubesSwitch);
	}
}

void CooperativeLevel::PostInitialize()
{
	Horizon::GameObject* player1 = GetGameObject("Qbert");
	Horizon::GameObject* player2 = GetGameObject("SecondQbert");
	std::vector<Horizon::GameObject*> pScoreDisplayObjects = GetGameObjects("ScoreDisplayObject");
	std::vector<Horizon::GameObject*> pHealthDisplayObjects = GetGameObjects("HealthDisplayObject");

	m_pPlayer1ScoreComponent = player1->GetComponent<Horizon::ScoreComponent>();
	m_pPlayer1ScoreComponent->AddObserver(new Horizon::ScoreDisplayObserver(pScoreDisplayObjects[0]->GetComponent<Horizon::ScoreDisplayComponent>()));
	m_pPlayer1ScoreComponent->IncreaseScore(m_Player1Score);

	m_pPlayer1HealthComponent = player1->GetComponent<Horizon::HealthComponent>();
	m_pPlayer1HealthComponent->AddObserver(new Horizon::HealthDisplayObserver(pHealthDisplayObjects[0]->GetComponent<Horizon::HealthDisplayComponent>()));
	m_pPlayer1HealthComponent->SetCurrentLives(m_Player1Lives);

	m_pPlayer2ScoreComponent = player2->GetComponent<Horizon::ScoreComponent>();
	m_pPlayer2ScoreComponent->AddObserver(new Horizon::ScoreDisplayObserver(pScoreDisplayObjects[1]->GetComponent<Horizon::ScoreDisplayComponent>()));
	m_pPlayer2ScoreComponent->IncreaseScore(m_Player2Score);

	m_pPlayer2HealthComponent = player2->GetComponent<Horizon::HealthComponent>();
	m_pPlayer2HealthComponent->AddObserver(new Horizon::HealthDisplayObserver(pHealthDisplayObjects[1]->GetComponent<Horizon::HealthDisplayComponent>()));
	m_pPlayer2HealthComponent->SetCurrentLives(m_Player2Lives);

	auto& soundSystem = Horizon::SoundSystemServiceLocator::GetSoundSystem();
	soundSystem.QueueEvent(0, 20);
}

void CooperativeLevel::Update()
{
	if (m_SwitchToNewLevel)
		NextLevel();

	if (m_LevelCompleted)
		return;

	if (m_pPlayer1HealthComponent->GetCurrentLives() == 0 || m_pPlayer2HealthComponent->GetCurrentLives() == 0)
	{
		BackToMainMenu();
	}
	else if (m_pPlayer1HealthComponent->GetCurrentLives() < m_Player1Lives || m_pPlayer2HealthComponent->GetCurrentLives() < m_Player2Lives)
	{
		ResetLevel();
	}

	for (CubeHandleComponent* const pCubeHandle : m_pCubeHandles)
	{
		if (!pCubeHandle->GetisActivated())
			return;
	}

	m_LevelCompleted = true;
	m_pTimedFunction->Activate();

	auto& soundSystem = Horizon::SoundSystemServiceLocator::GetSoundSystem();
	soundSystem.QueueEvent(1, 20);

	Horizon::TriggerManager::GetInstance().ClearTriggerComponents();
}

void CooperativeLevel::BackToMainMenu()
{
	Horizon::InputManager::GetInstance().ClearInput();
	Horizon::TriggerManager::GetInstance().ClearTriggerComponents();
	Horizon::SceneManager::GetInstance().AddActiveScene(new MainMenuScene());
}

void CooperativeLevel::ResetLevel()
{
	Horizon::InputManager::GetInstance().ClearInput();
	Horizon::TriggerManager::GetInstance().ClearTriggerComponents();
	m_Player1Score = m_pPlayer1ScoreComponent->GetScore();
	m_Player1Lives = m_pPlayer1HealthComponent->GetCurrentLives();
	m_Player2Score = m_pPlayer2ScoreComponent->GetScore();
	m_Player2Lives = m_pPlayer2HealthComponent->GetCurrentLives();
	Horizon::SceneManager::GetInstance().AddActiveScene(new CooperativeLevel(m_Level, m_Player1Score, m_Player1Lives, m_Player2Score, m_Player2Lives));
}

void CooperativeLevel::NextLevel()
{
	m_Player1Score = m_pPlayer1ScoreComponent->GetScore();
	m_Player1Lives = m_pPlayer1HealthComponent->GetCurrentLives() + 1;
	m_Player2Score = m_pPlayer2ScoreComponent->GetScore();
	m_Player2Lives = m_pPlayer2HealthComponent->GetCurrentLives() + 1;

	(m_Level == 3) ? Horizon::SceneManager::GetInstance().AddActiveScene(new MainMenuScene()) : Horizon::SceneManager::GetInstance().AddActiveScene(new CooperativeLevel(++m_Level, m_Player1Score, m_Player1Lives, m_Player2Score, m_Player2Lives));
}
