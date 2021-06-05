#include "MiniginPCH.h"
#include "TriggerManager.h"
#include "TriggerComponent.h"
#include "GameObject.h"
#include "MathHelper.h"
void Horizon::TriggerManager::AddTriggerComponent(TriggerComponent* pTriggerComponent)
{
	m_pTriggerComponents.push_back(pTriggerComponent);
}

//void Horizon::TriggerManager::RemoveTriggerComponent(TriggerComponent* pTriggerComponent)
//{
//	const auto it = find(m_pActiveTriggerComponents.begin(), m_pActiveTriggerComponents.end(), pTriggerComponent);
//
//	if (it != m_pActiveTriggerComponents.end())
//	{
//		m_pActiveTriggerComponents.erase(it);
//	}
//}

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
				return;

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
