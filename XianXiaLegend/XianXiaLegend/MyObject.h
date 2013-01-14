/*
*	对象基类
*/
#pragma once
#include "Myfuntion.h"

// 角色行动状态
enum ACTIONSTATE
{
	AS_MOUSEMOVE, // 鼠标移动
	AS_KEYMOVE,   // 键盘移动
	AS_WAIT,      // 待机
	AS_WALK,      // 走动
	AS_CHASE,     // 追赶
	AS_ATTACK,    // 攻击
	AS_BRUISE,    // 受伤
	AS_GUARD,     // 警戒
};

class CMyObject
{
protected:
	D3DXMATRIX m_matWorld;
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vRot;
	D3DXVECTOR3 m_vScal;
	int m_Type;                // 对象类型
	int m_iMeshID;             // 模型ID
	int m_iID;                 // 对象在场景中的ID
	D3DXVECTOR3 m_vOrigMax;    // 原始包围盒角值
	D3DXVECTOR3 m_vOrigMin;
	D3DXVECTOR3 m_vMax;        // 包围盒角值
	D3DXVECTOR3 m_vMin;
	D3DXVECTOR3 m_vCenter;     
	float m_fRadius;           // 包围球半径


public:
	CMyObject(void);
	virtual ~CMyObject(void);

	virtual bool Init(char* SceneFile,char* AddName) = 0;
	virtual void Update() = 0;
	virtual void Render(D3DXMATRIX* matWorld) = 0;
	virtual bool LoadDataFromFile(char* SceneFile,char* AddName);

	// 根据缩放,旋转,平移,计算出世界变换矩阵
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
	// 获取对象类型
	int	GetType() {return m_Type;}
	// 获取模型ID
	int GetMeshID() {return m_iMeshID;}
	// 获取对象ID
	int GetID() {return m_iID;}
};
