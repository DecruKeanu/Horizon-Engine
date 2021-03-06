#include "MiniginPCH.h"
#include "TriggerManager.h"
#include "TriggerComponent.h"
#include "GameObject.h"
#include "MathHelper.h"
void Horizon::TriggerManager::AddTriggerComponent(TriggerComponent* pTriggerComponent)
{

		m_pTriggerComponents.push_back(pTriggerComponent);
}

void Horizon::TriggerManager::ClearTriggerComponents()
{
	m_pTriggerComponents.clear();
}

void Horizon::TriggerManager::Update()
{
	for (size_t firstIdx{}; firstIdx < m_pTriggerComponents.size(); firstIdx++)
	{
		for (size_t secondIdx{ firstIdx + 1 }; secondIdx < m_pTriggerComponents.size(); secondIdx++)
		{
			TriggerComponent* const pFirstTriggerComponent = m_pTriggerComponents[firstIdx];
			TriggerComponent* const pSecondTriggerComponent = m_pTriggerComponents[secondIdx];

			GameObject* const pFirstGameObject = pFirstTriggerComponent->GetParent();
			GameObject* const pSecondGameObject = pSecondTriggerComponent->GetParent();

			if (pFirstGameObject->Equals(pSecondGameObject))
				continue;

			const IRect& firstRect = pFirstTriggerComponent->GetCollisionRect();
			const IRect& secondRect = pSecondTriggerComponent->GetCollisionRect();

			const bool areOverlapping = MathHelper::AreRectsOverlapping(firstRect, secondRect);

			if (areOverlapping)
			{
				pFirstTriggerComponent->OverlapsWith(pSecondTriggerComponent);
				pSecondTriggerComponent->OverlapsWith(pFirstTriggerComponent);

			}
		}
	}
}

size_t Horizon::TriggerManager::GetTriggersSize()
{
	return m_pTriggerComponents.size();
}
