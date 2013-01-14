/*
*	��Դ�����࣬���𱣴�Ͷ�ȡ����ģ����Դ.
*/
#pragma once
#include "common.h"
#include "mesh.h"
#include "skinmesh.h"
#include "billboard.h"
//using namespace std;

class CMeshManager
{
private:
	IDirect3DDevice9*    m_pDevice;
	char                 m_strTexture[128];
	int                  m_iMeshNum;                 // ��̬ģ������
	vector<CMesh*>       m_vecAllMesh;               // װ�ؾ�̬ģ������
	int                  m_iSkinMeshNum;             // ��̬ģ������
	vector<CSkinMesh*>   m_vecAllSkinMesh;           // װ�ض�̬ģ������

public:
	CMeshManager(IDirect3DDevice9* pDevice);
	~CMeshManager(void);

	// ͨ�������ļ�����ģ��
	bool LoadMeshFile(char* MeshFile);
	// ͨ��ID��ȡ��̬ģ��
	CMesh* GetMeshFromID(int ID) {return m_vecAllMesh[ID];}
	// ͨ��ID��ȡ��̬ģ��
	CSkinMesh* GetSkinMeshFromID(int ID) {return m_vecAllSkinMesh[ID];}
};
