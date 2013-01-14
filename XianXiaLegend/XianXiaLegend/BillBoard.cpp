#include "BillBoard.h"

CBillBoard::CBillBoard(IDirect3DDevice9* pDevice)
{
	m_pDevice = pDevice;
	m_iScalU = 1;
	m_iScalV = 1;
	m_fScalX = 1.0f;
	m_fScalY = 1.0f;
	m_fPitch = 0.0f;
	m_vPos = D3DXVECTOR3(0,0,0);
	m_Mode = DEFAULT;
	m_dTime = 1;
	m_dTimeCount = 0;
	m_iTextureNum = 0;
}

CBillBoard::~CBillBoard(void)
{
	m_vecPos.clear();
	iter = m_vecTexture.begin();
	for (;iter != m_vecTexture.end();iter++)
	{
		SAFE_RELEASE(*iter);
	}
	m_vecTexture.clear();
}

bool CBillBoard::Init(char* BillBoardFile,char* AddName)
{
	LoadDataFromFile(BillBoardFile,AddName);
	BILL_VERT vert[] =
	{
		{-1.0f, 1.0f,0.0f,0x77ffffff,0.0f * m_iScalU,0.0f * m_iScalV},
		{ 1.0f, 1.0f,0.0f,0x77ffffff,1.0f * m_iScalU,0.0f * m_iScalV},
		{-1.0f,-1.0f,0.0f,0x77ffffff,0.0f * m_iScalU,1.0f * m_iScalV},
		{ 1.0f,-1.0f,0.0f,0x77ffffff,1.0f * m_iScalU,1.0f * m_iScalV},
	};				  
	// 公告板顶点	  
	if (FAILED(m_pDevice->CreateVertexBuffer(sizeof(vert),D3DUSAGE_WRITEONLY,BILL_FVF,
												D3DPOOL_DEFAULT,&m_pVB,0)))
	{
		return false;
	}
	void* pvert;
	m_pVB->Lock(0,sizeof(vert),(void**)&pvert,0);
	memcpy(pvert,vert,sizeof(vert));
	m_pVB->Unlock();
	// 读取公告板纹理
	LoadTexture(m_TexturePath,m_iTextureNum);
	iter = m_vecTexture.begin();
	return true;
}

bool CBillBoard::Init(char* TexturePath,int TextureNum)
{
	strcpy(m_TexturePath,TexturePath);
	m_iTextureNum = TextureNum;
	BILL_VERT vert[] =
	{
		{-1.0f, 1.0f,0.0f,0x77ffffff,0.0f * m_iScalU,0.0f * m_iScalV},
		{ 1.0f, 1.0f,0.0f,0x77ffffff,1.0f * m_iScalU,0.0f * m_iScalV},
		{-1.0f,-1.0f,0.0f,0x77ffffff,0.0f * m_iScalU,1.0f * m_iScalV},
		{ 1.0f,-1.0f,0.0f,0x77ffffff,1.0f * m_iScalU,1.0f * m_iScalV},
	};				  
	// 公告板顶点	  
	if (FAILED(m_pDevice->CreateVertexBuffer(sizeof(vert),D3DUSAGE_WRITEONLY,BILL_FVF,
												D3DPOOL_DEFAULT,&m_pVB,0)))
	{
		return false;
	}
	void* pvert;
	m_pVB->Lock(0,sizeof(vert),(void**)&pvert,0);
	memcpy(pvert,vert,sizeof(vert));
	m_pVB->Unlock();
	// 读取公告板纹理
	LoadTexture(m_TexturePath,m_iTextureNum);
	iter = m_vecTexture.begin();
	return true;
}

void CBillBoard::Render(DOUBLE dElapsedTime)
{
	vector<D3DXVECTOR3>::iterator it = m_vecPos.begin();
	for (;it != m_vecPos.end();it++)
	{
		m_vPos = (*it);
		SetBillMode(m_Mode);

		m_pDevice->SetStreamSource(0,m_pVB,0,sizeof(BILL_VERT));
		m_pDevice->SetFVF(BILL_FVF);

		m_pDevice->SetTexture(0,*iter);
		m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,D3DTADDRESS_MIRROR );
		m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,D3DTADDRESS_MIRROR );
		if (m_dTimeCount >= m_dTime)
		{
			iter++;
			m_dTimeCount -= m_dTime;
		}
		else
		{
			m_dTimeCount += dElapsedTime;
		}
		if (iter == m_vecTexture.end())
		{
			iter = m_vecTexture.begin();
		}
		m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
	}
}

