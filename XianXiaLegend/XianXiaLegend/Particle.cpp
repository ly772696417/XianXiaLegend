#include "Particle.h"
const DWORD CParticleSystem::POINTVERTEX::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

CParticleSystem::CParticleSystem(LPDIRECT3DDEVICE9 pDevice/*,char *strTexture*/)
{
	m_pDevice	  =pDevice;

	m_dwType	  =EMITTERTYPE_PLANAR_ROUND;
	m_EmitWidth	  =50.0f; 
	m_EmitVel	  =100.0f; 
	m_EmitRate	  =64;
	m_EmitAngle   =D3DX_PI*0.02f; 
	m_bEnable     = false;

	m_fTime			=0.0f;
	m_fGravity		=-9.8f;
	m_fParticleLife	=3.0f;
	m_fParticleFade	=2.0f;
	m_fParticleSize	=16.0f;

	m_dwParticles    = 0;
	m_dwParticlesLim = 4096 ;

	m_dwFlush        = 512;				//小块粒子
	m_dwDiscard      = m_dwFlush*64;	//缓冲区大小
	m_dwBase         = m_dwDiscard;		//缓冲区的起始位置

	m_vPosition   =D3DXVECTOR3(0,50,0);
	m_clrEmit=0xFFFF8040;
	m_clrFade=0x018080FF;

	SetYawPitch(D3DX_PI*0.0f,D3DX_PI*0.5f);

	m_pTexture		 = NULL;
	m_pParticles     = NULL;
	m_pParticlesFree = NULL;
	m_pVB            = NULL;

	RestoreDeviceObjects(); 
}

CParticleSystem::~CParticleSystem()
{
	InvalidateDeviceObjects();

	while( m_pParticles )
	{
		PARTICLE* pSpark = m_pParticles;
		m_pParticles = pSpark->m_pNext;
		delete pSpark;
	}

	while( m_pParticlesFree )
	{
		PARTICLE *pSpark = m_pParticlesFree;
		m_pParticlesFree = pSpark->m_pNext;
		delete pSpark;
	}
}

