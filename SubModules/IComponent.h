#pragma once
#include "pch.h"
#include "IComponentManager.h"


// TODO : Component -> Module

// Describes ComponentType with Enum or Which Components this AObject has with bitflags
enum class EComponentsFlag
{
	NONE = 0,
	INPUT = 1,
	GUI = 1 << 1,
	// ENVIRONMENT, GAMEPLAY, PHYSICS, COLLISION, RENDER

};
DEFINE_ENUM_FLAG_OPERATORS(EComponentsFlag)


class AObject;
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

	virtual AObject* GetThis() = 0;
	inline const EComponentsFlag GetComponentType() const
	{
		return m_type;
	}

protected:
	EComponentsFlag m_type;
};