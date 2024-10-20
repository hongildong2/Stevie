# Work In Progress
- Now refactoring renderer backend to accommodate different Graphics API like D3D12 other than D3D11.
- No base code or library is used in renderer.
- Please refer to this page to see working version https://steveyoo.notion.site/Renderer-dccd9902113441ea98ffb6f0a74487c4
## Renderer Structure
- Core, `SSceneObject`
	- Has MeshComponent to render
- `IRenderer`
	- D3D11Renderer class implements renderer interface.
- `GraphicsCommon1.h`
	- Working renderer backends initializes common resources like shader, sampler states..
- `IRenderResource`
	- Intermediate classes to reference in contents layer, renderer backends cast its resource class type to use.
	- `RMaterial`, `RShader`, `RSamplerState`, ...
- `MeshComponent`
	- `RMeshGeometry`
		- Contains informations about vertex buffer, index buffer, input layout, only geometries.
	- `RMaterial`
		- Contains informations about resources like pixel shader, shader resources, sampler states, blend states, ...
		- `RMaterial` classes corresponds 1:1 to pixel shader.
- Rendering Process
	- Contents layer only calls `MeshComponent->Render();`.
	- Then renderer fetches `RMeshGeometry` and `RMaterial` from `MeshComponent`to make proper pipeline state required to render game object.
	- If pipeline state is set, call `RMeshGeometry->Draw()`.
	- Multiple Render Pass, Deferred Rendering is planned when refactoring is done.

## Warning
Note that current branch is __WIP__, thus contain a lot of unused classes which is waited to be ported to the new version.

## Build
requires VS2022, nuget packages referenced in VS Project

requires vcpkg with _imgui:dx11_, _assimp_

may require asset files to boot
