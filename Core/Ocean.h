#pragma once
#include "SSceneObject.h"
class Ocean final :
	public SSceneObject
{
public:
	Ocean();
	~Ocean() = default;

	Ocean(const Ocean& other) = delete;
	Ocean& operator=(const Ocean& other) = delete;

	virtual void Initialize() override;
	virtual void Update() override;
};

