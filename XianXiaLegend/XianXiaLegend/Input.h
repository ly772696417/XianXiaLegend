#pragma once
#include "Common.h"
#pragma comment(lib,"dinput8.lib")
#include "Dinput.h"

class CInput
{
private:
	IDirect3DDevice9* m_pDevice;
	IDirectInput8* m_pInput;
	IDirectInputDevice8* m_pKey;
	IDirectInputDevice8* m_pMouse;
	HWND m_hWnd;

	char m_keyState[256];		//µ±Ç°¼üÅÌ×´Ì¬
	char m_keyStateOld[256];	//ÉÏÒ»Ö¡¼üÅÌ×´Ì¬
	DIMOUSESTATE m_MouseState;
	DIMOUSESTATE m_MouseStateOld;


	CInput(IDirect3DDevice9* pDevice);
	~CInput(void);
	friend class CFrantoGE;

	void Init(HINSTANCE hInst,HWND hWnd);

public:	
	void Update();
	BOOL KeyDown(int keyValue);
	BOOL KeyHold(int keyValue);
	BOOL KeyUp(int keyValue);

	BOOL MouseDown(int keyValue);
	BOOL MouseHold(int keyValue);
	BOOL MouseUp(int keyValue);

	LONG GetX(){return m_MouseState.lX;}
	LONG GetY(){return m_MouseState.lY;}
	LONG GetZ(){return m_MouseState.lZ;}

	HWND GetHWnd() {return m_hWnd;}
};
