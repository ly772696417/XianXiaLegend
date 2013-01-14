#pragma once
#include "myfuntion.h"
//#include "tinyxml\tinyxml.h"
//#include ".\particlesystem.h"

//#include "utility.h"

class Particle;
class ParticleGroup;
//class TiXmlElement;


//using namespace std;
/** 粒子发射器，决定粒子的初始属性
		发射器的抽象类，定义了大部分发射器的共有属性和抽象接口。不同的
		发射器类提供不同的实现
*/
class ParticleEmitter
{
public:
	string m_Type;
	float m_fEmissionRate;		// 发射率。
	float m_fRemainder;			// 上一帧计算粒子数目的小数部分，避免发射不出粒子的情况。
	D3DXVECTOR3 m_vPosition;	// 粒子团本地空间的位置
	D3DXVECTOR3 m_vDirection;	// 粒子团本地空间的方向
	D3DXVECTOR3 m_vUp;			// Up向量，用来辅助产生随机向量
	float m_fAngle;				// 发射角
	float m_fMinTTL;			// 最小生存期
	float m_fMaxTTL;			// 最大生存期
	float m_fMinSpeed;			// 最小速度
	float m_fMaxSpeed;			// 最大速度
	DWORD m_dwStartColor;		// 起始颜色
	DWORD m_dwEndColor;			// 结束颜色

	// 由发射到停止发射之间的时间
	float m_fDurationMin;
	float m_fDurationMax;
	float m_fDurationRemain;
	// 由停止发射到再次发射之间的时间
	float m_fRepeatDelayMin;
	float m_fRepeatDelayMax;
	float m_fRepeatDelayRemain;

	bool m_bEnable;

	ParticleGroup* m_pParent;

public:
	ParticleEmitter( ParticleGroup* pParent);
	virtual ~ParticleEmitter(void);

	void SetDirection(D3DXVECTOR3& dir);
	void SetDuration(float min,float max);
	void SetRepeatDelay(float min,float max);
	void SetEnable(bool bEnable);

	// 初始化粒子属性，p为需要初始化的对象指针
	virtual void InitParticle(Particle* p);
	// 根据发射率和时间差返回发射粒子的数目
	virtual unsigned int GenEmissionCount(float fElapsedTime);
	// 产生粒子的生存期
	virtual float _GenEmissionTTL();
	// 在发射器指定的范围内产生粒子位置，输出参数，保存粒子的位置。
	virtual void _GenEmissionPosition(D3DXVECTOR3& destVector);
	// 产生粒子方向（归一化的），输出参数，保存粒子方向
	virtual void _GenEmissionDirection(D3DXVECTOR3& destVector);
	// 产生粒子速度，输出参数，乘过粒子速度常量
	virtual void _GenEmissionSpeed(D3DXVECTOR3& destVector);
	// 产生粒子顶点颜色，输出参数，保存粒子顶点色
	virtual void _GenEmissionColor(DWORD& color);

	void _InitDurationRepeat(void);

	bool LoadFromFile(char* strFile,char *strSection);
	//virtual bool Load(TiXmlElement* pElemEmitter);
	//virtual bool Save(TiXmlElement* pElemEmitter);

	virtual void Clone(ParticleEmitter* pe);
};

/** 点发射器
*/
class PointEmitter : public ParticleEmitter
{
public:
	PointEmitter(ParticleGroup* pParent);
	virtual ~PointEmitter(void);
};

/** 范围发射器
*/
class AreaEmitter : public ParticleEmitter
{


public:
	AreaEmitter(ParticleGroup* pParent);
	virtual ~AreaEmitter(void);

	void SetSize(float x,float y,float z);
	//virtual bool Load(TiXmlElement* pElemEmitter);
	//virtual bool Save(TiXmlElement* pElemEmitter);
	virtual void Clone(ParticleEmitter* pe);
public:
	// x,y,z三个方向的长度
	D3DXVECTOR3 m_vSize;
	D3DXVECTOR3 m_vXRange;
	D3DXVECTOR3 m_vYRange;
	D3DXVECTOR3 m_vZRange;
};

/** 盒子发射器，可以实现粒子在：立方体、长方体、面、线，上发射的效果。
*/
class BoxEmitter : public AreaEmitter
{
public:
	BoxEmitter(ParticleGroup* pParent);
	virtual ~BoxEmitter(void);

	virtual void _GenEmissionPosition(D3DXVECTOR3& destVector);
};

/** 椭圆发射器
*/
class EllipsoidEmitter : public AreaEmitter
{
public:
	EllipsoidEmitter(ParticleGroup* pParent);
	virtual ~EllipsoidEmitter(void);

	virtual void _GenEmissionPosition(D3DXVECTOR3& desVector);
};

/** 环发射
*/
class RingEmitter : public AreaEmitter
{
public:
	float m_fInnerX;	// 内环x方向的长度
	float m_fInnerY;	// 内环y方向的长度

public:
	RingEmitter(ParticleGroup* pParent);
	virtual ~RingEmitter(void);

	virtual void _GenEmissionPosition(D3DXVECTOR3& desVector);
	void SetInnerSize(float x,float y);
	//virtual bool Load(TiXmlElement* pElemEmitter);
	//virtual bool Save(TiXmlElement* pElemEmitter);
	virtual void Clone(ParticleEmitter* pe);


};

/** 空心椭圆发射器
*/
class HollowEllipsoidEmitter : public AreaEmitter
{
public:
	D3DXVECTOR3 m_vInnerSize;	// 内环x，y，z三个方向的长度

public:
	HollowEllipsoidEmitter(ParticleGroup* pParent);
	virtual ~HollowEllipsoidEmitter(void);

	virtual void _GenEmissionPosition(D3DXVECTOR3& desVector);
	virtual void InitParticle(Particle* p);
	void SetInnerSize(float x,float y,float z);
	virtual void Clone(ParticleEmitter* pe);
};