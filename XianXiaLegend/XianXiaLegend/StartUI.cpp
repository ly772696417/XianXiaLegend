#include "StartUI.h"

CStartUI::CStartUI(IDirect3DDevice9* pDevice,CInput* pInput)
{
	m_pDevice = pDevice;
	m_pInput = pInput;
	m_pStartUI = new CBillBoard(m_pDevice);
	m_dTime = 8;
	m_dTimeCount = 0;
}

CStartUI::~CStartUI(void)
{
	SAFE_DELETE(m_pStartUI);
}

bool CStartUI::Init(char* SceneFile,char* AddName)
{
	if (m_pStartUI->Init(SceneFile,AddName) == false)
		return false;

	m_pStartUI->SetMode(PARALLEL);
	return true;
}

void CStartUI::Update(int& State,DOUBLE dElapsedTime)
{
	if (m_pInput->KeyDown(DIK_RETURN) || m_pInput->MouseDown(0))
	{
		State = 2;
	}
	if (m_dTimeCount < m_dTime)
	{
		m_dTimeCount += dElapsedTime;
	}
	else
	{
		State = 2;
		m_dTimeCount = 0;
	}
}

void CStartUI::Rander(DOUBLE dElapsedTime)
{
	m_pStartUI->Render(dElapsedTime);
}