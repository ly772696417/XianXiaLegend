#pragma once
#include "myobject.h"
#include "meshmanager.h"

class CDeck : public CMyObject
{
private:
	IDirect3DDevice9* m_pDevice;
	CMeshManager* m_pMeshManager;

public:
	CDeck(IDirect3DDevice9* pDevice,CMeshManager* pMeshManager);
	~CDeck(void);

	bool Init(char* SceneFile,char* AddName);
	void Update();
	void Render(D3DXMATRIX* matWorld);
	bool LoadDataFromFile(char* SceneFile,char* AddName);
};
