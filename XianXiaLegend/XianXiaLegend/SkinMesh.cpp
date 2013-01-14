#include "SkinMesh.h"

HRESULT CAllocateHierarchy::CreateFrame(LPCSTR Name,LPD3DXFRAME *ppNewFrame)
{
    HRESULT hr = S_OK;
    D3DXFRAME_DERIVED *pFrame;

    *ppNewFrame = NULL;

    pFrame = new D3DXFRAME_DERIVED;
    if (pFrame == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    hr = CMyFuntion::CopyName(Name,&pFrame->Name);
    if (FAILED(hr))
        goto e_Exit;

    // initialize other data members of the frame
    D3DXMatrixIdentity(&pFrame->TransformationMatrix);
    D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);

    pFrame->pMeshContainer = NULL;
    pFrame->pFrameSibling = NULL;
    pFrame->pFrameFirstChild = NULL;

    *ppNewFrame = pFrame;
    pFrame = NULL;

e_Exit:
    delete pFrame;
    return hr;
}

HRESULT CAllocateHierarchy::CreateMeshContainer(LPCSTR Name,
												CONST D3DXMESHDATA *pMeshData,
												CONST D3DXMATERIAL *pMaterials,
												CONST D3DXEFFECTINSTANCE *pEffectInstances,
												DWORD NumMaterials,
												CONST DWORD *pAdjacency,
												LPD3DXSKININFO pSkinInfo,
												LPD3DXMESHCONTAINER *ppNewMeshContainer) 
{
    HRESULT hr;
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = NULL;
    UINT NumFaces;
    UINT iMaterial;
    UINT iBone,cBones;
    IDirect3DDevice9* pd3dDevice = NULL;

    ID3DXMesh* pMesh = NULL;

    *ppNewMeshContainer = NULL;

    // this sample does not handle patch meshes,so fail when one is found
    if (pMeshData->Type != D3DXMESHTYPE_MESH)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // get the pMesh interface pointer out of the mesh data structure
    pMesh = pMeshData->pMesh;

    // this sample does not FVF compatible meshes,so fail when one is found
    if (pMesh->GetFVF() == 0)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // allocate the overloaded structure to return as a D3DXMESHCONTAINER
    pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
    if (pMeshContainer == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memset(pMeshContainer,0,sizeof(D3DXMESHCONTAINER_DERIVED));

    // make sure and copy the name.  All memory as input belongs to caller,interfaces can be addref'd though
    hr = CMyFuntion::CopyName(Name,&pMeshContainer->Name);
    if (FAILED(hr))
        goto e_Exit;        

    pMesh->GetDevice(&pd3dDevice);
    NumFaces = pMesh->GetNumFaces();

    // if no normals are in the mesh,add them
    if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
    {
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

        // clone the mesh to make room for the normals
        hr = pMesh->CloneMeshFVF( pMesh->GetOptions(),
                                    pMesh->GetFVF() | D3DFVF_NORMAL,
                                    pd3dDevice,&pMeshContainer->MeshData.pMesh );
        if (FAILED(hr))
            goto e_Exit;

        // get the new pMesh pointer back out of the mesh container to use
        // NOTE: we do not release pMesh because we do not have a reference to it yet
        pMesh = pMeshContainer->MeshData.pMesh;

        // now generate the normals for the pmesh
        D3DXComputeNormals( pMesh,NULL );
    }
    else  // if no normals,just add a reference to the mesh for the mesh container
    {
        pMeshContainer->MeshData.pMesh = pMesh;
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

        pMesh->AddRef();
    }
        
    // allocate memory to contain the material information.  This sample uses
    //   the D3D9 materials and texture names instead of the EffectInstance style materials
    pMeshContainer->NumMaterials = max(1,NumMaterials);
    pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
    pMeshContainer->ppTextures = new IDirect3DTexture9*[pMeshContainer->NumMaterials];
    pMeshContainer->pAdjacency = new DWORD[NumFaces*3];
    if ((pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    memcpy(pMeshContainer->pAdjacency,pAdjacency,sizeof(DWORD) * NumFaces*3);
    memset(pMeshContainer->ppTextures,0,sizeof(IDirect3DTexture9*) * pMeshContainer->NumMaterials);

    // if materials provided,copy them
    if (NumMaterials > 0)            
    {
        memcpy(pMeshContainer->pMaterials,pMaterials,sizeof(D3DXMATERIAL) * NumMaterials);

        for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++)
        {
            if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL)
            {
                char strMeshPath[256];
                //WCHAR wszBuf[MAX_PATH];
                //MultiByteToWideChar( CP_ACP,0,pMeshContainer->pMaterials[iMaterial].pTextureFilename,-1,wszBuf,MAX_PATH );
                //wszBuf[MAX_PATH - 1] = L'\0';
                //DXUTFindDXSDKMediaFileCch( strTexturePath,MAX_PATH,wszBuf );

				sprintf(strMeshPath,"%s%s",m_pSkinMesh->m_strTexturePath,pMeshContainer->pMaterials[iMaterial].pTextureFilename);
                if( FAILED( D3DXCreateTextureFromFile( pd3dDevice,strMeshPath,
                                                        &pMeshContainer->ppTextures[iMaterial] ) ) )
                    pMeshContainer->ppTextures[iMaterial] = NULL;

                // don't remember a pointer into the dynamic memory,just forget the name after loading
                pMeshContainer->pMaterials[iMaterial].pTextureFilename = NULL;
            }
        }
    }
    else // if no materials provided,use a default one
    {
        pMeshContainer->pMaterials[0].pTextureFilename = NULL;
        memset(&pMeshContainer->pMaterials[0].MatD3D,0,sizeof(D3DMATERIAL9));
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
    }

    // if there is skinning information,save off the required data and then setup for HW skinning
    if (pSkinInfo != NULL)
    {
        // first save off the SkinInfo and original mesh data
        pMeshContainer->pSkinInfo = pSkinInfo;
        pSkinInfo->AddRef();

        pMeshContainer->pOrigMesh = pMesh;
        pMesh->AddRef();

        // Will need an array of offset matrices to move the vertices from the figure space to the bone's space
        cBones = pSkinInfo->GetNumBones();
        pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
        if (pMeshContainer->pBoneOffsetMatrices == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // get each of the bone offset matrices so that we don't need to get them later
        for (iBone = 0; iBone < cBones; iBone++)
        {
            pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
        }

        // GenerateSkinnedMesh will take the general skinning information and transform it to a HW friendly version
        hr = m_pSkinMesh->GenerateSkinnedMesh( pd3dDevice,pMeshContainer );
        if (FAILED(hr))
            goto e_Exit;
    }

    *ppNewMeshContainer = pMeshContainer;
    pMeshContainer = NULL;

e_Exit:
    SAFE_RELEASE(pd3dDevice);

    // call Destroy function to properly clean up the memory allocated 
    if (pMeshContainer != NULL)
    {
        DestroyMeshContainer(pMeshContainer);
    }

    return hr;
}

HRESULT CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree) 
{
    SAFE_DELETE_ARRAY( pFrameToFree->Name );
    SAFE_DELETE( pFrameToFree );
    return S_OK; 
}

HRESULT CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
    UINT iMaterial;
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

    SAFE_DELETE_ARRAY( pMeshContainer->Name );
    SAFE_DELETE_ARRAY( pMeshContainer->pAdjacency );
    SAFE_DELETE_ARRAY( pMeshContainer->pMaterials );
    SAFE_DELETE_ARRAY( pMeshContainer->pBoneOffsetMatrices );

    // release all the allocated textures
    if (pMeshContainer->ppTextures != NULL)
    {
        for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
        {
            SAFE_RELEASE( pMeshContainer->ppTextures[iMaterial] );
        }
    }

    SAFE_DELETE_ARRAY( pMeshContainer->ppTextures );
    SAFE_DELETE_ARRAY( pMeshContainer->ppBoneMatrixPtrs );
    SAFE_RELEASE( pMeshContainer->pBoneCombinationBuf );
    SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
    SAFE_RELEASE( pMeshContainer->pSkinInfo );
    SAFE_RELEASE( pMeshContainer->pOrigMesh );
    SAFE_DELETE( pMeshContainer );
    return S_OK;
}


