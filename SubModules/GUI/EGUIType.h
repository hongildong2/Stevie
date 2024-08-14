#pragma once
#include "pch.h"
#include "AObject.h"

enum class EGUIType
{
	MODEL = 0,
	LIGHT,
	OCEAN,
	POST_PROCESS,
	LENGTH
};

DEFINE_ENUM_FLAG_OPERATORS(EGUIType)

static_assert(static_cast<int>(EGUIType::LENGTH) == static_cast<int>(EObjectType::LENGTH), "THERE MUST BE GUI DEFINED FOR EACH OBJECT");