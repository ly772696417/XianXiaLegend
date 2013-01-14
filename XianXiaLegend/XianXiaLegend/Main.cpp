#include "FrantoGE.h"

CFrantoGE* FGE;

IDirect3DDevice9* g_pDevice;
CInput*           g_pInput;
CCamera*          g_pCamera;

CStartUI*         g_pStartUI;
CMeshManager*     g_pMeshManager;
CScene*           g_pScene;

CBillBoard*       g_pLoadUI;

void Loading();

D3DLIGHT9 light0,light1;

// 游戏状态：1开始界面，2场景界面
int g_iGameState = 1;
int g_iOldState = 1;

bool g_bFillMode = true;

LARGE_INTEGER     g_llStartTime;
LARGE_INTEGER     g_llCurTime;
LARGE_INTEGER     g_llFPF;         // 时钟频率
int	              g_iFPS;          // 帧频率
DOUBLE            g_dElapsedTime;  // 每帧时间
list<double>      g_dElapsedList;



void CalcFPS()
{
	QueryPerformanceCounter( &g_llCurTime );
	
	LONGLONG llTime = 0;// 时间间隔
	double dTime = 0;// 真实时间

	llTime = g_llCurTime.QuadPart - g_llStartTime.QuadPart;
	dTime = (double)llTime / (double)g_llFPF.QuadPart;

	g_dElapsedList.push_back(dTime);

	if (g_dElapsedList.size() > ELAPSEDTIME_COUNT)
	{
		g_dElapsedList.pop_front();
	}

	list<double>::iterator it = g_dElapsedList.begin();

	dTime = 0;

	while(it != g_dElapsedList.end())
	{
		dTime += *it;
		it++;
	}

	g_dElapsedTime = dTime / g_dElapsedList.size();
	g_iFPS = (int)(1 / g_dElapsedTime);

	g_llStartTime = g_llCurTime;
}

void Init()
{
	// 获取时钟频率与初始值
	QueryPerformanceFrequency(&g_llFPF);
	QueryPerformanceCounter(&g_llStartTime);

	switch (g_iGameState)
	{
	case 1:
		{
			g_pStartUI = new CStartUI(g_pDevice,g_pInput);
			g_pStartUI->Init(".\\IniFolder\\billboard.ini","start");
			g_pLoadUI = new CBillBoard(g_pDevice);
			g_pLoadUI->Init(".\\IniFolder\\billboard.ini","load");
		}
		break;
	case 2:
		{
			/*_beginthread(Loading,0,NULL);*/
		
			FGE->AddMeshFromFile(".\\IniFolder\\mesh.ini",g_pMeshManager);
			FGE->AddSceneFromFile(".\\IniFolder\\scene.ini",g_pMeshManager,g_pScene);
			SAFE_DELETE(g_pStartUI);

			/*_endthread();*/
		}
		break;
	default:
		break;
	}
	g_pCamera->SetFar(10000.0f);
	g_pCamera->SetMaxDistance(1000.0f);
}

void Update()
{
	CalcFPS();
	switch (g_iGameState)
	{
	case 1:
		{
			g_pInput->Update();
			g_pStartUI->Update(g_iGameState,g_dElapsedTime);
		}
		break;
	case 2:
		{
			if (g_iGameState != g_iOldState)
			{
				Init();
				g_iOldState = g_iGameState;
			}
			else
			{
				g_pInput->Update();
				g_pScene->Update(g_dElapsedTime,g_iFPS);
			}
		}
		break;
	default:
		break;
	}
	if (g_pInput->KeyDown(DIK_Z))
	{
		if (g_bFillMode)
			g_bFillMode = false;
		else
			g_bFillMode = true;
	}
}

void SetupMatrices()
{
	switch (g_iGameState)
	{
	case 1:
		{
			g_pDevice->SetRenderState(D3DRS_LIGHTING,false);
		}
		break;
	case 2:
		{
			//memset(&light1, 0, sizeof(light1));
			//light1.Type = D3DLIGHT_DIRECTIONAL;// 平行光
			//light1.Direction.x = -1.0f;// 方向
			//light1.Direction.y = -1.0f;
			//light1.Direction.z = 0.0f;
			//light1.Diffuse.r = 0.0f;// 漫反射
			//light1.Diffuse.g = 0.0f;
			//light1.Diffuse.b = 0.0f;
			//light1.Ambient.r = 0.0f;// 环境光
			//light1.Ambient.g = 0.0f;
			//light1.Ambient.b = 0.0f;

			//g_pDevice->SetRenderState(D3DRS_LIGHTING,true);
			//g_pDevice->SetLight(0, &light0);
			//g_pDevice->LightEnable(0, true);
			//g_pDevice->SetLight(1, &light1);
			//g_pDevice->LightEnable(1,true);

			g_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW); 
			// 纹理采样
			g_pDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
			g_pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
			// Alpha测试
			g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
			g_pDevice->SetRenderState(D3DRS_ALPHAREF,0x000000ff);
			g_pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_EQUAL);
			if (g_bFillMode)
				g_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
			else
				g_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
			// 深度缓冲
			g_pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
			g_pDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESS);
			g_pDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

			// 雾化
			//g_pDevice->SetRenderState(D3DRS_FOGENABLE,TRUE);
			//float density = 0.0005f;                                        // 浓度
			//g_pDevice->SetRenderState(D3DRS_FOGDENSITY,*(DWORD*)&density);
			//g_pDevice->SetRenderState(D3DRS_FOGTABLEMODE,D3DFOG_EXP2);      // 设置雾化计算方式
			//g_pDevice->SetRenderState(D3DRS_FOGCOLOR,0x00cc8a40);           // 颜色
			//float start = 1000;                                             // 雾化范围
			//float end = 5000;
			//g_pDevice->SetRenderState(D3DRS_FOGSTART,*(DWORD*)&start);
			//g_pDevice->SetRenderState(D3DRS_FOGEND,*(DWORD*)&end);
		}
		break;
	}
}

void Rander()
{
	Update();
	if (g_pDevice == NULL)
		return;
	SetupMatrices();
	g_pDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(204,138,64),1.0f,0);// 清空图形绘制区
	g_pDevice->BeginScene();

	switch (g_iGameState)
	{
	case 1:
		{
			if (g_iGameState == g_iOldState)
			{
				g_pStartUI->Rander(g_dElapsedTime);
			}
		}
		break;
	case 2:
		{
			if (g_iGameState != g_iOldState)
			{
				if (g_iOldState == 1)
				{
					g_pLoadUI->Render(g_dElapsedTime);
				}
			}
			else
			{
				g_pScene->Render();
			}
		}
		break;
	default:
		break;
	}

	g_pDevice->SetRenderState(D3DRS_ZENABLE,FALSE);
	//g_pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);
	g_pDevice->EndScene();
	g_pDevice->Present(NULL,NULL,NULL,NULL);
}

INT WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPTSTR lpCmdLine,int nCmdShow)
{
	FGE = new CFrantoGE;
	//FGE->System_SetStateBool(FGE_SETMULTSAMPLE,true);
	FGE->System_Init(Rander);
	g_pDevice = FGE->GetD3DDevice();
	g_pInput = FGE->GetInput();
	g_pCamera = FGE->GetCamera();

	Init();
	FGE->System_Start();
	delete FGE;
	return 0;
}

void Loading()
{
	g_pLoadUI->Render(g_dElapsedTime);
}

