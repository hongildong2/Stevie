#pragma once

#include "pch.h"
#include "SSceneObject.h"
class Ocean final :
	public SSceneObject
{
public:
	Ocean(RRenderer* pRenderer);
	~Ocean() = default;

private:
	RRenderer* m_pRenderer;
};

