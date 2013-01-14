#pragma once
#include "myfuntion.h"
//#include "tinyxml\tinyxml.h"
//#include ".\particlesystem.h"

//#include "utility.h"

class Particle;
class ParticleGroup;
//class TiXmlElement;


//using namespace std;
/** ���ӷ��������������ӵĳ�ʼ����
		�������ĳ����࣬�����˴󲿷ַ������Ĺ������Ժͳ���ӿڡ���ͬ��
		���������ṩ��ͬ��ʵ��
*/
class ParticleEmitter
{
public:
	string m_Type;
	float m_fEmissionRate;		// �����ʡ�
	float m_fRemainder;			// ��һ֡����������Ŀ��С�����֣����ⷢ�䲻�����ӵ������
	D3DXVECTOR3 m_vPosition;	// �����ű��ؿռ��λ��
	D3DXVECTOR3 m_vDirection;	// �����ű��ؿռ�ķ���
	D3DXVECTOR3 m_vUp;			// Up�������������������������
	float m_fAngle;				// �����
	float m_fMinTTL;			// ��С������
	float m_fMaxTTL;			// ���������
	float m_fMinSpeed;			// ��С�ٶ�
	float m_fMaxSpeed;			// ����ٶ�
	DWORD m_dwStartColor;		// ��ʼ��ɫ
	DWORD m_dwEndColor;			// ������ɫ

	// �ɷ��䵽ֹͣ����֮���ʱ��
	float m_fDurationMin;
	float m_fDurationMax;
	float m_fDurationRemain;
	// ��ֹͣ���䵽�ٴη���֮���ʱ��
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

	// ��ʼ���������ԣ�pΪ��Ҫ��ʼ���Ķ���ָ��
	virtual void InitParticle(Particle* p);
	// ���ݷ����ʺ�ʱ���ط������ӵ���Ŀ
	virtual unsigned int GenEmissionCount(float fElapsedTime);
	// �������ӵ�������
	virtual float _GenEmissionTTL();
	// �ڷ�����ָ���ķ�Χ�ڲ�������λ�ã�����������������ӵ�λ�á�
	virtual void _GenEmissionPosition(D3DXVECTOR3& destVector);
	// �������ӷ��򣨹�һ���ģ�������������������ӷ���
	virtual void _GenEmissionDirection(D3DXVECTOR3& destVector);
	// ���������ٶȣ�����������˹������ٶȳ���
	virtual void _GenEmissionSpeed(D3DXVECTOR3& destVector);
	// �������Ӷ�����ɫ������������������Ӷ���ɫ
	virtual void _GenEmissionColor(DWORD& color);

	void _InitDurationRepeat(void);

	bool LoadFromFile(char* strFile,char *strSection);
	//virtual bool Load(TiXmlElement* pElemEmitter);
	//virtual bool Save(TiXmlElement* pElemEmitter);

	virtual void Clone(ParticleEmitter* pe);
};

/** �㷢����
*/
class PointEmitter : public ParticleEmitter
{
public:
	PointEmitter(ParticleGroup* pParent);
	virtual ~PointEmitter(void);
};

/** ��Χ������
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
	// x,y,z��������ĳ���
	D3DXVECTOR3 m_vSize;
	D3DXVECTOR3 m_vXRange;
	D3DXVECTOR3 m_vYRange;
	D3DXVECTOR3 m_vZRange;
};

/** ���ӷ�����������ʵ�������ڣ������塢�����塢�桢�ߣ��Ϸ����Ч����
*/
class BoxEmitter : public AreaEmitter
{
public:
	BoxEmitter(ParticleGroup* pParent);
	virtual ~BoxEmitter(void);

	virtual void _GenEmissionPosition(D3DXVECTOR3& destVector);
};

/** ��Բ������
*/
class EllipsoidEmitter : public AreaEmitter
{
public:
	EllipsoidEmitter(ParticleGroup* pParent);
	virtual ~EllipsoidEmitter(void);

	virtual void _GenEmissionPosition(D3DXVECTOR3& desVector);
};

/** ������
*/
class RingEmitter : public AreaEmitter
{
public:
	float m_fInnerX;	// �ڻ�x����ĳ���
	float m_fInnerY;	// �ڻ�y����ĳ���

public:
	RingEmitter(ParticleGroup* pParent);
	virtual ~RingEmitter(void);

	virtual void _GenEmissionPosition(D3DXVECTOR3& desVector);
	void SetInnerSize(float x,float y);
	//virtual bool Load(TiXmlElement* pElemEmitter);
	//virtual bool Save(TiXmlElement* pElemEmitter);
	virtual void Clone(ParticleEmitter* pe);


};

/** ������Բ������
*/
class HollowEllipsoidEmitter : public AreaEmitter
{
public:
	D3DXVECTOR3 m_vInnerSize;	// �ڻ�x��y��z��������ĳ���

public:
	HollowEllipsoidEmitter(ParticleGroup* pParent);
	virtual ~HollowEllipsoidEmitter(void);

	virtual void _GenEmissionPosition(D3DXVECTOR3& desVector);
	virtual void InitParticle(Particle* p);
	void SetInnerSize(float x,float y,float z);
	virtual void Clone(ParticleEmitter* pe);
};