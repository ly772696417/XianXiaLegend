#include "Terrain.h"

CTerrain::CTerrain(IDirect3DDevice9* pDevice):
	m_pDevice(pDevice)
{
	D3DVERTEXELEMENT9 terrainVert[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		D3DDECL_END()
	};	
	m_pDevice->CreateVertexDeclaration(terrainVert, &m_pDECL);

	m_pTexture     = NULL;
	m_pVB          = NULL;
	m_pIB          = NULL;
	m_pZFrustum    = NULL;
	m_fLODRatio    = 0.0015f;
	m_iRow         = 129;
	m_iCol         = 129;
	m_iCellSpace   = 10.0f;
	m_iHeightScale = 1;
	m_iCellRow     = m_iRow - 1;
	m_iCellCol     = m_iCol - 1;
	m_iWidth       = m_iCellCol * m_iCellSpace;
	m_iDepth       = m_iCellRow * m_iCellSpace;
	m_iVertexNum   = m_iRow * m_iCol;
	m_iTriangle    = m_iCellRow * m_iCellCol * 2;
	m_iMaxTri      = m_iTriangle;
	//m_pOrigIndex   = NULL;
	m_pTreeRoot    = NULL;
	D3DXCreateFont(m_pDevice,20,20,400,0,false,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,DEFAULT_QUALITY,0,"宋体",&m_pFont);
	
	//m_pHeightmap.loadRAW(m_iRow,m_iCol,m_iCellSpace,"texture\\terrainHiMap1.bmp",m_iHeightScale);
	//m_pHeightmap.filter3x3();


}

CTerrain::~CTerrain(void)
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pTexture);
	SAFE_DELETE(m_pTreeRoot);
	//SAFE_DELETE_ARRAY(m_pOrigIndex);
}

bool CTerrain::Init(char* TerrainFile,int ID,ZFrustum* pZFrustum)
{
	char tmpAddName[256],texture[256],heightMap[256];
	sprintf(tmpAddName,"id_%d",ID);
	GetPrivateProfileString(tmpAddName,"Texture","ERROR",texture,sizeof(texture),TerrainFile);
	GetPrivateProfileString(tmpAddName,"HeightMap","ERROR",heightMap,sizeof(heightMap),TerrainFile);
	int Row = GetPrivateProfileInt(tmpAddName,"Row",-1,TerrainFile);
	int Col = GetPrivateProfileInt(tmpAddName,"Col",-1,TerrainFile);
	int CellSpace = GetPrivateProfileInt(tmpAddName,"CellSpace",-1,TerrainFile);
	int HeightScale = GetPrivateProfileInt(tmpAddName,"HeightScale",-1,TerrainFile);

	m_iRow = Row + 1;
	m_iCol = Col + 1;
	m_iCellSpace = CellSpace;
	m_iHeightScale = HeightScale;
	m_iCellRow = Row /*- 1*/;
	m_iCellCol = Col /*- 1*/;
	m_iWidth = m_iCellCol * m_iCellSpace;
	m_iDepth = m_iCellRow * m_iCellSpace;
	m_iVertexNum = m_iRow * m_iCol;
	m_iTriangle = m_iCellRow * m_iCellCol * 2;
	m_iMaxTri      = m_iTriangle;
	//m_pOrigIndex = new DWORD[m_iTriangle * 3];

	HRESULT hr = 0;
	hr = LoadTexture(texture);
	if (FAILED(hr))
		return false;

	hr = LoadHeightMap(heightMap);
	if (FAILED(hr))
		return false;
	for(int i = 0; i < m_vHeightData.size(); i++)
		m_vHeightData[i] *= HeightScale;

	CalcNormalMap();

	hr = ComputeVertex();
	hr = ComputeIndex();
	BuildTextures();
	BuildEffect();

	m_pZFrustum = pZFrustum;
	//m_pZFrustum = new ZFrustum();

	// 创建地形树
	m_pTreeRoot = new ZQuadTree(m_iRow,m_iCol);
	if (m_pTreeRoot == NULL)
		return false;
	LandVertex* pV;
	m_pVB->Lock(0,0,(void**)&pV,0);
	m_pVB->Unlock();
	if (m_pTreeRoot->Build(pV) == false)
		return false;


	return true;
}

