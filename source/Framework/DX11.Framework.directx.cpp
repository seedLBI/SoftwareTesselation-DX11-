#include "DX11.Framework.h"

namespace framework {
	namespace core {

		namespace VARS {

			namespace directx {
				IDXGISwapChain*				m_swapChain;
				ID3D11Device*				m_device;
				ID3D11DeviceContext*		m_deviceContext;
				ID3D11RenderTargetView*		m_renderTargetView;
				ID3D11Texture2D*			m_depthStencilBuffer;
				ID3D11DepthStencilState*	m_depthStencilState;
				ID3D11DepthStencilView*		m_depthStencilView;
				D3D11_VIEWPORT				m_viewport;

				bool isVSyncEnabled = true;

				std::array<ID3D11RasterizerState*, 4> raster_states;
				std::array< std::array<ID3D11BlendState*, 5>, 3> blend_states;




				void InitRasterStates() {
					D3D11_RASTERIZER_DESC rasterDesc;
					rasterDesc.AntialiasedLineEnable = true;
					rasterDesc.DepthBias = 0;
					rasterDesc.DepthBiasClamp = 0.0f;
					rasterDesc.DepthClipEnable = true;

					rasterDesc.CullMode = D3D11_CULL_NONE;
					rasterDesc.FillMode = D3D11_FILL_SOLID;

					rasterDesc.FrontCounterClockwise = false;
					rasterDesc.MultisampleEnable = true;
					rasterDesc.ScissorEnable = false;
					rasterDesc.SlopeScaledDepthBias = 0.0f;




					HRESULT result = m_device->CreateRasterizerState(&rasterDesc, &raster_states[framework::directx::RasterStates::SOLID_CULL_NONE]);
					if (FAILED(result))
						assert(1);

					rasterDesc.CullMode = D3D11_CULL_BACK;

					result = m_device->CreateRasterizerState(&rasterDesc, &raster_states[framework::directx::RasterStates::SOLID_CULL_BACK]);
					if (FAILED(result))
						assert(1);

					rasterDesc.CullMode = D3D11_CULL_FRONT;

					result = m_device->CreateRasterizerState(&rasterDesc, &raster_states[framework::directx::RasterStates::SOLID_CULL_FRONT]);
					if (FAILED(result))
						assert(1);


					rasterDesc.CullMode = D3D11_CULL_NONE;
					rasterDesc.FillMode = D3D11_FILL_WIREFRAME;


					result = m_device->CreateRasterizerState(&rasterDesc, &raster_states[framework::directx::RasterStates::WIREFRAME_CULL_NONE]);
					if (FAILED(result))
						assert(1);

				}
				void InitBlendStates() {


					auto SetAllOp = [](D3D11_BLEND_DESC desc, std::array<ID3D11BlendState*, 5>& arrOps) {
						for (size_t i = 0; i < 5; i++)
						{
							desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP(i + 1);
							m_device->CreateBlendState(&desc, &arrOps[i]);
						}
					};


					D3D11_BLEND_DESC blendDesc;
					ZeroMemory(&blendDesc, sizeof(blendDesc));
					

					for (int x = 0; x < 8; x++)
					{
						blendDesc.RenderTarget[x].BlendEnable = false;
						blendDesc.RenderTarget[x].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
					}

					blendDesc.AlphaToCoverageEnable = false;
					blendDesc.IndependentBlendEnable = true;
					
					SetAllOp(blendDesc, blend_states[0]); // off


					blendDesc.RenderTarget[0].BlendEnable = TRUE;
					blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
					blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
					blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
					blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
					blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;


					SetAllOp(blendDesc, blend_states[1]); // on


					blendDesc.RenderTarget[0].BlendEnable = TRUE;
					blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
					blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;


					SetAllOp(blendDesc, blend_states[2]); // alpha

				}

				void CreateRenderTarget() {
					using namespace framework::core::VARS::directx;

					ID3D11Texture2D* backBufferPtr;
					m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferPtr);
					m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
					backBufferPtr->Release();
				}
				void CreateDepthStencil(int width, int height) {
					using namespace framework::core::VARS::directx;

					D3D11_TEXTURE2D_DESC depthBufferDesc;
					ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
					depthBufferDesc.Width = width;
					depthBufferDesc.Height = height;
					depthBufferDesc.MipLevels = 1;
					depthBufferDesc.ArraySize = 1;
					depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
					depthBufferDesc.SampleDesc.Count = 1;
					depthBufferDesc.SampleDesc.Quality = 0;
					depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
					depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
					depthBufferDesc.CPUAccessFlags = 0;
					depthBufferDesc.MiscFlags = 0;

					m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);

