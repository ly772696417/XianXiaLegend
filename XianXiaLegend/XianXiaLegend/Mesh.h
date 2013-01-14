/*
*	静态模型类
*/
#pragma once

#include "common.h"

class CMesh
{
private:
	IDirect3DDevice9* m_pDevice;

	ID3DXBuffer* pD3DXMtrlBuffer;           // 存储材质的内存
	DWORD m_dwNumMaterials;                 // 子集数
	ID3DXMesh* m_pMesh;                     // 网格

	D3DMATERIAL9* m_pMeshMaterials;         // 材质
	IDirect3DTexture9** m_pMeshTextures;    // 纹理

	/*ID3DXMesh* m_pBoundingBox;*/

public:
	CMesh(IDirect3DDevice9* pDevice);
	~CMesh(void);

	HRESULT LoadMesh(char* strMeshPath,char* strTexturePath);
	void Render();
	ID3DXMesh* GetMesh(){return m_pMesh;}
	// 获取碰撞盒角值与碰撞球半径
	bool GetBoundingB_S(D3DXVECTOR3* pMax,D3DXVECTOR3* pMin,D3DXVECTOR3* pCenter,float* pRadius);
};
