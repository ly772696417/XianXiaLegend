#include "Scene.h"

CScene::CScene(IDirect3DDevice9* pDevice,CMeshManager* pMeshManager,CCamera* pCamera,CInput* pInput)
{
	m_pDevice = pDevice;
	m_pMeshManager = pMeshManager;
	m_pCamera = pCamera;
	m_pInput = pInput;
	m_pZFrustum = NULL;
	m_iSceneID = 0;
	m_iMeshNum = 0;
	m_iSkinNum = 0;
	m_pTerrain = NULL;
	m_pSkyBox = NULL;
	m_pMajor   = NULL;
	m_dElapsedTime = 0.0f;
	m_iFPS = 0;
	m_bHelp = false;
	D3DXCreateFont(m_pDevice,12,12,400,0,false,GB2312_CHARSET,OUT_TT_ONLY_PRECIS,DEFAULT_QUALITY,0,"宋体",&m_pFont);
	//memset(m_tmpPoint,0,sizeof(D3DXVECTOR3) * 3);
	ZeroMemory( m_tmpPoint,  sizeof(D3DXVECTOR3) * 3);
	m_CameraPos = D3DXVECTOR3(0,0,0);
	m_bEditor = false;
	m_ID = 0;

	//水波
	mLight.dirW = D3DXVECTOR3(0.0f, -2.0f, -1.0f);
	D3DXVec3Normalize(&mLight.dirW, &mLight.dirW);
	mLight.ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
	mLight.diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLight.spec    = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);

	D3DXMATRIX waterWorld;
	D3DXMatrixTranslation(&waterWorld, 0.0f, 400.0f, 0.0f);

	Mtrl waterMtrl;
	waterMtrl.ambient   = D3DXCOLOR(0.26f, 0.23f, 0.3f, 0.90f);
	waterMtrl.diffuse   = D3DXCOLOR(0.26f, 0.23f, 0.3f, 0.90f);
	waterMtrl.spec      = 1.0f*WHITE;
	waterMtrl.specPower = 64.0f;

	Water::InitInfo waterInitInfo;
	waterInitInfo.dirLight = mLight;
	waterInitInfo.mtrl     = waterMtrl;
	waterInitInfo.vertRows         = 128;
	waterInitInfo.vertCols         = 128;
	waterInitInfo.dx               = 50.0f;
	waterInitInfo.dz               = 50.0f;
	waterInitInfo.waveMapFilename0 = "water\\wave0.dds";
	waterInitInfo.waveMapFilename1 = "water\\wave1.dds";
	waterInitInfo.waveMapVelocity0 = D3DXVECTOR2(0.05f, 0.08f);
	waterInitInfo.waveMapVelocity1 = D3DXVECTOR2(-0.02f, 0.1f);
	waterInitInfo.texScale = 16.0f;
	waterInitInfo.toWorld = waterWorld;

	m_pWater = new Water(waterInitInfo,m_pDevice);

	m_pSkyBox = new Sky("sky\\grassenvmap1024.dds", 10000.0f,m_pDevice);

	m_pWater->setEnvMap(m_pSkyBox->getEnvMap());
}

CScene::~CScene(void)
{
	map<int,CDeck*>::iterator itD = m_mapMeshObj.begin();
	for (;itD != m_mapMeshObj.end();itD++)
	{
		delete (*itD).second;
	}
	m_mapMeshObj.clear();

	map<int,CMonster*>::iterator itM = m_mapSkinObj.begin();
	for (;itM != m_mapSkinObj.end();itM++)
	{
		delete (*itM).second;
	}
	m_mapSkinObj.clear();

	vector<CBillBoard*>::iterator itB = m_vecBoardObj.begin();
	for (;itB != m_vecBoardObj.end();itB++)
	{
		delete (*itB);
	}
	m_vecBoardObj.clear();

	map<int,CParticleSystem*>::iterator itP = m_mapParticle.begin();
	for (;itP != m_mapParticle.end();itP++)
	{
		delete (*itP).second;
	}
	m_mapParticle.clear();

	//SAFE_DELETE(m_pSky);
	SAFE_DELETE(m_pSkyBox);
	SAFE_DELETE(m_pMajor);
	SAFE_DELETE(m_pZFrustum);
}

