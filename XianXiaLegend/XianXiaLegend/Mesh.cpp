#include "Mesh.h"

CMesh::CMesh(IDirect3DDevice9* pDevice)
{
	m_pDevice = pDevice;
}

CMesh::~CMesh(void)
{
	SAFE_DELETE(m_pMeshTextures);
	SAFE_DELETE(m_pMeshMaterials);
	m_pMesh->Release();
}

HRESULT CMesh::LoadMesh(char* strMeshPath,char* strTexturePath)
{
	if (FAILED(D3DXLoadMeshFromX(strMeshPath,D3DXMESH_MANAGED,m_pDevice,NULL,
								&pD3DXMtrlBuffer,NULL,&m_dwNumMaterials,&m_pMesh)))
	{
		return E_FAIL;
	}
	// �Ӳ��ʼ����аѲ��ʺ���Ϣ��ѹ��ȡ����
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	m_pMeshMaterials = new D3DMATERIAL9[m_dwNumMaterials];
	m_pMeshTextures = new IDirect3DTexture9*[m_dwNumMaterials];
	for (DWORD i=0; i<m_dwNumMaterials; i++)
	{
		// ��������
		m_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;
		// ���ò������������ɫ
		m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;
		// ��������
		char tmpTexture[256];
		sprintf(tmpTexture,"%s%s",strTexturePath,d3dxMaterials[i].pTextureFilename);
		if (FAILED(D3DXCreateTextureFromFile(m_pDevice,tmpTexture,&m_pMeshTextures[i])))
		{
			m_pMeshTextures[i] = NULL;
		}
	}
	// �ͷŲ��ʻ�����������
	pD3DXMtrlBuffer->Release();

	//DWORD Faces = m_pMesh->GetNumFaces();
	//DWORD adjacencyInfo[m_pMesh->GetNumFaces() * 3];
	//m_pMesh->GenerateAdjacency(0.0f,adjacencyInfo);
	//DWORD optimizedAdjacencyInfo[m_pMesh->GetNumFaces() * 3];
	//m_pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
	//						adjacencyInfo,optimizedAdjacencyInfo,0,0);

	return S_OK;
}

void CMesh::Render()
{
	//m_pDevice->SetTransform(D3DTS_WORLD,matWorld);
	for (DWORD i=0; i<m_dwNumMaterials; i++)
	{
		// ���ò��ʺ���ͼ
		m_pDevice->SetMaterial(&m_pMeshMaterials[i]);
		m_pDevice->SetTexture(0,m_pMeshTextures[i]);
		// ��������
		m_pMesh->DrawSubset(i);
	}
}

bool CMesh::GetBoundingB_S(D3DXVECTOR3* pMax,D3DXVECTOR3* pMin,D3DXVECTOR3* pCenter,float* pRadius)
{
	HRESULT hr = 0;
	D3DXVECTOR3 tmpMin,tmpMax;
	BYTE* pV;
	m_pMesh->LockVertexBuffer(0,(void**)&pV);
	hr = D3DXComputeBoundingBox(
		(D3DXVECTOR3*)pV,                         // ָ���ڶ��������е�һ�����������
		m_pMesh->GetNumVertices(),	              // �ڶ��������еĵĶ�������
		D3DXGetFVFVertexSize(m_pMesh->GetFVF()),  // ÿ��������ֽڴ�С
		&tmpMin,								  // Χ�е���С��
		&tmpMax);								  // Χ�е�����
	m_pMesh->UnlockVertexBuffer();

	if (pMax != NULL)
		*pMin = tmpMin;
		
	if (pMin != NULL)
		*pMax = tmpMax;
		
	if (pCenter != NULL)
	{
		D3DXVECTOR3 tmpCenter;
		tmpCenter.x = (tmpMax.x - tmpMin.x) / 2;
		tmpCenter.y = (tmpMax.y - tmpMin.y) / 2;
		tmpCenter.z = (tmpMax.z - tmpMin.z) / 2;
		*pCenter = tmpCenter;
	}
	if (pRadius != NULL)
	{
		*pRadius = D3DXVec3Length(&(tmpMax - tmpMin));
	}

	if( FAILED(hr) )
		return false;

	return true;
}