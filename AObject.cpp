#include "pch.h"
#include "AObject.h"
#include "AObjectManager.h"

AObject::AObject(EObjectComponentsFlag componentFlags)
	: m_objectComponentsFlag(componentFlags)
	, m_bInstantiated(true) // Àá½Ã¸¸..
{
	auto* man = AObjectManager::GetInstance();
	man->OnAObjectConstruct(this);
}

void AObject::Destroy()
{
	m_bInstantiated = false;
}