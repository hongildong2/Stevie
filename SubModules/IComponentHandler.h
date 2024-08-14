#pragma once
#include "pch.h"

class IComponent;
class IComponentHandler
{
public:
	virtual void Register(IComponent* comp) = 0;
	virtual void UnRegister(IComponent* comp) = 0;
};