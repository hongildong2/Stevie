#pragma once
#include "pch.h"
#include "AObject.h"

class AObjectHandler
{
public:
	virtual void Register(AObject* obj) = 0;
	virtual void UnRegister(AObject* obj) = 0;
};