#include "Camera.h"

CCamera::CCamera(IDirect3DDevice9* pDevice,CInput* pInput)
{
	m_pDevice	= pDevice;
	m_pInput	= pInput;
	m_vPos		= D3DXVECTOR3( 0.0f,5.0f,-10.0f );
    m_vLookAt	= D3DXVECTOR3( 0.0f,0.0f,0.0f );
	m_vUp		= D3DXVECTOR3( 0.0f,1.0f,0.0f );
	m_fNear		= 1.0f;
	m_fFar		= 5000.0f;
	m_fFOV		= D3DX_PI/4;
	m_fAspect	= 800.0f/600.0f;
	m_fYaw		= D3DX_PI * 3 / 2;
	m_fPitch	= D3DX_PI / 6;
	m_fDistance = 300.0f;
	m_fMaxDistance = 3000.0f;
	m_fMinDistance = 50.0f;
	m_bMode		= true;
}

CCamera::~CCamera(void)
{
}

void CCamera::CalcEyePos()
{
	m_vPos.x = m_vLookAt.x + m_fDistance*cosf(m_fPitch)*cosf(m_fYaw);
	m_vPos.y = m_vLookAt.y + m_fDistance*sinf(m_fPitch);
	m_vPos.z = m_vLookAt.z + m_fDistance*cosf(m_fPitch)*sinf(m_fYaw);
}

void CCamera::SetCamera()
{
	// 计算视图变换
	D3DXMATRIX  mView;
  	D3DXMatrixLookAtLH( &mView,&m_vPos,&m_vLookAt,&m_vUp );
	m_pDevice->SetTransform(D3DTS_VIEW,&mView);

	// 计算投影变换
	D3DXMATRIX  mProj;
 	D3DXMatrixPerspectiveFovLH( &mProj,m_fFOV,m_fAspect,m_fNear,m_fFar );
	m_pDevice->SetTransform(D3DTS_PROJECTION,&mProj);
}

void CCamera::ChangeCamera(float fYaw,float fPitch)
{
	if (m_bMode == true)
	{
		m_fYaw -= fYaw;
		/*m_fYaw = (m_fYaw * 1000) % */
		if (m_fYaw > D3DX_PI * 2)
			m_fYaw = m_fYaw - D3DX_PI * 2;
		if (m_fYaw < 0)
			m_fYaw = m_fYaw + D3DX_PI * 2;
		if( m_fPitch+fPitch < D3DX_PI/2 && m_fPitch+fPitch > -D3DX_PI/3 )
			m_fPitch += fPitch;
	}
	CalcEyePos();
}

void CCamera::ChangeDistance(float fDistance)
{
	if (m_bMode == true)
	{
		if (m_fDistance+fDistance > m_fMinDistance && m_fDistance+fDistance < m_fMaxDistance)
			m_fDistance += fDistance;
	}
	CalcEyePos();
}

void CCamera::ChangeMode()
{
	if (m_bMode == true)
	{
		m_bMode = false;
		m_fYaw = D3DX_PI * 3 / 2;
		m_fPitch = D3DX_PI / 6;
		m_fDistance = m_fMaxDistance;
		CalcEyePos();
	}
	else
	{
		m_bMode = true;
	}
}

void CCamera::Cortrol()
{
	if (m_pInput->KeyDown(DIK_F9))
	{
		ChangeMode();
	}

	// 跟随视角摄像机
	if (m_bMode == true){
		if (m_pInput->MouseHold(1))
		{
			float tmpYaw = (float)m_pInput->GetX()/100.0f;
			ChangeCamera(tmpYaw,0);
			float tmpPitch = (float)m_pInput->GetY()/100.0f;
			ChangeCamera(0,tmpPitch);
		}
		if (m_pInput->GetZ() > 0)
		{
			float tmpDist = -20.0f;
			ChangeDistance(tmpDist);
		}
		if (m_pInput->GetZ() < 0)
		{
			float tmpDist = 20.0f;
			ChangeDistance(tmpDist);
		}
	}
	// 第三人称摄像机
	else
	{
		GetWindowRect(m_pInput->GetHWnd(),&m_winRect);
		POINT tmpPos;
		GetCursorPos(&tmpPos);// 获取屏幕x,y坐标
		if (tmpPos.x <= m_winRect.left)
		{
			m_vLookAt.x = m_vLookAt.x - 50.0f;
		}
		if (tmpPos.x >= m_winRect.right)
		{
			m_vLookAt.x = m_vLookAt.x + 50.0f;
		}
		if (tmpPos.y <= m_winRect.top)
		{
			m_vLookAt.z = m_vLookAt.z + 50.0f;
		}
		if (tmpPos.y >= m_winRect.bottom)
		{
			m_vLookAt.z = m_vLookAt.z - 50.0f;
		}
		CalcEyePos();
	}
}

