#pragma once
#include "pch.h"
#include "IComponentHandler.h"



class IComponent;
/*
	Manages only Most Base Components(AObject 바로 하단).
	Deeper Components Should be Managed By Lower Layer's Manager Like Tree Structure
	Ex) GUI -> GUI Layer By Object Type.
	If Object is Ocean, has Model, Material, Ocean GUI Components that inherits IGUIComponents
	Then, IGUIComponentManager lives in GUI Layer
*/
class IComponentManager final
{
public:

	void OnConstruct(IComponent* comp);
	void OnDestruct(IComponent* comp);
	void RegisterComponentHandler(IComponentHandler* handler);


	static IComponentManager* GetInstance()
	{
		if (mInstance == nullptr)
		{
			mInstance = new IComponentManager();
		}

		return mInstance;
	}

	static void Destroy()
	{
		delete mInstance;
		mInstance = nullptr;
	}

private:
	IComponentManager();
	~IComponentManager() = default;
	static IComponentManager* mInstance;

	std::vector<IComponentHandler*> m_handlers;
};

