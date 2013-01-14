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
	FGE_SETWINDOWS		= 1,  // bool		用窗口显示?		(default: true)
	FGE_SETZBUFFER		= 2,  // bool		使用深度缓冲?		(default: true)
	FGE_SETMULTSAMPLE	= 3,  // bool		使用图形反锯齿?		(default: false)
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
	HINSTANCE			hInst;					// 应用程序句柄
	HWND				hWnd;
	int                 iWinWidth;              // 窗口宽度
	int                 iWinHeight;             // 窗口高度

	// 系统设备
	CDevice*			pD3D;					// D3D对象及设备指针
	IDirect3DDevice9*	pDevice;
	CInput*				pInput;					// 输入类指针
	CCamera*			pCamera;				// 摄像机类指针
	void				(*pRender)();			// 渲染函数指针

	// 系统初始化参数
	bool				bWindow;				// 是否用窗口
	bool				bZBuffer;				// 是否启动深度
	bool				bMultsample;			// 是否使用图形反锯齿
	char				cTitle[128];     		// 窗口名称
	char*				cIcon;
	char*				cIconSm;

	// 游戏管理类指针
	//CMeshManager*		pMeshManager;           // 模型管理指针
	//CScene*             pScene;                 // 场景管理指针

public:
	CFrantoGE(void);
	~CFrantoGE(void);

	static LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);// 消息回调函数

	// 系统运行函数
	//未实现bool                   System_LoadInitFile(char* SceneFile); // 通过配置文件，设置系统初始化参数
	INT WINAPI             System_Init(void (*pRender)());     // 系统初始化函数，初始化窗口、设备、摄像机等
	void                   System_Start();                     // 系统开始函数，进入消息循环

	// 系统参数设置函数
	void                   System_SetStateBool(fgeBoolState state,bool value);       // 是否开启窗口模式、深度缓冲、图形反锯齿等
	void                   System_SetStateString(fgeStringState state,char* value);
	void                   System_SetWindowSize(int width,int height) {iWinWidth = width;iWinHeight = height;}

	// 获取系统参数函数
	IDirect3DDevice9*      GetD3DDevice() {return pDevice;}
	CInput*                GetInput() {return pInput;}
	CCamera*               GetCamera() {return pCamera;}
	HWND                   GetHWND() {return hWnd;}
	int                    GetWinWidth() {return iWinWidth;}
	int                    GetWinHeight() {return iWinHeight;}

	// 游戏管理函数
	//void SceneUpdate();
	//void SceneRender();                      // 场景渲染
	bool AddMeshFromFile(char* MeshFile,CMeshManager* &pOut);     // 通过配置文件加载模型
	bool AddSceneFromFile(char* SceneFile,CMeshManager* pMeshManager,CScene* &pOut);    // 通过配置文件加载场景
};
