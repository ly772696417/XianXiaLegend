#include "Major.h"
#include "Monster.h"

CMajor::CMajor(IDirect3DDevice9* pDevice,CMeshManager* pMeshManager,CInput* pInput)
{
	m_pDevice         = pDevice;
	m_pMeshManager    = pMeshManager;
	m_pInput          = pInput;
	m_pAI             = NULL;
	m_fMoveSpeed      = 1;
	m_pTerrain        = NULL;
	m_bKeyLock        = false;
	m_State           = AS_MOUSEMOVE;
	m_iSkill          = 1;
	m_dElapsedTime    = 0.1;
	m_dEndTime        = 0.0;
	m_dTimeCount      = 0.0;

	m_bGra            = true;
	m_fGravity        = 0.098f;
	m_fSpeed          = 0.0f; 

	m_bPEnable        = false;
}

CMajor::~CMajor(void)
{
	SAFE_DELETE(m_pAI);
}

bool CMajor::Init(CTerrain* pTerrain,
				  char* SceneFile,
				  char* AddName,
				  map<int,CDeck*>* MeshObj,
				  map<int,CMonster*>* SkinObj)
{
	if (LoadDataFromFile(SceneFile,AddName) == false)
		return false;
	m_pMeshManager->GetSkinMeshFromID(m_iMeshID)->GetBoundingB_S(&m_vOrigMax,&m_vOrigMin,&m_vCenter,&m_fRadius);
	m_vOrigRot = m_vRot;
	m_pTerrain = pTerrain;
	m_vPos.y = m_pTerrain->GetHeight(m_vPos.x,m_vPos.z);
	m_vDestPos = m_vPos;

	// 初始化动画
	m_pAI = new CAnimInstance();
	if (m_pAI == NULL)
		return false;
	m_pAI->Init(m_pMeshManager->GetSkinMeshFromID(m_iMeshID));
	m_pAI->SetTimeDelta(m_dElapsedTime);

	m_mapMeshObj = MeshObj;
	m_mapSkinObj = SkinObj;

	return true;
}

