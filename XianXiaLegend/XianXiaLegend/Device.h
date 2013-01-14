#pragma once
#include "Common.h"

class CDevice
{
	IDirect3D9*				m_pD3D;				// D3D指针
	IDirect3DDevice9*		m_pDevice;			// D3D设备指针

	D3DMULTISAMPLE_TYPE		Mult;				// 最大支持多重采样点
//public:
	CDevice(void);
	~CDevice(void);

	HRESULT Init(HWND hWnd,bool window,int width,int height,bool ZBuffer,bool Multsample);
	friend class CFrantoGE;
};
