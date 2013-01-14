#include "stdafx.h"
#include ".\particlesystem.h"
#include "assert.h"
//#define new VNEW

// ================================== 粒子 =================================
Particle::Particle() :
m_fWidth(0.f),
m_fHeight(0.f),
m_vPosition(0.f, 0.f, 0.f),
m_vDirection(0.f, 0.f, 0.f),
m_Color(0xffffffff),
m_fTimeToLive(0.f),
m_fTotalTimeToLive(0.f)
{
}

// ================================== 粒子团 ================================
ParticleGroup::ParticleGroup(void) :
m_pDevice(NULL),
m_pVB(NULL),
m_pIB(NULL),
m_uiPoolSize(0),
m_uiMaxNumVertices(0),
m_uiMaxNumIndices(0),
m_fDefaultWidth(1.f),
m_fDefaultHeight(1.f),
m_pCamera(NULL),
m_BillboardType(BT_POINT),
m_CommonDirection(1.f, 0.f, 0.f),
m_CommonUp(0.f, 0.f, 1.f),
m_pTexture(NULL),
//m_SceneBlendType(SBT_ADD_COLOR),
m_bAnimationTexture(false),
m_fTexAnimDuration(1.f),
m_CoordinateSystem(PCS_WORLD),
//m_bEnable(true),
//m_b2DParticle(false),
m_bRotationTexture(false)
{
}

ParticleGroup::~ParticleGroup(void)
{
}

//bool ParticleGroup::Load( LPCSTR pName )
//{
//	TiXmlDocument doc( pName );
//	doc.LoadFile();
//	TiXmlDocument *pDoc = &doc;
//
//	TiXmlElement *pRoot = NULL;
//	TiXmlElement *pElem = NULL;
//	string name;
//
//	pRoot = pDoc->FirstChildElement();
//
//	pElem = pRoot->FirstChildElement( "是否是2D粒子系统" );
//	name = pElem->GetText();
//	if( atoi( name.c_str() ) == 0 )
//	{
//		m_b2DParticle = false;
//	}
//	else
//	{
//		m_b2DParticle = true;
//	}
//
//	pElem = pRoot->FirstChildElement( "粒子系统坐标系" );
//	name = pElem->GetText();
//	m_CoordinateSystem = static_cast<ParticleCoordinateSystem>(atoi( name.c_str() ));
//
//	pElem = pRoot->FirstChildElement( "与场景混合方式" );
//	name = pElem->GetText();
//	m_SceneBlendType = static_cast<SceneBlendType>(atoi( name.c_str() ));
//
//	pElem = pRoot->FirstChildElement( "默认高度" );
//	name = pElem->GetText();
//	m_fDefaultHeight = static_cast<float>(atof( name.c_str() ));
//
//
//	pElem = pRoot->FirstChildElement( "默认宽度" );
//	name = pElem->GetText();
//	m_fDefaultWidth = static_cast<float>(atof( name.c_str() ));
//
//
//	pElem = pRoot->FirstChildElement( "纹理名" );
//	name = pElem->GetText();
//
//	//name = ".//Resource//Texture//" + name;
//	SetTexture( const_cast<LPSTR>(name.c_str()) );
//
//	pElem = pRoot->FirstChildElement( "最大粒子数" );
//	name = pElem->GetText();
//	SetPoolSize( atoi(name.c_str()) );
//
//	pElem = pRoot->FirstChildElement( "面向摄像机的方式" );
//	name = pElem->GetText();
//	m_BillboardType = static_cast<BillboardType>(atoi(name.c_str()));
//
//	pElem = pRoot->FirstChildElement( "粒子UP向量" );
//	m_CommonUp.x = static_cast<float>(atof( pElem->Attribute( "x" ) ));
//	m_CommonUp.y = static_cast<float>(atof( pElem->Attribute( "y" ) ));
//	m_CommonUp.z = static_cast<float>(atof( pElem->Attribute( "z" ) ));
//
//	pElem = pRoot->FirstChildElement( "粒子朝向" );
//	m_CommonDirection.x = static_cast<float>(atof( pElem->Attribute( "x" ) ));
//	m_CommonDirection.y = static_cast<float>(atof( pElem->Attribute( "y" ) ));
//	m_CommonDirection.z = static_cast<float>(atof( pElem->Attribute( "z" ) ));
//
//	pElem = pRoot->FirstChildElement( "纹理动画循环时间" );
//	name = pElem->GetText();
//	m_fTexAnimDuration = static_cast<float>(atof( name.c_str() ));
//
//
//	TiXmlElement *pElemEmitter = NULL;
//	pElemEmitter = pRoot->FirstChildElement( "发射器" );
//	for( ; pElemEmitter; pElemEmitter = pElemEmitter->NextSiblingElement("发射器") )
//	{
//		name = pElemEmitter->Attribute("类型");
//
//		ParticleEmitter *pEmitter = CreateEmitter( name );
//		if(pEmitter)
//		{
//			AddEmitter( pEmitter );
//			pEmitter->Load( pElemEmitter );
//		}
//	}
//
//	TiXmlElement *pElemAffector = NULL;
//	pElemAffector = pRoot->FirstChildElement( "效果器" );
//	for( ; pElemAffector; pElemAffector = pElemAffector->NextSiblingElement("效果器") )
//	{
//		name = pElemAffector->Attribute("类型");
//
//		ParticleAffector *pAffector = CreateAffector( name );
//		if(pAffector)
//		{
//			AddAffector( pAffector );
//			pAffector->Load( pElemAffector );
//		}
//
//	}
//
//	return true;
//}

