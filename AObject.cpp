#include "pch.h"
#include "AObject.h"
#include "AObjectManager.h"

AObject::AObject(const EObjectType type)
	: m_type(type)
	, m_objectComponentsFlag(EComponentsFlag::NONE)
	, m_bInstantiated(true) // Àá½Ã¸¸..
{
	auto* man = AObjectManager::GetInstance();
	man->OnConstruct(this);
}
void AObject::SetComponentFlag(const EComponentsFlag flag)
{
	m_objectComponentsFlag |= flag;
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