/*
*	公告板渲染类
*/
#pragma once
#include "myfuntion.h"

enum BILLMODE
{
	DEFAULT = 1,// 默认模式,跟随世界变换
	PARALLEL = 2,// 平行屏幕模式
	AXIAL = 3,// 轴向旋转模式
};

class CBillBoard
{
private:
	IDirect3DDevice9*			m_pDevice;
	IDirect3DVertexBuffer9*		m_pVB;

	int                         m_iScalU;
	int                         m_iScalV;
	float						m_fScalX;
	float                       m_fScalY;
	float						m_fPitch;
	D3DXVECTOR3					m_vPos;          // 当前渲染位置
	vector<D3DXVECTOR3>         m_vecPos;        // 对象要渲染的位置表

	int                         m_iTextureNum;
	char                        m_TexturePath[256];
	vector<IDirect3DTexture9*>	m_vecTexture;
	vector<IDirect3DTexture9*>::iterator iter;
	DOUBLE                      m_dTime;         // 纹理切换时间
	DOUBLE                      m_dTimeCount;
	BILLMODE					m_Mode;          // 公告板旋转模式

public:
	CBillBoard(IDirect3DDevice9* pDevice);
	~CBillBoard(void);

	bool Init(char* BillBoardFile,char* AddName);
	bool Init(char* TexturePath,int TextureNum);
	void Render(DOUBLE dElapsedTime);
	bool LoadDataFromFile(char* BillBoardFile,char* AddName);
	void AddPos(D3DXVECTOR3 vPos) {m_vecPos.push_back(vPos);}
	void SetMode(BILLMODE mode) {m_Mode = mode;}
	void SetScalUV(int u,int v) {m_iScalU = u;m_iScalV = v;}
	void SetScalXY(float x,float y) {m_fScalX = x;m_fScalY = y;}
	void SetPitch(float pitch) {m_fPitch = pitch;}
	// 设置纹理切换时间
	void SetTime(double dTime) {m_dTime = dTime;}

private:
	// 读入纹理组
	void LoadTexture(char* pFileName,int MaxNum); 
	// 公告板世界变换
	void SetBillMode(BILLMODE mode);
};
