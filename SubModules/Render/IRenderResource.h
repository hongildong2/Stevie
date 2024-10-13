#pragma once
#include "pch.h"

interface IRenderResource
{
public:
	IRenderResource() = default;
	virtual ~IRenderResource() = default;
	IRenderResource(const IRenderResource& other) = delete;
	IRenderResource& operator=(const IRenderResource& other) = delete;
};



/*
* TODO :: Manage Ref counts from manager
* TODO :: Use QueryInterface of COM to cast?
Inherit IUnknwon

STDMETHODIMP			QueryInterface(REFIID, void** ppv);
STDMETHODIMP_(ULONG)	AddRef();
STDMETHODIMP_(ULONG)	Release();

protected:
	virtual void CleanUp() = 0;
*/