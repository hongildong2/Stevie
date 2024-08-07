#pragma once
#include "pch.h"


// Describes ComponentType with Enum or Which Components this AObject has with bitflags
enum class EComponentsFlag
{
	NONE = 0,
	INPUT = 1,
	GUI = 1 << 1,
	// ENVIRONMENT, GAMEPLAY, PHYSICS, COLLISION, RENDER

};
DEFINE_ENUM_FLAG_OPERATORS(EComponentsFlag)


// Indicates Unique Object Types in Most Upper Layer(Game)
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

	inline const char* GetName() const
	{
		return m_name.c_str();
	}

protected:
	AObject(const char* name, const EObjectType type);
	virtual ~AObject() = default;

	void Destroy();
	void CompleteInitialization(); // Initialized On all Components

	// status
	bool m_bInstantiated;

	// description
	EComponentsFlag m_objectComponentsFlag;
	const EObjectType m_type;
	const std::string m_name;
};