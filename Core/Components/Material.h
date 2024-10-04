#pragma once
#include "pch.h"


class RTexture2D;
class RShader;

constexpr UINT MATERIAL_MAX_TEXTURE_SLOT = 10;
class Material // PBR Material, Ocean Material, ...
{

protected:
	RTexture2D* m_textures[MATERIAL_MAX_TEXTURE_SLOT];
	RShader* m_pixelShader;
};

