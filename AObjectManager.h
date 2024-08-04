#pragma once
#include "pch.h"
#include "AObject.h"
#include "IObjectHandler.h"

class AObjectManager final
{
public:
	void OnConstruct(AObject* obj);
	void OnInitialized(AObject* obj);
	void OnDestroy(AObject* obj); // Not Destructor
	void OnDestruction(AObject* obj); // Debugging
	void CollectGarbage();

	inline void RegisterIObjectHandler(IObjectHandler* handler)
	{
		m_hanlder = handler;
	}
	static AObjectManager* const GetInstance()
	{
		if (mInstance == nullptr)
		{
			mInstance = new AObjectManager();
		}

		return mInstance;
	}

	static void Destroy()
	{
		delete mInstance;
		mInstance = nullptr;
	}

private:
	AObjectManager();
	~AObjectManager() = default;

	static AObjectManager* mInstance;
	IObjectHandler* m_hanlder;
	std::unordered_set<AObject*> m_objects;

	std::vector<AObject*> m_toInitialize;
	std::vector<AObject*> m_toDestroy;
};