void ParticleGroup::Init( LPDIRECT3DDEVICE9 pDevice )
{
	m_pDevice = pDevice;
	SetTexture("Flare.tga");
	m_CommonUp.x = 0;
	m_CommonUp.y = 1;
	m_CommonUp.z = 0;
	m_CommonDirection.x = 0;
	m_CommonDirection.y = 0;
	m_CommonDirection.z = -1;
	m_fTexAnimDuration = 5.0f;

	string Type = "点发射器";
	ParticleEmitter *pEmitter = CreateEmitter(Type);
	AddEmitter(pEmitter);
	pEmitter->LoadFromFile(".\\Particle.ini","Fire");
}

void ParticleGroup::Update(float fElapsedTime, CCamera &cam)
{
	/*_LiveOrDead(fElapsedTime);
	_TriggerAffectors(fElapsedTime);
	_ApplyMotion(fElapsedTime);
	_TriggerEmitters(fElapsedTime);
	_FillBuffers();*/
}

void ParticleGroup::Render()
{

}

void ParticleGroup::SetPoolSize(unsigned int uiPoolSize)
{
	m_uiPoolSize = uiPoolSize;

	// 清除之前的粒子池
	m_ActiveParticles.clear();
	m_FreeParticles.clear();
	for(size_t i = 0;i < m_ParticlePool.size();++i)
	{
		delete m_ParticlePool[i];
	}
	m_ParticlePool.clear();

	// 根据给定的大小创建粒子池，统一分配内存
	m_ParticlePool.resize(uiPoolSize);
	for(size_t i = 0;i < m_ParticlePool.size();++i)
	{
		m_ParticlePool[i] = new Particle;
		m_FreeParticles.push_back(m_ParticlePool[i]);
	}
}

void ParticleGroup::SetTexture( LPSTR pName )
{
	assert( pName );
	m_TextureName = pName;

	size_t pos = m_TextureName.find_last_of("_");
	size_t suffixPos = m_TextureName.find_last_of(".");
	if( pos != string::npos )
	{
		m_bAnimationTexture = true;
		string strSide = m_TextureName.substr( pos+1, suffixPos-pos-1 );
		m_iSide = atoi( strSide.c_str() );
		m_iNumFrame = m_iSide * m_iSide;
		m_fTexDelta = 1.f / static_cast<float>(m_iSide);
	}

	SAFE_RELEASE( m_pTexture );

	string strDDSName(pName);
	ChangeFileNameToDDS( strDDSName );
	if( FAILED(D3DXCreateTextureFromFile( m_pDevice, strDDSName.c_str(), &m_pTexture )) )
		D3DXCreateTextureFromFile( m_pDevice, pName, &m_pTexture );
}

