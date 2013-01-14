/*
*	怪物类
*/
#pragma once
#include "myobject.h"
#include "meshmanager.h"
#include "animinstance.h"
#include "terrain.h"
#include "deck.h"
class CMajor;

class CMonster : public CMyObject
{
private:
	IDirect3DDevice9*        m_pDevice;
	CMeshManager*            m_pMeshManager;
	CAnimInstance*           m_pAI;                     // 动画实例
	CTerrain*                m_pTerrain;
	map<int,CDeck*>*         m_mapMeshObj;
	CMajor*                  m_pMajor;

	D3DXVECTOR3              m_vOrigRot;                // 不规则模型正方向,转换到负Z方向的旋转角
	float                    m_fMoveSpeed;              // 移动速度
	D3DXVECTOR3              m_vDir;                    // 移动方向
	D3DXVECTOR3              m_vDestPos;                // 移动目标
	ACTIONSTATE              m_State;                   // 行动状态
	int                      m_iSkillNum;               // 技能数量

	int                      m_iWatchDist;              // 视线距离,触发追击
	int                      m_iAttackDist;             // 攻击距离

	DOUBLE                   m_dElapsedTime;
	DOUBLE                   m_dTimeLapse;              // 触发事件的动画时间延时
	DOUBLE                   m_dEndTime;                // 计数结束时间
	DOUBLE                   m_dTimeCount;              // 时间计数

public:
	CMonster(IDirect3DDevice9* pDevice,CMeshManager* pMeshManager);
	~CMonster(void);

	bool Init(char* SceneFile,char* AddName) {return true;}
	bool Init(CTerrain* pTerrain,
				char* SceneFile,
				char* AddName,
				map<int,CDeck*>* MeshObj,
				CMajor* pMajor);
	void Update() {};
	void Update(DOUBLE dElapsedTime);
	void Render(D3DXMATRIX* matWorld);
	bool LoadDataFromFile(char* SceneFile,char* AddName);

	void SetState(ACTIONSTATE value) {m_State = value;}
	void SetDir(D3DXVECTOR3 vDir) {m_vDir = vDir;}
	void SetDestPos(D3DXVECTOR3 vDestPos) {m_vDestPos = vDestPos;}
	// 设定要计数的时限
	void SetEndTime(DOUBLE dTime) {m_dEndTime = dTime;}
	// 设定动画延时
	void SetTimeLapse(DOUBLE dTime) {m_dTimeLapse = dTime;}

	// 碰撞检测
	bool CollisionTest(D3DXVECTOR3 vPos);
};
