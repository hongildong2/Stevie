#pragma once
#include "pch.h"

enum class EGUIType
{
	MODEL = 1,
	MESH = 1 << 1,
	LIGHT = 1 << 2,
	OCEAN = 1 << 3
};

DEFINE_ENUM_FLAG_OPERATORS(EGUIType)