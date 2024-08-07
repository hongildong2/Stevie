#include "pch.h"
#include "IComponentManager.h"

IComponentManager* IComponentManager::mInstance = nullptr;

IComponentManager::IComponentManager()
{
	m_handlers.reserve(30);
}

void IComponentManager::OnConstruct(IComponent* comp)
{
	for (auto* handler : m_handlers)
	{
		handler->Register(comp);
	}
}

void IComponentManager::OnDestruct(IComponent* comp)
{
	for (auto* handler : m_handlers)
	{
		handler->UnRegister(comp);
	}
}

void IComponentManager::RegisterComponentHandler(IComponentHandler* handler)
{
	m_handlers.push_back(handler);
}