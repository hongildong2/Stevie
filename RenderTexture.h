#pragma once

class RenderTexture
{
public:
	RenderTexture(DXGI_FORMAT format);

	void SetDevice(ID3D11Device* device);

	void SizeResources(size_t width, size_t height);

	void ReleaseDevice();

	void SetWindow(const RECT& rect);

	ID3D11Texture2D* GetRenderTarget() const { return m_renderTarget.Get(); }
	ID3D11RenderTargetView* GetRenderTargetView() const { return m_renderTargetView.Get(); }
	ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView.Get(); }
	ID3D11UnorderedAccessView* GetUnorderedAccessView() const { return m_unorderedAccessView.Get(); }

	DXGI_FORMAT GetFormat() const { return m_format; }

private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_renderTarget;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_unorderedAccessView;


	DXGI_FORMAT m_format;

	size_t m_width;
	size_t m_height;
};