HRESULT CParticleSystem::RestoreDeviceObjects()
{
	HRESULT hr;

	// Create a vertex buffer for the particle system.  The size of this buffer
	// does not relate to the number of particles that exist.  Rather, the
	// buffer is used as a communication channel with the device.. we fill in 
	// a bit, and tell the device to draw.  While the device is drawing, we
	// fill in the next bit using NOOVERWRITE.  We continue doing this until 
	// we run out of vertex buffer space, and are forced to DISCARD the buffer
	// and start over at the beginning.

	if(FAILED(hr = m_pDevice->CreateVertexBuffer( m_dwDiscard * 
		sizeof(POINTVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 
		POINTVERTEX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL )))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CParticleSystem::InvalidateDeviceObjects()
{
	SAFE_RELEASE(m_pTexture); 
	SAFE_RELEASE(m_pVB );
	return S_OK;
}

HRESULT CParticleSystem::Update( float fSecsPerFrame)
{
	PARTICLE *pParticle, **ppParticle;
	D3DXVECTOR3 vDirLocal;   //粒子的本地坐标
	D3DXVECTOR3 vPosLocal;	

	if(fSecsPerFrame>1.0) return S_OK;

	m_fTime += fSecsPerFrame;

	ppParticle = &m_pParticles;
	//更新粒子状态
	while( *ppParticle )
	{    pParticle = *ppParticle;
		// 计算新位置
		float fT = m_fTime - pParticle->m_fTime0;

		//设置加速度

		//落下时
		//衰减
		pParticle->m_fFade -= fSecsPerFrame;

		pParticle->m_vPos    = pParticle->m_vVel0 * fT + pParticle->m_vPos0;
		pParticle->m_vPos.y += (0.5f * m_fGravity) * (fT * fT);
		pParticle->m_vVel.y  = pParticle->m_vVel0.y + m_fGravity * fT;


		//删除旧粒子
		if( /*pParticle->m_vPos.y <0 ||*/ m_fTime-pParticle->m_fTime0>m_fParticleLife )// m_fRadius)
		{
			//删除这个粒子,把它加入到消亡链表
			*ppParticle = pParticle->m_pNext;		
			pParticle->m_pNext = m_pParticlesFree;
			m_pParticlesFree = pParticle;

			m_dwParticles--;
		}
		else
		{  
			ppParticle = &pParticle->m_pNext;
		}
	}

	//发射新粒子,每次至少发射一个粒子
	if (m_bEnable)
	{
		DWORD dwParticlesEmit = m_dwParticles + 
			DWORD(fSecsPerFrame * m_EmitRate >= 1 ? m_EmitRate * fSecsPerFrame:	//发射率算出来的数量大于1
		fmodf(m_fTime,1/m_EmitRate)<fSecsPerFrame?1:0);			//波形分析,t2=t%T2<T1时,应该发射一个粒子

		while( m_dwParticles < m_dwParticlesLim && m_dwParticles < dwParticlesEmit )
		{
			if( m_pParticlesFree )	//如果消亡粒子链表里有,就从消亡链表里取出一个节点
			{
				pParticle = m_pParticlesFree;
				m_pParticlesFree = pParticle->m_pNext;
			}
			else					//如果没有,就再创建
			{
				if( NULL == ( pParticle = new PARTICLE ) )
					return E_OUTOFMEMORY;
			}

			pParticle->m_pNext = m_pParticles;	//加入到活动链表
			m_pParticles = pParticle;			
			m_dwParticles++;					

			// Emit new particle
			FLOAT fRand1 = ((FLOAT)rand()/(FLOAT)RAND_MAX) * D3DX_PI * 2.0f;
			FLOAT fRand2 = ((FLOAT)rand()/(FLOAT)RAND_MAX) * m_EmitAngle*0.5f;	//	m_EmitAngle锥形角度,所以是一半
			FLOAT Speed  = ((FLOAT)rand()/(FLOAT)RAND_MAX*0.2f+0.8f) * m_EmitVel;
			//初始化新加入的粒子
			vDirLocal.x=Speed*	cosf(fRand1) * sinf(fRand2);
			vDirLocal.y=Speed*	cosf(fRand2);
			vDirLocal.z=Speed*	sinf(fRand1) * sinf(fRand2);

			D3DXVec3TransformCoord(&vDirLocal,&vDirLocal,&m_mDir);

			fRand1=((FLOAT)rand()/((FLOAT)RAND_MAX))-0.5f;
			fRand2=((FLOAT)rand()/((FLOAT)RAND_MAX))-0.5f;


			float u,v,w;

			switch(m_dwType)
			{
			case EMITTERTYPE_PLANAR_QUADRATE:
				u=fRand1;
				w=fRand2;
				v=0;
				break;
			case EMITTERTYPE_PLANAR_ROUND:
				u=fRand1*sinf(fRand2*D3DX_PI/2);
				w=fRand1*cosf(fRand2*D3DX_PI/2);
				v=0;
				break;
			default:
				u=0;
				w=0;
				v=0;
				break;
			}
			vPosLocal.x=m_EmitWidth*u;  
			vPosLocal.y=m_EmitWidth*v;
			vPosLocal.z=m_EmitWidth*w; 

			pParticle->m_vVel0 = vDirLocal; 
			pParticle->m_vPos0 = m_vPosition+vPosLocal ;


			pParticle->m_vPos = pParticle->m_vPos0;
			pParticle->m_vVel = pParticle->m_vVel0;

			pParticle->m_clrEmit=m_clrEmit;
			pParticle->m_clrFade=m_clrFade;

			pParticle->m_fFade      = m_fParticleFade;
			pParticle->m_fTime0     = m_fTime;
		}
	}
	return S_OK;
}

HRESULT CParticleSystem::Render(float tFrame)
{
	// 保存当前渲染状态
	DWORD Data[6];
	m_pDevice->GetRenderState(D3DRS_LIGHTING,&Data[0]);
	m_pDevice->GetRenderState(D3DRS_ZWRITEENABLE,&Data[1]);
	m_pDevice->GetRenderState(D3DRS_ALPHATESTENABLE,&Data[2]);
	m_pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,&Data[3]);
	m_pDevice->GetRenderState(D3DRS_POINTSPRITEENABLE,&Data[4]);
	m_pDevice->GetRenderState(D3DRS_POINTSCALEENABLE,&Data[5]);

	HRESULT hr;

	D3DXMATRIX matWorld,matScal;
	D3DXMatrixIdentity(&matWorld);
	//D3DXMatrixScaling( &matScal, 1000,1000,1000 ); 
	//D3DXMatrixTranslation(&matWorld,m_vPosition.x,m_vPosition.y,m_vPosition.z);
	//matWorld = matScal*matWorld;
	m_pDevice->SetTransform(D3DTS_WORLD,&matWorld);

	m_pDevice->SetTexture(0,m_pTexture); 

	m_pDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// 纹理颜色和粒子颜色相乘
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_MODULATE);
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT);

	// 设置精灵渲染状态
	m_pDevice->SetRenderState( D3DRS_POINTSPRITEENABLE,TRUE );
	m_pDevice->SetRenderState( D3DRS_POINTSCALEENABLE,TRUE );
	m_pDevice->SetRenderState( D3DRS_POINTSIZE,FtoDW(m_fParticleSize) );
	m_pDevice->SetRenderState( D3DRS_POINTSIZE_MIN,FtoDW(0.00f) );
	m_pDevice->SetRenderState( D3DRS_POINTSCALE_A,FtoDW(0.00f) );
	m_pDevice->SetRenderState( D3DRS_POINTSCALE_B,FtoDW(0.00f) );
	m_pDevice->SetRenderState( D3DRS_POINTSCALE_C,FtoDW(1.00f) );

	m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(POINTVERTEX) );
	m_pDevice->SetFVF( POINTVERTEX::FVF );

	PARTICLE*    pParticle = m_pParticles;
	POINTVERTEX* pVertices;
	DWORD        dwNumParticlesToRender = 0;

	//锁定顶点缓冲区,以小块填充,如果所有的小块都填充了,就绘制它们,然后再锁定下一个块,
	//如果空间用完了,就从头开始,使用DISCARD方式销毁

	m_dwBase += m_dwFlush;

	if(m_dwBase >= m_dwDiscard)
		m_dwBase = 0;

	//dwBase开始是没有使用的缓冲区, 要使用m_dwFlush个顶点
	if( FAILED( hr = m_pVB->Lock( m_dwBase * sizeof(POINTVERTEX), m_dwFlush * sizeof(POINTVERTEX),
		(void**) &pVertices, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) ) )
	{
		return hr;
	}

	//	借助 D3DLOCK_NOOVERWRITE 对 VB 进行 Lock （锁定）。这告知 Direct3D 和驱动程序，
	//	您将要添加顶点，而并不修改您先前批处理过的顶点。 因此，如果当时正在进行一项 DMA 
	//	操作，则并不中断该操作.

	// Render each particle
	while( pParticle )
	{
		D3DXVECTOR3 vPos(pParticle->m_vPos);
		D3DXVECTOR3 vVel(pParticle->m_vVel);
		FLOAT       fLengthSq = D3DXVec3LengthSq(&vVel);
		//UINT        dwSteps;

		DWORD dwDiffuse=ColorLerp(pParticle->m_clrEmit,
			pParticle->m_clrFade,
			1-pParticle->m_fFade/(m_fParticleFade));

		// Render each particle a bunch of times to get a blurring effect
		pVertices->v     = vPos;
		pVertices->color = dwDiffuse;
		pVertices++;

		if( ++dwNumParticlesToRender == m_dwFlush )
		{
			// Done filling this chunk of the vertex buffer.  Lets unlock and
			// draw this portion so we can begin filling the next chunk.

			m_pVB->Unlock();

			if(FAILED(hr = m_pDevice->DrawPrimitive( D3DPT_POINTLIST, m_dwBase, dwNumParticlesToRender)))
				return hr;

			// Lock the next chunk of the vertex buffer.  If we are at the 
			// end of the vertex buffer, DISCARD the vertex buffer and start
			// at the beginning.  Otherwise, specify NOOVERWRITE, so we can
			// continue filling the VB while the previous chunk is drawing.
			m_dwBase += m_dwFlush;

			if(m_dwBase >= m_dwDiscard)
				m_dwBase = 0;

			if( FAILED( hr = m_pVB->Lock( m_dwBase * sizeof(POINTVERTEX), m_dwFlush * sizeof(POINTVERTEX),
				(void**) &pVertices, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) ) )
			{
				return hr;
			}

			dwNumParticlesToRender = 0;
		}

		pParticle = pParticle->m_pNext;
	}

	// Unlock the vertex buffer
	m_pVB->Unlock();

	// Render any remaining particles
	if( dwNumParticlesToRender )
	{
		if(FAILED(hr = m_pDevice->DrawPrimitive( D3DPT_POINTLIST, m_dwBase, dwNumParticlesToRender )))
			return hr;
	}

	// 重设渲染状态
	m_pDevice->SetRenderState(D3DRS_LIGHTING,Data[0]);
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE,Data[1]);
	m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,Data[2]);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,Data[3]);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE,Data[4]);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE,Data[5]);

	m_pDevice->SetTextureStageState(0,D3DTSS_COLOROP ,D3DTOP_BLENDTEXTUREALPHA);
	m_pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_CURRENT);

	return S_OK;
}

