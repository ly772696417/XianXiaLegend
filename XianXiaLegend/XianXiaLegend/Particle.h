/*
*	������
*/
#pragma once
#include "common.h"

// Helper function to stuff a FLOAT into a DWORD argument
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

#define	EMITTERTYPE_PLANAR_QUADRATE		0x0001
#define	EMITTERTYPE_PLANAR_ROUND		0x0002
#define	EMITTERTYPE_CUBLIC				0x0011
#define	EMITTERTYPE_BALL				0x0012
#define EMITTERTYPE_POINTLIST			0x0021
#define	EMITTERTYPE_TRIANGLELIST		0x0022
#define EMITTERTYPE_CUSTOM				0xF001

#define PARTICLETYPE_SINGLE				0x0001
#define PARTICLETYPE_STRIP				0x0002

class CParticleSystem
{
protected:

	struct POINTVERTEX
	{
		D3DXVECTOR3 v;
		D3DCOLOR    color;
		static const DWORD FVF;
	};

	struct PARTICLE
	{
		D3DXVECTOR3 m_vPos;       // ��ǰλ��
		D3DXVECTOR3 m_vVel;       // ��ǰ�ٶ�

		D3DXVECTOR3 m_vPos0;      // ��ʼλ��
		D3DXVECTOR3 m_vVel0;      // ��ʼ�ٶ�

		FLOAT       m_fTime0;     // ����ʱ��
		FLOAT       m_fFade;      //
		DWORD		m_clrEmit;    // ������ɫ
		DWORD		m_clrFade;    // ������ɫ
		PARTICLE*   m_pNext;      // ��һ������

	};

protected:
	float	  m_fGravity;          // ���ٶ�
	float	  m_fParticleLife;
	float	  m_fParticleFade;
	float	  m_EmitRate;	       // ��������
	float	  m_EmitVel;	       // ��ʼ�ٶ�
	float	  m_EmitAngle;         // ����ĽǶ�
	float	  m_EmitWidth;         // ������
	bool      m_bEnable;           // ���俪��

	float	  m_fParticleSize;
    float	  m_fTime;

	DWORD	  m_dwType;

    DWORD     m_dwBase;	           // ��ǰʹ�õ��ǻ������еĵڼ�������
	DWORD     m_dwFlush;           // ����С��Ĵ�С,ÿ����Ⱦ��һС��
    DWORD     m_dwDiscard;         // ��������

    DWORD     m_dwParticles;       // ��������
    DWORD     m_dwParticlesLim;    // ������������
	
	DWORD		m_clrEmit;
	DWORD		m_clrFade;

	D3DXVECTOR3 m_vPosition;        // λ��,���������λ��,
	D3DXVECTOR3 m_vDirection;       // ����
	D3DXMATRIX  m_mDir;			

    PARTICLE* m_pParticles;         // ���������
    PARTICLE* m_pParticlesFree;     // ������������

    // Geometry
    LPDIRECT3DVERTEXBUFFER9 m_pVB;  // ���㻺����ָ��
	LPDIRECT3DDEVICE9	m_pDevice;  // �豸
	LPDIRECT3DTEXTURE9	m_pTexture; // ����

public:
	CParticleSystem(LPDIRECT3DDEVICE9 pDevice/*,char *strTexture*/);
   ~CParticleSystem();

    HRESULT Update( float fSecsPerFrame);
    HRESULT Render(float tFrame);

	HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
	HRESULT LoadFromFile(char* strFile,char *strSection);
	// set
	void SetPosition(D3DXVECTOR3 pV) {m_vPosition = pV;}
	void SetYawPitch(float yaw,float pitch);
	void SetEnable(bool value) {m_bEnable = value;}
	// get
	D3DXVECTOR3 GetPosition() {return m_vPosition;}
	float GetEmitWidth() {return m_EmitWidth;}

private:
	DWORD ColorLerp(DWORD color1,DWORD color2,float fWeight);
};
