#pragma once

#include "common.h"
#include "device.h"
#include "input.h"
#include "camera.h"
#include "billboard.h"
#include "meshmanager.h"
#include "scene.h"
#include "startui.h"

enum fgeBoolState
{
	FGE_SETWINDOWS		= 1,  // bool		�ô�����ʾ?		(default: true)
	FGE_SETZBUFFER		= 2,  // bool		ʹ����Ȼ���?		(default: true)
	FGE_SETMULTSAMPLE	= 3,  // bool		ʹ��ͼ�η����?		(default: false)
};

enum fgeStringState
{
	FGE_SETTITLE		= 1,   // char*	window title		(default: "FGE")
	FGE_SETICON			= 2,   // char*	icon resource		(default: NULL)
	FGE_SETICONSM		= 3,   // char*	icon resource		(default: NULL)
};

class CFrantoGE
{
private:
	HINSTANCE			hInst;					// Ӧ�ó�����
	HWND				hWnd;
	int                 iWinWidth;              // ���ڿ��
	int                 iWinHeight;             // ���ڸ߶�

	// ϵͳ�豸
	CDevice*			pD3D;					// D3D�����豸ָ��
	IDirect3DDevice9*	pDevice;
	CInput*				pInput;					// ������ָ��
	CCamera*			pCamera;				// �������ָ��
	void				(*pRender)();			// ��Ⱦ����ָ��

	// ϵͳ��ʼ������
	bool				bWindow;				// �Ƿ��ô���
	bool				bZBuffer;				// �Ƿ��������
	bool				bMultsample;			// �Ƿ�ʹ��ͼ�η����
	char				cTitle[128];     		// ��������
	char*				cIcon;
	char*				cIconSm;

	// ��Ϸ������ָ��
	//CMeshManager*		pMeshManager;           // ģ�͹���ָ��
	//CScene*             pScene;                 // ��������ָ��

public:
	CFrantoGE(void);
	~CFrantoGE(void);

	static LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);// ��Ϣ�ص�����

	// ϵͳ���к���
	//δʵ��bool                   System_LoadInitFile(char* SceneFile); // ͨ�������ļ�������ϵͳ��ʼ������
	INT WINAPI             System_Init(void (*pRender)());     // ϵͳ��ʼ����������ʼ�����ڡ��豸���������
	void                   System_Start();                     // ϵͳ��ʼ������������Ϣѭ��

	// ϵͳ�������ú���
	void                   System_SetStateBool(fgeBoolState state,bool value);       // �Ƿ�������ģʽ����Ȼ��塢ͼ�η���ݵ�
	void                   System_SetStateString(fgeStringState state,char* value);
	void                   System_SetWindowSize(int width,int height) {iWinWidth = width;iWinHeight = height;}

	// ��ȡϵͳ��������
	IDirect3DDevice9*      GetD3DDevice() {return pDevice;}
	CInput*                GetInput() {return pInput;}
	CCamera*               GetCamera() {return pCamera;}
	HWND                   GetHWND() {return hWnd;}
	int                    GetWinWidth() {return iWinWidth;}
	int                    GetWinHeight() {return iWinHeight;}

	// ��Ϸ������
	//void SceneUpdate();
	//void SceneRender();                      // ������Ⱦ
	bool AddMeshFromFile(char* MeshFile,CMeshManager* &pOut);     // ͨ�������ļ�����ģ��
	bool AddSceneFromFile(char* SceneFile,CMeshManager* pMeshManager,CScene* &pOut);    // ͨ�������ļ����س���
};
