#pragma once
#include "pch.h"
#include "GUI\EGUIType.h"

class AGUIComponent
{
protected:
	AGUIComponent(EGUIFlags flags)
		: m_GUIFlags(flags)
	{
	}
	EGUIFlags m_GUIFlags;
};