bool CScene::Init(char* SceneFile)
{
	// 初始化粒子
	CParticleSystem* pNew1 = new CParticleSystem(m_pDevice);
	pNew1->LoadFromFile(".\\IniFolder\\Particle.ini","Fire"); 
	pNew1->SetPosition(D3DXVECTOR3(-495,470,1050));
	m_mapParticle[1] = pNew1;
	CParticleSystem* pNew2 = new CParticleSystem(m_pDevice);
	pNew2->LoadFromFile(".\\IniFolder\\Particle.ini","Fire"); 
	pNew2->SetPosition(D3DXVECTOR3(-145,470,1050));
	m_mapParticle[2] = pNew2;
	CParticleSystem* pNew = new CParticleSystem(m_pDevice);
	pNew->LoadFromFile(".\\IniFolder\\Particle.ini","Wind");
	m_mapParticle[0] = pNew;
	CParticleSystem* pNew3 = new CParticleSystem(m_pDevice);
	pNew3->LoadFromFile(".\\IniFolder\\Particle.ini","Weapon");
	m_mapParticle[3] = pNew3;
	CParticleSystem* pNew4 = new CParticleSystem(m_pDevice);
	pNew4->LoadFromFile(".\\IniFolder\\Particle.ini","Smoke"); 
	pNew4->SetPosition(D3DXVECTOR3(2280,50,-3046));
	m_mapParticle[4] = pNew4;
	CParticleSystem* pNew5 = new CParticleSystem(m_pDevice);
	pNew5->LoadFromFile(".\\IniFolder\\Particle.ini","Smoke"); 
	pNew5->SetPosition(D3DXVECTOR3(2517,75,-2800));
	m_mapParticle[5] = pNew5;
	CParticleSystem* pNew6 = new CParticleSystem(m_pDevice);
	pNew6->LoadFromFile(".\\IniFolder\\Particle.ini","Smoke"); 
	pNew6->SetPosition(D3DXVECTOR3(2857,75,-3030));
	m_mapParticle[6] = pNew6;

	m_iSceneID = GetPrivateProfileInt("head","SceneID",-1,SceneFile);

	m_pZFrustum = new ZFrustum();
	m_pTerrain = new CTerrain(m_pDevice);
	m_pTerrain->Init(".\\IniFolder\\terrain.ini",m_iSceneID,m_pZFrustum);

	LoadSceneFile(SceneFile);
	LoadBillBoardFile(".\\IniFolder\\billboard.ini");
	
	m_pCamera->SetCamera();
	return true;
}

