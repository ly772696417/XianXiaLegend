#include "Water.h"

Water::Water(InitInfo& initInfo,LPDIRECT3DDEVICE9 _pDirext3DDevice9)
{
	m_pDirext3DDevice9 = _pDirext3DDevice9;

	D3DVERTEXELEMENT9 VertexPTElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};	
	m_pDirext3DDevice9->CreateVertexDeclaration(VertexPTElements, &m_pDECL);

	mInitInfo = initInfo;

	mWidth = (initInfo.vertCols-1)*initInfo.dx;
	mDepth = (initInfo.vertRows-1)*initInfo.dz;

	mWaveMapOffset0 = D3DXVECTOR2(0.0f, 0.0f);
	mWaveMapOffset1 = D3DXVECTOR2(0.0f, 0.0f);

	DWORD numTris  = (initInfo.vertRows-1)*(initInfo.vertCols-1)*2;
	DWORD numVerts = initInfo.vertRows*initInfo.vertCols;


	//===============================================================
	// Allocate the mesh.


	D3DVERTEXELEMENT9 elems[MAX_FVF_DECL_SIZE];
	UINT numElems = 0;
	m_pDECL->GetDeclaration(elems, &numElems);
	D3DXCreateMesh(numTris, numVerts, 
		D3DXMESH_MANAGED, elems, m_pDirext3DDevice9, &mMesh);


	//===============================================================
	// Write the grid vertices and triangles to the mesh.

	VertexPT* v = 0;
	mMesh->LockVertexBuffer(0, (void**)&v);
	
	std::vector<D3DXVECTOR3> verts;
	std::vector<DWORD> indices;
	GenTriGrid(mInitInfo.vertRows, mInitInfo.vertCols, mInitInfo.dx, 
		mInitInfo.dz, D3DXVECTOR3(0.0f, 0.0f, 0.0f), verts, indices);

	for(int i = 0; i < mInitInfo.vertRows; ++i)
	{
		for(int j = 0; j < mInitInfo.vertCols; ++j)
		{
			DWORD index   = i * mInitInfo.vertCols + j;
			v[index].pos  = verts[index];
			v[index].tex0 = D3DXVECTOR2((float)j/mInitInfo.vertCols, 
				                        (float)i/mInitInfo.vertRows)
										* initInfo.texScale;
		}
	}
	mMesh->UnlockVertexBuffer();

	//===============================================================
	// Write triangle data so we can compute normals.

	WORD* indexBuffPtr = 0;
	mMesh->LockIndexBuffer(0, (void**)&indexBuffPtr);
	DWORD* attBuff = 0;
	mMesh->LockAttributeBuffer(0, &attBuff);
	for(UINT i = 0; i < mMesh->GetNumFaces(); ++i)
	{
		indexBuffPtr[i*3+0] = (WORD)indices[i*3+0];
		indexBuffPtr[i*3+1] = (WORD)indices[i*3+1];
		indexBuffPtr[i*3+2] = (WORD)indices[i*3+2];

		attBuff[i] = 0; // All in subset 0.
	}
	mMesh->UnlockIndexBuffer();
	mMesh->UnlockAttributeBuffer();

	//===============================================================
	// Optimize for the vertex cache and build attribute table.

	DWORD* adj = new DWORD[mMesh->GetNumFaces()*3];
	mMesh->GenerateAdjacency(EPSILON, adj);
	mMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE|D3DXMESHOPT_ATTRSORT,
		adj, 0, 0, 0);
	delete[] adj;


	//===============================================================
	// Create textures/effect.

	D3DXCreateTextureFromFile(m_pDirext3DDevice9, initInfo.waveMapFilename0.c_str(), &mWaveMap0);
	D3DXCreateTextureFromFile(m_pDirext3DDevice9, initInfo.waveMapFilename1.c_str(), &mWaveMap1);
	buildFX();
}

Water::~Water()
{
	SAFE_RELEASE(mMesh);
	SAFE_RELEASE(mFX);
	SAFE_RELEASE(mWaveMap0);
	SAFE_RELEASE(mWaveMap1);
}

DWORD Water::getNumTriangles()
{
	return mMesh->GetNumFaces();
}

DWORD Water::getNumVertices()
{
	return mMesh->GetNumVertices();
}

void Water::onLostDevice()
{
	mFX->OnLostDevice();
}

void Water::onResetDevice()
{
	mFX->OnResetDevice();
}

void Water::update(float dt)
{
	// Update texture coordinate offsets.  These offsets are added to the
	// texture coordinates in the vertex shader to animate them.
	mWaveMapOffset0 += mInitInfo.waveMapVelocity0 * dt;
	mWaveMapOffset1 += mInitInfo.waveMapVelocity1 * dt;

	// Textures repeat every 1.0 unit, so reset back down to zero
	// so the coordinates do not grow too large.
	if(mWaveMapOffset0.x >= 1.0f || mWaveMapOffset0.x <= -1.0f)
		mWaveMapOffset0.x = 0.0f;
	if(mWaveMapOffset1.x >= 1.0f || mWaveMapOffset1.x <= -1.0f)
		mWaveMapOffset1.x = 0.0f;
	if(mWaveMapOffset0.y >= 1.0f || mWaveMapOffset0.y <= -1.0f)
		mWaveMapOffset0.y = 0.0f;
	if(mWaveMapOffset1.y >= 1.0f || mWaveMapOffset1.y <= -1.0f)
		mWaveMapOffset1.y = 0.0f;
}