// ================================ CSkinMesh ==================================
CSkinMesh::CSkinMesh(IDirect3DDevice9* pDevice)
{
	m_pDevice = pDevice;
	m_pFrameRoot = NULL;
	m_pAnimController = NULL;
	m_SkinningMethod = D3DNONINDEXED;
	m_pBoneMatrices = NULL;
	m_NumBoneMatricesMax = 0;
	m_pEffect = NULL;  
}

CSkinMesh::~CSkinMesh(void)
{
	SAFE_RELEASE(m_pEffect);
	SAFE_DELETE_ARRAY(m_pBoneMatrices);
	CAllocateHierarchy Alloc(this);
	D3DXFrameDestroy(m_pFrameRoot,&Alloc);
	SAFE_RELEASE(m_pAnimController);
}

HRESULT CSkinMesh::LoadMesh(char* strMeshPath,char* strTexturePath)
{
	HRESULT    hr;
	m_strTexturePath = strTexturePath;
	CAllocateHierarchy Alloc(this);   // 创建Frame层次结构体系
	
	hr = D3DXLoadMeshHierarchyFromX(
		strMeshPath,            // 骨骼动画.X文件名
		D3DXMESH_MANAGED,       // X文件内存存放位置
		m_pDevice,
		&Alloc,					 // Frame层次对象生成接口
		NULL,
		&m_pFrameRoot,          // 返回具有层次结构的根Frame的指针
		&m_pAnimController);	 // 返回相应的动画控制器
	if (FAILED(hr))
		return hr;

	hr = _SetupBoneMatrixPointers(m_pFrameRoot);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void CSkinMesh::Render(D3DXMATRIX* matWorld)
{
	_UpdateFrameMatrices(m_pFrameRoot,matWorld);
	_DrawFrame(m_pFrameRoot);
}

bool CSkinMesh::CloneAnim(ID3DXAnimationController** ppAC)
{
	if (FAILED(m_pAnimController->CloneAnimationController(
			m_pAnimController->GetMaxNumAnimationOutputs(),
			m_pAnimController->GetMaxNumAnimationSets(),
			m_pAnimController->GetMaxNumTracks(),
			m_pAnimController->GetMaxNumEvents(),
			ppAC)))
			return false;
	return true;
}

HRESULT CSkinMesh::GenerateSkinnedMesh( IDirect3DDevice9 *pd3dDevice,D3DXMESHCONTAINER_DERIVED *pMeshContainer )
{
    HRESULT hr = S_OK;
    D3DCAPS9 d3dCaps;
    pd3dDevice->GetDeviceCaps( &d3dCaps );

    if( pMeshContainer->pSkinInfo == NULL )
        return hr;

    m_bUseSoftwareVP = false;

    SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
    SAFE_RELEASE( pMeshContainer->pBoneCombinationBuf );

    // if non-indexed skinning mode selected,use ConvertToBlendedMesh to generate drawable mesh
    if( m_SkinningMethod == D3DNONINDEXED )
    {

        hr = pMeshContainer->pSkinInfo->ConvertToBlendedMesh
                                   (
                                       pMeshContainer->pOrigMesh,
                                       D3DXMESH_MANAGED|D3DXMESHOPT_VERTEXCACHE,
                                       pMeshContainer->pAdjacency,
                                       NULL,NULL,NULL,
                                       &pMeshContainer->NumInfl,
                                       &pMeshContainer->NumAttributeGroups,
                                       &pMeshContainer->pBoneCombinationBuf,
                                       &pMeshContainer->MeshData.pMesh
                                   );
        if (FAILED(hr))
            goto e_Exit;


        // If the device can only do 2 matrix blends,ConvertToBlendedMesh cannot approximate all meshes to it
        // Thus we split the mesh in two parts: The part that uses at most 2 matrices and the rest. The first is
        // drawn using the device's HW vertex processing and the rest is drawn using SW vertex processing.
        LPD3DXBONECOMBINATION rgBoneCombinations  = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

        // look for any set of bone combinations that do not fit the caps
        for (pMeshContainer->iAttributeSW = 0; pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups; pMeshContainer->iAttributeSW++)
        {
            DWORD cInfl   = 0;

            for (DWORD iInfl = 0; iInfl < pMeshContainer->NumInfl; iInfl++)
            {
                if (rgBoneCombinations[pMeshContainer->iAttributeSW].BoneId[iInfl] != UINT_MAX)
                {
                    ++cInfl;
                }
            }

            if (cInfl > d3dCaps.MaxVertexBlendMatrices)
            {
                break;
            }
        }

        // if there is both HW and SW,add the Software Processing flag
        if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
        {
            ID3DXMesh* pMeshTmp;

            hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(D3DXMESH_SOFTWAREPROCESSING|pMeshContainer->MeshData.pMesh->GetOptions(),
                                                pMeshContainer->MeshData.pMesh->GetFVF(),
                                                pd3dDevice,&pMeshTmp);
            if (FAILED(hr))
            {
                goto e_Exit;
            }

            pMeshContainer->MeshData.pMesh->Release();
            pMeshContainer->MeshData.pMesh = pMeshTmp;
            pMeshTmp = NULL;
        }
    }
    // if indexed skinning mode selected,use ConvertToIndexedsBlendedMesh to generate drawable mesh
    else if( m_SkinningMethod == D3DINDEXED )
    {
        DWORD NumMaxFaceInfl;
        DWORD Flags = D3DXMESHOPT_VERTEXCACHE;

        IDirect3DIndexBuffer9* pIB;
        hr = pMeshContainer->pOrigMesh->GetIndexBuffer(&pIB);
        if (FAILED(hr))
            goto e_Exit;

        hr = pMeshContainer->pSkinInfo->GetMaxFaceInfluences(pIB,pMeshContainer->pOrigMesh->GetNumFaces(),&NumMaxFaceInfl);
        pIB->Release();
        if (FAILED(hr))
            goto e_Exit;

        // 12 entry palette guarantees that any triangle (4 independent influences per vertex of a tri)
        // can be handled
        NumMaxFaceInfl = min(NumMaxFaceInfl,12);

        if( d3dCaps.MaxVertexBlendMatrixIndex + 1 < NumMaxFaceInfl )
        {
            // HW does not support indexed vertex blending. Use SW instead
            pMeshContainer->NumPaletteEntries = min(256,pMeshContainer->pSkinInfo->GetNumBones());
            pMeshContainer->UseSoftwareVP = true;
            m_bUseSoftwareVP = true;
            Flags |= D3DXMESH_SYSTEMMEM;
        }
        else
        {
            // using hardware - determine palette size from caps and number of bones
            // If normals are present in the vertex data that needs to be blended for lighting,then 
            // the number of matrices is half the number specified by MaxVertexBlendMatrixIndex.
            pMeshContainer->NumPaletteEntries = min( ( d3dCaps.MaxVertexBlendMatrixIndex + 1 ) / 2,
                                                     pMeshContainer->pSkinInfo->GetNumBones() );
            pMeshContainer->UseSoftwareVP = false;
            Flags |= D3DXMESH_MANAGED;
        }

        hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
                                                (
                                                pMeshContainer->pOrigMesh,
                                                Flags,
                                                pMeshContainer->NumPaletteEntries,
                                                pMeshContainer->pAdjacency,
                                                NULL,NULL,NULL,
                                                &pMeshContainer->NumInfl,
                                                &pMeshContainer->NumAttributeGroups,
                                                &pMeshContainer->pBoneCombinationBuf,
                                                &pMeshContainer->MeshData.pMesh);
        if (FAILED(hr))
            goto e_Exit;
    }
    // if vertex shader indexed skinning mode selected,use ConvertToIndexedsBlendedMesh to generate drawable mesh
    else if( ( m_SkinningMethod == D3DINDEXEDVS ) || ( m_SkinningMethod == D3DINDEXEDHLSLVS ) )
    {
        // Get palette size
        // First 9 constants are used for other data.  Each 4x3 matrix takes up 3 constants.
        // (96 - 9) /3 i.e. Maximum constant count - used constants 
        UINT MaxMatrices = 26; 
        pMeshContainer->NumPaletteEntries = min(MaxMatrices,pMeshContainer->pSkinInfo->GetNumBones());

        DWORD Flags = D3DXMESHOPT_VERTEXCACHE;
        if (d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1,1))
        {
            pMeshContainer->UseSoftwareVP = false;
            Flags |= D3DXMESH_MANAGED;
        }
        else
        {
            pMeshContainer->UseSoftwareVP = true;
            m_bUseSoftwareVP = true;
            Flags |= D3DXMESH_SYSTEMMEM;
        }

        SAFE_RELEASE(pMeshContainer->MeshData.pMesh);

        hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
                                                (
                                                pMeshContainer->pOrigMesh,
                                                Flags,
                                                pMeshContainer->NumPaletteEntries,
                                                pMeshContainer->pAdjacency,
                                                NULL,NULL,NULL,            
                                                &pMeshContainer->NumInfl,
                                                &pMeshContainer->NumAttributeGroups,
                                                &pMeshContainer->pBoneCombinationBuf,
                                                &pMeshContainer->MeshData.pMesh);
        if (FAILED(hr))
            goto e_Exit;


        // FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
        DWORD NewFVF = (pMeshContainer->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
        if (NewFVF != pMeshContainer->MeshData.pMesh->GetFVF())
        {
            ID3DXMesh* pMesh;
            hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(),NewFVF,pd3dDevice,&pMesh);
            if (!FAILED(hr))
            {
                pMeshContainer->MeshData.pMesh->Release();
                pMeshContainer->MeshData.pMesh = pMesh;
                pMesh = NULL;
            }
        }

        D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
        LPD3DVERTEXELEMENT9 pDeclCur;
        hr = pMeshContainer->MeshData.pMesh->GetDeclaration(pDecl);
        if (FAILED(hr))
            goto e_Exit;

        // the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR,so update the type 
        //   NOTE: this cannot be done with CloneMesh,that would convert the UBYTE4 data to float and then to D3DCOLOR
        //          this is more of a "cast" operation
        pDeclCur = pDecl;
        while (pDeclCur->Stream != 0xff)
        {
            if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
                pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
            pDeclCur++;
        }

        hr = pMeshContainer->MeshData.pMesh->UpdateSemantics(pDecl);
        if (FAILED(hr))
            goto e_Exit;

        // allocate a buffer for bone matrices,but only if another mesh has not allocated one of the same size or larger
        if( m_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones() )
        {
            m_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

            // Allocate space for blend matrices
            delete[] m_pBoneMatrices; 
            m_pBoneMatrices  = new D3DXMATRIXA16[m_NumBoneMatricesMax];
            if( m_pBoneMatrices == NULL )
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
        }

    }
    // if software skinning selected,use GenerateSkinnedMesh to create a mesh that can be used with UpdateSkinnedMesh
    else if( m_SkinningMethod == SOFTWARE )
    {
        hr = pMeshContainer->pOrigMesh->CloneMeshFVF(D3DXMESH_MANAGED,pMeshContainer->pOrigMesh->GetFVF(),
                                              pd3dDevice,&pMeshContainer->MeshData.pMesh);
        if (FAILED(hr))
            goto e_Exit;

        hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(NULL,&pMeshContainer->NumAttributeGroups);
        if (FAILED(hr))
            goto e_Exit;

        delete[] pMeshContainer->pAttributeTable;
        pMeshContainer->pAttributeTable  = new D3DXATTRIBUTERANGE[pMeshContainer->NumAttributeGroups];
        if (pMeshContainer->pAttributeTable == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(pMeshContainer->pAttributeTable,NULL);
        if (FAILED(hr))
            goto e_Exit;

        // allocate a buffer for bone matrices,but only if another mesh has not allocated one of the same size or larger
        if (m_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones())
        {
            m_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

            // Allocate space for blend matrices
            delete[] m_pBoneMatrices; 
            m_pBoneMatrices  = new D3DXMATRIXA16[m_NumBoneMatricesMax];
            if( m_pBoneMatrices == NULL )
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
        }
    }
    else  // invalid m_SkinningMethod value
    {        
        // return failure due to invalid skinning method value
        hr = E_INVALIDARG;
        goto e_Exit;
    }

e_Exit:
    return hr;
}

HRESULT CSkinMesh::_SetupBoneMatrixPointers(LPD3DXFRAME pFrame)
{
	HRESULT hr;

	if (pFrame->pMeshContainer != NULL)
	{	
		hr = _SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer);
		if (FAILED(hr))
			return hr;
	}

	if (pFrame->pFrameSibling != NULL)
	{
		hr = _SetupBoneMatrixPointers(pFrame->pFrameSibling);
		if (FAILED(hr))
			return hr;
	}

	if (pFrame->pFrameFirstChild != NULL)
	{
		hr = _SetupBoneMatrixPointers(pFrame->pFrameFirstChild);
		if (FAILED(hr))
			return hr;
	}
	return S_OK;
}