void CScene::Update(DOUBLE dElapsedTime,int iFPS)
{
	m_dElapsedTime = dElapsedTime;
	m_iFPS = iFPS;

	if (m_pInput->KeyDown(DIK_NUMPAD0))
	{
		if (m_bEditor)
			m_bEditor = false;
		else
			m_bEditor = true;
	}
	if (m_pInput->KeyDown(DIK_H))
	{
		if (m_bHelp)
			m_bHelp = false;
		else
			m_bHelp = true;
	}
	m_pCamera->Cortrol();
	// 地形编辑控制
	if (m_bEditor)
	{
		if (m_pInput->MouseDown(0))
		{
			Ray ray = CMyFuntion::CalculateRay(m_pInput->GetHWnd(),m_pDevice);
			map<int,CDeck*>::iterator itD = m_mapMeshObj.begin();
			for (;itD != m_mapMeshObj.end();itD++)
			{
				if (D3DXBoxBoundProbe(
					&(*itD).second->GetMin(),
					&(*itD).second->GetMax(),
					&ray.origin,&ray.dir))
				{
					m_ID = (*itD).first;
				}
			}
		}
		// 前移
		if (m_pInput->KeyDown(DIK_NUMPAD8))
		{
			D3DXVECTOR3 tmpPos = m_mapMeshObj[m_ID]->GetPos();
			tmpPos.z++;
			m_mapMeshObj[m_ID]->SetPos(tmpPos);
		}
		if (m_pInput->KeyHold(DIK_NUMPAD8) && m_pInput->KeyHold(DIK_LSHIFT))
		{
			D3DXVECTOR3 tmpPos = m_mapMeshObj[m_ID]->GetPos();
			tmpPos.z += 5;
			m_mapMeshObj[m_ID]->SetPos(tmpPos);
		}
		// 后移
		if (m_pInput->KeyDown(DIK_NUMPAD2))
		{
			D3DXVECTOR3 tmpPos = m_mapMeshObj[m_ID]->GetPos();
			tmpPos.z--;
			m_mapMeshObj[m_ID]->SetPos(tmpPos);
		}
		if (m_pInput->KeyHold(DIK_NUMPAD2) && m_pInput->KeyHold(DIK_LSHIFT))
		{
			D3DXVECTOR3 tmpPos = m_mapMeshObj[m_ID]->GetPos();
			tmpPos.z -= 5;
			m_mapMeshObj[m_ID]->SetPos(tmpPos);
		}
		// 左移
		if (m_pInput->KeyDown(DIK_NUMPAD4))
		{
			D3DXVECTOR3 tmpPos = m_mapMeshObj[m_ID]->GetPos();
			tmpPos.x--;
			m_mapMeshObj[m_ID]->SetPos(tmpPos);
		}
		if (m_pInput->KeyHold(DIK_NUMPAD4) && m_pInput->KeyHold(DIK_LSHIFT))
		{
			D3DXVECTOR3 tmpPos = m_mapMeshObj[m_ID]->GetPos();
			tmpPos.x -= 5;
			m_mapMeshObj[m_ID]->SetPos(tmpPos);
		}
		// 右移
		if (m_pInput->KeyDown(DIK_NUMPAD6))
		{
			D3DXVECTOR3 tmpPos = m_mapMeshObj[m_ID]->GetPos();
			tmpPos.x++;
			m_mapMeshObj[m_ID]->SetPos(tmpPos);
		}
		if (m_pInput->KeyHold(DIK_NUMPAD6) && m_pInput->KeyHold(DIK_LSHIFT))
		{
			D3DXVECTOR3 tmpPos = m_mapMeshObj[m_ID]->GetPos();
			tmpPos.x += 5;
			m_mapMeshObj[m_ID]->SetPos(tmpPos);
		}
		// 上移
		if (m_pInput->KeyDown(DIK_NUMPAD7))
		{
			D3DXVECTOR3 tmpPos = m_mapMeshObj[m_ID]->GetPos();
			tmpPos.y++;
			m_mapMeshObj[m_ID]->SetPos(tmpPos);
		}
		if (m_pInput->KeyHold(DIK_NUMPAD7) && m_pInput->KeyHold(DIK_LSHIFT))
		{
			D3DXVECTOR3 tmpPos = m_mapMeshObj[m_ID]->GetPos();
			tmpPos.y += 5;
			m_mapMeshObj[m_ID]->SetPos(tmpPos);
		}
		// 下移
		if (m_pInput->KeyDown(DIK_NUMPAD9))
		{
			D3DXVECTOR3 tmpPos = m_mapMeshObj[m_ID]->GetPos();
			tmpPos.y--;
			m_mapMeshObj[m_ID]->SetPos(tmpPos);
		}
		if (m_pInput->KeyHold(DIK_NUMPAD9) && m_pInput->KeyHold(DIK_LSHIFT))
		{
			D3DXVECTOR3 tmpPos = m_mapMeshObj[m_ID]->GetPos();
			tmpPos.y -= 5;
			m_mapMeshObj[m_ID]->SetPos(tmpPos);
		}
		// 顺时针旋转
		if (m_pInput->KeyDown(DIK_NUMPAD3))
		{
			D3DXVECTOR3 tmpRot = m_mapMeshObj[m_ID]->GetRot();
			tmpRot.y -= 0.01;
			m_mapMeshObj[m_ID]->SetRot(tmpRot);
		}
		if (m_pInput->KeyHold(DIK_NUMPAD3) && m_pInput->KeyHold(DIK_LSHIFT))
		{
			D3DXVECTOR3 tmpRot = m_mapMeshObj[m_ID]->GetRot();
			tmpRot.y -= 0.05;
			m_mapMeshObj[m_ID]->SetRot(tmpRot);
		}
		// 逆时针旋转
		if (m_pInput->KeyDown(DIK_NUMPAD1))
		{
			D3DXVECTOR3 tmpRot = m_mapMeshObj[m_ID]->GetRot();
			tmpRot.y += 0.01;
			m_mapMeshObj[m_ID]->SetRot(tmpRot);
		}
		if (m_pInput->KeyHold(DIK_NUMPAD1) && m_pInput->KeyHold(DIK_LSHIFT))
		{
			D3DXVECTOR3 tmpRot = m_mapMeshObj[m_ID]->GetRot();
			tmpRot.y += 0.05;
			m_mapMeshObj[m_ID]->SetRot(tmpRot);
		}
		// 放大
		if (m_pInput->KeyDown(DIK_NUMPADPLUS))
		{
			D3DXVECTOR3 tmpScal = m_mapMeshObj[m_ID]->GetScal();
			tmpScal.x++;
			tmpScal.y++;
			tmpScal.z++;
			m_mapMeshObj[m_ID]->SetScal(tmpScal);
		}
		if (m_pInput->KeyHold(DIK_NUMPADPLUS) && m_pInput->KeyHold(DIK_LSHIFT))
		{
			D3DXVECTOR3 tmpScal = m_mapMeshObj[m_ID]->GetScal();
			tmpScal.x++;
			tmpScal.y++;
			tmpScal.z++;
			m_mapMeshObj[m_ID]->SetScal(tmpScal);
		}
		// 缩小
		if (m_pInput->KeyDown(DIK_NUMPADMINUS))
		{
			D3DXVECTOR3 tmpScal = m_mapMeshObj[m_ID]->GetScal();
			if (tmpScal.x > 1)
			{
				tmpScal.x--;
				tmpScal.y--;
				tmpScal.z--;
				m_mapMeshObj[m_ID]->SetScal(tmpScal);
			}
		}
		if (m_pInput->KeyHold(DIK_NUMPADMINUS) && m_pInput->KeyHold(DIK_LSHIFT))
		{
			D3DXVECTOR3 tmpScal = m_mapMeshObj[m_ID]->GetScal();
			if (tmpScal.x > 1)
			{
				tmpScal.x--;
				tmpScal.y--;
				tmpScal.z--;
				m_mapMeshObj[m_ID]->SetScal(tmpScal);
			}
		}
	}
	m_pWater->update(dElapsedTime);
	// 更新天空
	//m_pSky->Update(m_pCamera->GetLookAt());
	// 更新装饰物
	map<int,CDeck*>::iterator itD = m_mapMeshObj.begin();
	for(;itD != m_mapMeshObj.end();itD++)
	{
		itD->second->Update();
	}
	// 更新怪物
	map<int,CMonster*>::iterator itM = m_mapSkinObj.begin();
	for(;itM != m_mapSkinObj.end();itM++)
	{
		itM->second->Update(m_dElapsedTime);
	}
	// 跟随镜头视点更新
	if (m_bEditor)
	{
		if (m_pCamera->GetMode() == true)
			m_pCamera->SetLookAt(m_mapMeshObj[m_ID]->GetPos()+D3DXVECTOR3(0,28.0f,0));
	}
	else
	{
		if (m_pCamera->GetMode() == true)
			m_pCamera->SetLookAt(m_pMajor->GetPos()+D3DXVECTOR3(0,28.0f,0));
	}
	// 更新主角
	if (m_bEditor == false)
	{
		D3DXVECTOR3 vDir = m_pCamera->GetLookAt() - m_pCamera->GetPos();
		m_pMajor->Update(m_dElapsedTime,vDir);
	}
	// 摄像机测试碰撞
	if (m_bEditor == false)
	{
		if (CollisionCamera(m_CameraPos))
			m_pCamera->SetPos(m_CameraPos);
	}
	m_pCamera->SetCamera();
	// 更新平截台体
	D3DXMATRIX proj,view,viewProj;
	m_pDevice->GetTransform(D3DTS_VIEW,&view);
	m_pDevice->GetTransform(D3DTS_PROJECTION,&proj);
	viewProj = view * proj;
	m_pZFrustum->Make(&viewProj,m_pTerrain->GetCellSpace());
	// 更新地形	
	m_pTerrain->Update();

	map<int,CParticleSystem*>::iterator itP = m_mapParticle.begin();
	for(;itP != m_mapParticle.end();itP++)
	{
		(*itP).second->Update(m_dElapsedTime);
	}
}

