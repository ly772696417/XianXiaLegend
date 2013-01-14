#pragma once
#include "Common.h"
#include <d3dx9math.h>
#include "Input.h"

class CCamera
{
private:
	IDirect3DDevice9*  m_pDevice;
	CInput*            m_pInput;
	D3DXVECTOR3        m_vPos;
	D3DXVECTOR3        m_vLookAt;
	D3DXVECTOR3        m_vUp;
	float              m_fNear;                // ���ý���
	float              m_fFar;                 // Զ�ý���
	float              m_fFOV;                 // �нǣ����ȣ�
	float              m_fAspect;              // �ݺ��
	float              m_fYaw;                 // ƫ�ƽǣ����ȣ�
	float              m_fPitch;               // ���ǣ����ȣ�
	float              m_fDistance;            // �۾��㵽�۲��ľ���
	float              m_fMaxDistance;
	float              m_fMinDistance;
	bool               m_bMode;                // true=β���ӽǣ�false=�����˳��ӽ�
	RECT               m_winRect;              // ���ڴ�С

public:
	CCamera(IDirect3DDevice9* pDevice,CInput* pInput);
	~CCamera(void);
	friend class CFrantoGE;

	// ���������
	void SetCamera();
	void SetLookAt(D3DXVECTOR3 vLookAt) {m_vLookAt = vLookAt;CalcEyePos();}
	void SetPos(D3DXVECTOR3 vPos) {m_vPos = vPos;}
	void SetFar(float fFar){m_fFar = fFar;}
	// �������������
	void SetDistance(float fDist){m_fDistance = fDist;}
	// ����������������
	void SetMaxDistance(float fMaxDist){m_fMaxDistance = fMaxDist;}
	void SetMinDistance(float fMinDist){m_fMinDistance = fMinDist;}
	void SetMode(bool bMode){m_bMode = bMode;}

	D3DXVECTOR3	GetPos() {return m_vPos;}
	D3DXVECTOR3	GetLookAt() {return m_vLookAt;}
	// ��������ƫ�ƽǣ����ȣ�
	//float GetYaw() {return m_fYaw;}
	float GetDistance() {return m_fDistance;}
	bool GetMode() {return m_bMode;}
	D3DXVECTOR3 GetUp() {return m_vUp;}

	// �ı�������Ƕ�
	void ChangeCamera(float fYaw,float fPitch);
	// �ı�۲����
	void ChangeDistance(float fDistance);
	// �ı������ģʽ
	void ChangeMode();
	void Cortrol();

private:
	// �����۾���
	void CalcEyePos();
};
