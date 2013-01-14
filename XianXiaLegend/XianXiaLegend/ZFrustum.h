/*
*	平截台体类
*/
#pragma once
#include "Common.h"

class ZFrustum
{
private:
	D3DXVECTOR3 m_vtx[8];             // 平截台体8个顶点
	D3DXVECTOR3 m_vPos;               // 当前摄像机的世界坐标
	D3DXPLANE m_plane[6];             // 平截台体6个面
	float m_fEpsilon;                 // 碰撞判断修正值,使碰撞边界稍稍增大

public:
	ZFrustum(void);
	~ZFrustum(void);

	// 输入摄像机投影矩阵,得到6个平面
	bool Make(D3DXMATRIX* pmatViewProj,float fEpsilon);
	// 点v在平截台体内,返回TRUE,不在平截台体内,返回FALSE
	bool IsIn(D3DXVECTOR3* pv);
	// 具有中心圆点(v)和半径(radius)的边界球体(bounding sphere)在平截台体内,返回TRUE,不在平截台体内,返回FALSE
	bool IsInSphere(D3DXVECTOR3* pv,float radius);
	// 得到当前摄像机的世界坐标
	D3DXVECTOR3* GetPos(){return &m_vPos;}
};