HRESULT CParticleSystem::LoadFromFile(char* strFile,char *strSection)
{
#define STRTEMP_SIZE 128

	char	strTemp[STRTEMP_SIZE];
	float	fTemp;
	DWORD	dwTemp;

	// 加速度
	GetPrivateProfileString(strSection,"GravityField","0",strTemp,STRTEMP_SIZE,strFile);
	fTemp=(float)atof(strTemp);  
	m_fGravity=fTemp;
	// 发射宽度
	GetPrivateProfileString(strSection,"EmitWidth","0",strTemp,STRTEMP_SIZE,strFile);
	fTemp=(float)atof(strTemp);  
	m_EmitWidth=fTemp;
	//发射角度
	GetPrivateProfileString(strSection,"EmitAngle","0",strTemp,STRTEMP_SIZE,strFile);
	fTemp=(float)atof(strTemp);  
	m_EmitAngle=fTemp*D3DX_PI/180;
	//发射频率
	GetPrivateProfileString(strSection,"EmitRate","0",strTemp,STRTEMP_SIZE,strFile);
	fTemp=(float)atof(strTemp);  
	if(fTemp>0) m_EmitRate=fTemp;
	//发射速率
	GetPrivateProfileString(strSection,"EmitVel","0",strTemp,STRTEMP_SIZE,strFile);
	fTemp=(float)atof(strTemp);  
	if(fTemp>0) m_EmitVel=fTemp;
	// 发射颜色
	GetPrivateProfileString(strSection,"EmitColor","0",strTemp,STRTEMP_SIZE,strFile);
	m_clrEmit=strtoul (strTemp,0,0);  
	// 结束颜色
	GetPrivateProfileString(strSection,"FadeColor","0",strTemp,STRTEMP_SIZE,strFile);
	m_clrFade=strtoul (strTemp,0,0);  

	GetPrivateProfileString(strSection,"EmitType","0",strTemp,STRTEMP_SIZE,strFile);
	m_dwType=strtoul (strTemp,0,0);  
	//发射方向
	float yaw,pitch;
	GetPrivateProfileString(strSection,"EmitYaw","0",strTemp,STRTEMP_SIZE,strFile);
	yaw=(float)atof(strTemp)*D3DX_PI/180; 
	GetPrivateProfileString(strSection,"EmitPitch","0",strTemp,STRTEMP_SIZE,strFile);
	pitch=(float)atof(strTemp)*D3DX_PI/180; 
	SetYawPitch(yaw,pitch);
	//粒子大小
	GetPrivateProfileString(strSection,"ParticleSize","0",strTemp,STRTEMP_SIZE,strFile);
	fTemp=(float)atof(strTemp);  
	if(fTemp>0) m_fParticleSize=fTemp;
	//粒子生命周期
	GetPrivateProfileString(strSection,"ParticleLife","0",strTemp,STRTEMP_SIZE,strFile);
	fTemp=(float)atof(strTemp);  
	if(fTemp>0) m_fParticleLife=fTemp;

	GetPrivateProfileString(strSection,"ParticleFade","0",strTemp,STRTEMP_SIZE,strFile);
	fTemp=(float)atof(strTemp);  
	if(fTemp>0) m_fParticleFade=fTemp;
	//粒子数量上限
	GetPrivateProfileString(strSection,"ParticlesLimit","0",strTemp,STRTEMP_SIZE,strFile);
	dwTemp=atol(strTemp);  
	if(dwTemp>0) m_dwParticlesLim=dwTemp;
	//纹理
	GetPrivateProfileString(strSection,"ParticleTexture","\0",strTemp,STRTEMP_SIZE,strFile);

	if(strTemp[0]!=0)
	{
		SAFE_RELEASE(m_pTexture);
		D3DXCreateTextureFromFile(m_pDevice,strTemp,&m_pTexture); 
	}
	return S_OK;	
}