void CTerrain::CalcNormalMap(){
	int n = m_vHeightData.size();
	m_vNormalData.resize(n);

	for (int i = 0;i<n;i++)
	{
		//m_vNormalData[i] = D3DXVECTOR3(0.0f,1.0f,0.0f);
		m_vNormalData[i] = CalcNormal(i);
	}
}

D3DXVECTOR3 CTerrain::CalcNormal(int i){
	D3DXVECTOR3 avg(0.0f,0.0f,0.0f);
	float num = 0.0f;

	int n = m_vHeightData.size();

	//for (int i = 0;i<n;i++)
	{
		if((i/m_iRow == 0) || (i/m_iRow == m_iCol - 1) || (i%m_iRow == 0) || (i%m_iRow == m_iCol - 1)){
			avg+=D3DXVECTOR3(0.0f,1.0f,0.0f);
			num+=1.0f;
		}
		else
		{
			avg+=QuadNormal(i);
			//avg+=D3DXVECTOR3(0.0f,1.0f,0.0f);
			num+=1.0f;
		}
	}

	avg /= i;
	D3DXVec3Normalize(&avg,&avg);

	return avg;
}

D3DXVECTOR3 CTerrain::QuadNormal(int i){
	//return D3DXVECTOR3(0.0f,1.0f,0.0f);

	float h0 = m_vHeightData[i];
	float h1 = m_vHeightData[i+1];
	float h2 = m_vHeightData[i+m_iRow];
	float h3 = m_vHeightData[i+m_iRow+1];

	D3DXVECTOR3 u = D3DXVECTOR3(m_iCellSpace, h1-h0, 0.0f);
	D3DXVECTOR3 v = D3DXVECTOR3(0.0f, h2-h0, -m_iCellSpace);

	D3DXVECTOR3 n1(0.0f, 1.0f, 0.0f);
	D3DXVec3Cross(&n1, &u, &v);
	D3DXVec3Normalize(&n1, &n1);

	//return n1;

	D3DXVECTOR3 s = D3DXVECTOR3(-m_iCellSpace, h2-h3, 0.0f);
	D3DXVECTOR3 t = D3DXVECTOR3(0.0f, h1-h3, m_iCellSpace);

	D3DXVECTOR3 n2(0.0f, 1.0f, 0.0f);
	D3DXVec3Cross(&n2, &s, &t);
	D3DXVec3Normalize(&n2, &n2);

	return (n1 + n2) * 0.5f;
}

bool CTerrain::LoadTexture(char* SceneFile)
{
	HRESULT hr = 0;
	hr = D3DXCreateTextureFromFile(m_pDevice,SceneFile,&m_pTexture);
	if(FAILED(hr))
		return false;
	return true;
}

void CTerrain::Update(/*D3DXVECTOR3 vPos,D3DXVECTOR3 vRot,D3DXVECTOR3 vScal*/)
{
	//D3DXMATRIX proj,view,viewProj;
	//m_pDevice->GetTransform(D3DTS_VIEW,&view);
	//m_pDevice->GetTransform(D3DTS_PROJECTION,&proj);
	//viewProj = view * proj;
	//m_pZFrustum->Make(&viewProj,m_iCellSpace);
	LandVertex* pV;
	DWORD* pI;
	m_pVB->Lock(0,0,(void**)&pV,0);
	m_pVB->Unlock();
	m_pIB->Lock(0,0,(void**)&pI,0);
	m_pIB->Unlock();
	m_iTriangle = m_pTreeRoot->GenerateIndex(pI,pV,m_pZFrustum,m_fLODRatio);
}

