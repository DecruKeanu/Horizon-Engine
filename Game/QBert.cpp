#include "GamePCH.h"
#include "QBert.h"
#include <GameObject.h>
#include <TextureComponent.h>
#include <TransformComponent.h>
#include <TriggerComponent.h>
#include "PlayerInputComponent.h"
#include "GameSpriteComponent.h"
#include "MovementComponent.h"
#include "PlayerDiscComponent.h"
#include "SpriteComponent.h"
#include <HealthComponent.h>
#include <ScoreComponent.h>
#include <TextComponent.h>
#include <ScoreDisplayComponent.h>
#include <ScoreDisplayObserver.h>
#include <HealthDisplayComponent.h>
#include <HealthDisplayObserver.h>
#include <ResourceManager.h>
#include <SoundSystemServiceLocator.h>
#include <Scene.h>

using namespace Horizon;

QBert::QBert(const rapidjson::Value& jsonObject) :
	m_Value{ jsonObject }
{
	Initialize();
}

GameObject* QBert::GetGameObject() const
{
	return m_pGameObject;
}

void QBert::Initialize()
{
	const int fallOffset = 50;
	int positionX = m_Value["positionX"].GetInt();
	int positionY = m_Value["positionY"].GetInt() - fallOffset;
	const bool isFirstPlayer = m_Value["IsFirstQbert"].GetBool();
	const int srcPosX = isFirstPlayer ? 0 : 126;
	const int srcPosY = 0;
	const int srcWidth = 120;
	const int srcHeight = 16;
	const float scale = 2.f;

	positionX += srcWidth / 8;
	positionY -= srcHeight;

	const std::string objectName = isFirstPlayer ? "Qbert" : "SecondQbert";
	GameObject* const pGameObject = new GameObject(objectName);

	GameSpriteComponent* const pSpriteComponent = new GameSpriteComponent(pGameObject, "QBertTextures.png", SDL_Rect{ srcPosX, srcPosY, srcWidth, srcHeight }, 8);
	TransformComponent* const QBertTransform = new TransformComponent(pGameObject, positionX, positionY);
	MovementComponent* const pMovementComponent = new MovementComponent(pGameObject, { positionX,positionY + fallOffset }, false);
	PlayerInputComponent* const pInputComponent = new PlayerInputComponent(pGameObject, isFirstPlayer);

	TriggerComponent* const pTriggerBodyComponent = new TriggerComponent(pGameObject, "BodyTrigger", { 4 ,10, int(scale * 15) - 8, int(scale * srcHeight) - 10 });
	TriggerComponent* const pTriggerFeetComponent = new TriggerComponent(pGameObject, "FeetTrigger", { int(15 * 0.5f) ,int(srcHeight * 1.8f), int(15), int(srcHeight * 0.2) });
	ScoreComponent* const pScoreComponent = new ScoreComponent(pGameObject);
	HealthComponent* const pHealthComponent = new HealthComponent(pGameObject);

	PlayerDiscComponent* const pPlayerDiscComponent = new PlayerDiscComponent(pGameObject);

	pTriggerFeetComponent->SetOnTriggerCallBack([pScoreComponent, pMovementComponent, pInputComponent, pPlayerDiscComponent](GameObject*, GameObject* pOverlappedGameObject, TriggerComponent::TriggerAction triggerAction, const std::string&)
		{
			if (triggerAction == TriggerComponent::TriggerAction::Enter && pOverlappedGameObject->GetIdentifier() == "FlyingDisc")
			{
				pPlayerDiscComponent->PlayerOnDisc(pOverlappedGameObject->GetComponent<TransformComponent>());
				pInputComponent->DeactivateInput();
			}
			else if (triggerAction == TriggerComponent::TriggerAction::Exit && pOverlappedGameObject->GetIdentifier() == "FlyingDisc")
			{
				pInputComponent->ResetInput();
			}
		});
	pTriggerBodyComponent->SetOnTriggerCallBack([pScoreComponent, pHealthComponent](GameObject*, GameObject* pOverlappedGameObject, TriggerComponent::TriggerAction triggerAction, const std::string&)
		{
			if (triggerAction == TriggerComponent::TriggerAction::Enter && pOverlappedGameObject->GetIdentifier() == "SlickSam")
			{
				pScoreComponent->IncreaseScore(300);
				pOverlappedGameObject->Deactivate();
			}
			else if (triggerAction == TriggerComponent::TriggerAction::Enter && pOverlappedGameObject->GetIdentifier() == "UggWrongway")
			{
				auto& soundSystem = SoundSystemServiceLocator::GetSoundSystem();
				soundSystem.QueueEvent(7, 36);
				pHealthComponent->DecreaseLive();
			}
			else if (triggerAction == TriggerComponent::TriggerAction::Enter && pOverlappedGameObject->GetIdentifier() == "Coily")
			{
				auto& soundSystem = SoundSystemServiceLocator::GetSoundSystem();
				soundSystem.QueueEvent(7, 36);
				pHealthComponent->DecreaseLive();
			}
		});


	pGameObject->AddComponent(pSpriteComponent);
	pGameObject->AddComponent(QBertTransform);
	pGameObject->AddComponent(pMovementComponent);
	pGameObject->AddComponent(pInputComponent);
	pGameObject->AddComponent(pTriggerBodyComponent);
	pGameObject->AddComponent(pTriggerFeetComponent);
	pGameObject->AddComponent(pScoreComponent);
	pGameObject->AddComponent(pHealthComponent);

	pGameObject->AddComponent(pPlayerDiscComponent);

	m_pGameObject = pGameObject;
}
