/*
*	������
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
	CAnimInstance*      m_pAI;           // ����ʵ��
	CTerrain*           m_pTerrain;
	map<int,CDeck*>*    m_mapMeshObj;
	map<int,CMonster*>* m_mapSkinObj;

	D3DXVECTOR3         m_vOrigRot;      // ������ģ��������,ת������Z�������ת��
	float               m_fMoveSpeed;    // �ƶ��ٶ�(��)
	D3DXVECTOR3         m_vDir;          // �ƶ�����
	D3DXVECTOR3         m_vDestPos;      // Ŀ��λ��
	bool                m_bKeyLock;      // ������,true����Ӧ����,false��Ӧ����
	ACTIONSTATE         m_State;         // �ж�״̬
	int                 m_iSkill;        // ��ǰʹ�õļ���
	DOUBLE              m_dElapsedTime;  // ÿ֡ʱ��
	DOUBLE              m_dEndTime;      // ��������ʱ��
	DOUBLE              m_dTimeCount;    // ʱ�������

	bool	            m_bGra;          // ��������
	float	            m_fGravity;      // �������ٶ�
	float               m_fSpeed;        // �����ٶ�
	
	bool                m_bPEnable;      // ����������Ⱦ����

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
	// ��ײ���
	bool CollisionTest(D3DXVECTOR3 vPos);
	// �������Ǳ�����
	void SetBruise();
	void SetMoveSpeed(float Speed) {m_fMoveSpeed = Speed;}

	int GetSkill() {return m_iSkill;}
	ACTIONSTATE GetState() {return m_State;}
	// �����Ƿ�Ҫ��Ⱦ
	bool IsPEnable() {return m_bPEnable;}
};