void CTerrain::Render()
{
	/*D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_pDevice->SetTransform(D3DTS_WORLD,&matWorld);
	m_pDevice->SetRenderState(D3DRS_LIGHTING,false);
	m_pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	m_pDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESS);
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	if (m_pTexture != NULL)
	{
		m_pDevice->SetTexture(0,m_pTexture);
		m_pDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	}
	m_pDevice->SetFVF(LAND_FVF);
	m_pDevice->SetStreamSource(0,m_pVB,0,sizeof(LandVertex));
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,m_iVertexNum,0,m_iTriangle);*/

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(LandVertex));
	m_pDevice->SetVertexDeclaration(m_pDECL);
	m_pDevice->SetIndices(m_pIB);


	D3DXMATRIX projMat;
	m_pDevice->GetTransform(D3DTS_PROJECTION,&projMat);
	D3DXMATRIX viewMat;
	m_pDevice->GetTransform(D3DTS_VIEW,&viewMat);

	D3DXMATRIX matrix = viewMat * projMat;
	mTerrainFX->SetMatrix(mhWVP, &matrix);

	mTerrainFX->SetTechnique(mhTech);

	UINT numPasses = 0;
	mTerrainFX->Begin(&numPasses, 0);
	mTerrainFX->BeginPass(0);

	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 
		m_iVertexNum, 0, m_iTriangle);

	mTerrainFX->EndPass();

	mTerrainFX->End();

	//char text[128];
	//itoa(m_iTriangle,text,10);
	//RECT rect;
	//rect.top = 20;
	//rect.left = 50;
	//rect.bottom = 200;
	//rect.right = 200;
	//m_pFont->DrawText(0,text,-1,&rect,DT_TOP|DT_LEFT,0xff0088ff);
}

bool CTerrain::IntersectTerrain(IDirect3DVertexBuffer9* pVB,
								IDirect3DIndexBuffer9* pIB,
								const Ray& ray,
								const int iNumFaces,
								float* fDist,
								D3DXVECTOR3* VRet)
{
	LandVertex* pV;
	WORD* pI;
	D3DINDEXBUFFER_DESC IB_Desc;
	D3DXVECTOR3 orig = ray.origin;
	D3DXVECTOR3 dir = ray.dir;
	D3DXVECTOR3* Triangle[3];
	UINT n;

	// 保存交点的u,v值与距离，交点面的索引
	float u,v,Dist = 1e+32f;
	float tu,tv,tDist;
	int iFace = -1;

	pIB->GetDesc(&IB_Desc);
    pVB->Lock(0,0,(void**)&pV,0);
    pIB->Lock(0,0,(void**)&pI,0);

	for (int i = 0;i < iNumFaces;i++)
	{
		// 获取第i个三角形的顶点
		for (int j = 0;j < 3;j++)
		{
			if (IB_Desc.Format == D3DFMT_INDEX16)
				n = *(pI + (3 * i + j));
			else if (IB_Desc.Format == D3DFMT_INDEX32)
				n = *((DWORD*)pI + (3 * i + j));
			else
				return FALSE;
			Triangle[j] = (D3DXVECTOR3*)(pV + n);
		}
		// 保存最近的交点
		if (D3DXIntersectTri(Triangle[0],Triangle[1],Triangle[2],&orig,&dir,&tu,&tv,&tDist) && tDist < Dist)
		{
			iFace = i;
			Dist = tDist;
			u = tu;
			v = tv;
		}
	}

	if (fDist != NULL)
		*fDist = Dist;

	
	// 计算交点坐标
	if (VRet != NULL)
	{
		if (iFace < iNumFaces && iFace >= 0)
		{
			for(int j=0;j<3;j++)
			{
				if (IB_Desc.Format == D3DFMT_INDEX16)
					n = *(pI + (3 * iFace + j));
				else if (IB_Desc.Format == D3DFMT_INDEX32)
					n = *((DWORD*)pI + (3 * iFace + j));
				else
					return FALSE;
				Triangle[j] = (D3DXVECTOR3*)(pV + n);
			}
			*VRet = *Triangle[0] + u * (*Triangle[1] - *Triangle[0]) + v * (*Triangle[2] - *Triangle[0]);

			pVB->Unlock();
			pIB->Unlock();
			return TRUE;
		}
	}
	pVB->Unlock();
    pIB->Unlock();
    return FALSE;
}

