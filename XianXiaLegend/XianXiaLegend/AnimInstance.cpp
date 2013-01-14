#include "AnimInstance.h"

CAnimInstance::CAnimInstance(void)
{
	m_pAC             = NULL;
    m_dwCurrentTrack  = 0;
	SetAnimName("");
	m_dwAnimIndex     = 3;
	m_dTimeMax        = 0;
	m_dTimeCurrent    = 0;
	m_timeDelta       = 0.02;
	m_bMoving         = true;
}

CAnimInstance::~CAnimInstance(void)
{
	SAFE_RELEASE(m_pAC);
}

void CAnimInstance::Init(CSkinMesh* pSkinMesh)
{
	pSkinMesh->CloneAnim(&m_pAC);
	SetAnimName("wait");
	SetAnimTrack();
}

void CAnimInstance::Render()
{
	if (m_pAC != NULL)
	{
		if (m_bMoving)
		{
			m_pAC->AdvanceTime(m_timeDelta,NULL);
			m_dTimeCurrent += m_timeDelta;
		}
		else
		{
			m_dTimeCurrent = 0.0;
			m_pAC->ResetTime();
		}
	}
}

void CAnimInstance::SetAnimName(char* strName)
{
	strcpy(m_strNextAnim,strName);
}

void CAnimInstance::SetAnimTrack()
{
	DWORD dwNewTrack = 0;
	ID3DXAnimationSet* pAS;

	m_pAC->GetAnimationSetByName(m_strNextAnim,&pAS);
	m_pAC->SetTrackAnimationSet(dwNewTrack,pAS);
	m_dTimeMax = pAS->GetPeriod();
	pAS->Release();

	m_pAC->UnkeyAllTrackEvents(dwNewTrack);
	m_pAC->SetTrackEnable(dwNewTrack,true);
	m_pAC->KeyTrackSpeed(dwNewTrack,1.0f,m_dTimeCurrent,IDLE_TRANSITION_TIME,D3DXTRANSITION_LINEAR);
	m_pAC->KeyTrackWeight(dwNewTrack,1.0f,m_dTimeCurrent,IDLE_TRANSITION_TIME,D3DXTRANSITION_LINEAR);
	m_dwCurrentTrack = dwNewTrack;
	strcpy(m_strCurrentAnim,m_strNextAnim);
}

void CAnimInstance::ChangeTrack(bool bResetPosition)
{
	DWORD dwNewTrack = (m_dwCurrentTrack == 0 ? 1 : 0);
	ID3DXAnimationSet* pAS;

	m_pAC->GetAnimationSetByName(m_strNextAnim,&pAS);
	m_pAC->SetTrackAnimationSet(dwNewTrack,pAS);
	m_dTimeMax = pAS->GetPeriod();
	pAS->Release();

	m_pAC->UnkeyAllTrackEvents(m_dwCurrentTrack);
	m_pAC->UnkeyAllTrackEvents(dwNewTrack);

	m_pAC->KeyTrackEnable(m_dwCurrentTrack,false,m_dTimeCurrent + IDLE_TRANSITION_TIME);
	m_pAC->KeyTrackSpeed(m_dwCurrentTrack,0.0f,m_dTimeCurrent,IDLE_TRANSITION_TIME,D3DXTRANSITION_LINEAR);
	m_pAC->KeyTrackWeight(m_dwCurrentTrack,0.0f,m_dTimeCurrent,IDLE_TRANSITION_TIME,D3DXTRANSITION_LINEAR);	

	m_pAC->SetTrackEnable(dwNewTrack,true);
	m_pAC->KeyTrackSpeed(dwNewTrack,1.0f,m_dTimeCurrent,IDLE_TRANSITION_TIME,D3DXTRANSITION_LINEAR);
	m_pAC->KeyTrackWeight(dwNewTrack,1.0f,m_dTimeCurrent,IDLE_TRANSITION_TIME,D3DXTRANSITION_LINEAR);
	if (bResetPosition)
		m_pAC->SetTrackPosition(dwNewTrack,0.0);
	
	m_dwCurrentTrack = dwNewTrack;
	strcpy(m_strCurrentAnim,m_strNextAnim);
}

