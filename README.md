![r1](https://github.com/user-attachments/assets/74c46747-c8dd-4026-9986-ca88b04bf35a)

# Features
- Physically Based Rendering Shader.
- Using abstracted renderer, `Renderer`, can utilize multiple Graphics API.
  
## Realtime Ocean Simulation Based on [Tessendorf Paper](https://www.researchgate.net/publication/264839743_Simulating_Ocean_Water)
- Fast Fourier Transform on GPU using Compute Shader, sampling JONSWAP Spectrum.
- Realtime processing of over a million wave vectors.
- Tessellation and dynamic LOD.
- Subsurface Scattered PBR Shader for ocean surface based on [Bruneton](https://inria.hal.science/inria-00443630/file/article-1.pdf) paper.
- [Video](https://www.youtube.com/watch?v=rSCYnYdFnr0)

## PostProcess and Effect
- Dynamically generated cloud.
	- 3D Perlin noise texture using Compute Shader
 	- Volumetric rendering pixel shader.
- HDRI Pipeline, Gamma correction, Depth only pass.
- Physically Based Bloom using Compute Shader.
- Fog effect.
- Dynamic PCSS Shadow using light's shadow map.
- [Video](https://www.youtube.com/watch?v=D6w55CkHi5U)

## Renderer Structure

- Core, `SSceneObject`
	- Has MeshComponent to render
- `RRenderer`
	- D3D11Renderer class implements renderer interface.
- `GraphicsCommon.h`
	- Working renderer backends initializes common resources like shader, sampler states.
- `MeshComponent`
	- `RMeshGeometry`
		- Contains informations about geometry pipeline like vertex buffer, index buffer, input layout.
	- `RMaterial`
		- Contains informations about resources like pixel shader, shader resources, sampler states, blend states, ...
		- `RMaterial` classes corresponds 1:1 to pixel shader.
  	- MeshComponent has multiple properties which can decide different draw policy for each render frame. 
- Rendering Process
	1. Contents layer only calls `MeshComponent->Render();`.
 	2. Renderer populates render list with `RenderItem` struct to capture object's render pass, render states.
	3. Renderer decides draw policy of `RenderItem` that is required to geometry pipeline or some render pass looking up to `RMeshGeometry` and `RMaterial`'s property.
	4. Renderer then loops `RenderItem` to render each item according to its render pass. There can be multiple render pass and `RenderItem` might be drawn multiple times varying render states.
	5. Call `Draw()` according to mesh type.
 	6. Using this structure, render thread can diverge from flow of main game thread's loop. It can be multithreaded as each states recorded in `RenderItem` is stateless.

# Build
requires Visual Studio 2022, nuget packages are referenced in VS Project
