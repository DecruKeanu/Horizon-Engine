#include "GamePCH.h"
#include "FlyingDiscSpriteComponent.h"

#include <SpriteComponent.h>
#include <TimedFunctionComponent.h>

FlyingDiscSpriteComponent::FlyingDiscSpriteComponent(Horizon::GameObject* pParent, const std::string& textureName, const SDL_Rect& srcRect) : Component(pParent)
{
	//Component makes sprite because sprite is knowhere else used and this class defines the spritebehaviour
	m_pSpriteComponent = new Horizon::SpriteComponent(m_pGameObject,textureName, srcRect, 4);
	m_pSpriteComponent->Scale(2.f);
	m_pGameObject->AddComponent(m_pSpriteComponent);

	InitializeSpriteTimedFunction();
}

void FlyingDiscSpriteComponent::InitializeSpriteTimedFunction()
{
	Horizon::TimedFunctionComponent* const pSpriteTimedFunction = new Horizon::TimedFunctionComponent(m_pGameObject,true, 0.1f);
	pSpriteTimedFunction->SetTimerFunction([this](float)
		{
			m_pSpriteComponent->NextSprite();
		});

	pSpriteTimedFunction->Activate();
	m_pGameObject->AddComponent(pSpriteTimedFunction);
}

