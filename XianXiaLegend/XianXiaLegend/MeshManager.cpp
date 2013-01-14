#include "MeshManager.h"

CMeshManager::CMeshManager(IDirect3DDevice9* pDevice)
{
	m_pDevice = pDevice;
	m_iMeshNum = 0;
	m_iSkinMeshNum = 0;
}

CMeshManager::~CMeshManager(void)
{
	for (int i=0;i<m_iMeshNum;i++)
	{
		delete m_vecAllMesh[i];
	}
	m_vecAllMesh.clear();

	for (int i=0;i<m_iSkinMeshNum;i++)
	{
		delete m_vecAllSkinMesh[i];
	}
	m_vecAllSkinMesh.clear();
}

bool CMeshManager::LoadMeshFile(char* MeshFile)
{
	m_iMeshNum = GetPrivateProfileInt("head","MeshNum",0,MeshFile);
	m_iSkinMeshNum = GetPrivateProfileInt("head","SkinMeshNum",0,MeshFile);
	//m_iBillBoardNum = GetPrivateProfileInt("head","BillBoardNum",0,SceneFile);
	GetPrivateProfileString("head","TexturePath","ERROR",m_strTexture,sizeof(m_strTexture),MeshFile);
	char tmpAddName[32];
	char strMeshPath[256];

	if (m_iMeshNum)
	{
		for (int i=0;i<m_iMeshNum;i++)
		{
			sprintf(tmpAddName,"meshid_%d",i);
			GetPrivateProfileString(tmpAddName,"strPath","ERROR",strMeshPath,sizeof(strMeshPath),MeshFile);
			CMesh* pNew = new CMesh(m_pDevice);
			pNew->LoadMesh(strMeshPath,m_strTexture);
			m_vecAllMesh.push_back(pNew);
		}
	}

	if (m_iSkinMeshNum)
	{
		for (int i=0;i<m_iSkinMeshNum;i++)
		{
			sprintf(tmpAddName,"skinmeshid_%d",i);
			GetPrivateProfileString(tmpAddName,"strPath","ERROR",strMeshPath,sizeof(strMeshPath),MeshFile);
			CSkinMesh* pNew = new CSkinMesh(m_pDevice);
			pNew->LoadMesh(strMeshPath,m_strTexture);
			m_vecAllSkinMesh.push_back(pNew);
		}
	}
	return true;
}