void Water::draw(D3DXVECTOR3 _cameraPos)
{
	D3DXMATRIX projMat,matTrans;
	D3DXMatrixTranslation(&matTrans,0,100,0);
	m_pDirext3DDevice9->SetTransform(D3DTS_WORLD,&matTrans);
	m_pDirext3DDevice9->GetTransform(D3DTS_PROJECTION,&projMat);
	D3DXMATRIX viewMat;
	m_pDirext3DDevice9->GetTransform(D3DTS_VIEW,&viewMat);

	D3DXMATRIX matrix = matTrans*  viewMat * projMat ;

	mFX->SetMatrix(mhWVP, &matrix);
	mFX->SetValue(mhEyePosW, &_cameraPos, sizeof(D3DXVECTOR3));
	//mFX->SetValue(mhEyePosW, &D3DXVECTOR3(0,30,0), sizeof(D3DXVECTOR3));
	mFX->SetValue(mhWaveMapOffset0, &mWaveMapOffset0, sizeof(D3DXVECTOR2));
	mFX->SetValue(mhWaveMapOffset1, &mWaveMapOffset1, sizeof(D3DXVECTOR2));

	UINT numPasses = 0;
	mFX->Begin(&numPasses, 0);
	mFX->BeginPass(0);

	mMesh->DrawSubset(0);

	mFX->EndPass();
	mFX->End();
}

void Water::buildFX()
{
	ID3DXBuffer* errors = 0;
	D3DXCreateEffectFromFile(m_pDirext3DDevice9, "FXFolder\\Water.fx",
		0, 0, D3DXSHADER_DEBUG, 0, &mFX, &errors);
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

 	mhTech           = mFX->GetTechniqueByName("WaterTech");
	mhWorld          = mFX->GetParameterByName(0, "gWorld");
	mhWorldInv       = mFX->GetParameterByName(0, "gWorldInv");
	mhWVP            = mFX->GetParameterByName(0, "gWVP");
	mhEyePosW        = mFX->GetParameterByName(0, "gEyePosW");
	mhLight          = mFX->GetParameterByName(0, "gLight");
	mhMtrl           = mFX->GetParameterByName(0, "gMtrl");
	mhWaveMap0       = mFX->GetParameterByName(0, "gWaveMap0");
    mhWaveMap1       = mFX->GetParameterByName(0, "gWaveMap1");
	mhWaveMapOffset0 = mFX->GetParameterByName(0, "gWaveMapOffset0");
	mhWaveMapOffset1 = mFX->GetParameterByName(0, "gWaveMapOffset1");
	mhEnvMap         = mFX->GetParameterByName(0, "gEnvMap");


	// We don't need to set these every frame since they do not change.
	mFX->SetMatrix(mhWorld, &mInitInfo.toWorld);
	D3DXMATRIX worldInv;
	D3DXMatrixInverse(&worldInv, 0, &mInitInfo.toWorld);
	mFX->SetMatrix(mhWorldInv, &worldInv);
	mFX->SetTechnique(mhTech);
	mFX->SetTexture(mhWaveMap0, mWaveMap0);
	mFX->SetTexture(mhWaveMap1, mWaveMap1);
	mFX->SetValue(mhLight, &mInitInfo.dirLight, sizeof(DirLight));
	mFX->SetValue(mhMtrl, &mInitInfo.mtrl, sizeof(Mtrl));
}

void Water::setEnvMap(IDirect3DCubeTexture9* envMap)
{
	mFX->SetTexture(mhEnvMap, envMap);
}

void Water::GenTriGrid(int numVertRows, int numVertCols,
				float dx, float dz, 
				const D3DXVECTOR3& center, 
				std::vector<D3DXVECTOR3>& verts,
				std::vector<DWORD>& indices)
{
	int numVertices = numVertRows*numVertCols;
	int numCellRows = numVertRows-1;
	int numCellCols = numVertCols-1;

	int numTris = numCellRows*numCellCols*2;

	float width = (float)numCellCols * dx;
	float depth = (float)numCellRows * dz;

	//===========================================
	// Build vertices.

	// We first build the grid geometry centered about the origin and on
	// the xz-plane, row-by-row and in a top-down fashion.  We then translate
	// the grid vertices so that they are centered about the specified 
	// parameter 'center'.

	verts.resize( numVertices );

	// Offsets to translate grid from quadrant 4 to center of 
	// coordinate system.
	float xOffset = -width * 0.5f; 
	float zOffset =  depth * 0.5f;

	int k = 0;
	for(float i = 0; i < numVertRows; ++i)
	{
		for(float j = 0; j < numVertCols; ++j)
		{
			// Negate the depth coordinate to put in quadrant four.  
			// Then offset to center about coordinate system.
			verts[k].x =  j * dx + xOffset;
			verts[k].z = -i * dz + zOffset;
			verts[k].y =  0.0f;

			// Translate so that the center of the grid is at the
			// specified 'center' parameter.
			D3DXMATRIX T;
			D3DXMatrixTranslation(&T, center.x, center.y, center.z);
			D3DXVec3TransformCoord(&verts[k], &verts[k], &T);

			++k; // Next vertex
		}
	}

	//===========================================
	// Build indices.

	indices.resize(numTris * 3);

	// Generate indices for each quad.
	k = 0;
	for(DWORD i = 0; i < (DWORD)numCellRows; ++i)
	{
		for(DWORD j = 0; j < (DWORD)numCellCols; ++j)
		{
			indices[k]     =   i   * numVertCols + j;
			indices[k + 1] =   i   * numVertCols + j + 1;
			indices[k + 2] = (i+1) * numVertCols + j;

			indices[k + 3] = (i+1) * numVertCols + j;
			indices[k + 4] =   i   * numVertCols + j + 1;
			indices[k + 5] = (i+1) * numVertCols + j + 1;

			// next quad
			k += 6;
		}
	}
}