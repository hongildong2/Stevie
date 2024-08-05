#pragma once
#include "pch.h"

enum class EComponentsFlag
{
	NONE = 0,
	INPUT = 1,
	GUI = 1 << 1,
	// ENVIRONMENT, GAMEPLAY, PHYSICS, COLLISION, RENDER

};
DEFINE_ENUM_FLAG_OPERATORS(EComponentsFlag)

enum class EObjectType
{
	MODEL = 0,
	LIGHT,
	OCEAN,
	LENGTH
};

class Model;
class Light;
class Ocean;

class AObject
{
public:

	void SetComponentFlag(const EComponentsFlag flag);
	inline const EComponentsFlag GetComponents() const
	{
		return m_objectComponentsFlag;
	}

	inline const bool IsInstantiated() const
	{
		return m_bInstantiated;
	}

protected:
	AObject(const EObjectType type);
	virtual ~AObject() = default;

	void Destroy();
	void CompleteInitialization(); // Initialized On all Components

	const EObjectType m_type;
	EComponentsFlag m_objectComponentsFlag;
	bool m_bInstantiated;
};