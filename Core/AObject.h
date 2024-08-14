#pragma once
#include "pch.h"

// Indicates Unique Object Types in Most Upper Layer(Game)
enum class EObjectType
{
	MODEL = 0,
	LIGHT,
	OCEAN,
	POST_PROCESS,
	LENGTH
};


// 이걸상속받으면 무적권 Core에
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

	AObject(const AObject& other) = delete;
	AObject& operator=(const AObject& other) = delete;

	void Destroy();
	void CompleteInitialization(); // Initialized On all Components

	// status
	bool m_bInstantiated;

	// description
	EComponentsFlag m_objectComponentsFlag;
	const EObjectType m_type;
	const std::string m_name;
};