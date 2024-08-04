#pragma once
#include "pch.h"

enum class EGUIFlags
{
	MODEL = 1 << 0,
	MATERIAL = 1 << 1,
	LIGHT = 1 << 2,
	OCEAN = 1 << 3
};

DEFINE_ENUM_FLAG_OPERATORS(EGUIFlags)