void CMajor::Update(DOUBLE dElapsedTime,D3DXVECTOR3 vDir)
{
	m_dElapsedTime = dElapsedTime;
	m_pAI->SetTimeDelta(m_dElapsedTime);

	if (m_bKeyLock == false && m_pInput->MouseDown(0))
	{
		m_State = AS_MOUSEMOVE;
		Ray ray = CMyFuntion::CalculateRay(m_pInput->GetHWnd(),m_pDevice);
		// 得到地形的变换矩阵
		D3DXMATRIX matWorld,matInv;
		D3DXMatrixIdentity(&matWorld);

		// 计算出他的逆矩阵
		D3DXMatrixInverse(&matInv,NULL,&matWorld);
		// 把射线的起点和方向变换到地形的本地空间
		D3DXVec3TransformCoord(&ray.origin,&ray.origin,&matInv);
		D3DXVec3TransformNormal(&ray.dir,&ray.dir,&matInv);
		D3DXVec3Normalize(&ray.dir,&ray.dir);
		// 最后计算射线根地形的碰撞
		CTerrain::IntersectTerrain(
			m_pTerrain->GetVertexBuffer(),
			m_pTerrain->GetIndexBuffer(),
			ray,
			m_pTerrain->GetMaxTri(),
			NULL,
			&m_vDestPos);
		// 把得到的碰撞点按地形的世界变换而变换出碰撞点在世界空间的位置
		D3DXVec3TransformCoord(&m_vDestPos,&m_vDestPos,&matWorld);

		m_vDir = m_vDestPos - m_vPos;
        m_vDir.y = 0;
		D3DXVec3Normalize(&m_vDir,&m_vDir);

		m_pAI->SetAnimName("walk");
		m_pAI->ChangeTrack(true);
	}

	// 转向
	if (m_bKeyLock == false && m_pInput->KeyDown(DIK_W))
	{
		m_State = AS_KEYMOVE;
		m_bKeyLock = true;
		m_vDir = vDir;
		m_vDir.y = 0;
		D3DXVec3Normalize(&m_vDir,&m_vDir);

		m_pAI->SetAnimName("run");
		m_pAI->ChangeTrack(true);
	}
	if (m_bKeyLock == false && m_pInput->KeyDown(DIK_S))
	{
		m_State = AS_KEYMOVE;
		m_bKeyLock = true;
		m_vDir = vDir;
		m_vDir.y = 0;
		m_vDir.x = -m_vDir.x;
		m_vDir.z = -m_vDir.z;
		D3DXVec3Normalize(&m_vDir,&m_vDir);

		m_pAI->SetAnimName("run");
		m_pAI->ChangeTrack(true);
	}
	if (m_bKeyLock == false && m_pInput->KeyDown(DIK_A))
	{
		m_State = AS_KEYMOVE;
		m_bKeyLock = true;
		m_vDir = vDir;
		m_vDir.y = 0;
		D3DXVECTOR3 vY = D3DXVECTOR3(0,1,0);
		D3DXVec3Cross(&m_vDir,&m_vDir,&vY);
		D3DXVec3Normalize(&m_vDir,&m_vDir);

		m_pAI->SetAnimName("run");
		m_pAI->ChangeTrack(true);
	}
	if (m_bKeyLock == false && m_pInput->KeyDown(DIK_D))
	{
		m_State = AS_KEYMOVE;
		m_bKeyLock = true;
		m_vDir = vDir;
		m_vDir.y = 0;
		D3DXVECTOR3 vY = D3DXVECTOR3(0,1,0);
		D3DXVec3Cross(&m_vDir,&vY,&m_vDir);
		D3DXVec3Normalize(&m_vDir,&m_vDir);

		m_pAI->SetAnimName("run");
		m_pAI->ChangeTrack(true);
	}

	if (m_pInput->KeyUp(DIK_W) &&
		m_pInput->KeyHold(DIK_S) == false &&
		m_pInput->KeyHold(DIK_A) == false &&
		m_pInput->KeyHold(DIK_D) == false)
	{
		m_pAI->SetAnimName("wait");
		m_pAI->ChangeTrack(false);
		m_bKeyLock = false;
		m_State = AS_WAIT;
	}
	if (m_pInput->KeyUp(DIK_S) &&
		m_pInput->KeyHold(DIK_W) == false &&
		m_pInput->KeyHold(DIK_A) == false &&
		m_pInput->KeyHold(DIK_D) == false)
	{
		m_pAI->SetAnimName("wait");
		m_pAI->ChangeTrack(false);
		m_bKeyLock = false;
		m_State = AS_WAIT;
	}
	if (m_pInput->KeyUp(DIK_A) &&
		m_pInput->KeyHold(DIK_S) == false &&
		m_pInput->KeyHold(DIK_W) == false &&
		m_pInput->KeyHold(DIK_D) == false)
	{
		m_pAI->SetAnimName("wait");
		m_pAI->ChangeTrack(false);
		m_bKeyLock = false;
		m_State = AS_WAIT;
	}
	if (m_pInput->KeyUp(DIK_D) &&
		m_pInput->KeyHold(DIK_S) == false &&
		m_pInput->KeyHold(DIK_A) == false &&
		m_pInput->KeyHold(DIK_W) == false)
	{
		m_pAI->SetAnimName("wait");
		m_pAI->ChangeTrack(false);
		m_bKeyLock = false;
		m_State = AS_WAIT;
	}

	// 攻击选择
	if (m_pInput->KeyDown(DIK_1) && m_State != AS_ATTACK)
	{
		m_iSkill = 1;
	}
	if (m_pInput->KeyDown(DIK_2) && m_State != AS_ATTACK)
	{
		m_iSkill = 2;
	}
	if (m_pInput->KeyDown(DIK_3) && m_State != AS_ATTACK)
	{
		m_iSkill = 3;
	}

	// 攻击
	if (m_bKeyLock == false && m_pInput->KeyDown(DIK_SPACE))
	{
		m_State = AS_ATTACK;
		m_dTimeCount = 0.0;
		m_bKeyLock = true;
		m_pAI->SetMoving(false);
		switch (m_iSkill)
		{
		case 1:
			{
				m_pAI->SetAnimName("attack");
				m_pAI->ChangeTrack(true);
				map<int,CMonster*>::iterator it = (*m_mapSkinObj).begin();
				for(;it != (*m_mapSkinObj).end();it++)
				{
					Ray ray;
					ray.origin = m_vPos;
					ray.dir = m_vDir;
					D3DXVECTOR3 vMin = (*it).second->GetMin();
					D3DXVECTOR3 vMax = (*it).second->GetMax();
					if (D3DXBoxBoundProbe(&vMin,&vMax,&ray.origin,&ray.dir))
					{
						D3DXVECTOR3 vMajor = m_vPos;
						D3DXVECTOR3 vMonster = (*it).second->GetPos();
						vMajor.y = 0;
						vMonster.y = 0;
						if (D3DXVec3Length(&(vMajor - vMonster)) <= 75)
						{
							(*it).second->SetState(AS_BRUISE);
							(*it).second->SetEndTime(0.0);
							(*it).second->SetTimeLapse(m_pAI->GetTimeMax() / 2);
							(*it).second->SetDir(ray.dir);
							break;
						}
					}
				}
			}
			break;
		case 2:// 冲刺
			{
				m_pAI->SetAnimName("skill1");
				m_pAI->ChangeTrack(true);
				m_bPEnable = true;
				map<int,CMonster*>::iterator it = (*m_mapSkinObj).begin();
				for(;it != (*m_mapSkinObj).end();it++)
				{
					Ray ray;
					ray.origin = m_vPos;
					ray.dir = m_vDir;
					D3DXVECTOR3 vMin = (*it).second->GetMin();
					D3DXVECTOR3 vMax = (*it).second->GetMax();
					if (D3DXBoxBoundProbe(&vMin,&vMax,&ray.origin,&ray.dir))
					{
						D3DXVECTOR3 vMajor = m_vPos;
						D3DXVECTOR3 vMonster = (*it).second->GetPos();
						vMajor.y = 0;
						vMonster.y = 0;
						if (D3DXVec3Length(&(vMajor - vMonster)) <= 75)
						{
							(*it).second->SetState(AS_BRUISE);
							(*it).second->SetEndTime(1.5);
							(*it).second->SetTimeLapse(m_pAI->GetTimeMax());
							(*it).second->SetDir(ray.dir);
							break;
						}
					}
				}
			}
			break;
		case 3:// 坠击
			{
				m_pAI->SetAnimName("skill2");
				m_pAI->ChangeTrack(true);
				map<int,CMonster*>::iterator it = (*m_mapSkinObj).begin();
				for(;it != (*m_mapSkinObj).end();it++)
				{
					Ray ray;
					ray.origin = m_vPos;
					D3DXVECTOR3 vDir = (*it).second->GetPos() - m_vPos;
					vDir.y = 0;
					D3DXVec3Normalize(&vDir,&vDir);
					ray.dir = vDir;
					D3DXVECTOR3 vMin = (*it).second->GetMin();
					D3DXVECTOR3 vMax = (*it).second->GetMax();
					if (D3DXBoxBoundProbe(&vMin,&vMax,&ray.origin,&ray.dir))
					{
						D3DXVECTOR3 vMajor = m_vPos;
						D3DXVECTOR3 vMonster = (*it).second->GetPos();
						vMajor.y = 0;
						vMonster.y = 0;
						if (D3DXVec3Length(&(vMajor - vMonster)) <= 150)
						{
							(*it).second->SetState(AS_BRUISE);
							(*it).second->SetEndTime(1);
							(*it).second->SetTimeLapse(m_pAI->GetTimeMax());
							(*it).second->SetDir(ray.dir);
						}
					}
				}
			}
			break;
		}
	}

	// 事件逻辑
	switch (m_State)
	{
	case AS_MOUSEMOVE:
		{
			if (D3DXVec3Length(&(m_vDestPos - m_vPos)) >= m_fMoveSpeed * m_dElapsedTime)
			{
				D3DXVECTOR3 vPos = m_vPos + m_vDir * m_fMoveSpeed * m_dElapsedTime;
				vPos.y = m_pTerrain->GetHeight(vPos.x,vPos.z);
				float Slope = CMyFuntion::CalcSlope(m_vPos,vPos);
				if (Slope <= TERRAIN_SLOPE && 
					Slope >= -TERRAIN_SLOPE &&
					CollisionTest(vPos) == false)
					m_vPos = vPos;
				else 
					m_vDestPos = m_vPos;
			}
			else if (strcmp("walk",m_pAI->GetAnimName()) == 0)
			{
				m_pAI->SetAnimName("wait");
				m_pAI->ChangeTrack(true);
			}
		}
		break;
	case AS_KEYMOVE:
		{
			/*if (m_pInput->KeyHold(DIK_W))
			{*/
				D3DXVECTOR3 vPos = m_vPos + m_vDir * m_fMoveSpeed * m_dElapsedTime * 2;
				vPos.y = m_pTerrain->GetHeight(vPos.x,vPos.z);
				float Slope = CMyFuntion::CalcSlope(m_vPos,vPos);
				if (Slope <= TERRAIN_SLOPE && 
					Slope >= -TERRAIN_SLOPE &&
					CollisionTest(vPos) == false)
				{
					m_vPos = vPos;
					m_vDestPos = m_vPos;
				}
		/*	}
			else if (m_pInput->KeyHold(DIK_S))
			{
				D3DXVECTOR3 vPos = m_vPos + m_vDir * m_fMoveSpeed * m_dElapsedTime * 2;
				vPos.y = m_pTerrain->GetHeight(vPos.x,vPos.z);
				float Slope = CMyFuntion::CalcSlope(m_vPos,vPos);
				if (Slope <= TERRAIN_SLOPE && 
					Slope >= -TERRAIN_SLOPE &&
					CollisionTest(vPos) == false)
				{
					m_vPos = vPos;
					m_vDestPos = m_vPos;
				}
			}
			else if (m_pInput->KeyHold(DIK_A))
			{
				D3DXVECTOR3 vPos = m_vPos + m_vDir * m_fMoveSpeed * m_dElapsedTime * 2;
				vPos.y = m_pTerrain->GetHeight(vPos.x,vPos.z);
				float Slope = CMyFuntion::CalcSlope(m_vPos,vPos);
				if (Slope <= TERRAIN_SLOPE && 
					Slope >= -TERRAIN_SLOPE &&
					CollisionTest(vPos) == false)
				{
					m_vPos = vPos;
					m_vDestPos = m_vPos;
				}
			}
			else if (m_pInput->KeyHold(DIK_D))
			{
				D3DXVECTOR3 vPos = m_vPos + m_vDir * m_fMoveSpeed * m_dElapsedTime * 2;
				vPos.y = m_pTerrain->GetHeight(vPos.x,vPos.z);
				float Slope = CMyFuntion::CalcSlope(m_vPos,vPos);
				if (Slope <= TERRAIN_SLOPE && 
					Slope >= -TERRAIN_SLOPE &&
					CollisionTest(vPos) == false)
				{
					m_vPos = vPos;
					m_vDestPos = m_vPos;
				}
			}*/
		}
		break;
	case AS_WAIT:
		{
			m_pAI->SetMoving(true);
		}
		break;
	case AS_ATTACK:
		{
			m_pAI->SetMoving(true);
			if (m_iSkill == 3)
			{
				if (m_dTimeCount >= (m_pAI->GetTimeMax() * 0.6))
				{
					m_bPEnable = true;
				}
			}
			if (m_dTimeCount > m_pAI->GetTimeMax())
			{
				m_bKeyLock = false;
				m_pAI->SetAnimName("ready");
				m_pAI->ChangeTrack(true);
				m_pAI->SetMoving(false);
				m_State = AS_GUARD;
				m_dTimeCount = 0;
				if (m_bPEnable)
					m_bPEnable = false;
			}
			else
			{
				m_dTimeCount += m_dElapsedTime;
			}
		}
		break;
	case AS_BRUISE:
		{
			m_pAI->SetMoving(true);
			if (m_bPEnable)
				m_bPEnable = false;
			if (m_dTimeCount > m_pAI->GetTimeMax())
			{
				m_dTimeCount = 0;
				m_State = AS_GUARD;
				m_pAI->SetAnimName("ready");
				m_pAI->ChangeTrack(true);
				m_pAI->SetMoving(false);
			}
			else
			{
				m_dTimeCount += m_dElapsedTime;
			}
		}
		break;
	case AS_GUARD:
		{
			m_pAI->SetMoving(true);
			if (m_dTimeCount > 5000)
			{
				m_dTimeCount = 0;
				m_State = AS_WAIT;
				m_pAI->SetAnimName("wait");
				m_pAI->ChangeTrack(true);
				m_pAI->SetMoving(false);
			}
			else
			{
				m_dTimeCount += m_dElapsedTime;
			}
		}
		break;
	}

	if(m_bGra)
	{
		m_fSpeed += (m_fGravity* m_dElapsedTime);
		if (m_vPos.y - m_pTerrain->GetHeight(m_vPos.x,m_vPos.z) > m_fSpeed*m_dElapsedTime)
			m_vPos.y -= (m_fSpeed*m_dElapsedTime);
		else
			m_vPos.y = m_pTerrain->GetHeight(m_vPos.x,m_vPos.z);
	}
	// 由人物方向转换动画偏移角
	m_vRot.y = CMyFuntion::DirToRot(m_vDir,m_vOrigRot).y;

	Transform();
}

