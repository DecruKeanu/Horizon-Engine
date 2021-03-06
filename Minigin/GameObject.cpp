#include "MiniginPCH.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "TriggerManager.h"
#include "Renderer.h"

#include "TextureComponent.h"
#include "TriggerComponent.h"
#include "TextComponent.h"
#include "FPS.h"

#include "Timer.h"
#include "TimedFunctionComponent.h"

size_t Horizon::GameObject::m_LastId{};

Horizon::GameObject::GameObject() :
	m_Identifier{ "NoIdentifier" },
	m_Id{ m_LastId++ },
	m_IsActive{ true }
{

}

Horizon::GameObject::GameObject(const std::string& identifier) :
	m_Identifier{ identifier },
	m_Id{ m_LastId++ },
	m_IsActive{ true }
{

}

Horizon::GameObject::GameObject(const std::string& identifier, float activationTime) :
	m_Identifier{ identifier },
	m_Id{ m_LastId++ },
	m_IsActive{ false }
{
	TimedFunctionComponent* const pTimedFunction = new TimedFunctionComponent(this, false,true, activationTime);

	pTimedFunction->SetTimerFunction([this](float)
		{
			Activate();
		});
	pTimedFunction->Activate();

	AddComponent(pTimedFunction);
}

Horizon::GameObject::~GameObject()
{
	for (Component* const pComponent : m_pObjectComponents)
		SafeDelete(pComponent);
}

void Horizon::GameObject::Initialize()
{
	for (Component* const pComponent : m_pObjectComponents)
		pComponent->Initialize();

	for (Component* const pComponent : m_pObjectComponents)
		pComponent->PostInitialize();
}

void Horizon::GameObject::FixedUpdate()
{
	if (!m_IsActive)
		return;

	for (Component* const pComponent : m_pObjectComponents)
		pComponent->FixedUpdate();
}

void Horizon::GameObject::Update()
{
	for (Component* const pComponent : m_pObjectComponents)
		pComponent->PersistentUpdate();

	if (!m_IsActive)
		return;

	for (Component* const pComponent : m_pObjectComponents)
		pComponent->Update();
}

void Horizon::GameObject::LateUpdate()
{
	if (!m_IsActive)
		return;

	for (Component* const pComponent : m_pObjectComponents)
		pComponent->LateUpdate();
}

void Horizon::GameObject::Render() const
{
	if (!m_IsActive)
		return;

	for (const Component* const pComponent : m_pObjectComponents)
		pComponent->Render();
}

void Horizon::GameObject::AddComponent(Component* component)
{
	m_pObjectComponents.push_back(component);
}

const std::string& Horizon::GameObject::GetIdentifier() const
{
	return m_Identifier;
}

bool Horizon::GameObject::Equals(GameObject* pOther) const
{
	return (m_Id == pOther->m_Id);
}

bool Horizon::GameObject::GetIsActive() const
{
	return m_IsActive;
}

void Horizon::GameObject::Activate()
{
	m_IsActive = true;
}

void Horizon::GameObject::Deactivate()
{
	m_IsActive = false;
}
