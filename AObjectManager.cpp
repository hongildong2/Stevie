#include "pch.h"
#include "AObjectManager.h"


AObjectManager* AObjectManager::mInstance = nullptr;

AObjectManager::AObjectManager()
	: m_hanlder(nullptr)
{
	m_objects.reserve(1000);
	m_toInitialize.reserve(1000);
	m_toDestroy.reserve(1000);
}
void AObjectManager::OnConstruct(AObject* obj)
{
	// m_toInitialize.push_back(obj); 지금은 패스
	m_objects.insert(obj);
}

void AObjectManager::OnInitialized(AObject* obj)
{
	// m_objects.insert(obj);
	// m_hander.Register(obj);
	// m_toInitialize.erase(obj);
}

void AObjectManager::OnDestruction(AObject* obj)
{
	// m_hander.UnRegister(obj);
	// m_toDestroy.erase(obj);
}

void AObjectManager::OnDestroy(AObject* obj)
{
	m_toDestroy.push_back(obj);
}

void AObjectManager::CollectGarbage()
{
	for (auto* obj : m_toDestroy)
	{
		// TODO : For Each Component layer, delist AObject From it
		m_objects.erase(obj);
		// delete obj;
	}
}