//bool IntersectTerrain(IDirect3DVertexBuffer9* pVB,
//					  DWORD* dwIB,
//					  const Ray& ray,
//					  int iNumFaces,
//					  D3DXVECTOR3& VRet)
//{
//	LandVertex* pV;
//	D3DXVECTOR3 orig = ray.origin;
//	D3DXVECTOR3 dir = ray.dir;
//	D3DXVECTOR3* Triangle[3];
//	UINT n;
//
//	// 保存交点的u,v值与距离，交点面的索引
//	float u,v,Dist = 1e+32f;
//	float tu,tv,tDist;
//	int iFace = -1;
//
//    pVB->Lock(0,0,(void**)&pV,0);
//
//	for (int i = 0;i < iNumFaces;i++)
//	{
//		// 获取第i个三角形的顶点
//		for (int j = 0;j < 3;j++)
//		{
//			n = dwIB[3 * i + j];
//			Triangle[j] = (D3DXVECTOR3*)(pV + n);
//		}
//		// 保存最近的交点
//		if (D3DXIntersectTri(Triangle[0],Triangle[1],Triangle[2],&orig,&dir,&tu,&tv,&tDist) && tDist < Dist)
//		{
//			iFace = i;
//			Dist = tDist;
//			u = tu;
//			v = tv;
//		}
//	}
//	// 计算交点坐标
//	if (iFace < iNumFaces && iFace >= 0)
//	{
//		for(int j=0;j<3;j++)
//		{
//			n = dwIB[3 * iFace + j];
//			Triangle[j] = (D3DXVECTOR3*)(pV + n);
//		}
//		VRet = *Triangle[0] + u * (*Triangle[1] - *Triangle[0]) + v * (*Triangle[2] - *Triangle[0]);
//
//		pVB->Unlock();
//		return TRUE;
//	}
//	pVB->Unlock();
//    return FALSE;
//}

bool CTerrain::LoadHeightMap(char* SceneFile)
{
	char basePath[256],type[32];
	sscanf(SceneFile,".%[^.].%[^.]",basePath,type);

	if (strcmp("bmp",type) == 0)
	{
		BITMAP bmp;
		HBITMAP hBmp;
		HDC hDC;
		HBITMAP oldBmp;

		hDC = CreateCompatibleDC(NULL);
		hBmp = (HBITMAP)LoadImage(NULL,SceneFile,IMAGE_BITMAP,m_iCol,m_iRow,LR_LOADFROMFILE);
		oldBmp = (HBITMAP)SelectObject(hDC,hBmp);

		// 从高度图中读取高度信息
		GetObject(hBmp,sizeof(bmp),&bmp);
		m_iCol = bmp.bmWidth;
		m_iRow = bmp.bmHeight;
		m_vHeightData.resize(m_iVertexNum);
		DWORD color;
		for (int i=0;i<m_iRow;i++)
		{
			for (int j=0;j<m_iCol;j++)
			{
				color = GetPixel(hDC,j,i);
				m_vHeightData[i*m_iCol+j] = *(BYTE*)&color;
			}
		}
		DeleteObject(oldBmp);
		DeleteObject(hDC);
		DeleteObject(hBmp);
	}else if (strcmp("raw",type) == 0)
	{
		std::vector<BYTE> in(m_iVertexNum);

		std::ifstream inFile(SceneFile,std::ios_base::binary);

		if( inFile == 0 )
			return false;

		inFile.read(
			(char*)&in[0],// buffer
			in.size());// number of bytes to read into buffer

		inFile.close();

		// copy BYTE vector to int vector
		m_vHeightData.resize(m_iVertexNum);

		for(int i = 0; i < in.size(); i++)
		{
			m_vHeightData[i] = in[i];
			int a = in[i];
		}
	}else
	{
		return false;
	}
	return true;
}

