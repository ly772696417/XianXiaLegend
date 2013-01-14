#pragma once
#include "common.h"
#include "input.h"
#include "billboard.h"

class CStartUI
{
private:
	IDirect3DDevice9* m_pDevice;
	CInput* m_pInput;
	CBillBoard* m_pStartUI;
	DOUBLE m_dTime;
	DOUBLE m_dTimeCount;

public:
	CStartUI(IDirect3DDevice9* pDevice,CInput* pInput);
	~CStartUI(void);

	bool Init(char* SceneFile,char* AddName);
	void Update(int& State,DOUBLE dElapsedTime);
	void Rander(DOUBLE dElapsedTime);
};
