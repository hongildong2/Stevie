#include "pch.h"
#include "SubModules/GUI/DTOs.h"

MaterialDTO::MaterialDTO(const Material& rhs)
{
	this->albedo = rhs.albedo;
	this->aoFactor = rhs.aoFactor;
	this->bUseTexture = rhs.bUseTexture;

	this->metallic = rhs.metallic;
	this->metallicFactor = rhs.metallicFactor;

	this->roughness = rhs.roughness;
	this->roughnessFactor = rhs.roughnessFactor;

	this->specular = rhs.specular;
	this->t1 = rhs.t1;

	this->IBLStrength = rhs.IBLStrength;

	this->bUseAlbedoTexture = rhs.bUseAlbedoTexture;
	this->bUseAOTexture = rhs.bUseAOTexture;
	this->bUseMetallicTexture = rhs.bUseMetallicTexture;
	this->bUseNormalTexture = rhs.bUseNormalTexture;

	this->bUseRoughnessTexture = rhs.bUseRoughnessTexture;
	this->bUseEmissiveTexture = rhs.bUseEmissiveTexture;
	this->bUseOpacityTexture = rhs.bUseOpacityTexture;
}

LightDTO::LightDTO(const LightData& rhs)
{
	this->color = rhs.color;
	this->direction = rhs.direction;

	this->fallOffEnd = rhs.fallOffEnd;
	this->fallOffStart = rhs.fallOffStart;

	this->haloRadius = rhs.haloRadius;
	this->haloStrength = rhs.haloStrength;

	this->positionWorld = rhs.positionWorld;
	this->radiance = rhs.radiance;
	this->radius = rhs.radius;

	this->spotPower = rhs.spotPower;

}