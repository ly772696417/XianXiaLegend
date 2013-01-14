#pragma once
#include "skinmesh.h"

class CAnimInstance
{
private:
	ID3DXAnimationController*   m_pAC;                    // 动画控制器
	DWORD                       m_dwCurrentTrack;         // 当前动画集
	char                        m_strCurrentAnim[32];     // 当前动画名称
	char                        m_strNextAnim[32];        // 下一个动画名称
	DWORD                       m_dwAnimIndex;            // 动画索引
	DOUBLE                      m_dTimeMax;               // 最大动画时间
	DOUBLE                      m_dTimeCurrent;           // 当前动画时间
	DOUBLE						m_timeDelta;              // 每次动画推进时间
	bool						m_bMoving;                // 动画播放开关

public:
	CAnimInstance(void);
	~CAnimInstance(void);

	void Init(CSkinMesh* pSkinMesh);
	void Render();
	// 动画切换
	void ChangeTrack(bool bResetPosition);
	// 设置动画集索引
	void SetAnimIndex(DWORD Index) {m_dwAnimIndex = Index;}
	// 设置动画集名称
	void SetAnimName(char* strName);
	// 设置动画
	void SetAnimTrack();
	// 设置动画推进时间
	void SetTimeDelta(float fTimeDelta) {m_timeDelta = fTimeDelta;}
	void SetMoving(bool bMoving) {m_bMoving = bMoving;}
	void SetTimeMax(double dTimeMax) {m_dTimeMax = dTimeMax;}

	char* GetAnimName() {return m_strCurrentAnim;}
	DOUBLE GetTimeMax() {return m_dTimeMax;}
	DOUBLE GetTimeDelta() {return m_timeDelta;}
	bool GetMoving() {return m_bMoving;}
};