bool CTerrain::ComputeVertex()
{	
	/*HRESULT hr = 0;
	hr = m_pDevice->CreateVertexBuffer(
		m_iVertexNum * sizeof(LandVertex),
		D3DUSAGE_WRITEONLY,
		LAND_FVF,
		D3DPOOL_DEFAULT,
		&m_pVB,
		0);
	if (FAILED(hr))
		return false;*/

	m_pDevice->CreateVertexBuffer(m_iVertexNum*sizeof(LandVertex),D3DUSAGE_WRITEONLY,
		0,D3DPOOL_MANAGED,&m_pVB,0);

	int startX = - m_iWidth / 2;
	int startZ = m_iDepth / 2;
	int endX = m_iWidth / 2;
	int endZ = - m_iDepth / 2;

	// 计算纹理u，v每格距离
	float uCellSize = 1.0f / (float)m_iCellCol;
	float vCellSize = 1.0f / (float)m_iCellRow;
	LandVertex* pV;
	if (FAILED(m_pVB->Lock(0,m_iVertexNum * sizeof(LandVertex),(void**)&pV,0)))
		return false;

	int i = 0;
	for (int z = startZ;z >= endZ;z -= m_iCellSpace)
	{
		int j = 0;
		for (int x = startX;x <= endX;x += m_iCellSpace)
		{
			int index = i * m_iCol + j;
			pV[index].x = (float)x;
			if (i != (m_iRow - 1) && j != (m_iCol - 1))
			{
				int heightIndex = i * (m_iCol - 1) + j;
				pV[index].y = (float)m_vHeightData[index];
			}
			// 补最后一行和一列
			else
			{
				pV[index].y = 0;
			}
			pV[index].z = (float)z;
			pV[index].u = (float)j;// * uCellSize;
			pV[index].v = (float)i;// * vCellSize;
			pV[index].tu = (float)j/m_iCellRow;
			pV[index].tv = (float)i/m_iCellRow;
			//int indexNormal =j * m_iCol + i;
			pV[index].normal = m_vNormalData[index];
			j++;
		}
		i++;
	}
	m_pVB->Unlock();
	
	return true;
}

bool CTerrain::ComputeIndex()
{
	HRESULT hr = 0;
	int nBufferSize = (m_iCol-1)*(m_iRow-1)*6*sizeof(DWORD);
	hr = m_pDevice->CreateIndexBuffer(
		nBufferSize,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&m_pIB,
		NULL);
	if (FAILED(hr))
		return false;

	DWORD* pI;
	m_pIB->Lock(0,0,(void**)&pI,0);
	int i,j;
	for (i=0,j=0;i<m_iVertexNum;i++)
	{
		if((i+1)%m_iCol && i/m_iCol!=m_iRow-1)
		{
			pI[j+0] = /*(WORD)*/i;
			pI[j+1] = i + 1;
			pI[j+2] = i + m_iCol;
			pI[j+3] = i + m_iCol;
			pI[j+4] = i + 1;
			pI[j+5] = i + m_iCol + 1;
			j += 6;
		}
	}
	m_pIB->Unlock();
	/*for (i = 0;i < m_iTriangle * 3;i++)
	{
		m_pOrigIndex[i] = pI[i];
	}*/

	return true;
}

void CTerrain::BuildTextures(){
	D3DXCreateTextureFromFile(m_pDevice, 
		"terrain/grass.jpg", &mLayerMap0);

	D3DXCreateTextureFromFile(m_pDevice, 
		"terrain/dirt.jpg", &mLayerMap1);

	D3DXCreateTextureFromFile(m_pDevice, 
		"terrain/stone.jpg", &mLayerMap2);

	D3DXCreateTextureFromFile(m_pDevice, 
		"terrain/blend_hm17.jpg", &mBlendMap);
}

