#pragma once
#include "AObject.h"
#include "IComponent.h"
#include "GUI\EGUIType.h"

class IGUIComponent : public IComponent
{
public:
	IGUIComponent(EGUIType type)
		: IComponent(EComponentsFlag::GUI)
		, m_type(type)
	{
	}

	inline const EGUIType GetGUIType() const
	{
		return m_type;
	}

protected:
	EGUIType m_type;
};