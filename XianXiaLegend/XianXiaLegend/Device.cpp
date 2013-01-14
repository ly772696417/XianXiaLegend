#include "Device.h"

CDevice::CDevice(void)
{
	m_pD3D = NULL;
	m_pDevice = NULL;
}

CDevice::~CDevice(void)
{
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pD3D);
}

HRESULT CDevice::Init(HWND hWnd,bool window,int width,int height,bool ZBuffer,bool Multsample)
{
	// 创建Direct3D的COM接口对象
	if (NULL == (m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		return E_FAIL;
	}

	// 获取显卡设备的显示模式
	D3DDISPLAYMODE d3ddm;
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm)))
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp;// Direct3D设备信息
	ZeroMemory(&d3dpp,sizeof(d3dpp));
	d3dpp.Windowed = window;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.BackBufferWidth = width;//d3ddm.Width;
	d3dpp.BackBufferHeight = height;//d3ddm.Height;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// 选择深度缓冲格式
	if (m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,
								d3ddm.Format,D3DUSAGE_DEPTHSTENCIL,
								D3DRTYPE_SURFACE,D3DFMT_D32) == D3D_OK)
	// 选择32位深度缓冲区
	{
		d3dpp.AutoDepthStencilFormat = D3DFMT_D32;
		if (ZBuffer == true)
			d3dpp.EnableAutoDepthStencil = TRUE;
	}
	else if (m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,
								d3ddm.Format,D3DUSAGE_DEPTHSTENCIL,
								D3DRTYPE_SURFACE,D3DFMT_D24X8) == D3D_OK)
	// 选择24位深度缓冲区
	{
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
		if (ZBuffer == true)
			d3dpp.EnableAutoDepthStencil = TRUE;
	}
	else if (m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,
								d3ddm.Format,D3DUSAGE_DEPTHSTENCIL,
								D3DRTYPE_SURFACE,D3DFMT_D16) == D3D_OK)
	// 选择24位深度缓冲区
	{
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		if (ZBuffer == true)
			d3dpp.EnableAutoDepthStencil = TRUE;
	}
	else
	{
		d3dpp.EnableAutoDepthStencil = FALSE;
	}

	// 图形反锯齿
	for (int i=16;i>1;i--)
	{
		if (SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,d3ddm.Format,
															FALSE,(D3DMULTISAMPLE_TYPE)i,NULL)))
		{
			Mult = (D3DMULTISAMPLE_TYPE)i;
			break;
		}
		if (i == 1)
		{
			Mult = (D3DMULTISAMPLE_TYPE)0;
		}
	}
	if (Multsample == true && Mult != 0)
	{
		d3dpp.MultiSampleType = Mult;
	}

	// 创建Direct3D设备接口
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,
						D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&m_pDevice)))
	{
		return E_FAIL;
	}
	return S_OK;
}
