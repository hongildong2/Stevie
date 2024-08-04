#include "pch.h"
#include "AObject.h"
#include "AObjectManager.h"

AObject::AObject(EObjectComponentsFlag componentFlags)
	: m_objectComponentsFlag(componentFlags)
	, m_bInstantiated(true) // Àá½Ã¸¸..
{
	auto* man = AObjectManager::GetInstance();
	man->OnConstruct(this);
}

void AObject::Destroy()
{
	m_bInstantiated = false;
}

void AObject::CompleteInitialization()
{
	auto* man = AObjectManager::GetInstance();
	m_bInstantiated = true;
	man->OnInitialized(this);
}