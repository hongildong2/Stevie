#pragma once

#include "pch.h"
#include "SSceneObject.h"
class Ocean final :
	public SSceneObject
{
public:
	Ocean(IRenderer* pRenderer);
	~Ocean() = default;

private:
	IRenderer* m_pRenderer;
};

