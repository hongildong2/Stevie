#pragma once
#include "pch.h"
#include "Light.h"
#include "Ocean.h"
#include "Model.h"

class GUI
{

public:
private:

	std::vector<Light*> m_lights;
	std::vector<Ocean*> m_oceans;
	std::vector<Model*> m_models;
};

