#include "GamePCH.h"
#include "QBertMovementComponent.h"
#include "SpriteComponent.h"
#include <SubjectComponent.h>
#include "InputManager.h"
#include "GameCommands.h"
#include "Logger.h"
#include <cmath>
#include "Timer.h"
#include "Scene.h"
#include <SceneManager.h>
#include "CubeHandleComponent.h"
#include <GameObject.h>
using namespace Horizon;

QBertMovementComponent::QBertMovementComponent(GameObject* parent) : Component(parent),
m_CurrentMovement{ Movement::idle },
m_OriginalPoint{},
m_ElapsedTime{},
m_CanInputBeRegistered{ true },
m_pCubes{}
{
}

QBertMovementComponent::~QBertMovementComponent()
{

}

void QBertMovementComponent::Initialize()
{
	m_pTransformComponent = m_pGameObject->GetComponent<TransformComponent>();
	m_pSpriteComponent = m_pGameObject->GetComponent<SpriteComponent>();
	if (m_pTransformComponent == nullptr)
	{
		Logger::LogWarning("QBertMovementComponent::Initialize >> QBert does not have transformComponent");
		return;
	}
	if (m_pSpriteComponent == nullptr)
	{
		Logger::LogWarning("QBertMovementComponent::Initialize >> QBert does not have spriteComponent");
		return;
	}

	m_pCubes = SceneManager::GetInstance().GetActiveScene()->GetGameObjects("Cube");

	m_OriginalPoint = { m_pTransformComponent->GetPosition().x, m_pTransformComponent->GetPosition().y };
	InputManager::GetInstance().AddKeyboardInput(SDLK::SDLK_z, KeyboardButtonState::KeyDown, std::make_unique<MoveUpCommand>(this));
	InputManager::GetInstance().AddKeyboardInput(SDLK::SDLK_q, KeyboardButtonState::KeyDown, std::make_unique<MoveLeftCommand>(this));
	InputManager::GetInstance().AddKeyboardInput(SDLK::SDLK_s, KeyboardButtonState::KeyDown, std::make_unique<MoveDownCommand>(this));
	InputManager::GetInstance().AddKeyboardInput(SDLK::SDLK_d, KeyboardButtonState::KeyDown, std::make_unique<MoveRightCommand>(this));
}

void QBertMovementComponent::Update()
{
	IPoint2 desiredPos{};

	switch (m_CurrentMovement)
	{
	case Movement::idle:
		return;
	case Movement::leftDown:
		desiredPos = { m_OriginalPoint.x - 32,m_OriginalPoint.y + 48 };
		m_CanInputBeRegistered = false;
		m_pSpriteComponent->SetCurrentSprite(7);
		break;
	case Movement::leftUp:
		desiredPos = { m_OriginalPoint.x - 32,m_OriginalPoint.y - 48 };
		m_CanInputBeRegistered = false;
		m_pSpriteComponent->SetCurrentSprite(3);
		break;
	case Movement::rightDown:
		desiredPos = { m_OriginalPoint.x + 32,m_OriginalPoint.y + 48 };
		m_CanInputBeRegistered = false;
		m_pSpriteComponent->SetCurrentSprite(5);
		break;
	case Movement::rightUp:
		desiredPos = { m_OriginalPoint.x + 32,m_OriginalPoint.y - 48 };
		m_CanInputBeRegistered = false;
		m_pSpriteComponent->SetCurrentSprite(1);
		break;
	}


	m_ElapsedTime += Timer::GetInstance().GetDeltaTime();

	const int xPos = int(m_OriginalPoint.x + (desiredPos.x - m_OriginalPoint.x) * m_ElapsedTime);
	const int yPos = int(m_OriginalPoint.y + (desiredPos.y - m_OriginalPoint.y) * m_ElapsedTime);

	m_pTransformComponent->SetPosition(xPos, yPos, 0);

	if (m_ElapsedTime >= 1.f)
	{
		m_pTransformComponent->SetPosition(desiredPos.x, desiredPos.y, 0);
		m_ElapsedTime = 0.f;
		m_CanInputBeRegistered = true;
		m_OriginalPoint = { m_pTransformComponent->GetPosition().x, m_pTransformComponent->GetPosition().y };
		m_CurrentMovement = Movement::idle;
		m_pSpriteComponent->PreviousSprite();

		const Horizon::IPoint2 playerPos2D = { m_pTransformComponent->GetPosition().x,m_pTransformComponent->GetPosition().y };

		bool isCubeActivated = false;
		for (GameObject* const pCube : m_pCubes)
		{
			CubeHandleComponent* const pCubeHandle = pCube->GetComponent<CubeHandleComponent>();
			if (pCubeHandle->playerOnCube(playerPos2D))
			{
				pCubeHandle->ActivateCube();
				isCubeActivated = true;
				break;
			}
		}
		if (isCubeActivated == false)
			std::cout << "player falls";
	}
}

void QBertMovementComponent::SetCurrentMovement(Movement movement)
{
	if (m_CanInputBeRegistered == true)
		m_CurrentMovement = movement;
}

