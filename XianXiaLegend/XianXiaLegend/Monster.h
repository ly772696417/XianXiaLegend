/*
*	������
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
	CAnimInstance*           m_pAI;                     // ����ʵ��
	CTerrain*                m_pTerrain;
	map<int,CDeck*>*         m_mapMeshObj;
	CMajor*                  m_pMajor;

	D3DXVECTOR3              m_vOrigRot;                // ������ģ��������,ת������Z�������ת��
	float                    m_fMoveSpeed;              // �ƶ��ٶ�
	D3DXVECTOR3              m_vDir;                    // �ƶ�����
	D3DXVECTOR3              m_vDestPos;                // �ƶ�Ŀ��
	ACTIONSTATE              m_State;                   // �ж�״̬
	int                      m_iSkillNum;               // ��������

	int                      m_iWatchDist;              // ���߾���,����׷��
	int                      m_iAttackDist;             // ��������

	DOUBLE                   m_dElapsedTime;
	DOUBLE                   m_dTimeLapse;              // �����¼��Ķ���ʱ����ʱ
	DOUBLE                   m_dEndTime;                // ��������ʱ��
	DOUBLE                   m_dTimeCount;              // ʱ�����

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
	// �趨Ҫ������ʱ��
	void SetEndTime(DOUBLE dTime) {m_dEndTime = dTime;}
	// �趨������ʱ
	void SetTimeLapse(DOUBLE dTime) {m_dTimeLapse = dTime;}

	// ��ײ���
	bool CollisionTest(D3DXVECTOR3 vPos);
};
