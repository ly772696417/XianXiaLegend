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
	float              m_fNear;                // 近裁截面
	float              m_fFar;                 // 远裁截面
	float              m_fFOV;                 // 夹角（弧度）
	float              m_fAspect;              // 纵横比
	float              m_fYaw;                 // 偏移角（弧度）
	float              m_fPitch;               // 仰角（弧度）
	float              m_fDistance;            // 眼睛点到观察点的距离
	float              m_fMaxDistance;
	float              m_fMinDistance;
	bool               m_bMode;                // true=尾随视角，false=第三人称视角
	RECT               m_winRect;              // 窗口大小

public:
	CCamera(IDirect3DDevice9* pDevice,CInput* pInput);
	~CCamera(void);
	friend class CFrantoGE;

	// 设置摄像机
	void SetCamera();
	void SetLookAt(D3DXVECTOR3 vLookAt) {m_vLookAt = vLookAt;CalcEyePos();}
	void SetPos(D3DXVECTOR3 vPos) {m_vPos = vPos;}
	void SetFar(float fFar){m_fFar = fFar;}
	// 设置摄像机距离
	void SetDistance(float fDist){m_fDistance = fDist;}
	// 设置最大摄像机距离
	void SetMaxDistance(float fMaxDist){m_fMaxDistance = fMaxDist;}
	void SetMinDistance(float fMinDist){m_fMinDistance = fMinDist;}
	void SetMode(bool bMode){m_bMode = bMode;}

	D3DXVECTOR3	GetPos() {return m_vPos;}
	D3DXVECTOR3	GetLookAt() {return m_vLookAt;}
	// 获得摄像机偏移角（弧度）
	//float GetYaw() {return m_fYaw;}
	float GetDistance() {return m_fDistance;}
	bool GetMode() {return m_bMode;}
	D3DXVECTOR3 GetUp() {return m_vUp;}

	// 改变摄像机角度
	void ChangeCamera(float fYaw,float fPitch);
	// 改变观察距离
	void ChangeDistance(float fDistance);
	// 改变摄像机模式
	void ChangeMode();
	void Cortrol();

private:
	// 计算眼睛点
	void CalcEyePos();
};
