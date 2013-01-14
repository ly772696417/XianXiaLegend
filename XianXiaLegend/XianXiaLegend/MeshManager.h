/*
*	资源管理类，负责保存和读取所有模型资源.
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
	int                  m_iMeshNum;                 // 静态模型数量
	vector<CMesh*>       m_vecAllMesh;               // 装载静态模型容器
	int                  m_iSkinMeshNum;             // 动态模型数量
	vector<CSkinMesh*>   m_vecAllSkinMesh;           // 装载动态模型容器

public:
	CMeshManager(IDirect3DDevice9* pDevice);
	~CMeshManager(void);

	// 通过配置文件读入模型
	bool LoadMeshFile(char* MeshFile);
	// 通过ID获取静态模型
	CMesh* GetMeshFromID(int ID) {return m_vecAllMesh[ID];}
	// 通过ID获取动态模型
	CSkinMesh* GetSkinMeshFromID(int ID) {return m_vecAllSkinMesh[ID];}
};