					D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
					ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
					depthStencilDesc.DepthEnable = true;
					depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
					depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
					depthStencilDesc.StencilEnable = true;
					depthStencilDesc.StencilReadMask = 0xFF;
					depthStencilDesc.StencilWriteMask = 0xFF;

					depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
					depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

					depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
					depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

					m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
					m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

					D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
					ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
					depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
					depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
					depthStencilViewDesc.Texture2D.MipSlice = 0;

					m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
					m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
				}

				void OnResize(int width, int height) {
					using namespace framework::core::VARS::directx;
					using namespace framework::core::VARS::window;

					if (m_device == nullptr || m_swapChain == nullptr) return;

					// Если в полноэкранном режиме, игнорируем изменения размера
					if (framework::window::IsFullscreen()) return;

					// Обновляем сохраненные размеры
					windowedWidth = width;
					windowedHeight = height;

					// Освобождаем старые ресурсы
					if (m_renderTargetView) {
						m_renderTargetView->Release();
						m_renderTargetView = nullptr;
					}

					if (m_depthStencilView) {
						m_depthStencilView->Release();
						m_depthStencilView = nullptr;
					}

					if (m_depthStencilBuffer) {
						m_depthStencilBuffer->Release();
						m_depthStencilBuffer = nullptr;
					}

					// Изменяем размер буферов swap chain
					HRESULT hr = m_swapChain->ResizeBuffers(
						0, // Сохраняем текущее количество буферов
						width,
						height,
						DXGI_FORMAT_UNKNOWN, // Сохраняем текущий формат
						0  // Флаги
					);

					if (SUCCEEDED(hr) && width > 0 && height > 0) {
						// Воссоздаем ресурсы с новыми размерами
						CreateRenderTarget();
						CreateDepthStencil(width, height);

						// Обновляем вьюпорт
						m_viewport.Width = static_cast<float>(width);
						m_viewport.Height = static_cast<float>(height);
						m_deviceContext->RSSetViewports(1, &m_viewport);
					}
				}
			}

