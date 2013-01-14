#include "ZFrustum.h"

ZFrustum::ZFrustum(void)
{
}

ZFrustum::~ZFrustum(void)
{
}

// 输入摄像机投影矩阵,得到6个平面
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

	// 求得view*proj的逆矩阵
	D3DXMatrixInverse(&matInv,NULL,pmatViewProj);
	// 将8个顶点转换到世界空间
	for (i = 0;i < 8;i++)
	{
		D3DXVec3TransformCoord(&m_vtx[i],&m_vtx[i],&matInv);
		D3DXVECTOR3 v = m_vtx[i];
	}
	// 求出摄像机的大致坐标(近裁截面中心,非准确摄像机位置)
	m_vPos = (m_vtx[0] + m_vtx[5]) / 2.0f;

	// 通过8个顶点的世界坐标,制作平截台体平面,法线方向向外
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

// 点pv在平截台体内,返回TRUE,不在平截台体内,返回FALSE
bool ZFrustum::IsIn(D3DXVECTOR3* pv)
{
	for (int i=3;i<6;i++)
	{
		if (D3DXPlaneDotCoord(&m_plane[i],pv) > m_fEpsilon)
			return false;
	}
	return true;
}

// 具有中心圆点(pv)和半径(radius)的边界球体(bounding sphere)在平截台体内,返回TRUE,不在平截台体内,返回FALSE
bool ZFrustum::IsInSphere(D3DXVECTOR3* pv,float radius)
{
	for(int i=3;i<6;i++)
	{
		if (D3DXPlaneDotCoord(&m_plane[i],pv) > radius + m_fEpsilon)
			return false;
	}
	return true;
}