#pragma once
#include "pch.h"

enum class EObjectComponentsFlag
{
	INPUT = 1,
	GUI = 1 << 2,
	// ENVIRONMENT, GAMEPLAY, PHYSICS, COLLISION, RENDER

};
DEFINE_ENUM_FLAG_OPERATORS(EObjectComponentsFlag)

class AObject
{
protected:
	AObject(EObjectComponentsFlag componentFlags);
	virtual ~AObject() = default;

	void Destroy();
	// void Initialize();

	inline const EObjectComponentsFlag GetComponents() const
	{
		return m_objectComponentsFlag;
	}

	EObjectComponentsFlag m_objectComponentsFlag;
	bool m_bInstantiated;
};