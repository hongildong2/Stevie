#pragma once
#include "pch.h"
#include "SSceneObject.h"

class Cloud :
	public SSceneObject
{
public:
	Cloud(IRenderer* pRenderer);
	~Cloud() = default;
};

