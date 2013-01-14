/*
*	ƽ��̨����
*/
#pragma once
#include "Common.h"

class ZFrustum
{
private:
	D3DXVECTOR3 m_vtx[8];             // ƽ��̨��8������
	D3DXVECTOR3 m_vPos;               // ��ǰ���������������
	D3DXPLANE m_plane[6];             // ƽ��̨��6����
	float m_fEpsilon;                 // ��ײ�ж�����ֵ,ʹ��ײ�߽���������

public:
	ZFrustum(void);
	~ZFrustum(void);

	// ���������ͶӰ����,�õ�6��ƽ��
	bool Make(D3DXMATRIX* pmatViewProj,float fEpsilon);
	// ��v��ƽ��̨����,����TRUE,����ƽ��̨����,����FALSE
	bool IsIn(D3DXVECTOR3* pv);
	// ��������Բ��(v)�Ͱ뾶(radius)�ı߽�����(bounding sphere)��ƽ��̨����,����TRUE,����ƽ��̨����,����FALSE
	bool IsInSphere(D3DXVECTOR3* pv,float radius);
	// �õ���ǰ���������������
	D3DXVECTOR3* GetPos(){return &m_vPos;}
};
