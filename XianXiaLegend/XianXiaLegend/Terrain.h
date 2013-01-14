/*
*	地形生成类
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
	IDirect3DVertexBuffer9*  m_pVB;// 顶点缓冲区
	IDirect3DIndexBuffer9*   m_pIB;// 索引缓冲区
	//DWORD*                   m_pOrigIndex/*[98304]*/;// 索引指针
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

	ZFrustum*                m_pZFrustum;// 截台体
	float                    m_fLODRatio;

	int                      m_iRow;// 顶点行数
	int                      m_iCol;// 顶点列数
	int                      m_iCellSpace;// 单元格距离
	int                      m_iHeightScale;// 高度缩放系数

	int                      m_iCellRow;// 单元格行数
	int                      m_iCellCol;// 单元格列数
	int                      m_iWidth;// 总宽度
	int                      m_iDepth;// 总深度
	int                      m_iVertexNum;// 顶点个数
	int                      m_iTriangle;// 三角形个数
	int                      m_iMaxTri;// 最大三角形个数

	ZQuadTree*               m_pTreeRoot;// LOD树根节点

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

	// 射线与地形碰撞
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
	// 获得原始地形索引表
	//DWORD* GetOrigIndex() {return m_pOrigIndex;}
	int GetTerrainWidth() {return m_iWidth;}
	int GetTerrainDepth() {return m_iDepth;}

private:
	bool LoadHeightMap(char* SceneFile);// 读取高度图
	bool ComputeVertex();// 计算顶点
	bool ComputeIndex();// 计算索引点
	void BuildTextures();
	void BuildEffect();

	void CalcNormalMap();
	D3DXVECTOR3 CalcNormal(int i);
	D3DXVECTOR3 QuadNormal(int i);
};
