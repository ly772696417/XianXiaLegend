#include "Myobject.h"

CMyObject::CMyObject(void)
{
	D3DXMatrixIdentity(&m_matWorld);
	m_vPos    = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vRot    = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vScal   = D3DXVECTOR3(1.0f,1.0f,1.0f);
	m_Type    = 0;
	m_iMeshID = 0;
	m_iID     = 0;
	m_vOrigMax = D3DXVECTOR3(0,0,0);
	m_vOrigMin = D3DXVECTOR3(0,0,0);
	m_vMax    = D3DXVECTOR3(0,0,0);
	m_vMin    = D3DXVECTOR3(0,0,0);
	m_vCenter = D3DXVECTOR3(0,0,0);
	m_fRadius = 0.0f; 
}

CMyObject::~CMyObject(void)
{
}

void CMyObject::Transform()
{
	D3DXMATRIX matTrans,matScal,matRot;
	D3DXMatrixTranslation(&matTrans,m_vPos.x,m_vPos.y,m_vPos.z);
	D3DXMatrixRotationYawPitchRoll(&matRot,m_vRot.y,m_vRot.x,m_vRot.z);// 欧拉角旋转变化
	D3DXMatrixScaling(&matScal,m_vScal.x,m_vScal.y,m_vScal.z);
	m_matWorld = matScal * matRot * matTrans;

	D3DXMATRIX matVer;
	matVer = matScal * matTrans;
	D3DXVec3TransformCoord(&m_vMax,&m_vOrigMax,&matVer);
	D3DXVec3TransformCoord(&m_vMin,&m_vOrigMin,&matVer);
}

bool CMyObject::LoadDataFromFile(char* SceneFile,char* AddName)
{
	m_Type = GetPrivateProfileInt(AddName,"Type",-1,SceneFile);
	m_iMeshID = GetPrivateProfileInt(AddName,"iMeshID",-1,SceneFile);
	char strData[256];
	GetPrivateProfileString(AddName,"vPos","ERROR",strData,sizeof(strData),SceneFile);
	m_vPos = CMyFuntion::GetVec3FromStr(strData);
	GetPrivateProfileString(AddName,"vRot","ERROR",strData,sizeof(strData),SceneFile);
	m_vRot = CMyFuntion::GetVec3FromStr(strData);
	GetPrivateProfileString(AddName,"vSca","ERROR",strData,sizeof(strData),SceneFile);
	m_vScal = CMyFuntion::GetVec3FromStr(strData);
	return true;
}