void CParticleSystem::SetYawPitch(float yaw,float pitch)
{
	m_vDirection= 
		D3DXVECTOR3(cosf(pitch)*sinf(yaw),
					sinf(pitch),
					cosf(pitch)*cosf(yaw));

	
	D3DXVECTOR3	vNormal=D3DXVECTOR3(1,0,0); 
	if(abs(D3DXVec3Dot(&vNormal,&m_vDirection))<1.0e-6)
	{
		vNormal=D3DXVECTOR3(0,0,1); 
	}
	
	memset(m_mDir,0,sizeof(m_mDir));

	
	m_mDir._44=1.0f;
	memcpy(&m_mDir._21,m_vDirection,sizeof(m_vDirection));
	D3DXVec3Cross((D3DXVECTOR3*)&m_mDir._31,&vNormal,&m_vDirection);  
	D3DXVec3Cross((D3DXVECTOR3*)&m_mDir._11,
		(D3DXVECTOR3*)&m_mDir._21,
		(D3DXVECTOR3*)&m_mDir._31);
}



DWORD	CParticleSystem::ColorLerp(DWORD color1,DWORD color2,float fWeight)
{
		DWORD Weight=(BYTE)(fWeight*0x100);
		if(fWeight<=0.0f) return color1;
		else if(fWeight>=1) return color2; 
		else
		{	
			DWORD IWeight=Weight^0xFF;
			DWORD dwTemp=0;
			dwTemp =(((0xFF00FF00&color1)>>8)*IWeight+ 
				     ((0xFF00FF00&color2)>>8)*Weight
				    )&0xFF00FF00;

			dwTemp|=(((0x00FF00FF&color1)*IWeight+
				      (0x00FF00FF&color2)*Weight
				     )&0xFF00FF00)>>8; 
			return dwTemp;
		}

}