Particle* ParticleGroup::CreateParticle()
{
	// 没有可用的粒子指针，返回NULL
	if(m_FreeParticles.empty())
	{
		return NULL;
	}

	// 将可用粒子池中的第一个数据删除(pop_front)，加到不可用粒子池最后
	// (push_back)，splice函数可以实现这两步。
	Particle *p = m_FreeParticles.front();
	//m_FreeParticles.pop_front();
	//m_ActiveParticles.push_back(p);
	m_ActiveParticles.splice(
		m_ActiveParticles.end(),
		m_FreeParticles,
		m_FreeParticles.begin());

	return p;
}

ParticleGroup::ParticleActiveList::iterator 
ParticleGroup::DeleteParticle(ParticleActiveList::iterator& i)
{
	// 将不可用粒子池中的数据删除，加入到可用粒子池最后
	m_FreeParticles.push_back(*i);
	return m_ActiveParticles.erase(i);
}

ParticleEmitter *ParticleGroup::CreateEmitter( string &name )
{
	ParticleEmitter *pEmitter = NULL;
	if( name == "点发射器" )
	{
		pEmitter = new PointEmitter(this);
	}
	else if( name == "盒子发射器" )
	{
		pEmitter = new BoxEmitter(this);
	}
	else if( name == "椭圆发射器" )
	{
		pEmitter = new EllipsoidEmitter(this);
	}
	else if( name == "环发射器" )
	{
		pEmitter = new RingEmitter(this);
	}
	else if( name == "空心椭圆发射器" )
	{
		pEmitter = new HollowEllipsoidEmitter(this);
	}

	return pEmitter;
}

//ParticleEmitter *ParticleGroup::CreateEmitter(char* name)
//{
//	ParticleEmitter *pEmitter = NULL;
//	if(!strcmp("点发射器",name))
//	{
//		pEmitter = new PointEmitter(this);
//	}
//	else if(!strcmp("盒子发射器",name))
//	{
//		pEmitter = new BoxEmitter(this);
//	}
//	else if(!strcmp("椭圆发射器",name))
//	{
//		pEmitter = new EllipsoidEmitter(this);
//	}
//	else if(!strcmp("环发射器",name))
//	{
//		pEmitter = new RingEmitter(this);
//	}
//	else if(!strcmp("空心椭圆发射器",name))
//	{
//		pEmitter = new HollowEllipsoidEmitter(this);
//	}
//
//	return pEmitter;
//}

void ParticleGroup::AddEmitter(ParticleEmitter *pe)
{
	m_Emitters.push_back(pe);
}

void ParticleGroup::Clone( ParticleGroup *p )
{
	assert(p);

	p->m_pDevice = m_pDevice;
	//p->m_b2DParticle = m_b2DParticle;
	p->m_CoordinateSystem = m_CoordinateSystem;
	p->m_bRotationTexture = m_bRotationTexture;
	//p->m_SceneBlendType = m_SceneBlendType;
	p->m_fDefaultHeight = m_fDefaultHeight;
	p->m_fDefaultWidth = m_fDefaultWidth;
	p->m_CommonDirection = m_CommonDirection;
	p->m_CommonUp = m_CommonUp;
	p->SetPoolSize( m_uiPoolSize );


	p->m_TextureName = m_TextureName;
	p->m_pTexture = m_pTexture;
	if(m_pTexture)
	{
		m_pTexture->AddRef();
		p->m_bAnimationTexture = m_bAnimationTexture;	
		p->m_iSide = m_iSide;
		p->m_iNumFrame = m_iNumFrame;
		p->m_fTexAnimDuration = m_fTexAnimDuration;
		p->m_fTexDelta = m_fTexDelta;
	}

	p->m_BillboardType = m_BillboardType;
	p->m_CommonUp = m_CommonUp;

	ParticleEmitter *pSrcEmitter = NULL;
	ParticleEmitter *pDestEmitter = NULL;
	ParticleEmitterVector::iterator iEmit = m_Emitters.begin();
	for( ; iEmit != m_Emitters.end(); ++iEmit )
	{
		pSrcEmitter = (*iEmit);
		// 调用p的CreateEmitter，因为内部要为每个Emitter设置父
		pDestEmitter = p->CreateEmitter( pSrcEmitter->m_Type );
		if(pDestEmitter)
		{
			pSrcEmitter->Clone( pDestEmitter );
			p->AddEmitter(pDestEmitter);
		}
	}

	//ParticleAffector *pSrcAffector = NULL;
	//ParticleAffector *pDestAffector = NULL;
	//ParticleAffectorVector::iterator iAffect = m_Affectors.begin();
	/*for( ; iAffect != m_Affectors.end(); ++iAffect )
	{
		pSrcAffector = (*iAffect);
		pDestAffector = p->CreateAffector( pSrcAffector->m_Type );
		if(pDestAffector)
		{
			pSrcAffector->Clone(pDestAffector);
			p->AddAffector(pDestAffector);
		}
	}*/
}




