#include "Monster.h"
#include "Major.h"

CMonster::CMonster(IDirect3DDevice9* pDevice,CMeshManager* pMeshManager)
{
	m_pDevice         = pDevice;
	m_pMeshManager    = pMeshManager;
	m_pAI             = NULL;
	m_pMajor          = NULL;
	m_fMoveSpeed      = 1;
	m_State           = AS_WAIT;
	m_iSkillNum       = 1;
	m_dElapsedTime    = 0.1;
	m_iWatchDist      = 0;
	m_iAttackDist     = 30;
	m_dTimeLapse      = 0.0;
	m_dEndTime        = 0.0;
	m_dTimeCount      = 0.0;
}

CMonster::~CMonster(void)
{
	SAFE_DELETE(m_pAI);
}

bool CMonster::Init(CTerrain* pTerrain,
					char* SceneFile,
					char* AddName,
					map<int,CDeck*>* MeshObj,
					CMajor* pMajor)
{
	if (LoadDataFromFile(SceneFile,AddName) == false)
		return false;
	m_pMeshManager->GetSkinMeshFromID(m_iMeshID)->GetBoundingB_S(&m_vOrigMax,&m_vOrigMin,&m_vCenter,&m_fRadius);
	m_vOrigRot = m_vRot;
	m_pTerrain = pTerrain;
	m_vPos.y = m_pTerrain->GetHeight(m_vPos.x,m_vPos.z);
	m_vDestPos = m_vPos;

	m_pAI = new CAnimInstance();
	if (m_pAI == NULL)
		return false;
	m_pAI->Init(m_pMeshManager->GetSkinMeshFromID(m_iMeshID));
	m_pAI->SetTimeDelta(m_dElapsedTime);

	m_mapMeshObj = MeshObj;
	m_pMajor = pMajor;

	Transform();
	m_iAttackDist = m_vMax.z - m_vMin.z;

	return true;
}