			void InitDirectX() {
				using namespace framework::core::VARS::directx;
				using namespace framework::directx;


				HRESULT result;


				auto gpus = GetAvailableGPU();
				auto monitors = GetAvailableMonitor();

				PrintInfoGPU(gpus);
				PrintAvailableMonitors(monitors);

				auto primary_monitor	= GetPrimary(monitors);
				auto highest_res		= primary_monitor.GetHighestResolution();
				auto best_gpu			= gpus[GetIndexBestGPU(gpus)];




				IDXGIFactory* factory = nullptr;
				result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
				if (FAILED(result)) {
					std::cerr << "Failed to create DXGI factory!" << std::endl;
					assert(1);
				}

				IDXGIAdapter* adapter = nullptr;
				result = factory->EnumAdapters(best_gpu.index, &adapter);
				if (FAILED(result)) {
					std::cerr << "Failed to enumerate adapter at index " << best_gpu.index << std::endl;
					factory->Release();
					assert(1);
				}

				IDXGIAdapter* adapter_first = nullptr;
				result = factory->EnumAdapters(0, &adapter_first);
				if (FAILED(result)) {
					std::cerr << "Failed to enumerate adapter at index " << best_gpu.index << std::endl;
					factory->Release();
					assert(1);
				}


				



				DXGI_SWAP_CHAIN_DESC swapChainDesc;
				ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

				swapChainDesc.BufferCount = 1;
				swapChainDesc.BufferDesc.Width  = highest_res.size.first;
				swapChainDesc.BufferDesc.Height = highest_res.size.second;
				swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				swapChainDesc.BufferDesc.RefreshRate.Numerator	 = highest_res.refresh_rates.back().hz.first;
				swapChainDesc.BufferDesc.RefreshRate.Denominator = highest_res.refresh_rates.back().hz.second;
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.OutputWindow = framework::window::GetHandle();
				swapChainDesc.SampleDesc.Count = 1;
				swapChainDesc.SampleDesc.Quality = 0;
				swapChainDesc.Windowed = true;
				swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
				swapChainDesc.Flags = 0;

				D3D_FEATURE_LEVEL featureLevel;
				featureLevel = D3D_FEATURE_LEVEL_11_0;

				result = D3D11CreateDeviceAndSwapChain(
					adapter,
					D3D_DRIVER_TYPE_UNKNOWN,
					NULL, 
					0, 
					&featureLevel, 
					1,
					D3D11_SDK_VERSION, 
					&swapChainDesc, 
					&m_swapChain, 
					&m_device, 
					NULL, 
					&m_deviceContext);

				if (FAILED(result))
					assert(1);

				if (adapter) adapter->Release();
				if (factory) factory->Release();


				//m_swapChain->SetFullscreenState(TRUE, output);





				CreateRenderTarget();
				CreateDepthStencil(highest_res.size.first, highest_res.size.second);
				InitRasterStates();
				InitBlendStates();


				SetRasterState(RasterStates::WIREFRAME_CULL_NONE);
				SetBlendState(BlendStates::ALPHA, BlendOp::ADD);


				m_viewport.Width  = (float)highest_res.size.first;
				m_viewport.Height = (float)highest_res.size.second;
				m_viewport.MinDepth = 0.0f;
				m_viewport.MaxDepth = 1.0f;
				m_viewport.TopLeftX = 0.0f;
				m_viewport.TopLeftY = 0.0f;

				GetContext()->RSSetViewports(1, &m_viewport);

			}
			void TerminateDirectX() {
				using namespace framework::core::VARS::directx;

				if (m_swapChain) {
					m_swapChain->SetFullscreenState(false, NULL);
				}

				for (size_t i = 0; i < raster_states.size(); i++) {
					raster_states[i]->Release();
					raster_states[i] = 0;
				}

				for (size_t i = 0; i < blend_states.size(); i++) {
					for (size_t j = 0; j < blend_states[i].size(); j++)
					{
						blend_states[i][j]->Release();
						blend_states[i][j] = 0;
					}
				}

				if (m_depthStencilView) {
					m_depthStencilView->Release();
					m_depthStencilView = 0;
				}

				if (m_depthStencilState) {
					m_depthStencilState->Release();
					m_depthStencilState = 0;
				}

				if (m_depthStencilBuffer) {
					m_depthStencilBuffer->Release();
					m_depthStencilBuffer = 0;
				}

				if (m_renderTargetView) {
					m_renderTargetView->Release();
					m_renderTargetView = 0;
				}

				if (m_deviceContext) {
					m_deviceContext->Release();
					m_deviceContext = 0;
				}

				if (m_device) {
					m_device->Release();
					m_device = 0;
				}

				if (m_swapChain) {
					m_swapChain->Release();
					m_swapChain = 0;
				}

				return;
			}
		}


		
	}

	namespace directx {
		using namespace framework::core::VARS::directx;

		ID3D11Device* GetDevice() {
			return m_device;
		}
		ID3D11DeviceContext* GetContext() {
			return m_deviceContext;
		}

		void BeginScene(const float& red, const float& green, const float& blue, const float& alpha) {
			

			float color_array[4];

			color_array[0] = red;
			color_array[1] = green;
			color_array[2] = blue;
			color_array[3] = alpha;

			m_deviceContext->ClearRenderTargetView(m_renderTargetView, color_array);
			m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);


		}

		void BeginScene(const DirectX::XMFLOAT4& color) {
			BeginScene(color.x, color.y, color.z, color.w);
		}

		void EndScene() {
			using namespace framework::core::VARS::directx;

			if (isVSyncEnabled)
				m_swapChain->Present(1, 0);
			else
				m_swapChain->Present(0, 0);
		}

		void SetBackBufferRenderTarget() {
			m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
		}

		void ResetViewport() {
			if (!m_deviceContext)
				return;

			m_viewport.Width = window::GetSize().x;
			m_viewport.Height = window::GetSize().y;
			m_deviceContext->RSSetViewports(1, &m_viewport);
			

		}
		void SetRasterState(const RasterStates& state) {
			GetContext()->RSSetState(raster_states[state]);
		}
		void SetBlendState(const BlendStates& state, const BlendOp& op) {
			float blendFactor[4] = { .0f,.0f,.0f,.0f };
			GetContext()->OMSetBlendState(framework::core::VARS::directx::blend_states[state][op], blendFactor, 0xffffffff);
		}


		void EnableVSync() {
			isVSyncEnabled = true;
		}
		void DisableVSync() {
			isVSyncEnabled = false;
		}
		void ToggleVSync() {
			isVSyncEnabled = !isVSyncEnabled;
		}


	}
}