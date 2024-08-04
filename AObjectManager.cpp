#include "pch.h"
#include "AObjectManager.h"


AObjectManager* AObjectManager::mInstance = nullptr;

AObjectManager::AObjectManager()
{
	m_objects.reserve(1000);
	m_toInitialize.reserve(1000);
	m_toDestroy.reserve(1000);
}
void AObjectManager::OnAObjectConstruct(AObject* obj)
{
	// m_toInitialize.push_back(obj); 지금은 패스
	m_objects.insert(obj);
}

void AObjectManager::OnAObjectDestroy(AObject* obj)
{
	m_toDestroy.push_back(obj);
}

void AObjectManager::CollectGarbage()
{
	for (auto* obj : m_toDestroy)
	{
		// TODO : For Each Component layer, delist AObject From it
		m_objects.erase(obj);
	}
}


