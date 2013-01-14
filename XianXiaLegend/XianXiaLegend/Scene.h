/*
*	������Ⱦ����
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
	ZFrustum*           m_pZFrustum;        // ��̨��

    int                 m_iSceneID;         // ����ID
	int                 m_iMeshNum;         // ��̬��������
	map<int,CDeck*>     m_mapMeshObj;       // ��̬ģ�Ͷ����
	int                 m_iSkinNum;         // ��̬��������
	map<int,CMonster*>  m_mapSkinObj;       // ��̬ģ�Ͷ����
	vector<CBillBoard*> m_vecBoardObj;      // ���������
	CTerrain*           m_pTerrain;         // ����
	Water *m_pWater; //ˮ��
	DirLight mLight;
	Sky *m_pSkyBox;
	CMajor*             m_pMajor;

	map<int,CParticleSystem*> m_mapParticle;
	//CParticleSystem*    m_pParticleFire;
	//CParticleSystem*    m_pParticleFire2;
	//CParticleSystem*    m_pParticleWind;

	DOUBLE              m_dElapsedTime;     // ÿ֡ʱ��
	int                 m_iFPS;             // ֡Ƶ��
	bool                m_bHelp;            // �Ƿ�򿪰�����
	ID3DXFont*          m_pFont;

	D3DXVECTOR3         m_CameraPos;        // �������ײʱ����λ��

	bool                m_bEditor;          // �����༭������

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
	// �������ײ
	bool CollisionCamera(D3DXVECTOR3 &vPos);
};
