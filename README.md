# Work In Progress
- Now refactoring renderer backend to accomodate different Graphics API other than D3D11
- No base code or library is included for renderer
- Please refer to this page to see working version https://steveyoo.notion.site/766d6dcf88f7414b80f4c20527e84b0b?pvs=74

## Renderer Structure
- Core, SSceneObject
	- Has MeshComponent to render
- IRenderer
	- D3D11Renderer class implements abstracted renderer interface.
- GraphicsCommon1.h
	- Working renderer backends initializes common resources like shader, sampler states..
- IRenderResource
	- Intermediate Classes to reference in contents layer, renderer backends cast to its resource class type to use.
	- RMaterial, RShader, RSamplerState,...
- MeshComponent
	- RMeshGeometry
		- Contains informations about vertex buffer, index buffer, input layout, only geometries.
	- RMaterial
		- Contains informations about pipeline states like pixel shader, shader resources, sampler states, blend states,.. 
		- RMaterial classes corresponds 1:1 to pixel shader.
- Rendering
	- Contents layer only calls ``MeshComponent->Render();``
	- Then renderer fetchs `RMeshGeometry` and `RMaterial` from `MeshComponent`to make proper pipeline state required to render game object
	- If pipeline state is set, call `RMeshGeometry->Draw()`.
	- Multiple Render pass, Deferred Rendering planned when refactoring is done.

## Warning
Note that current branch is __WIP__, thus contain a lot of unused classes which is waited to be ported to the new version.

## Build
requires VS2022, nuget packages referenced in VS Project

requires vcpkg with _imgui:dx11_, _assimp_
may require asset files to boot