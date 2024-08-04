#pragma once
#include "pch.h"

enum class EObjectComponentsFlag
{
	INPUT = 1,
	GUI = 1 << 1,
	// ENVIRONMENT, GAMEPLAY, PHYSICS, COLLISION, RENDER

};
DEFINE_ENUM_FLAG_OPERATORS(EObjectComponentsFlag)

class AObject
{
public:
	inline const EObjectComponentsFlag GetComponents() const
	{
		return m_objectComponentsFlag;
	}

protected:
	AObject(EObjectComponentsFlag componentFlags);
	virtual ~AObject() = default;

	void Destroy();
	void CompleteInitialization(); // Initialized On all Components



	EObjectComponentsFlag m_objectComponentsFlag;
	bool m_bInstantiated;
};