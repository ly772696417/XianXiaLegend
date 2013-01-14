#pragma once
#include "common.h"

class CMyFuntion
{
public:
	CMyFuntion(void);
	~CMyFuntion(void);

	// 字符串复制
	static HRESULT CopyName(LPCSTR Name,LPSTR *pNewName);
	// 方向转换偏移量
	static D3DXVECTOR3 DirToRot(D3DXVECTOR3 vDir,D3DXVECTOR3 vOrigRot);
	// 获取世界空间的鼠标点击射线
	static Ray CalculateRay(HWND hWnd,IDirect3DDevice9* pDevice);
	// 获取射线与三角形的交点
	/*static bool IntersectTriangle(
		const Ray& ray,
		D3DXVECTOR3& v1,
		D3DXVECTOR3& v2,
		D3DXVECTOR3& v3,
		float* t,
		float* u,
		float* v);*/
	// 包围盒与包围盒碰撞
	static bool CollisonTestAABB(D3DXVECTOR3 AMin,D3DXVECTOR3 AMax,D3DXVECTOR3 BMin,D3DXVECTOR3 BMax);
	// 点与包围盒碰撞
	static bool CollisonTestPoint(D3DXVECTOR3 A,D3DXVECTOR3 BMin,D3DXVECTOR3 BMax);
	// 字符转换向量
	static D3DXVECTOR3 GetVec3FromStr(char* str);
	// 画线
	static void DrawLine(
		LPDIRECT3DDEVICE9 pDevice,
		D3DXVECTOR3 &v1,
		D3DXVECTOR3 &v2,
		D3DCOLOR color);
	// 计算两点间的斜率
	static float CalcSlope(D3DXVECTOR3 start,D3DXVECTOR3 end);
	// 随机值： 0-1
	static float UnitRandom();
	// 随机值： fLow-fHigh
	static float RangeRandom(float fLow, float fHigh);
	// 随机值： (-1) - (1)
	static float SymmetricRandom();
};
