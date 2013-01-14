#pragma once
#include <vector>
#include <list>
#include <map>
using namespace std;
#include "ParticleEmitter.h"
//#include "tinyxml.h"
#include "camera.h"

// ������
class Particle
{
public:
	float m_fWidth;             // ���
	float m_fHeight;            // �߶�
	DWORD m_Color;              // ��ǰ��ɫ��ͨ��ָ������ɫ��
	D3DXVECTOR3 m_vPosition;    // λ��
	D3DXVECTOR3 m_vDirection;   // �����г��ȵķ��򣬰����ٶȣ�
	float m_fTimeToLive;        // ���ܴ���ʱ��
	float m_fTotalTimeToLive;   // �ܹ�����ʱ��

	// ��ǰ����ת����
	float m_fRotation;		
	// ���ȣ���ת���ٶ�
	float m_fRotationSpeed;

public:
	Particle(void);
};

/** ����������
*/
enum BillboardType
{
	/// ��Զ���������
	BT_POINT,
	/// �������ӵķ��з����������m_vDirection
	BT_ORIENT_SELF,
	/// ���Ź̶�����m_CommonDirection�����������Ź̶�������ת�����������
	BT_ORIENT_COMMON_DIRECTION,
	/// ��ֱ�̶�����m_CommonDirection������������޹أ� ���û�ָ���ķ���m_CommonUp����
	BT_PERPENDICULAR_COMMON_DIRECTION,
};

/** �������������������ϵ
*/
enum ParticleCoordinateSystem
{
	/// �������ռ�
	PCS_WORLD,
	/// ��Ա��ؿռ�
	PCS_LOCAL
};

// ������
class ParticleGroup
{
public:
	// ���Ӷ���
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
	// ���Ӷ���أ�ʹ�ö�̬����std::vector
	ParticlePool m_ParticlePool;
	// ���������ӳأ�ʹ������std::list
	ParticleActiveList m_ActiveParticles;
	// �������ӳأ�ʹ������std::list
	ParticleFreeList m_FreeParticles;
	// ���ӳش�С��������������ͬʱ���ڵ�������Ŀ
	unsigned int m_uiPoolSize;
	//�����Ķ�����Ŀ�����������Billboard�����ֵΪm_uiPoolSize*4
	unsigned int m_uiMaxNumVertices;
	//������������Ŀ�����������Billboard�����ֵΪm_uiPoolSize*2
	unsigned int m_uiMaxNumIndices;

	typedef std::vector<ParticleEmitter*> ParticleEmitterVector;
	//typedef std::vector<ParticleAffector*> ParticleAffectorVector;
	// ����������
	ParticleEmitterVector m_Emitters;
	// Ч��������
	//ParticleAffectorVector m_Affectors;

	// Vertex Buffer
	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	// Index Buffer
	LPDIRECT3DINDEXBUFFER9 m_pIB;

	// ���ӵ�Ĭ�Ͽ��
	float m_fDefaultWidth;
	// ���ӵ�Ĭ�ϸ߶�
	float m_fDefaultHeight;

	// ������
	string m_TextureName;
	LPDIRECT3DTEXTURE9 m_pTexture;
	// ����������
	int m_iSide;
	int m_iNumFrame;
	float m_fTexDelta;
	float m_fTexAnimDuration;
	bool m_bAnimationTexture;
	bool m_bRotationTexture;

	IDirect3DDevice9*   m_pDevice;
	// ��ǰ�������ָ��
	CCamera *m_pCamera;
	D3DXVECTOR3 m_vCamDir;
	D3DXVECTOR3 m_vCamRight;
	D3DXVECTOR3 m_vCamUp;

	// ��������������������ϵ
	ParticleCoordinateSystem m_CoordinateSystem;
	
	// ���������������ķ�ʽ
	BillboardType m_BillboardType;
	// ��m_BillboardTypeΪBT_ORIENT_COMMON_DIRECTIONʱ��������Ϊ����ָ��ķ���
	D3DXVECTOR3 m_CommonDirection;
	// ��m_BillboardTypeΪBT_PERPENDICULAR_COMMON_DIRECTIONʱ����������m_CommonDirection�������ӵ�x, y��
	D3DXVECTOR3 m_CommonUp;

	// ����屾������ϵ��x�ᣬ����������ϵ�еķ���
	D3DXVECTOR3	m_vBillboardX;
	// ����屾������ϵ��y�ᣬ����������ϵ�еķ���
	D3DXVECTOR3	m_vBillboardY;
	// ������ĸ����е�����꣬����ڽ���������ϵԭ���Ƶ������ԭ�������ϵ
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
	//������ʱ������������������ԣ�ͬʱ����VertexBuffer��IndexBuffer
	void Update( float fElapsedTime, CCamera &cam );
	//�����ú��ʵ���Ⱦ״̬����Ⱦ����
	void Render();

	// �������ӳش�С
	void SetPoolSize(unsigned int uiPoolSize);
	void SetTexture(LPSTR pName);



	// ����һ�����ӣ����ؿ������Ӷ����ָ��
	Particle* CreateParticle();
	// ɾ��һ������
	ParticleActiveList::iterator DeleteParticle(ParticleActiveList::iterator& i);

	// ����һ��������
	ParticleEmitter *CreateEmitter(string &name);
	// ����һ��������
	void AddEmitter(ParticleEmitter *pe);

	/** Clone�µ�ʵ��
		@remarks
			Ϊ�˱���ÿ�ζ�������Դ������xml�ļ����˴�ѡ��Ԥ�ȼ������е�����xml�ļ���
			��Ϊģ�塣ÿ�δ�������ʱ��ʹ��Clone��������ģ������ʵ����
		@param 
			p ��ҪClone����ʵ��
		@param
			bDeepCopyTexture �Ƿ��������Ĭ��Ϊfalse������ֱ��copy����ָ��
			������������ԣ�trueΪ���¼����������¼�������������ԡ�
	*/
	void Clone( ParticleGroup *p );

	//bool ParticleGroup::Load( LPCSTR pName );

protected:
	/**���������ӳش�С����VB��IB����������ǹ����Ļ�����ֵΪm_uiPoolSize*4
	*/
	void _CreateBuffers();
	/** ���VB��IB
	*/
	void _FillBuffers();
	/** �������ӵ�x,y������������ϵ�µķ���
		@remarks
			������������������ķ�ʽm_BillboardType��ͬ������m_vBillboardX��
			m_vBillboardY��
	*/
	void _GenBillboardAxis(Particle *p = NULL);
	void _GenVertexOffset(float fWidth, float fHeight);



};