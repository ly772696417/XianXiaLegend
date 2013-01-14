/*
*	�������
*/
#pragma once
#include "Myfuntion.h"

// ��ɫ�ж�״̬
enum ACTIONSTATE
{
	AS_MOUSEMOVE, // ����ƶ�
	AS_KEYMOVE,   // �����ƶ�
	AS_WAIT,      // ����
	AS_WALK,      // �߶�
	AS_CHASE,     // ׷��
	AS_ATTACK,    // ����
	AS_BRUISE,    // ����
	AS_GUARD,     // ����
};

class CMyObject
{
protected:
	D3DXMATRIX m_matWorld;
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vRot;
	D3DXVECTOR3 m_vScal;
	int m_Type;                // ��������
	int m_iMeshID;             // ģ��ID
	int m_iID;                 // �����ڳ����е�ID
	D3DXVECTOR3 m_vOrigMax;    // ԭʼ��Χ�н�ֵ
	D3DXVECTOR3 m_vOrigMin;
	D3DXVECTOR3 m_vMax;        // ��Χ�н�ֵ
	D3DXVECTOR3 m_vMin;
	D3DXVECTOR3 m_vCenter;     
	float m_fRadius;           // ��Χ��뾶


public:
	CMyObject(void);
	virtual ~CMyObject(void);

	virtual bool Init(char* SceneFile,char* AddName) = 0;
	virtual void Update() = 0;
	virtual void Render(D3DXMATRIX* matWorld) = 0;
	virtual bool LoadDataFromFile(char* SceneFile,char* AddName);

	// ��������,��ת,ƽ��,���������任����
	void Transform();
	
	void SetPos(D3DXVECTOR3 vPos) {m_vPos = vPos;}
	void SetRot(D3DXVECTOR3 vRot) {m_vRot = vRot;}
	void SetScal(D3DXVECTOR3 vScal) {m_vScal = vScal;}
	void SetID(int ID) {m_iID = ID;}
	
	D3DXMATRIX GetWorldMat() {return m_matWorld;}
	D3DXVECTOR3 GetPos() {return m_vPos;}
	D3DXVECTOR3 GetRot() {return m_vRot;}
	D3DXVECTOR3 GetScal() {return m_vScal;}
	D3DXVECTOR3 GetMax() {return m_vMax;}
	D3DXVECTOR3 GetMin() {return m_vMin;}
	D3DXVECTOR3 GetCenter() {return m_vCenter;}
	float GetRadius() {return m_fRadius;}
	// ��ȡ��������
	int	GetType() {return m_Type;}
	// ��ȡģ��ID
	int GetMeshID() {return m_iMeshID;}
	// ��ȡ����ID
	int GetID() {return m_iID;}
};