void ParticleGroup::_CreateBuffers()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);

	m_uiMaxNumVertices = m_uiPoolSize * 4;
	m_uiMaxNumIndices = m_uiPoolSize * 6;

	// 3 concept: D3DUSAGE, D3DPOOL, D3DLOCK
	// D3DUSAGE: dynamic or not, write only or not
	//			 D3DUSAGE_DYNAMIC and D3DPOOL_MANAGED are incompatible and should not be used together. because both of them are use to lock.
	// D3DPOOL: see figure
	// D3DLOCK: D3DLOCK_DISCARD&D3DLOCK_NOOVERWRITE valid on D3DUSAGE_DYNAMIC.
	m_pDevice->CreateVertexBuffer(sizeof(ParticleVertex) * m_uiMaxNumVertices,
		D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,ParticleVertex::FVF_Flags,D3DPOOL_DEFAULT,&m_pVB,NULL);
	m_pDevice->CreateIndexBuffer(sizeof(WORD) * m_uiMaxNumIndices,
		D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&m_pIB,NULL);

	WORD *pIndices = NULL;
	m_pIB->Lock(0,sizeof(WORD) * m_uiMaxNumIndices,(void**)&pIndices,0);
	for(unsigned int idx,idxOff,iParticle = 0;iParticle < m_uiPoolSize;iParticle++)
	{
		idx    = iParticle * 6;
		idxOff = iParticle * 4;

		/*	0-----1
		    |    /|
		    |  /  |
		    |/    |
		    2-----3	 */
		pIndices[idx]   = idxOff + 0; 
		pIndices[idx+1] = idxOff + 1;
		pIndices[idx+2] = idxOff + 2;
		pIndices[idx+3] = idxOff + 1;
		pIndices[idx+4] = idxOff + 3;
		pIndices[idx+5] = idxOff + 2;
	}
	m_pIB->Unlock();
}

