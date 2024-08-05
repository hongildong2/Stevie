#pragma once
#include "pch.h"
#include "AObject.h"
#include "IComponentManager.h"

class IComponent
{
public:
	IComponent(EComponentsFlag type)
		: m_type(type)
	{
		auto* cMan = IComponentManager::GetInstance();
		cMan->OnConstruct(this);
	}

	virtual ~IComponent()
	{
		auto* cMan = IComponentManager::GetInstance();
		cMan->OnDestruct(this);
	}

	virtual const AObject* GetThis() const = 0;
	inline const EComponentsFlag GetComponentType() const
	{
		return m_type;
	}

protected:
	EComponentsFlag m_type;
};