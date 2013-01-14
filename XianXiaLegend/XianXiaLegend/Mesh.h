/*
*	��̬ģ����
*/
#pragma once

#include "common.h"

class CMesh
{
private:
	IDirect3DDevice9* m_pDevice;

	ID3DXBuffer* pD3DXMtrlBuffer;           // �洢���ʵ��ڴ�
	DWORD m_dwNumMaterials;                 // �Ӽ���
	ID3DXMesh* m_pMesh;                     // ����

	D3DMATERIAL9* m_pMeshMaterials;         // ����
	IDirect3DTexture9** m_pMeshTextures;    // ����

	/*ID3DXMesh* m_pBoundingBox;*/

public:
	CMesh(IDirect3DDevice9* pDevice);
	~CMesh(void);

	HRESULT LoadMesh(char* strMeshPath,char* strTexturePath);
	void Render();
	ID3DXMesh* GetMesh(){return m_pMesh;}
	// ��ȡ��ײ�н�ֵ����ײ��뾶
	bool GetBoundingB_S(D3DXVECTOR3* pMax,D3DXVECTOR3* pMin,D3DXVECTOR3* pCenter,float* pRadius);
};
