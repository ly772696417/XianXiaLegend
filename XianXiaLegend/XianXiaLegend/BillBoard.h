/*
*	�������Ⱦ��
*/
#pragma once
#include "myfuntion.h"

enum BILLMODE
{
	DEFAULT = 1,// Ĭ��ģʽ,��������任
	PARALLEL = 2,// ƽ����Ļģʽ
	AXIAL = 3,// ������תģʽ
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
	D3DXVECTOR3					m_vPos;          // ��ǰ��Ⱦλ��
	vector<D3DXVECTOR3>         m_vecPos;        // ����Ҫ��Ⱦ��λ�ñ�

	int                         m_iTextureNum;
	char                        m_TexturePath[256];
	vector<IDirect3DTexture9*>	m_vecTexture;
	vector<IDirect3DTexture9*>::iterator iter;
	DOUBLE                      m_dTime;         // �����л�ʱ��
	DOUBLE                      m_dTimeCount;
	BILLMODE					m_Mode;          // �������תģʽ

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
	// ���������л�ʱ��
	void SetTime(double dTime) {m_dTime = dTime;}

private:
	// ����������
	void LoadTexture(char* pFileName,int MaxNum); 
	// ���������任
	void SetBillMode(BILLMODE mode);
};