void ParticleGroup::_FillBuffers()
{
	Particle *pParticle = NULL;
	/*if (m_BillboardType != BT_ORIENT_SELF)
	{
		_GenBillboardAxis();
	}*/
	m_fLeft = -0.5f;
	m_fRight = 0.5f;
	m_fTop = 0.5f;
	m_fBottom = -0.5f;

	_GenVertexOffset(m_fDefaultWidth, m_fDefaultHeight);

	static float basicTexCoord[8] = {
		0.f, 0.f,
		1.f, 0.f,
		0.f, 1.f,
		1.f, 1.f,
	};
	float *pfTexCoord = 0;
	ParticleVertex *pParticleVertex = NULL;
	m_pVB->Lock(0,static_cast<UINT>(m_ActiveParticles.size())*4*sizeof(ParticleVertex),
		(void**)&pParticleVertex,D3DLOCK_DISCARD);
	ParticleActiveList::iterator i = m_ActiveParticles.begin();
	int index = 0;
	int indexOff = 0;
	for(;i != m_ActiveParticles.end();i++)
	{
		pParticle = *i;

		// 如果面向自己的速度方向，则将每个粒子指针传入_GenBillboardAxis
		/*if( m_BillboardType == BT_ORIENT_SELF )
		{
			_GenBillboardAxis(pParticle);
		}*/
		// 如果此ParticleGroup粒子大小不统一，则需要为每个粒子计算顶点偏移
		/*if( pParticle->m_bOwnDimension == true )
		{
			_GenVertexOffset( pParticle->m_fWidth, pParticle->m_fHeight );
		}*/

		// pfTexCoord保存固定的纹理坐标
		pfTexCoord = basicTexCoord;
		indexOff = index * 4;

		// 如果有纹理动画，则pfTexCoord保存每一帧的纹理坐标
		/* 
		0-----1
		|    /|
		|  /  |
		|/    |
		2-----3
		*/
		if(m_bAnimationTexture == true)
		{
			static float animTexCoord[8];
			float fLiveTime = pParticle->m_fTotalTimeToLive - pParticle->m_fTimeToLive;
			// clamp
			while(fLiveTime > m_fTexAnimDuration)
				fLiveTime -= m_fTexAnimDuration;
			int iCurrFrame = static_cast<int>(m_iNumFrame * (fLiveTime / m_fTexAnimDuration));

			int x = iCurrFrame % m_iSide;
			int y = iCurrFrame / m_iSide;

			animTexCoord[0] = x * m_fTexDelta;// x / (float)m_iSide
			animTexCoord[1] = y * m_fTexDelta;
			animTexCoord[2] = ( x + 1 )* m_fTexDelta;
			animTexCoord[3] = ( y ) * m_fTexDelta;
			animTexCoord[4] = x * m_fTexDelta; 
			animTexCoord[5] = ( y + 1 ) * m_fTexDelta;
			animTexCoord[6] = ( x + 1 ) * m_fTexDelta;
			animTexCoord[7] = ( y + 1 ) * m_fTexDelta;

			pfTexCoord = animTexCoord;
		}
		// 如果有旋转，则pfTexCoord保存计算旋转后的纹理坐标
		else if( m_bRotationTexture )
		{
			static float rotTexDataBase[8] = {
				-0.5, 0.5,
				0.5, 0.5,
				-0.5,-0.5,
				0.5,-0.5 };
				static float rotTexData[8];

				const float cos_rot( cosf(pParticle->m_fRotation) );
				const float sin_rot( sinf(pParticle->m_fRotation) );

				rotTexData[0] = (cos_rot * rotTexDataBase[0]) + (sin_rot * rotTexDataBase[1]) + 0.5f;
				rotTexData[1] = (sin_rot * rotTexDataBase[0]) - (cos_rot * rotTexDataBase[1]) + 0.5f;

				rotTexData[2] = (cos_rot * rotTexDataBase[2]) + (sin_rot * rotTexDataBase[3]) + 0.5f;
				rotTexData[3] = (sin_rot * rotTexDataBase[2]) - (cos_rot * rotTexDataBase[3]) + 0.5f;

				rotTexData[4] = (cos_rot * rotTexDataBase[4]) + (sin_rot * rotTexDataBase[5]) + 0.5f;
				rotTexData[5]= (sin_rot * rotTexDataBase[4]) - (cos_rot * rotTexDataBase[5]) + 0.5f;

				rotTexData[6] = (cos_rot * rotTexDataBase[6]) + (sin_rot * rotTexDataBase[7]) + 0.5f;
				rotTexData[7] = (sin_rot * rotTexDataBase[6]) - (cos_rot * rotTexDataBase[7]) + 0.5f;
				pfTexCoord = rotTexData;
		}

		// 如果m_bAnimationTexture && m_bRotationTexture同时被设为true，则
		// 旋转纹理坐标会产生错误，这种情况下使用旋转顶点的方式来处理。
		D3DXMATRIX matRot;
		D3DXVECTOR3 vTempOffset[4];
		if( m_bAnimationTexture && m_bRotationTexture )
		{
			D3DXVECTOR3 axis;
			D3DXVec3Cross( &axis, &(m_vOffset[3] - m_vOffset[0]), 
				&(m_vOffset[2] - m_vOffset[1]) );
			D3DXMatrixRotationAxis( &matRot, &axis, pParticle->m_fRotation );

			D3DXVec3TransformCoord( &vTempOffset[0], &m_vOffset[0], &matRot );
			D3DXVec3TransformCoord( &vTempOffset[1], &m_vOffset[1], &matRot );
			D3DXVec3TransformCoord( &vTempOffset[2], &m_vOffset[2], &matRot );
			D3DXVec3TransformCoord( &vTempOffset[3], &m_vOffset[3], &matRot );
		}
		else
		{
			vTempOffset[0] = m_vOffset[0];
			vTempOffset[1] = m_vOffset[1];
			vTempOffset[2] = m_vOffset[2];
			vTempOffset[3] = m_vOffset[3];
		}

		pParticleVertex[indexOff].pos = pParticle->m_vPosition + vTempOffset[0];
		pParticleVertex[indexOff].diffuse = pParticle->m_Color;
		pParticleVertex[indexOff].tu = *pfTexCoord++;
		pParticleVertex[indexOff].tv = *pfTexCoord++;
		indexOff++;

		pParticleVertex[indexOff].pos = pParticle->m_vPosition + vTempOffset[1];
		pParticleVertex[indexOff].diffuse = pParticle->m_Color;
		pParticleVertex[indexOff].tu = *pfTexCoord++;
		pParticleVertex[indexOff].tv = *pfTexCoord++;
		indexOff++;

		pParticleVertex[indexOff].pos = pParticle->m_vPosition + vTempOffset[2];
		pParticleVertex[indexOff].diffuse = pParticle->m_Color;
		pParticleVertex[indexOff].tu = *pfTexCoord++;
		pParticleVertex[indexOff].tv = *pfTexCoord++;
		indexOff++;

		pParticleVertex[indexOff].pos = pParticle->m_vPosition + vTempOffset[3];
		pParticleVertex[indexOff].diffuse = pParticle->m_Color;
		pParticleVertex[indexOff].tu = *pfTexCoord++;
		pParticleVertex[indexOff].tv = *pfTexCoord++;

		index++;
	}
	m_pVB->Unlock();
}