bool CBillBoard::LoadDataFromFile(char* BillBoardFile,char* AddName)
{
	char tmpNum[32];
	char tmpData[256];
	char tmpKeyName[32];
	
	m_Mode = (BILLMODE)GetPrivateProfileInt(AddName,"iBillMode",-1,BillBoardFile);
	GetPrivateProfileString(AddName,"fScalU","ERROR",tmpNum,sizeof(tmpNum),BillBoardFile);
	m_iScalU = atof(tmpNum);
	GetPrivateProfileString(AddName,"fScalV","ERROR",tmpNum,sizeof(tmpNum),BillBoardFile);
	m_iScalV = atof(tmpNum);
	GetPrivateProfileString(AddName,"fScalX","ERROR",tmpNum,sizeof(tmpNum),BillBoardFile);
	m_fScalX = atof(tmpNum);
	GetPrivateProfileString(AddName,"fScalY","ERROR",tmpNum,sizeof(tmpNum),BillBoardFile);
	m_fScalY = atof(tmpNum);
	GetPrivateProfileString(AddName,"fPitch","ERROR",tmpNum,sizeof(tmpNum),BillBoardFile);			
	m_fPitch = atof(tmpNum);
	GetPrivateProfileString(AddName,"strPath","ERROR",m_TexturePath,sizeof(m_TexturePath),BillBoardFile);
	m_iTextureNum = GetPrivateProfileInt(AddName,"iTextureNum",-1,BillBoardFile);
	GetPrivateProfileString(AddName,"fTime","ERROR",tmpNum,sizeof(tmpNum),BillBoardFile);			
	m_dTime = atof(tmpNum);	

	int PosMaxNum = GetPrivateProfileInt(AddName,"PosMaxNum",-1,BillBoardFile);
	if (PosMaxNum)
	{
		for (int i = 0;i < PosMaxNum;i++)
		{
			sprintf(tmpKeyName,"vPos_%d",i);
			GetPrivateProfileString(AddName,tmpKeyName,"ERROR",tmpData,sizeof(tmpData),BillBoardFile);
			D3DXVECTOR3 newPos = CMyFuntion::GetVec3FromStr(tmpData);
			m_vecPos.push_back(newPos);
		}
		return true;
	}
	return false;
}

void CBillBoard::SetBillMode(BILLMODE mode)
{
	switch(mode)
	{
	case 1:
		{
			D3DXMATRIX matBillboard;
			D3DXMATRIX matScal,matRotX;
			D3DXMatrixScaling(&matScal,m_fScalX,m_fScalY,0);
			float fAngle = m_fPitch;
			D3DXMatrixRotationX(&matRotX,fAngle);
			matBillboard = matScal * matRotX;
			matBillboard._41 = m_vPos.x;
			matBillboard._42 = m_vPos.y;
			matBillboard._43 = m_vPos.z;
			m_pDevice->SetTransform(D3DTS_WORLD,&matBillboard);
		}
		break;
	case 2:
		{
			D3DXMATRIX matBillboard,mView,mInvView;
			D3DXMATRIX matScal,matRotX;
			D3DXMatrixScaling(&matScal,m_fScalX,m_fScalY,0);
			float fAngle = m_fPitch;
			D3DXMatrixRotationX(&matRotX,fAngle);
			matBillboard = matScal * matRotX;

			m_pDevice->GetTransform(D3DTS_VIEW,&mView);
			D3DXMatrixInverse(&mInvView,NULL,&mView);

			matBillboard = matBillboard * mInvView;

			matBillboard._41 = m_vPos.x;
			matBillboard._42 = m_vPos.y;
			matBillboard._43 = m_vPos.z;

			m_pDevice->SetTransform(D3DTS_WORLD,&matBillboard);
		}
		break;
	case 3:
		{
			D3DXMATRIX matBillboard,mView,mInvView;
			//D3DXMatrixIdentity(&matBillboard);
			D3DXMATRIX matScal,matRotX;
			D3DXMatrixScaling(&matScal,m_fScalX,m_fScalY,0);
			float fAngle = m_fPitch;
			D3DXMatrixRotationX(&matRotX,fAngle);
			matBillboard = matScal * matRotX;

			m_pDevice->GetTransform(D3DTS_VIEW,&mView);
			D3DXMatrixInverse(&mInvView,NULL,&mView);

			//matBillboard = matBillboard * mInvView;

			//matBillboard._11 = mView._11;
			//matBillboard._13 = mView._13;
			//matBillboard._31 = mView._31;
			//matBillboard._33 = mView._33;

			D3DXMATRIX msrc;
			D3DXMatrixIdentity(&msrc);
			msrc._11 = mInvView._11; msrc._13 = mInvView._13;
			msrc._31 = mInvView._31; msrc._33 = mInvView._33;

			matBillboard = matBillboard * msrc;

			matBillboard._41 = m_vPos.x;
			matBillboard._42 = m_vPos.y;
			matBillboard._43 = m_vPos.z;

			m_pDevice->SetTransform(D3DTS_WORLD,&matBillboard);
		}
		break;
	default:
		break;
	}
}

void CBillBoard::LoadTexture(char* TexturePath,int TextureNum)
{
	char cBaseFileName[255];
	char cExName[10];
	char cNum[5];
	sscanf(TexturePath,"%[^0-9]%[0-9]%[^0-9]",cBaseFileName,cNum,cExName);

	for (int i=1;i<=TextureNum;i++)
	{
		char cLoadFileName[255];
		sprintf(cLoadFileName,"%s%d%s",cBaseFileName,i,cExName);
		IDirect3DTexture9* pTempTexture;
		if (FAILED(D3DXCreateTextureFromFile(m_pDevice,cLoadFileName,&pTempTexture)))
		{
			pTempTexture = NULL;
			break;
		}
		m_vecTexture.push_back(pTempTexture);
	}
}