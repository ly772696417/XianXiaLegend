#include "FrantoGE.h"

CFrantoGE::CFrantoGE(void)
{
	pD3D         = NULL;
	pDevice      = NULL;
	pCamera      = NULL;
	pInput       = NULL;
	pRender      = NULL;

	bWindow      = false;
	bZBuffer     = true;
	bMultsample  = false;
	strcpy(cTitle,"XianYuanLegend");
	cIcon        = NULL;
	cIconSm      = NULL;
	iWinWidth    = 800;
	iWinHeight   = 600;

	//pMeshManager = NULL;
	//pScene       = NULL;
}

CFrantoGE::~CFrantoGE(void)
{
	//SAFE_DELETE(pScene);
	//SAFE_DELETE(pMeshManager);
	SAFE_DELETE(pCamera);
	SAFE_DELETE(pInput);
	SAFE_DELETE(pD3D);
}

LRESULT CALLBACK CFrantoGE::WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd,msg,wParam,lParam);
}

INT WINAPI CFrantoGE::System_Init(void (*pRender)())
{
	this->pRender = pRender;

	hInst = GetModuleHandle(NULL);// 获取当前应用程序句柄
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = (HICON)cIcon;
	wcex.hCursor = LoadCursor(NULL,IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "XianYuanLegend";
	wcex.hIconSm = (HICON)cIconSm;
	RegisterClassEx(&wcex);

	HWND hWnd = CreateWindow("XianYuanLegend",cTitle,WS_SYSMENU,
								0,0,iWinWidth + 8,iWinHeight + 26,NULL,NULL,hInst,NULL);

	if (!hWnd)
		return false;

	ShowWindow(hWnd,1);
	UpdateWindow(hWnd);

	pD3D = new CDevice();
	if (pD3D != NULL)
	{
		pD3D->Init(hWnd,bWindow,iWinWidth,iWinHeight,bZBuffer,bMultsample);
		pDevice = pD3D->m_pDevice;
	}

	pInput = new CInput(pDevice);
	if (pInput != NULL)
		pInput->Init(hInst,hWnd);

	pCamera = new CCamera(pDevice,pInput);

	return 0;
}

void CFrantoGE::System_SetStateBool(fgeBoolState state,bool value)
{
	switch(state)
	{
	case 1:
		bWindow = value;
		break;
	case 2:
		bZBuffer = value;
		break;
	case 3:
		bMultsample = value;
	default:
		break;
	}
}

void CFrantoGE::System_SetStateString(fgeStringState state,char* value)
{
	switch(state)
	{
	case 1:
		strcpy(cTitle,value);
		break;
	case 2:

		break;
	case 3:

		break;
	default:
		break;
	}
}

void CFrantoGE::System_Start()
{
	MSG msg;
	ZeroMemory(&msg,sizeof(MSG));
	while(msg.message != WM_QUIT)
	{
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			pRender();
		}
	}
	UnregisterClass("XianXiaLegend",hInst);
}

//void CFrantoGE::SceneUpdate()
//{
//	pScene->Update();
//}

//void CFrantoGE::SceneRender()
//{
//	pScene->Render();
//}

bool CFrantoGE::AddMeshFromFile(char* MeshFile,CMeshManager* &pOut)
{
	pOut = new CMeshManager(pDevice);
	if (pOut->LoadMeshFile(MeshFile))
		return true;
	else
		return false;
}

bool CFrantoGE::AddSceneFromFile(char* SceneFile,CMeshManager* pMeshManager,CScene* &pOut)
{
	if (pMeshManager == NULL)
		return false;
	pOut = new CScene(pDevice,pMeshManager,pCamera,pInput);
	if (pOut->Init(SceneFile))
		return true;
	else
		return false;
}