void ParticleGroup::_GenBillboardAxis(Particle *p)
{
	switch( m_BillboardType )
	{
	case BT_POINT:
		{
			D3DXVec3Normalize( &m_vBillboardX, &m_vCamRight );
			D3DXVec3Normalize( &m_vBillboardY, &m_vCamUp );
		}
		break;
	case BT_ORIENT_SELF:
		{
			// 当p->m_vDirection为(0,0,0)时，m_vBillboardY、m_vBillboardX非法
			// 这时使用BT_POINT面向摄像机
			if(p->m_vDirection == D3DXVECTOR3(0.f,0.f,0.f))
			{
				D3DXVec3Normalize( &m_vBillboardX, &m_vCamRight );
				D3DXVec3Normalize( &m_vBillboardY, &m_vCamUp );				
			}
			else
			{
				m_vBillboardY = p->m_vDirection;
				D3DXVec3Normalize( &m_vBillboardY, &m_vBillboardY );
				D3DXVec3Cross( &m_vBillboardX, 
					&(-m_vCamDir), &m_vBillboardY );
				D3DXVec3Normalize( &m_vBillboardX, &m_vBillboardX );
			}
		}
		break;
	case BT_ORIENT_COMMON_DIRECTION:
		{
			m_vBillboardY = m_CommonDirection;
			D3DXVec3Cross( &m_vBillboardX, &(-m_vCamDir), &m_vBillboardY );
			D3DXVec3Normalize( &m_vBillboardX, &m_vBillboardX );
		}
		break;
	case BT_PERPENDICULAR_COMMON_DIRECTION:
		{
			m_vBillboardY = m_CommonUp;
			m_vBillboardX = m_CommonDirection;
		}
		break;
	}
}

void ParticleGroup::_GenVertexOffset(float fWidth, float fHeight)
{
	m_vLeftOff = m_vBillboardX * fWidth * m_fLeft;
	m_vRightOff = m_vBillboardX * fWidth * m_fRight;
	m_vTopOff = m_vBillboardY * fHeight * m_fTop;
	m_vBottomOff = m_vBillboardY * fHeight * m_fBottom;

	m_vOffset[0] = m_vLeftOff + m_vBottomOff;
	m_vOffset[1] = m_vRightOff + m_vBottomOff;
	m_vOffset[2] = m_vLeftOff + m_vTopOff;
	m_vOffset[3] = m_vRightOff + m_vTopOff;
}
