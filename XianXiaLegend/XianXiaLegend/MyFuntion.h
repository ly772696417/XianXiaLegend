#pragma once
#include "common.h"

class CMyFuntion
{
public:
	CMyFuntion(void);
	~CMyFuntion(void);

	// �ַ�������
	static HRESULT CopyName(LPCSTR Name,LPSTR *pNewName);
	// ����ת��ƫ����
	static D3DXVECTOR3 DirToRot(D3DXVECTOR3 vDir,D3DXVECTOR3 vOrigRot);
	// ��ȡ����ռ�����������
	static Ray CalculateRay(HWND hWnd,IDirect3DDevice9* pDevice);
	// ��ȡ�����������εĽ���
	/*static bool IntersectTriangle(
		const Ray& ray,
		D3DXVECTOR3& v1,
		D3DXVECTOR3& v2,
		D3DXVECTOR3& v3,
		float* t,
		float* u,
		float* v);*/
	// ��Χ�����Χ����ײ
	static bool CollisonTestAABB(D3DXVECTOR3 AMin,D3DXVECTOR3 AMax,D3DXVECTOR3 BMin,D3DXVECTOR3 BMax);
	// �����Χ����ײ
	static bool CollisonTestPoint(D3DXVECTOR3 A,D3DXVECTOR3 BMin,D3DXVECTOR3 BMax);
	// �ַ�ת������
	static D3DXVECTOR3 GetVec3FromStr(char* str);
	// ����
	static void DrawLine(
		LPDIRECT3DDEVICE9 pDevice,
		D3DXVECTOR3 &v1,
		D3DXVECTOR3 &v2,
		D3DCOLOR color);
	// ����������б��
	static float CalcSlope(D3DXVECTOR3 start,D3DXVECTOR3 end);
	// ���ֵ�� 0-1
	static float UnitRandom();
	// ���ֵ�� fLow-fHigh
	static float RangeRandom(float fLow, float fHigh);
	// ���ֵ�� (-1) - (1)
	static float SymmetricRandom();
};
