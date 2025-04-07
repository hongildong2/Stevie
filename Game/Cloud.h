#pragma once
#include "pch.h"
#include "SSceneObject.h"

class Cloud :
	public SSceneObject
{
public:
	Cloud(RRenderer* pRenderer);
	~Cloud() = default;
};

