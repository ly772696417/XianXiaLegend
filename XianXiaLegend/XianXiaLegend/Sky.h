#pragma  once

#include "Common.h"

class Sky
{
public:
	Sky(const std::string& envmapFilename, float skyRadius,LPDIRECT3DDEVICE9 _pDirect3DDevice);
	~Sky();

	IDirect3DCubeTexture9* getEnvMap();
	float getRadius();

	DWORD getNumTriangles();
	DWORD getNumVertices();

	void onLostDevice();
	void onResetDevice();

	void draw();

private:
	LPDIRECT3DDEVICE9 m_pDirect3DDevice;

	ID3DXMesh* mSphere;
	float mRadius;
	IDirect3DCubeTexture9* mEnvMap;
	ID3DXEffect* mFX;
	D3DXHANDLE mhTech;
	D3DXHANDLE mhEnvMap;
	D3DXHANDLE mhWVP;
};