void CScene::Render()
{
	//m_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	// 渲染天空
	//D3DXMATRIX matWorld = m_pSky->GetWorldMat();
	//m_pSky->Render(&matWorld);
	m_pSkyBox->draw();

	// 渲染场景装饰
	map<int,CDeck*>::iterator itD = m_mapMeshObj.begin();
	for(;itD != m_mapMeshObj.end();itD++)
	{
		D3DXVECTOR3 vCenter = itD->second->GetPos();
		if (m_pZFrustum->IsInSphere(&vCenter,itD->second->GetRadius()))
		{
			D3DXMATRIX matWorld = itD->second->GetWorldMat();
			itD->second->Render(&matWorld);
		}
	}

	// 渲染怪物
	map<int,CMonster*>::iterator itM = m_mapSkinObj.begin();
	for(;itM != m_mapSkinObj.end();itM++)
	{
		D3DXVECTOR3 vCenter = itM->second->GetPos();
		if (m_pZFrustum->IsInSphere(&vCenter,itM->second->GetRadius()))
		{
			D3DXMATRIX matWorld = itM->second->GetWorldMat();
			itM->second->Render(&matWorld);
		}
	}

	// 渲染主角
	D3DXMATRIX matWorld = m_pMajor->GetWorldMat();
	m_pMajor->Render(&matWorld);

	// 渲染地形
	m_pTerrain->Render();

	// 渲染公告板
	DWORD AlphaText,AlphaBlend,CullMode;
	m_pDevice->GetRenderState(D3DRS_ALPHATESTENABLE,&AlphaText);
	m_pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,&AlphaBlend);
	m_pDevice->GetRenderState(D3DRS_CULLMODE,&CullMode);

	m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE); 

	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	m_pDevice->SetRenderState(D3DRS_BLENDOPALPHA,D3DBLENDOP_ADD);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	vector<CBillBoard*>::iterator itB = m_vecBoardObj.begin();
	for (;itB != m_vecBoardObj.end();itB++)
	{
		(*itB)->Render(m_dElapsedTime);
	}

	m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,AlphaText);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,AlphaBlend);
	m_pDevice->SetRenderState(D3DRS_CULLMODE,CullMode); 


	// 渲染粒子
	map<int,CParticleSystem*>::iterator itP = m_mapParticle.begin();
	for(;itP != m_mapParticle.end();itP++)
	{
		if ((*itP).first == 0)
		{
			if (m_pMajor->IsPEnable() && m_pMajor->GetSkill() == 3)
			{
				(*itP).second->SetPosition(m_pMajor->GetPos());
				(*itP).second->SetEnable(true);
			}
			else
			{
				(*itP).second->SetEnable(false);
			}
		}
		else if ((*itP).first == 3)
		{
			if (m_pMajor->IsPEnable() && m_pMajor->GetSkill() == 2)
			{
				D3DXMATRIX* Weapon;
				D3DXVECTOR3 EmitterPos(0.0f,0.0f,0.0f);
				m_pMeshManager->GetSkinMeshFromID(m_pMajor->GetMeshID())->GetBoneMatrix("Rfinger",&Weapon);
				D3DXVec3TransformCoord(&EmitterPos,&EmitterPos,Weapon);
				(*itP).second->SetPosition(EmitterPos);
				(*itP).second->SetEnable(true);
			}
			else
			{
				(*itP).second->SetEnable(false);
			}
		}
		else
		{
			if (m_pZFrustum->IsInSphere(&(itP)->second->GetPosition(),itP->second->GetEmitWidth()))
			{
				(*itP).second->SetEnable(true);
			}
			else
			{
				(*itP).second->SetEnable(false);
			}
		}
		(*itP).second->Render(m_dElapsedTime);
	}

	// 渲染文字
	char text[256];
	RECT rect;
	rect.top = 20;
	rect.left = 50;
	rect.bottom = 110;
	rect.right = 750;
	sprintf(text,"FPS=%d",m_iFPS);
	m_pFont->DrawText(0,text,-1,&rect,DT_TOP|DT_LEFT,0xffffffff);
	rect.left = 150;
	sprintf(text,"X= %d,Y= %d,Z= %d",
		(int)(m_pMajor->GetPos()).x,
		(int)(m_pMajor->GetPos()).y,
		(int)(m_pMajor->GetPos()).z);
	m_pFont->DrawTextA(0,text,-1,&rect,DT_TOP|DT_LEFT,0xffffffff);
	rect.left = 430;
	sprintf(text,"帮助:H   地表三角形:%d",m_pTerrain->GetTri());
	m_pFont->DrawTextA(0,text,-1,&rect,DT_TOP|DT_LEFT,0xffffffff);

	rect.top = 400;
	rect.left =550;
	rect.bottom = 590;
	rect.right = 780;
	sprintf(text,"");
	m_pFont->DrawTextA(0,text,-1,&rect,DT_BOTTOM|DT_RIGHT,0xffffffff);
	
	if (m_bHelp)
	{
		rect.top = 200;
		rect.left = 100;
		rect.bottom = 800;
		rect.right = 700;
		sprintf(text,"技能选择：1-3\n\n攻击：空格键\n\n奔跑：W、S、A、D\n\n\\
				移动：鼠标左键\n\n摄像机控制：鼠标右键\n\n摄像机切换：F9\n\n网格显示：Z");
		m_pFont->DrawTextA(0,text,-1,&rect,DT_TOP|DT_LEFT,0xff0088ff);
		rect.left = 400;
		sprintf(text,"地形编辑模式：小键盘0\n\n前后左右：8、2、4、6\n\n上下：7、9\n\n\\
				旋转：1、3\n\n缩放：-、+\n\n连续操作加 LShift");
		m_pFont->DrawTextA(0,text,-1,&rect,DT_TOP|DT_LEFT,0xff0088ff);
	}
	if (m_bEditor)
	{
		rect.top = 50;
		rect.left = 50;
		rect.bottom = 200;
		rect.right = 750;
		D3DXVECTOR3 tmpPos = m_mapMeshObj[m_ID]->GetPos();
		D3DXVECTOR3 tmpRot = m_mapMeshObj[m_ID]->GetRot();
		D3DXVECTOR3 tmpScal = m_mapMeshObj[m_ID]->GetScal();
		sprintf(text,"ID：%d  缩放：X= %d Y= %d Z= %d\n位置：X= %d Y= %d Z= %d\n旋转：X= %f Y= %f Z= %f",
				m_ID,(int)tmpScal.x,(int)tmpScal.y,(int)tmpScal.z,
				(int)tmpPos.x,(int)tmpPos.y,(int)tmpPos.z,
				tmpRot.x,tmpRot.y,tmpRot.z);
		m_pFont->DrawTextA(0,text,-1,&rect,DT_TOP|DT_LEFT,0xffffffff);
	}

		D3DXMATRIX matTrans;

	//D3DXMatrixTranslation(&matTrans,3520,100,3008);
	//m_pDevice->SetTransform(D3DTS_WORLD,&matTrans);

	m_pWater->draw(m_CameraPos);
}

bool CScene::CollisionCamera(D3DXVECTOR3 &vPos)
{
	float tmpDist;
	D3DXVECTOR3 CameraPos = m_pCamera->GetPos();
	D3DXVECTOR3 CameraLookAt = m_pCamera->GetLookAt();
	D3DXVECTOR3 CameraDir = CameraPos - CameraLookAt;

	map<int,CDeck*>::iterator it = m_mapMeshObj.begin();
	for (;it != m_mapMeshObj.end();it++)
	{
		if (CMyFuntion::CollisonTestPoint(m_pCamera->GetPos(),(*it).second->GetMin(),(*it).second->GetMax()))
		{
			BOOL bHit;
			D3DXVECTOR3 tmpPos;
			Ray ray;
			float CamDist;
			// 摄像机位置、射线转换到模型本地空间
			D3DXMATRIX matWorld,matInv;
			matWorld = (*it).second->GetWorldMat();
			D3DXMatrixInverse(&matInv,NULL,&matWorld);
			D3DXVec3TransformCoord(&tmpPos,&CameraPos,&matInv);
			D3DXVec3TransformCoord(&ray.origin,&CameraLookAt,&matInv);
			D3DXVec3TransformNormal(&ray.dir,&CameraDir,&matInv);
			D3DXVec3Normalize(&ray.dir,&ray.dir);
			CamDist = D3DXVec3Length(&(tmpPos - ray.origin));
			tmpDist = CamDist;

			ID3DXMesh* pMesh = m_pMeshManager->GetMeshFromID((*it).second->GetMeshID())->GetMesh();
			D3DXIntersect(pMesh,&ray.origin,&ray.dir,&bHit,NULL,NULL,NULL,&tmpDist,NULL,NULL);
			if (bHit != 0 && tmpDist < CamDist)
			{
				/*LPDIRECT3DVERTEXBUFFER9 pVB;
				LPVOID vP = NULL;
				pMesh->GetVertexBuffer( &pVB );
				DWORD dwSize = D3DXGetFVFVertexSize( pMesh->GetFVF() );
				pVB->Lock( FaceIndex*dwSize, dwSize, &vP, 0 );
				m_tmpPoint[0] = *(D3DXVECTOR3*)(vP);
				pVB->Unlock();
				pVB->Lock( FaceIndex*dwSize+dwSize, dwSize, &vP, 0 );
				m_tmpPoint[1] = *(D3DXVECTOR3*)(vP);
				pVB->Unlock();
				pVB->Lock( FaceIndex*dwSize+2*dwSize, dwSize, &vP, 0 );
				m_tmpPoint[2] = *(D3DXVECTOR3*)(vP);
				pVB->Unlock();
				m_ID = (*it).second->GetID();*/

				vPos = ray.origin + ray.dir * tmpDist;

				D3DXVec3TransformCoord( &vPos, &vPos, &matWorld );
				return true;
			}
		}
	}
	if (CameraPos.y < m_pTerrain->GetHeight(CameraPos.x,CameraPos.z))
	{
		D3DXVECTOR3 tmpPos;
		Ray ray;
		float CamDist;
		// 得到地形的变换矩阵
		D3DXMATRIX matWorld,matInv;
		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixInverse(&matInv,NULL,&matWorld);
		// 转换空间
		D3DXVec3TransformCoord(&tmpPos,&CameraPos,&matInv);
		D3DXVec3TransformCoord(&ray.origin,&CameraLookAt,&matInv);
		D3DXVec3TransformNormal(&ray.dir,&CameraDir,&matInv);
		D3DXVec3Normalize(&ray.dir,&ray.dir);
		CamDist = D3DXVec3Length(&(tmpPos - ray.origin));
		tmpDist = CamDist;

		CTerrain::IntersectTerrain(
			m_pTerrain->GetVertexBuffer(),
			m_pTerrain->GetIndexBuffer(),
			ray,
			m_pTerrain->GetTri(),
			&tmpDist,
			NULL);
		if (tmpDist < CamDist)
		{
			vPos = ray.origin + ray.dir * tmpDist;
			D3DXVec3TransformCoord( &vPos, &vPos, &matWorld );
			vPos.y += 20; 
			return true;
		}
	}
	return false;
}

bool CScene::LoadSceneFile(char* SceneFile)
{

	int temNum = GetPrivateProfileInt("head","Num",-1,SceneFile);

	if (temNum == 0)
		return false;
	char tmpAddName[32];

	for (int i = 0;i < temNum;i++)
	{
		sprintf(tmpAddName,"id_%d",i);
		int Type = GetPrivateProfileInt(tmpAddName,"Type",-1,SceneFile);
		if (Type == 0)
		{
			//CSky* pNew = new CSky(m_pDevice,m_pMeshManager);
			//pNew->Init(SceneFile,tmpAddName);
			//m_pSky = pNew;
		}
		else if (Type == 1)
		{
			CDeck* pNew = new CDeck(m_pDevice,m_pMeshManager);
			pNew->Init(SceneFile,tmpAddName);
			pNew->SetID(m_iMeshNum);
			m_mapMeshObj[m_iMeshNum] = pNew;
			m_iMeshNum++;
		}
		else if (Type == 2)
		{
			CMajor* pNew = new CMajor(m_pDevice,m_pMeshManager,m_pInput);
			pNew->Init(m_pTerrain,SceneFile,tmpAddName,&m_mapMeshObj,&m_mapSkinObj/*,&m_mapParticle*/);
			m_pMajor = pNew;
		}
		else if (Type == 3)
		{
			CMonster* pNew = new CMonster(m_pDevice,m_pMeshManager);
			pNew->Init(m_pTerrain,SceneFile,tmpAddName,&m_mapMeshObj,m_pMajor);
			pNew->SetID(m_iSkinNum);
			m_mapSkinObj[m_iSkinNum] = pNew;
			m_iSkinNum++;
		}
	}
	return true;
}

bool CScene::LoadBillBoardFile(char* BillBoardFile)
{
	int tmpMaxNum = GetPrivateProfileInt("head","BillBoardNum",-1,BillBoardFile);
	char tmpAddName[128];
	for (int i = 0;i < tmpMaxNum;i++)
	{
		sprintf(tmpAddName,"billboardid_%d",i);
		CBillBoard* pNew = new CBillBoard(m_pDevice);
		pNew->Init(BillBoardFile,tmpAddName);
		m_vecBoardObj.push_back(pNew);
	}
	return true;
}