void CMonster::Update(DOUBLE dElapsedTime)
{
	m_dElapsedTime = dElapsedTime;
	m_pAI->SetTimeDelta(m_dElapsedTime);

	switch (m_State)
	{
	case AS_WAIT:
		{
			// 发现敌人追击
			if (D3DXVec3Length(&(m_pMajor->GetPos() - m_vPos)) <= m_iWatchDist)
			{
				m_dTimeCount = 0;
				m_dTimeLapse = 0;
				m_dEndTime = 0;
				m_State = AS_CHASE;
				m_pAI->SetMoving(false);
				m_pAI->SetAnimName("run");
				m_pAI->ChangeTrack(true);
				break;
			}
			// 无事待机
			if (m_dTimeCount > m_dEndTime)
			{
				m_dTimeCount = 0;
				if (m_pAI->GetMoving() == false)
				{
					m_dEndTime = m_pAI->GetTimeMax();
					m_pAI->SetMoving(true);
				}
				else
				{
					m_dEndTime = (double)CMyFuntion::RangeRandom(5.0f,15.0f);
					m_pAI->SetMoving(false);
				}
			}
			else
			{
				m_dTimeCount += m_dElapsedTime;
			}
		}
		break;
	case AS_WALK:
		break;
	case AS_CHASE:
		{
			// 进入攻击距离,攻击
			if (D3DXVec3Length(&(m_pMajor->GetPos() - m_vPos)) <= m_iAttackDist)
			{
				m_dTimeCount = 0;
				m_dTimeLapse = 0;
				m_dEndTime = 0;
				m_State = AS_ATTACK;
				m_pAI->SetAnimName("attack1");
				m_pAI->ChangeTrack(true);
				m_pAI->SetMoving(false);
				break;
			}
			// 离开视线范围
			if (D3DXVec3Length(&(m_pMajor->GetPos() - m_vPos)) > m_iWatchDist)
			{
				m_dTimeCount = 0;
				m_dTimeLapse = 0;
				m_dEndTime = 0;
				m_State = AS_WAIT;
				m_pAI->SetAnimName("wait");
				m_pAI->ChangeTrack(true);
				m_pAI->SetMoving(false);
				break;
			}
			// 追击
			m_pAI->SetMoving(true);
			D3DXVECTOR3 tmpDir = m_pMajor->GetPos() - m_vPos;
			tmpDir.y = 0;
			D3DXVec3Normalize(&tmpDir,&tmpDir);
			m_vDir = tmpDir;

			D3DXVECTOR3 vPos = m_vPos + m_vDir * m_fMoveSpeed * m_dElapsedTime * 2;
			vPos.y = m_pTerrain->GetHeight(vPos.x,vPos.z);
			float Slope = CMyFuntion::CalcSlope(m_vPos,vPos);
			if (Slope <= TERRAIN_SLOPE && 
				Slope >= -TERRAIN_SLOPE &&
				CollisionTest(vPos) == false)
				m_vPos = vPos;
		}
		break;
	case AS_ATTACK:
		{
			// 离开攻击范围
			if (D3DXVec3Length(&(m_pMajor->GetPos() - m_vPos)) > m_iAttackDist)
			{
				m_dTimeCount = 0;
				m_dTimeLapse = 0;
				m_dEndTime = 0;
				m_State = AS_CHASE;
				m_pAI->SetAnimName("run");
				m_pAI->ChangeTrack(true);
				m_pAI->SetMoving(false);
				break;
			}
			// 攻击
			if (m_dTimeCount > m_dEndTime)
			{
				m_dTimeCount = 0;
				if (m_pAI->GetMoving() == false)
				{
					m_pAI->SetMoving(true);
					m_dEndTime = m_pAI->GetTimeMax();
				}
				else
				{
					int tmpNum = CMyFuntion::RangeRandom(1,m_iSkillNum+0.99999);
					char tmpSkill[32];
					sprintf(tmpSkill,"attack%d",tmpNum);
					m_pAI->SetAnimName(tmpSkill);
					m_pAI->ChangeTrack(true);
					m_pAI->SetMoving(false);
					m_dEndTime = (double)CMyFuntion::RangeRandom(0.5f,1.0f);
					if (m_pMajor->GetState() != AS_ATTACK)
						m_pMajor->SetBruise();
				}
			}
			else
			{
				m_dTimeCount += m_dElapsedTime;
			}
		}
		break;
	case AS_BRUISE:
		{
			if (m_dTimeLapse > 0)
			{
				m_dTimeLapse -= m_dElapsedTime;
				m_dTimeCount = 0;
			}
			else
			{
				m_pAI->SetMoving(false);
				if (m_dTimeCount < m_dEndTime)
				{
					D3DXVECTOR3 vPos = m_vPos + m_vDir * m_fMoveSpeed * m_dElapsedTime * 2;
					vPos.y = m_pTerrain->GetHeight(vPos.x,vPos.z);
					float Slope = CMyFuntion::CalcSlope(m_vPos,vPos);
					if (Slope <= TERRAIN_SLOPE &&
						Slope >= -TERRAIN_SLOPE && 
						CollisionTest(vPos) == false)
					{
						m_vPos = vPos;
					}
					m_dTimeCount += m_dElapsedTime;
				}
				else
				{
					m_vDestPos = m_vPos;
					m_vDir.x = -m_vDir.x;
					m_vDir.z = -m_vDir.z;
					m_vRot.y = CMyFuntion::DirToRot(m_vDir,m_vOrigRot).y;
					m_State = AS_WAIT;
				}
			}
		}
		break;
	}
	m_vPos.y = m_pTerrain->GetHeight(m_vPos.x,m_vPos.z);
	// 由人物方向转换动画偏移角
	if (m_State != AS_BRUISE)
		m_vRot.y = CMyFuntion::DirToRot(m_vDir,m_vOrigRot).y;

	Transform();
}

void CMonster::Render(D3DXMATRIX* matWorld)
{
	m_pDevice->SetTransform(D3DTS_WORLD,matWorld);
	m_pAI->Render();
	m_pMeshManager->GetSkinMeshFromID(m_iMeshID)->Render(matWorld);
}

bool CMonster::LoadDataFromFile(char* SceneFile,char* AddName)
{
	if (CMyObject::LoadDataFromFile(SceneFile,AddName) == false)
		return false;
	char strData[64];
	GetPrivateProfileString(AddName,"fMoveSpeed","ERROR",strData,sizeof(strData),SceneFile);
	m_fMoveSpeed = atof(strData);
	m_iWatchDist = GetPrivateProfileInt(AddName,"iWatchDist",0,SceneFile);
	m_iSkillNum = GetPrivateProfileInt(AddName,"iSkillNum",0,SceneFile);
	return true;
}

bool CMonster::CollisionTest(D3DXVECTOR3 vPos)
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
