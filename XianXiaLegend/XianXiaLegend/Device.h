#pragma once
#include "Common.h"

class CDevice
{
	IDirect3D9*				m_pD3D;				// D3Dָ��
	IDirect3DDevice9*		m_pDevice;			// D3D�豸ָ��

	D3DMULTISAMPLE_TYPE		Mult;				// ���֧�ֶ��ز�����
//public:
	CDevice(void);
	~CDevice(void);

	HRESULT Init(HWND hWnd,bool window,int width,int height,bool ZBuffer,bool Multsample);
	friend class CFrantoGE;
};