HRESULT CSkinMesh::_SetupBoneMatrixPointersOnMesh(D3DXMESHCONTAINER* pMeshContainerBase)
{
	UINT iBone,cBones;
	D3DXFRAME_DERIVED* pFrame;
	D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

	// 蒙皮动画,则设置骨骼的变换矩阵
	if (pMeshContainer->pSkinInfo != NULL)
	{
		cBones = pMeshContainer->pSkinInfo->GetNumBones();
		// 创建矩阵指针数组
		pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];
		// 创建骨骼
		if (pMeshContainer->ppBoneMatrixPtrs == NULL)
			return E_OUTOFMEMORY;

		for (iBone=0;iBone<cBones;iBone++)
		{
			// 通过蒙皮信息接口(SkinInfo)中的GetBoneName函数获取每个绑定
			// 的骨骼的名称,然后在帧中查找名称对应的帧(每个帧对应于一个骨骼)
			pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrameRoot,pMeshContainer->pSkinInfo->GetBoneName(iBone));

			if (pFrame == NULL)
				return E_FAIL;
			// 初始化矩阵指针
			pMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;
		}
	}
	return S_OK;
}

void CSkinMesh::_DrawFrame(LPD3DXFRAME pFrame)
{
	LPD3DXMESHCONTAINER pMeshContainer;

	pMeshContainer = pFrame->pMeshContainer;
	while (pMeshContainer != NULL)
	{
		_DrawMeshContainer(m_pDevice,pMeshContainer,pFrame);
		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}

	if (pFrame->pFrameSibling != NULL)
	{
		_DrawFrame(pFrame->pFrameSibling);
	}

	if (pFrame->pFrameFirstChild != NULL)
	{
		_DrawFrame(pFrame->pFrameFirstChild);
	}
}

