/*
*	场景渲染管理
*/
#pragma once
#include "meshmanager.h"
#include "camera.h"
#include "input.h"
#include "deck.h"
#include "major.h"
#include "monster.h"
#include "Sky.h"
#include "terrain.h"
#include "myfuntion.h"
#include "billboard.h"
//#include "boardobj.h"
#include "particle.h"
//#include "particlesystem.h"
#include "Water.h"

class CScene
{
private:
	enum EObjectType{SKY,DECK,MAJOR,MONSTER,BILLBOARD};
	IDirect3DDevice9*   m_pDevice;
	CMeshManager*       m_pMeshManager;
	CCamera*            m_pCamera;
	CInput*             m_pInput;
	ZFrustum*           m_pZFrustum;        // 截台体

    int                 m_iSceneID;         // 场景ID
	int                 m_iMeshNum;         // 静态对象数量
	map<int,CDeck*>     m_mapMeshObj;       // 静态模型对象表
	int                 m_iSkinNum;         // 动态对象数量
	map<int,CMonster*>  m_mapSkinObj;       // 动态模型对象表
	vector<CBillBoard*> m_vecBoardObj;      // 公告板对象表
	CTerrain*           m_pTerrain;         // 地形
	Water *m_pWater; //水波
	DirLight mLight;
	Sky *m_pSkyBox;
	CMajor*             m_pMajor;

	map<int,CParticleSystem*> m_mapParticle;
	//CParticleSystem*    m_pParticleFire;
	//CParticleSystem*    m_pParticleFire2;
	//CParticleSystem*    m_pParticleWind;

	DOUBLE              m_dElapsedTime;     // 每帧时间
	int                 m_iFPS;             // 帧频率
	bool                m_bHelp;            // 是否打开帮助　
	ID3DXFont*          m_pFont;

	D3DXVECTOR3         m_CameraPos;        // 摄像机碰撞时修正位置

	bool                m_bEditor;          // 场景编辑器开关

	D3DXVECTOR3  m_tmpPoint[3];
	int          m_ID;

public:
	CScene(IDirect3DDevice9* pDevice,CMeshManager* pMeshManager,CCamera* pCamera,CInput* pInput);
	~CScene(void);

	bool Init(char* SceneFile);
	void Update(DOUBLE dElapsedTime,int iFPS);
	void Render();

private:
	bool LoadSceneFile(char* SceneFile);
	bool LoadBillBoardFile(char* BillBoardFile);
	// 摄像机碰撞
	bool CollisionCamera(D3DXVECTOR3 &vPos);
};
