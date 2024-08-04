#pragma once
#include "pch.h"
#include "AObject.h"

class AObjectManager final
{
public:
	void OnAObjectConstruct(AObject* obj);
	void OnAObjectDestroy(AObject* obj); // Not Destructor
	void CollectGarbage();


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
	std::unordered_set<AObject*> m_objects;

	std::vector<AObject*> m_toInitialize;
	std::vector<AObject*> m_toDestroy;
};