void CMajor::Render(D3DXMATRIX* matWorld)
{
	m_pDevice->SetTransform(D3DTS_WORLD,matWorld);
	m_pAI->Render();
	m_pMeshManager->GetSkinMeshFromID(m_iMeshID)->Render(matWorld);

	CMyFuntion::DrawLine(m_pDevice,m_vPos,m_vDestPos,0xffff0000);
}

bool CMajor::LoadDataFromFile(char* SceneFile,char* AddName)
{
	if (CMyObject::LoadDataFromFile(SceneFile,AddName) == false)
		return false;
	char strData[64];
	GetPrivateProfileString(AddName,"fMoveSpeed","ERROR",strData,sizeof(strData),SceneFile);
	m_fMoveSpeed = atof(strData);
	return true;
}

bool CMajor::CollisionTest(D3DXVECTOR3 vPos)
{
	D3DXMATRIX matWorld,matTrans,matScal;
	D3DXMatrixTranslation(&matTrans,vPos.x,m_pTerrain->GetHeight(vPos.x,vPos.z),vPos.z);
	D3DXMatrixScaling(&matScal,m_vScal.x,m_vScal.y,m_vScal.z);
	matWorld = matScal * matTrans;
	D3DXVECTOR3 tmpMax = m_vOrigMax;
	D3DXVECTOR3 tmpMin = m_vOrigMin;
	D3DXVec3TransformCoord(&tmpMax,&tmpMax,&matWorld);
	D3DXVec3TransformCoord(&tmpMin,&tmpMin,&matWorld);

	map<int,CDeck*>::iterator it = (*m_mapMeshObj).begin();

	for(;it != (*m_mapMeshObj).end();it++)
	{
		if (CMyFuntion::CollisonTestAABB(tmpMin,tmpMax,(*it).second->GetMin(),(*it).second->GetMax()))
		{
			float fDist = 0;
			BOOL bHit;
			
			Ray ray;
			ray.origin = vPos;
			ray.origin.y = vPos.y;
			ray.dir = m_vDir;
			// 射线转换到模型本地空间
			D3DXMATRIX matWorld,matInv;
			matWorld = (*it).second->GetWorldMat();
			D3DXMatrixInverse(&matInv,NULL,&matWorld);
			D3DXVec3TransformCoord(&ray.origin,&ray.origin,&matInv);
			D3DXVec3TransformNormal(&ray.dir,&ray.dir,&matInv);

			ID3DXMesh* pMesh = m_pMeshManager->GetMeshFromID((*it).second->GetMeshID())->GetMesh();
			D3DXIntersect(pMesh,&ray.origin,&ray.dir,&bHit,NULL,NULL,NULL,&fDist,NULL,NULL);
			if (bHit != 0 && fDist <= ((m_vMax.z - m_vMin.z) / 2))
				return true;
		}
	}
	return false;
}

void CMajor::SetBruise()
{
	m_dTimeCount = 0;
	m_State = AS_BRUISE;
	m_pAI->SetAnimName("behited");
	m_pAI->ChangeTrack(true);
	m_pAI->SetMoving(false);
}