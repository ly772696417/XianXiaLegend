/*
*	����������
*/
#pragma once
#include <vector>
#include "ZQuadTree.h"
#include <fstream>
#include "Heightmap.h"
#include "Common.h"

class CTerrain
{
private:
	IDirect3DDevice9*        m_pDevice;
	IDirect3DTexture9*       m_pTexture;
	IDirect3DVertexBuffer9*  m_pVB;// ���㻺����
	IDirect3DIndexBuffer9*   m_pIB;// ����������
	//DWORD*                   m_pOrigIndex/*[98304]*/;// ����ָ��
	IDirect3DTexture9*      mLayerMap0;
	IDirect3DTexture9*      mLayerMap1;
	IDirect3DTexture9*      mLayerMap2;
	IDirect3DTexture9*      mBlendMap;
	ID3DXEffect*            mTerrainFX;
	D3DXHANDLE              mhTech;
	D3DXHANDLE              mhWVP;
	D3DXHANDLE              mhTexScale;
	D3DXHANDLE              mhDirToSunW;
	D3DXHANDLE              mhLayerMap0;
	D3DXHANDLE              mhLayerMap1;
	D3DXHANDLE              mhLayerMap2;
	D3DXHANDLE              mhBlendMap;

	IDirect3DVertexDeclaration9* m_pDECL;

	//CHeightmap m_pHeightmap;

	ZFrustum*                m_pZFrustum;// ��̨��
	float                    m_fLODRatio;

	int                      m_iRow;// ��������
	int                      m_iCol;// ��������
	int                      m_iCellSpace;// ��Ԫ�����
	int                      m_iHeightScale;// �߶�����ϵ��

	int                      m_iCellRow;// ��Ԫ������
	int                      m_iCellCol;// ��Ԫ������
	int                      m_iWidth;// �ܿ��
	int                      m_iDepth;// �����
	int                      m_iVertexNum;// �������
	int                      m_iTriangle;// �����θ���
	int                      m_iMaxTri;// ��������θ���

	ZQuadTree*               m_pTreeRoot;// LOD�����ڵ�

	std::vector<BYTE>        m_vHeightData;
	std::vector<D3DXVECTOR3> m_vNormalData;

	ID3DXFont*               m_pFont;

	//struct LandVertex
	//{
	//	float x,y,z;	
	//	//DWORD iColor;
	//	//float nx,ny,nz;
	//	float u,v;
	//	static const DWORD FVF;
	//};

public:
	CTerrain(IDirect3DDevice9* pDevice);
	~CTerrain(void);

	bool Init(char* TerrainFile,int ID,ZFrustum* pZFrustum);
	bool LoadTexture(char* SceneFile);
	void Update(/*D3DXVECTOR3 vPos = D3DXVECTOR3(0,0,0),
				 D3DXVECTOR3 vRot = D3DXVECTOR3(0,0,0),
				 D3DXVECTOR3 vScal = D3DXVECTOR3(0,0,0)*/);
	void Render();

	// �����������ײ
	static bool IntersectTerrain(
		IDirect3DVertexBuffer9* pVB,
		IDirect3DIndexBuffer9* pIB,
		const Ray& ray,
		const int iNumFaces,
		float* fDist,
		D3DXVECTOR3* VRet);

	IDirect3DVertexBuffer9* GetVertexBuffer() {return m_pVB;}
	IDirect3DIndexBuffer9* GetIndexBuffer() {return m_pIB;}
	float GetHeight(float x,float z);
	int GetCellSpace() {return m_iCellSpace;}
	int GetTri() {return m_iTriangle;}
	int GetMaxTri() {return m_iMaxTri;}
	// ���ԭʼ����������
	//DWORD* GetOrigIndex() {return m_pOrigIndex;}
	int GetTerrainWidth() {return m_iWidth;}
	int GetTerrainDepth() {return m_iDepth;}

private:
	bool LoadHeightMap(char* SceneFile);// ��ȡ�߶�ͼ
	bool ComputeVertex();// ���㶥��
	bool ComputeIndex();// ����������
	void BuildTextures();
	void BuildEffect();

	void CalcNormalMap();
	D3DXVECTOR3 CalcNormal(int i);
	D3DXVECTOR3 QuadNormal(int i);
};
