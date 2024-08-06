#pragma once
#include "pch.h"
#include "AObject.h"
#include "IObjectHandler.h"

class AObjectManager final
{
public:

	// TODO : Object constructed is not yet initialized, not usable. Thus it is placed on just toInitialize. Will be Initialize Next Tick or any other Timing Controlled by IObjectHandler
	void OnConstruct(AObject* obj);
	// TODO : Destructor will be called when CollectGarbage() is called, removing object's component's reference from component layer(IComponentHandler)
	void OnDestruction(AObject* obj); // Debugging

	// TODO : Object must be initialized before usage, timing is controlled by IObjectHandler
	void OnInitialized(AObject* obj);
	// TODO : Object destroyed does not instantly Destructed, just marked as not instantiated. Destruction Timing will be handled by IObjectHandler
	void OnDestroy(AObject* obj); // Not Destructor

	/// <summary>
	/// IObjectHandler manually manages timing of destroyed(uninstantiated) object's garbage collection. (DEFAULT = Per Tick)
	/// </summary>
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

