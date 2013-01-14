/*
*	粒子类
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
		D3DXVECTOR3 m_vPos;       // 当前位置
		D3DXVECTOR3 m_vVel;       // 当前速度

		D3DXVECTOR3 m_vPos0;      // 初始位置
		D3DXVECTOR3 m_vVel0;      // 初始速度

		FLOAT       m_fTime0;     // 创建时间
		FLOAT       m_fFade;      //
		DWORD		m_clrEmit;    // 发射颜色
		DWORD		m_clrFade;    // 结束颜色
		PARTICLE*   m_pNext;      // 下一个粒子

	};

protected:
	float	  m_fGravity;          // 加速度
	float	  m_fParticleLife;
	float	  m_fParticleFade;
	float	  m_EmitRate;	       // 发射速率
	float	  m_EmitVel;	       // 初始速度
	float	  m_EmitAngle;         // 发射的角度
	float	  m_EmitWidth;         // 发射宽度
	bool      m_bEnable;           // 发射开关

	float	  m_fParticleSize;
    float	  m_fTime;

	DWORD	  m_dwType;

    DWORD     m_dwBase;	           // 当前使用的是缓冲区中的第几个顶点
	DWORD     m_dwFlush;           // 顶点小块的大小,每次渲染这一小块
    DWORD     m_dwDiscard;         // 顶点数量

    DWORD     m_dwParticles;       // 粒子数量
    DWORD     m_dwParticlesLim;    // 粒子数量限制
	
	DWORD		m_clrEmit;
	DWORD		m_clrFade;

	D3DXVECTOR3 m_vPosition;        // 位置,世界坐标的位置,
	D3DXVECTOR3 m_vDirection;       // 方向
	D3DXMATRIX  m_mDir;			

    PARTICLE* m_pParticles;         // 活动粒子链表
    PARTICLE* m_pParticlesFree;     // 消亡粒子链表

    // Geometry
    LPDIRECT3DVERTEXBUFFER9 m_pVB;  // 顶点缓冲区指针
	LPDIRECT3DDEVICE9	m_pDevice;  // 设备
	LPDIRECT3DTEXTURE9	m_pTexture; // 纹理

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
