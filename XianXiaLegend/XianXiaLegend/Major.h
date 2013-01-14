/*
*	主角类
*/
#pragma once
#include "Input.h"
#include "Myobject.h"
#include "Meshmanager.h"
#include "Animinstance.h"
#include "Terrain.h"
#include "Deck.h"
#include "Particle.h"
class CMonster;

class CMajor : public CMyObject
{
private:
	IDirect3DDevice9*   m_pDevice;
	CMeshManager*       m_pMeshManager;
	CInput*             m_pInput;
	CAnimInstance*      m_pAI;           // 动画实例
	CTerrain*           m_pTerrain;
	map<int,CDeck*>*    m_mapMeshObj;
	map<int,CMonster*>* m_mapSkinObj;

	D3DXVECTOR3         m_vOrigRot;      // 不规则模型正方向,转换到负Z方向的旋转角
	float               m_fMoveSpeed;    // 移动速度(秒)
	D3DXVECTOR3         m_vDir;          // 移动方向
	D3DXVECTOR3         m_vDestPos;      // 目标位置
	bool                m_bKeyLock;      // 按键锁,true不响应按键,false响应按键
	ACTIONSTATE         m_State;         // 行动状态
	int                 m_iSkill;        // 当前使用的技能
	DOUBLE              m_dElapsedTime;  // 每帧时间
	DOUBLE              m_dEndTime;      // 计数结束时间
	DOUBLE              m_dTimeCount;    // 时间计数器

	bool	            m_bGra;          // 重力开关
	float	            m_fGravity;      // 重力加速度
	float               m_fSpeed;        // 下落速度
	
	bool                m_bPEnable;      // 技能粒子渲染开关

public:
	CMajor(IDirect3DDevice9* pDevice,CMeshManager* pMeshManager,CInput* pInput);
	~CMajor(void);

	bool Init(char* SceneFile,char* AddName) {return true;}
	bool Init(
		CTerrain* pTerrain,
		char* SceneFile,
		char* AddName,
		map<int,CDeck*>* MeshObj,
		map<int,CMonster*>* SkinObj);
	void Update() {};
	void Update(DOUBLE dElapsedTime,D3DXVECTOR3 vDir);
	void Render(D3DXMATRIX* matWorld);
	bool LoadDataFromFile(char* SceneFile,char* AddName);
	// 碰撞检测
	bool CollisionTest(D3DXVECTOR3 vPos);
	// 触发主角被击中
	void SetBruise();
	void SetMoveSpeed(float Speed) {m_fMoveSpeed = Speed;}

	int GetSkill() {return m_iSkill;}
	ACTIONSTATE GetState() {return m_State;}
	// 粒子是否要渲染
	bool IsPEnable() {return m_bPEnable;}
};

