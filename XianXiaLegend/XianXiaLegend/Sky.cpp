#include "Sky.h"

Sky::Sky(const std::string& envmapFilename, float skyRadius,LPDIRECT3DDEVICE9 _pDirect3DDevice)
: mRadius(skyRadius),m_pDirect3DDevice(_pDirect3DDevice)
{
	D3DXCreateSphere(m_pDirect3DDevice, skyRadius, 30, 30, &mSphere, 0);
	D3DXCreateCubeTextureFromFile(m_pDirect3DDevice, envmapFilename.c_str(), &mEnvMap);

	ID3DXBuffer* errors = 0;
	D3DXCreateEffectFromFile(m_pDirect3DDevice, "FXFolder\\sky.fx", 0, 0, 0, 
		0, &mFX, &errors);
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	mhTech   = mFX->GetTechniqueByName("SkyTech");
	mhWVP    = mFX->GetParameterByName(0, "gWVP");
	mhEnvMap = mFX->GetParameterByName(0, "gEnvMap");

	// Set effect parameters that do not vary.
	mFX->SetTechnique(mhTech);
	mFX->SetTexture(mhEnvMap, mEnvMap);
}

Sky::~Sky()
{
	SAFE_RELEASE(mSphere);
	SAFE_RELEASE(mEnvMap);
	SAFE_RELEASE(mFX);
}

DWORD Sky::getNumTriangles()
{
	return mSphere->GetNumFaces();
}

DWORD Sky::getNumVertices()
{
	return mSphere->GetNumVertices();
}

IDirect3DCubeTexture9* Sky::getEnvMap()
{
	return mEnvMap;
}

float Sky::getRadius()
{
	return mRadius;
}

void Sky::onLostDevice()
{
	mFX->OnLostDevice();
}

void Sky::onResetDevice()
{
	mFX->OnResetDevice();
}

void Sky::draw()
{
	// Sky always centered about camera's position.
	D3DXMATRIX W;

	//D3DXVECTOR3 p = gCamera->pos();

	D3DXVECTOR3 p = D3DXVECTOR3(0,30,0);
	D3DXMATRIX projMat;
	m_pDirect3DDevice->GetTransform(D3DTS_PROJECTION,&projMat);
	D3DXMATRIX viewMat;
	m_pDirect3DDevice->GetTransform(D3DTS_VIEW,&viewMat);

	D3DXMATRIX matrix = viewMat * projMat;
	D3DXMatrixTranslation(&W, p.x, p.y, p.z);
	//mFX->SetMatrix(mhWVP, &(W*gCamera->viewProj()));
	mFX->SetMatrix(mhWVP, &(W*matrix));
	
	UINT numPasses = 0;
	mFX->Begin(&numPasses, 0);
	mFX->BeginPass(0);
	mSphere->DrawSubset(0);
	mFX->EndPass();
	mFX->End();
}