void CSkinMesh::_DrawMeshContainer( IDirect3DDevice9 *pd3dDevice,LPD3DXMESHCONTAINER pMeshContainerBase,LPD3DXFRAME pFrameBase )
{
    HRESULT hr;
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
    D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
    UINT iMaterial;
    UINT NumBlend;
    UINT iAttrib;
    DWORD AttribIdPrev;
    LPD3DXBONECOMBINATION pBoneComb;

    UINT iMatrixIndex;
    UINT iPaletteEntry;
    D3DXMATRIXA16 matTemp;
    D3DCAPS9 d3dCaps;
    pd3dDevice->GetDeviceCaps( &d3dCaps );

    // first check for skinning
    if (pMeshContainer->pSkinInfo != NULL)
    {
        if( m_SkinningMethod == D3DNONINDEXED )
        {
            AttribIdPrev = UNUSED32; 
            pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

            // Draw using default vtx processing of the device (typically HW)
            for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
            {
                NumBlend = 0;
                for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
                {
                    if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
                    {
                        NumBlend = i;
                    }
                }

                if( d3dCaps.MaxVertexBlendMatrices >= NumBlend + 1 )
                {
                    // first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
                    for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
                    {
                        iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
                        if (iMatrixIndex != UINT_MAX)
                        {
                            D3DXMatrixMultiply( &matTemp,&pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
                            V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ),&matTemp ) );
                        }
                    }

                    V( pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,NumBlend) );

                    // lookup the material used for this subset of faces
                    if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32))
                    {
                        V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D ) );
                        V( pd3dDevice->SetTexture( 0,pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] ) );
                        AttribIdPrev = pBoneComb[iAttrib].AttribId;
                    }

                    // draw the subset now that the correct material and matrices are loaded
                    V( pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib) );
                }
            }

            // If necessary,draw parts that HW could not handle using SW
            if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
            {
                AttribIdPrev = UNUSED32; 
                V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
                for (iAttrib = pMeshContainer->iAttributeSW; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
                {
                    NumBlend = 0;
                    for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
                    {
                        if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
                        {
                            NumBlend = i;
                        }
                    }

                    if (d3dCaps.MaxVertexBlendMatrices < NumBlend + 1)
                    {
                        // first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
                        for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
                        {
                            iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
                            if (iMatrixIndex != UINT_MAX)
                            {
                                D3DXMatrixMultiply( &matTemp,&pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
                                V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ),&matTemp ) );
                            }
                        }

                        V( pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,NumBlend) );

                        // lookup the material used for this subset of faces
                        if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32))
                        {
                            V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D ) );
                            V( pd3dDevice->SetTexture( 0,pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] ) );
                            AttribIdPrev = pBoneComb[iAttrib].AttribId;
                        }

                        // draw the subset now that the correct material and matrices are loaded
                        V( pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib) );
                    }
                }
                V( pd3dDevice->SetSoftwareVertexProcessing( FALSE ) );
            }

            V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,0) );
        }
        else if (m_SkinningMethod == D3DINDEXED)
        {
            // if hw doesn't support indexed vertex processing,switch to software vertex processing
            if (pMeshContainer->UseSoftwareVP)
            {
                // If hw or pure hw vertex processing is forced,we can't render the
                // mesh,so just exit out.  Typical applications should create
                // a device with appropriate vertex processing capability for this
                // skinning method.
                if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
                    return;

                V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
            }

            // set the number of vertex blend indices to be blended
            if (pMeshContainer->NumInfl == 1)
            {
                V( pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_0WEIGHTS) );
            }
            else
            {
                V( pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,pMeshContainer->NumInfl - 1) );
            }

            if (pMeshContainer->NumInfl)
                V( pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE,TRUE) );

            // for each attribute group in the mesh,calculate the set of matrices in the palette and then draw the mesh subset
            pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
            for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
            {
                // first calculate all the world matrices
                for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
                {
                    iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
                    if (iMatrixIndex != UINT_MAX)
                    {
                        D3DXMatrixMultiply( &matTemp,&pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
                        V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( iPaletteEntry ),&matTemp ) );
                    }
                }
                
                // setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
                V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D ) );
                V( pd3dDevice->SetTexture( 0,pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] ) );

                // finally draw the subset with the current world matrix palette and material state
                V( pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib ) );
            }

            // reset blending state
            V( pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE,FALSE) );
            V( pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,0) );

            // remember to reset back to hw vertex processing if software was required
            if (pMeshContainer->UseSoftwareVP)
            {
                V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
            }
        }
        else if (m_SkinningMethod == D3DINDEXEDVS) 
        {
            // Use COLOR instead of UBYTE4 since Geforce3 does not support it
            // vConst.w should be 3,but due to COLOR/UBYTE4 issue,mul by 255 and add epsilon
            D3DXVECTOR4 vConst( 1.0f,0.0f,0.0f,765.01f );

            if (pMeshContainer->UseSoftwareVP)
            {
                // If hw or pure hw vertex processing is forced,we can't render the
                // mesh,so just exit out.  Typical applications should create
                // a device with appropriate vertex processing capability for this
                // skinning method.
                if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
                    return;

                V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
            }

            V( pd3dDevice->SetVertexShader( m_pIndexedVertexShader[pMeshContainer->NumInfl - 1] ) );

            pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
            for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
            {
                // first calculate all the world matrices
                for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
                {
                    iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
                    if (iMatrixIndex != UINT_MAX)
                    {
                        D3DXMatrixMultiply(&matTemp,&pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
                        D3DXMatrixMultiplyTranspose(&matTemp,&matTemp,&m_matView);
                        V( pd3dDevice->SetVertexShaderConstantF(iPaletteEntry*3 + 9,(float*)&matTemp,3) );
                    }
                }

                // Sum of all ambient and emissive contribution
                D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
                D3DXCOLOR color2(.25,.25,.25,1.0);
                D3DXCOLOR ambEmm;
                D3DXColorModulate(&ambEmm,&color1,&color2);
                ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);

                // set material color properties 
                V( pd3dDevice->SetVertexShaderConstantF(8,(float*)&(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Diffuse),1) );
                V( pd3dDevice->SetVertexShaderConstantF(7,(float*)&ambEmm,1) );
                vConst.y = pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Power;
                V( pd3dDevice->SetVertexShaderConstantF(0,(float*)&vConst,1) );

                V( pd3dDevice->SetTexture(0,pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]) );

                // finally draw the subset with the current world matrix palette and material state
                V( pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib ) );
            }

            // remember to reset back to hw vertex processing if software was required
            if (pMeshContainer->UseSoftwareVP)
            {
                V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
            }
            V( pd3dDevice->SetVertexShader( NULL ) );
        }
        else if (m_SkinningMethod == D3DINDEXEDHLSLVS) 
        {
            if (pMeshContainer->UseSoftwareVP)
            {
                // If hw or pure hw vertex processing is forced,we can't render the
                // mesh,so just exit out.  Typical applications should create
                // a device with appropriate vertex processing capability for this
                // skinning method.
                if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
                    return;

                V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
            }

            pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
            for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
            { 
                // first calculate all the world matrices
                for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
                {
                    iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
                    if (iMatrixIndex != UINT_MAX)
                    {
                        D3DXMatrixMultiply(&matTemp,&pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
                        D3DXMatrixMultiply(&m_pBoneMatrices[iPaletteEntry],&matTemp,&m_matView);
                    }
                }
                V( m_pEffect->SetMatrixArray( "mWorldMatrixArray",m_pBoneMatrices,pMeshContainer->NumPaletteEntries) );

                // Sum of all ambient and emissive contribution
                D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
                D3DXCOLOR color2(.25,.25,.25,1.0);
                D3DXCOLOR ambEmm;
                D3DXColorModulate(&ambEmm,&color1,&color2);
                ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);

                // set material color properties 
                V( m_pEffect->SetVector("MaterialDiffuse",(D3DXVECTOR4*)&(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Diffuse)) );
                V( m_pEffect->SetVector("MaterialAmbient",(D3DXVECTOR4*)&ambEmm) );

                // setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
                V( pd3dDevice->SetTexture( 0,pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] ) );

                // Set CurNumBones to select the correct vertex shader for the number of bones
                V( m_pEffect->SetInt( "CurNumBones",pMeshContainer->NumInfl -1) );

                // Start the effect now all parameters have been updated
                UINT numPasses;
                V( m_pEffect->Begin( &numPasses,D3DXFX_DONOTSAVESTATE ) );
                for( UINT iPass = 0; iPass < numPasses; iPass++ )
                {
                    V( m_pEffect->BeginPass( iPass ) );

                    // draw the subset with the current world matrix palette and material state
                    V( pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib ) );

                    V( m_pEffect->EndPass() );
                }

                V( m_pEffect->End() );

                V( pd3dDevice->SetVertexShader(NULL) );
            }

            // remember to reset back to hw vertex processing if software was required
            if (pMeshContainer->UseSoftwareVP)
            {
                V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
            }
        }
        else if (m_SkinningMethod == SOFTWARE)
        {
            D3DXMATRIX  Identity;
            DWORD       cBones  = pMeshContainer->pSkinInfo->GetNumBones();
            DWORD       iBone;
            PBYTE       pbVerticesSrc;
            PBYTE       pbVerticesDest;

            // set up bone transforms
            for (iBone = 0; iBone < cBones; ++iBone)
            {
                D3DXMatrixMultiply
                (
                    &m_pBoneMatrices[iBone],                // output
                    &pMeshContainer->pBoneOffsetMatrices[iBone],
                    pMeshContainer->ppBoneMatrixPtrs[iBone]
                );
            }

            // set world transform
            D3DXMatrixIdentity(&Identity);
            V( pd3dDevice->SetTransform(D3DTS_WORLD,&Identity) );

            V( pMeshContainer->pOrigMesh->LockVertexBuffer(D3DLOCK_READONLY,(LPVOID*)&pbVerticesSrc) );
            V( pMeshContainer->MeshData.pMesh->LockVertexBuffer(0,(LPVOID*)&pbVerticesDest) );

            // generate skinned mesh
            pMeshContainer->pSkinInfo->UpdateSkinnedMesh(m_pBoneMatrices,NULL,pbVerticesSrc,pbVerticesDest);

            V( pMeshContainer->pOrigMesh->UnlockVertexBuffer() );
            V( pMeshContainer->MeshData.pMesh->UnlockVertexBuffer() );

            for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
            {
                V( pd3dDevice->SetMaterial(&(pMeshContainer->pMaterials[pMeshContainer->pAttributeTable[iAttrib].AttribId].MatD3D)) );
                V( pd3dDevice->SetTexture(0,pMeshContainer->ppTextures[pMeshContainer->pAttributeTable[iAttrib].AttribId]) );
                V( pMeshContainer->MeshData.pMesh->DrawSubset(pMeshContainer->pAttributeTable[iAttrib].AttribId) );
            }
        }
        else // bug out as unsupported mode
        {
            return;
        }
    }
    else  // standard mesh,just draw it after setting material properties
    {
        V( pd3dDevice->SetTransform(D3DTS_WORLD,&pFrame->CombinedTransformationMatrix) );

        for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
        {
            V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D ) );
            V( pd3dDevice->SetTexture( 0,pMeshContainer->ppTextures[iMaterial] ) );
            V( pMeshContainer->MeshData.pMesh->DrawSubset(iMaterial) );
        }
    }
}

