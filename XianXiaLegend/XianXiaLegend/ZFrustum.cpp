#include "ZFrustum.h"

ZFrustum::ZFrustum(void)
{
}

ZFrustum::~ZFrustum(void)
{
}

// ���������ͶӰ����,�õ�6��ƽ��
bool ZFrustum::Make(D3DXMATRIX* pmatViewProj,float fEpsilon)
{
	m_fEpsilon = fEpsilon;
	int i;
	D3DXMATRIX matInv;

	m_vtx[0].x = -1.0f; m_vtx[0].y = -1.0f; m_vtx[0].z = 0.0f;
	m_vtx[1].x =  1.0f; m_vtx[1].y = -1.0f; m_vtx[1].z = 0.0f;
	m_vtx[2].x =  1.0f; m_vtx[2].y = -1.0f; m_vtx[2].z = 1.0f;
	m_vtx[3].x = -1.0f; m_vtx[3].y = -1.0f; m_vtx[3].z = 1.0f;
	m_vtx[4].x = -1.0f; m_vtx[4].y =  1.0f; m_vtx[4].z = 0.0f;
	m_vtx[5].x =  1.0f; m_vtx[5].y =  1.0f; m_vtx[5].z = 0.0f;
	m_vtx[6].x =  1.0f; m_vtx[6].y =  1.0f; m_vtx[6].z = 1.0f;
	m_vtx[7].x = -1.0f; m_vtx[7].y =  1.0f; m_vtx[7].z = 1.0f;

	// ���view*proj�������
	D3DXMatrixInverse(&matInv,NULL,pmatViewProj);
	// ��8������ת��������ռ�
	for (i = 0;i < 8;i++)
	{
		D3DXVec3TransformCoord(&m_vtx[i],&m_vtx[i],&matInv);
		D3DXVECTOR3 v = m_vtx[i];
	}
	// ���������Ĵ�������(���ý�������,��׼ȷ�����λ��)
	m_vPos = (m_vtx[0] + m_vtx[5]) / 2.0f;

	// ͨ��8���������������,����ƽ��̨��ƽ��,���߷�������
	// top
	D3DXPlaneFromPoints(&m_plane[0],m_vtx + 4,m_vtx + 7,m_vtx + 6);
	// bottom
	D3DXPlaneFromPoints(&m_plane[1],m_vtx,m_vtx + 1,m_vtx + 2);
	// near
	D3DXPlaneFromPoints(&m_plane[2],m_vtx,m_vtx + 4,m_vtx + 5);
	// far
	D3DXPlaneFromPoints(&m_plane[3],m_vtx + 2,m_vtx + 6,m_vtx + 7);
	// left
	D3DXPlaneFromPoints(&m_plane[4],m_vtx,m_vtx + 3,m_vtx + 7);
	// right
	D3DXPlaneFromPoints(&m_plane[5],m_vtx + 1,m_vtx + 5,m_vtx + 6);

	return true;
}

// ��pv��ƽ��̨����,����TRUE,����ƽ��̨����,����FALSE
bool ZFrustum::IsIn(D3DXVECTOR3* pv)
{
	for (int i=3;i<6;i++)
	{
		if (D3DXPlaneDotCoord(&m_plane[i],pv) > m_fEpsilon)
			return false;
	}
	return true;
}

// ��������Բ��(pv)�Ͱ뾶(radius)�ı߽�����(bounding sphere)��ƽ��̨����,����TRUE,����ƽ��̨����,����FALSE
bool ZFrustum::IsInSphere(D3DXVECTOR3* pv,float radius)
{
	for(int i=3;i<6;i++)
	{
		if (D3DXPlaneDotCoord(&m_plane[i],pv) > radius + m_fEpsilon)
			return false;
	}
	return true;
}