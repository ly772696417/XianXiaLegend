/*
*	动态模型类
*/
#pragma once
#include "MyFuntion.h"

#define IDLE_TRANSITION_TIME 0.125f

enum METHOD
{
    D3DNONINDEXED,
    D3DINDEXED,
    SOFTWARE,
    D3DINDEXEDVS,
    D3DINDEXEDHLSLVS,
    NONE
};

struct D3DXFRAME_DERIVED: public D3DXFRAME
{
    D3DXMATRIXA16        CombinedTransformationMatrix;
};

struct D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER
{
    IDirect3DTexture9**  ppTextures;       // array of textures,entries are NULL if no texture specified    
                                
    // SkinMesh info             
    ID3DXMesh*           pOrigMesh;
    LPD3DXATTRIBUTERANGE pAttributeTable;
    DWORD                NumAttributeGroups; 
    DWORD                NumInfl;
    ID3DXBuffer*         pBoneCombinationBuf;
    D3DXMATRIX**         ppBoneMatrixPtrs;
    D3DXMATRIX*          pBoneOffsetMatrices;
    DWORD                NumPaletteEntries;
    bool                 UseSoftwareVP;
    DWORD                iAttributeSW;     // used to denote the split between SW and HW if necessary for non-indexed skinning
};
class CSkinMesh;

class CAllocateHierarchy : public ID3DXAllocateHierarchy
{
public:
    STDMETHOD(CreateFrame)(THIS_ LPCSTR Name,LPD3DXFRAME *ppNewFrame);

    STDMETHOD(CreateMeshContainer)(THIS_ 
        LPCSTR Name,
        CONST D3DXMESHDATA *pMeshData,
        CONST D3DXMATERIAL *pMaterials,
        CONST D3DXEFFECTINSTANCE *pEffectInstances,
        DWORD NumMaterials,
        CONST DWORD *pAdjacency,
        LPD3DXSKININFO pSkinInfo,
        LPD3DXMESHCONTAINER *ppNewMeshContainer);

    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);

    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);

	CAllocateHierarchy(CSkinMesh* pSkinMesh):m_pSkinMesh(pSkinMesh) {}

public:
	CSkinMesh* m_pSkinMesh;
};

class CSkinMesh
{
public:
	char*                       m_strTexturePath;         // 纹理相对路径

private:
	IDirect3DDevice9* m_pDevice;
	LPD3DXFRAME					m_pFrameRoot;
	ID3DXAnimationController*	m_pAnimController;        // 动画控制器

	METHOD                      m_SkinningMethod;         // 动画枚举
	D3DXMATRIXA16*              m_pBoneMatrices;		  // 骨骼矩阵
	UINT                        m_NumBoneMatricesMax;	  // 骨骼矩阵最大数

	bool                        m_bUseSoftwareVP;
	D3DXMATRIXA16				m_matView;
	DWORD                       m_dwBehaviorFlags;
	IDirect3DVertexShader9*     m_pIndexedVertexShader[4];
	ID3DXEffect*                m_pEffect;  

public:
	CSkinMesh(IDirect3DDevice9* pDevice);
	~CSkinMesh(void);

	HRESULT LoadMesh(char* strMeshPath,char* strTexturePath);
	void Render(D3DXMATRIX* matWorld);
	// 生成蒙皮
	HRESULT GenerateSkinnedMesh(IDirect3DDevice9* pd3dDevice,D3DXMESHCONTAINER_DERIVED* pMeshContainer);
	// 克隆动画控制器
	bool CloneAnim(ID3DXAnimationController** pAC);
	// 获取包围盒角值
	bool GetBoundingB_S(D3DXVECTOR3* pMax,D3DXVECTOR3* pMin,D3DXVECTOR3* pCenter,float* pRadius);
	ID3DXMesh* GetMesh();
	// 按名称获取骨骼变换矩阵
	void GetBoneMatrix(const char* boneName,D3DXMATRIX** ppMatOut);

private:
	// 遍历Frame，设置ppBoneMatrixPtrs
	HRESULT	_SetupBoneMatrixPointers(LPD3DXFRAME pFrame);
	// 更新MeshContainer的ppBoneMatrixPtrs
	HRESULT _SetupBoneMatrixPointersOnMesh(D3DXMESHCONTAINER* pMeshContainerBase); 
	void _UpdateFrameMatrices(LPD3DXFRAME pFrameBase,LPD3DXMATRIX pParentMatrix);
	void _DrawFrame(LPD3DXFRAME pFrame);
	void _DrawMeshContainer(IDirect3DDevice9 *pd3dDevice,LPD3DXMESHCONTAINER pMeshContainerBase,LPD3DXFRAME pFrameBase );
	bool _GetMesh(LPD3DXFRAME pFrame,ID3DXMesh* &pMesh);
};