void CSkinMesh::_UpdateFrameMatrices(LPD3DXFRAME pFrameBase,LPD3DXMATRIX pParentMatrix)
{
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

	if (pParentMatrix != NULL)
		D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix,
		&pFrame->TransformationMatrix,pParentMatrix);
	else
		pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;

	if (pFrame->pFrameSibling != NULL)
	{
		_UpdateFrameMatrices(pFrame->pFrameSibling,pParentMatrix);
	}

	if (pFrame->pFrameFirstChild != NULL)
	{
		_UpdateFrameMatrices(pFrame->pFrameFirstChild,&pFrame->CombinedTransformationMatrix);
	}
}

bool CSkinMesh::GetBoundingB_S(D3DXVECTOR3* pMax,D3DXVECTOR3* pMin,D3DXVECTOR3* pCenter,float* pRadius)
{
	ID3DXMesh* temMesh = GetMesh();
	
	HRESULT hr = 0;
	D3DXVECTOR3 tmpMin,tmpMax;
	BYTE* pV;
	temMesh->LockVertexBuffer(0,(void**)&pV);
	hr = D3DXComputeBoundingBox(
		(D3DXVECTOR3*)pV,                         // 指向在顶点数组中第一个顶点的向量
		temMesh->GetNumVertices(),	              // 在顶点数组中的的顶点数。
		D3DXGetFVFVertexSize(temMesh->GetFVF()),  // 每个顶点的字节大小
		&tmpMin,								  // 围盒的最小点
		&tmpMax);								  // 围盒的最大点
	temMesh->UnlockVertexBuffer();

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

	if (FAILED(hr))
		return false;
	return true;
}

ID3DXMesh* CSkinMesh::GetMesh()
{
	ID3DXMesh* pMesh = NULL;
	if (_GetMesh(m_pFrameRoot,pMesh))
		return pMesh;
	return NULL;
}

bool CSkinMesh::_GetMesh(LPD3DXFRAME pFrame,ID3DXMesh* &pMesh)
{
	if (pFrame->pMeshContainer)
	{
		pMesh = pFrame->pMeshContainer->MeshData.pMesh;
		if (pMesh != NULL)
			return true;
	}
	if (pFrame->pFrameSibling)
		return _GetMesh(pFrame->pFrameSibling,pMesh);
	if (pFrame->pFrameFirstChild)
		return _GetMesh(pFrame->pFrameFirstChild,pMesh);
	return false;
}

void CSkinMesh::GetBoneMatrix(const char* boneName,D3DXMATRIX** ppMatOut)
{
	D3DXFRAME_DERIVED *t_pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrameRoot,boneName);
	
	if(t_pFrame)
		*ppMatOut = &t_pFrame->CombinedTransformationMatrix;
}