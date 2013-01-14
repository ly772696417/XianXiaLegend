#include "Deck.h"

CDeck::CDeck(IDirect3DDevice9* pDevice,CMeshManager* pMeshManager)
{
	m_pDevice = pDevice;
	m_pMeshManager = pMeshManager;
}

CDeck::~CDeck(void)
{
}

bool CDeck::Init(char* SceneFile,char* AddName)
{
	if (LoadDataFromFile(SceneFile,AddName) == false)
		return false;
	m_pMeshManager->GetMeshFromID(m_iMeshID)->GetBoundingB_S(&m_vOrigMax,&m_vOrigMin,&m_vCenter,&m_fRadius);
	m_fRadius *= m_vScal.x;
	return true;
}

void CDeck::Update()
{
	Transform();
}

void CDeck::Render(D3DXMATRIX* matWorld)
{
	DWORD AlphaText,AlphaBlend;
	m_pDevice->GetRenderState(D3DRS_ALPHATESTENABLE,&AlphaText);
	m_pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,&AlphaBlend);
	switch (m_iMeshID)
	{
	case 4:
		{
			m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,true);
			m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
			m_pDevice->SetRenderState(D3DRS_BLENDOPALPHA,D3DBLENDOP_ADD);
			m_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
			m_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_SRCCOLOR);
		}
		break;
	case 7:
		{
			m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,true);
			m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
			m_pDevice->SetRenderState(D3DRS_BLENDOPALPHA,D3DBLENDOP_ADD);
			m_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR);
			m_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		}
		break;
	default:
		break;
	}

	m_pDevice->SetTransform(D3DTS_WORLD,matWorld);
	m_pMeshManager->GetMeshFromID(m_iMeshID)->Render();

	m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,AlphaText);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,AlphaBlend);
}

bool CDeck::LoadDataFromFile(char* SceneFile,char* AddName)
{
	return CMyObject::LoadDataFromFile(SceneFile,AddName);
}