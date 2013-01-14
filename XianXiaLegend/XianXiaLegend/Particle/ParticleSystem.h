#pragma once
#include <vector>
#include <list>
#include <map>
using namespace std;
#include "ParticleEmitter.h"
//#include "tinyxml.h"
#include "camera.h"

// 粒子类
class Particle
{
public:
	float m_fWidth;             // 宽度
	float m_fHeight;            // 高度
	DWORD m_Color;              // 当前颜色（通常指顶点颜色）
	D3DXVECTOR3 m_vPosition;    // 位置
	D3DXVECTOR3 m_vDirection;   // 方向（有长度的方向，包含速度）
	float m_fTimeToLive;        // 还能存活的时间
	float m_fTotalTimeToLive;   // 总共存活的时间

	// 当前的旋转弧度
	float m_fRotation;		
	// 弧度，旋转的速度
	float m_fRotationSpeed;

public:
	Particle(void);
};

/** 公告板的类型
*/
enum BillboardType
{
	/// 永远面向摄像机
	BT_POINT,
	/// 沿着粒子的飞行方向，粒子类的m_vDirection
	BT_ORIENT_SELF,
	/// 沿着固定方向（m_CommonDirection），总是沿着固定方向旋转，面向摄像机
	BT_ORIENT_COMMON_DIRECTION,
	/// 垂直固定方向（m_CommonDirection），与摄像机无关， 由用户指定的方向m_CommonUp决定
	BT_PERPENDICULAR_COMMON_DIRECTION,
};

/** 粒子物理计算的相对坐标系
*/
enum ParticleCoordinateSystem
{
	/// 相对世界空间
	PCS_WORLD,
	/// 相对本地空间
	PCS_LOCAL
};

// 粒子团
class ParticleGroup
{
public:
	// 粒子顶点
	class ParticleVertex
	{
	public:
		D3DXVECTOR3 pos;
		DWORD diffuse;
		float tu, tv;

		enum
		{
			FVF_Flags = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1
		};
	};

	typedef std::vector<Particle*> ParticlePool;
	typedef std::list<Particle*> ParticleActiveList;
	typedef std::list<Particle*> ParticleFreeList;
	// 粒子对象池，使用动态数组std::vector
	ParticlePool m_ParticlePool;
	// 不可用粒子池，使用链表std::list
	ParticleActiveList m_ActiveParticles;
	// 可用粒子池，使用链表std::list
	ParticleFreeList m_FreeParticles;
	// 粒子池大小，此粒子团最大可同时存在的粒子数目
	unsigned int m_uiPoolSize;
	//　最大的顶点数目，如果粒子是Billboard，则此值为m_uiPoolSize*4
	unsigned int m_uiMaxNumVertices;
	//　最大的索引数目，如果粒子是Billboard，则此值为m_uiPoolSize*2
	unsigned int m_uiMaxNumIndices;

	typedef std::vector<ParticleEmitter*> ParticleEmitterVector;
	//typedef std::vector<ParticleAffector*> ParticleAffectorVector;
	// 发射器数组
	ParticleEmitterVector m_Emitters;
	// 效果器数组
	//ParticleAffectorVector m_Affectors;

	// Vertex Buffer
	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	// Index Buffer
	LPDIRECT3DINDEXBUFFER9 m_pIB;

	// 粒子的默认宽度
	float m_fDefaultWidth;
	// 粒子的默认高度
	float m_fDefaultHeight;

	// 纹理名
	string m_TextureName;
	LPDIRECT3DTEXTURE9 m_pTexture;
	// 纹理动画参数
	int m_iSide;
	int m_iNumFrame;
	float m_fTexDelta;
	float m_fTexAnimDuration;
	bool m_bAnimationTexture;
	bool m_bRotationTexture;

	IDirect3DDevice9*   m_pDevice;
	// 当前摄像机的指针
	CCamera *m_pCamera;
	D3DXVECTOR3 m_vCamDir;
	D3DXVECTOR3 m_vCamRight;
	D3DXVECTOR3 m_vCamUp;

	// 进行粒子运算的相对坐标系
	ParticleCoordinateSystem m_CoordinateSystem;
	
	// 公告板面向摄像机的方式
	BillboardType m_BillboardType;
	// 当m_BillboardType为BT_ORIENT_COMMON_DIRECTION时，此向量为粒子指向的方向。
	D3DXVECTOR3 m_CommonDirection;
	// 当m_BillboardType为BT_PERPENDICULAR_COMMON_DIRECTION时，此向量和m_CommonDirection决定粒子的x, y轴
	D3DXVECTOR3 m_CommonUp;

	// 公告板本地坐标系的x轴，在世界坐标系中的方向
	D3DXVECTOR3	m_vBillboardX;
	// 公告板本地坐标系的y轴，在世界坐标系中的方向
	D3DXVECTOR3	m_vBillboardY;
	// 公告板四个边中点的坐标，相对于将世界坐标系原点移到公告板原点的坐标系
	D3DXVECTOR3 m_vLeftOff;
	D3DXVECTOR3 m_vRightOff;
	D3DXVECTOR3 m_vTopOff;
	D3DXVECTOR3 m_vBottomOff;
	float m_fLeft;
	float m_fRight;
	float m_fTop;
	float m_fBottom;
	D3DXVECTOR3 m_vOffset[4];

public:
	ParticleGroup(void);
	~ParticleGroup(void);

	void Init( LPDIRECT3DDEVICE9 pDevice );
	//　根据时间差、摄像机计算粒子属性，同时更新VertexBuffer、IndexBuffer
	void Update( float fElapsedTime, CCamera &cam );
	//　设置合适的渲染状态，渲染粒子
	void Render();

	// 设置粒子池大小
	void SetPoolSize(unsigned int uiPoolSize);
	void SetTexture(LPSTR pName);



	// 创建一个粒子，返回可用粒子对象的指针
	Particle* CreateParticle();
	// 删除一个粒子
	ParticleActiveList::iterator DeleteParticle(ParticleActiveList::iterator& i);

	// 创建一个发射器
	ParticleEmitter *CreateEmitter(string &name);
	// 增加一个发射器
	void AddEmitter(ParticleEmitter *pe);

	/** Clone新的实例
		@remarks
			为了避免每次都加载资源，解析xml文件。此处选择预先加载所有的粒子xml文件，
			做为模板。每次创建粒子时会使用Clone函数根据模板做出实例。
		@param 
			p 需要Clone出的实例
		@param
			bDeepCopyTexture 是否深拷贝纹理，默认为false，代表直接copy纹理指针
			和纹理相关属性，true为重新加载纹理并重新计算纹理相关属性。
	*/
	void Clone( ParticleGroup *p );

	//bool ParticleGroup::Load( LPCSTR pName );

protected:
	/**　根据粒子池大小创建VB、IB，如果粒子是公告板的话，此值为m_uiPoolSize*4
	*/
	void _CreateBuffers();
	/** 填充VB、IB
	*/
	void _FillBuffers();
	/** 计算粒子的x,y轴在世界坐标系下的方向
		@remarks
			根据粒子面向摄像机的方式m_BillboardType不同，计算m_vBillboardX、
			m_vBillboardY。
	*/
	void _GenBillboardAxis(Particle *p = NULL);
	void _GenVertexOffset(float fWidth, float fHeight);



};