//
//DWORD	CParticleSystem::ColorLerp(DWORD color1,DWORD color2,float fWeight)
//{
//	if(fWeight<=0.01f) return color1;
//	else if(fWeight>=1) return color2; 
//
//	else
//	{
//		D3DXCOLOR c1(color1);
//		D3DXCOLOR c2(color2);
//		BYTE bColor[4];
//		bColor[2]=255*(c1.r+fWeight*(c2.r-c1.r));
//		bColor[1]=255*(c1.g+fWeight*(c2.g-c1.g));
//		bColor[0]=255*(c1.b+fWeight*(c2.b-c1.b));
//		bColor[3]=255*(c1.a+fWeight*(c2.a-c1.a));
//		return *(DWORD*)bColor;
//	
//	
//	}
//
//}

/*
DWORD	CParticleSystem::ColorLerp(DWORD color1,DWORD color2,float fWeight)
{
	if(fWeight<=0.01f) return color1;
	else if(fWeight>=1) return color2; 

	else
	{
		BYTE bColor[4];
		BYTE *bColor1=&color1
		BYTE *bColor1=&color1

		bColor[0]=c1.r+fWeight*(c2.r-c1.r);
		bColor[1]=c1.g+fWeight*(c2.g-c1.g);
		bColor[2]=c1.b+fWeight*(c2.b-c1.b);
		bColor[3]=c1.a+fWeight*(c2.a-c1.a);


		return *(DWORD*)bColor;
	
	
	}

}
*/

