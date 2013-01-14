#pragma once

#include "Common.h"
#include "Vertex.h"

class Water
{
public:
	struct InitInfo
	{
		DirLight dirLight;
		Mtrl     mtrl;
		int      vertRows;
		int      vertCols;
		float    dx;
		float    dz;
		std::string waveMapFilename0;
		std::string waveMapFilename1;
		D3DXVECTOR2 waveMapVelocity0;
		D3DXVECTOR2 waveMapVelocity1;
		float texScale;
		D3DXMATRIX toWorld;
	};

public:
	Water(InitInfo& initInfo,LPDIRECT3DDEVICE9 _pDirext3DDevice9);

	~Water();

	DWORD getNumTriangles();
	DWORD getNumVertices();

	void onLostDevice();
	void onResetDevice();

	void update(float dt);
	void draw(D3DXVECTOR3 _cameraPos);

	void setEnvMap(IDirect3DCubeTexture9* envMap);

	void GenTriGrid(int numVertRows, int numVertCols,
		float dx, float dz, 
		const D3DXVECTOR3& center, 
		std::vector<D3DXVECTOR3>& verts,
		std::vector<DWORD>& indices);

private:
	void buildFX();

private:
	ID3DXMesh* mMesh;
	ID3DXEffect* mFX;

	// The two normal maps to scroll.
	IDirect3DTexture9* mWaveMap0;
	IDirect3DTexture9* mWaveMap1;

	// Offset of normal maps for scrolling (vary as a function of time)
	D3DXVECTOR2 mWaveMapOffset0;
	D3DXVECTOR2 mWaveMapOffset1;

	InitInfo mInitInfo;
	float mWidth;
	float mDepth;

	D3DXHANDLE mhTech;
	D3DXHANDLE mhWVP;
	D3DXHANDLE mhWorld;
	D3DXHANDLE mhWorldInv;
	D3DXHANDLE mhLight;
	D3DXHANDLE mhMtrl;
	D3DXHANDLE mhEyePosW;
	D3DXHANDLE mhWaveMap0;
	D3DXHANDLE mhWaveMap1;
	D3DXHANDLE mhWaveMapOffset0;
	D3DXHANDLE mhWaveMapOffset1;
	D3DXHANDLE mhEnvMap;

	LPDIRECT3DDEVICE9 m_pDirext3DDevice9;
	IDirect3DVertexDeclaration9* m_pDECL;

	D3DXVECTOR3 m_cameraPos;
};
