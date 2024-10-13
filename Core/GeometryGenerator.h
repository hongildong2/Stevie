#pragma once
#include "pch.h"

/*
* reference : honglab introduction to graphics course
*/
MeshData MakeSphere(const float radius, const unsigned int numSlices, const unsigned int numStacks);
MeshData MakeBox(const float scale);
MeshData MakeSquare(const float scale);
void MakeCWQuadPatches(unsigned int splitLevel, MeshData* const outMesh);