void CTerrain::BuildEffect(){
	ID3DXBuffer* errors = 0;
	D3DXCreateEffectFromFile(m_pDevice, "FXFolder\\terrain_multitexture.fx",
		0, 0, D3DXSHADER_DEBUG, 0, &mTerrainFX, &errors);

	if(errors)
	{
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
		errors->Release();
	}

	mhTech      = mTerrainFX->GetTechniqueByName("Terrain_MultiTex");
	mhWVP       = mTerrainFX->GetParameterByName(0, "gWVP");
	mhTexScale  = mTerrainFX->GetParameterByName(0, "gTexScale");
	mhDirToSunW = mTerrainFX->GetParameterByName(0, "gDirToSunW");

	mhLayerMap0   = mTerrainFX->GetParameterByName(0, "gLayerMap0");
	mhLayerMap1   = mTerrainFX->GetParameterByName(0, "gLayerMap1");
	mhLayerMap2   = mTerrainFX->GetParameterByName(0, "gLayerMap2");
	mhBlendMap    = mTerrainFX->GetParameterByName(0, "gBlendMap");

	mTerrainFX->SetTexture(mhLayerMap0, mLayerMap0);
	mTerrainFX->SetTexture(mhLayerMap1, mLayerMap1);
	mTerrainFX->SetTexture(mhLayerMap2, mLayerMap2);
	mTerrainFX->SetTexture(mhBlendMap, mBlendMap);

	mTerrainFX->SetValue(mhTexScale, &D3DXVECTOR2(0.06f, 0.06f), sizeof(D3DXVECTOR2));
	mTerrainFX->SetValue(mhDirToSunW, &D3DXVECTOR3(1.0f, 1.0f, 0.0f), sizeof(D3DXVECTOR3));
}

float CTerrain::GetHeight(float x,float z)
{
	// 地形起始点
	int X = -(m_iWidth >> 1);
	int Z = m_iDepth >> 1;
	/*float Height = 0;*/

	int col,row;// 点所在地形格行列
	col = int((x - X) / m_iCellSpace);
	row = int((Z - z) / m_iCellSpace);

	// 四个角的y值
	//  A--B
    //  | /|
	//  |/ |
   	//  C--D
	D3DXVECTOR3 A,B,C,D;
	LandVertex* pV;
	m_pVB->Lock(0,0,(void**)&pV,0);
	A = *(D3DXVECTOR3*)&pV[m_iCol * row + col];
	B = *(D3DXVECTOR3*)&pV[m_iCol * row + col + 1];
	C = *(D3DXVECTOR3*)&pV[m_iCol * (row + 1) + col];
	D = *(D3DXVECTOR3*)&pV[m_iCol * (row + 1) + col + 1];
	m_pVB->Unlock();

	//// 点在单元格中的相对位置
	//int dx = int(x - X) % m_iCellSpace;
	//int dz = int(Z - z) % m_iCellSpace;
	//// 点在x,z方向的比重
	//int u = dx / m_iCellSpace;
	//int v = dz / m_iCellSpace;
	//if (dz > dx)
	//{
	//	Height = A + (B - A) * u + (C - A) * v;
	//}
	//else
	//{
	//	Height = D + (C - D) * (1 - u) + (B - D) * (1 - v);
	//}

	Ray ray;
	ray.origin = D3DXVECTOR3(x,255 * m_iHeightScale + 50,z);
	ray.dir = D3DXVECTOR3(0,-1,0);

	float u,v;
	D3DXVECTOR3 vRet;
	if (D3DXIntersectTri(&A,&B,&C,&ray.origin,&ray.dir,&u,&v,NULL))
		vRet = A + u * (B - A) + v * (C - A);
	else if (D3DXIntersectTri(&C,&B,&D,&ray.origin,&ray.dir,&u,&v,NULL))
		vRet = C + u * (B - C) + v * (D - C);
	else
		vRet = D3DXVECTOR3(-9999,-9999,-9999);
	
	return vRet.y;
}