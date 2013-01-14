/*
*	四叉树类，分割地形，裁剪并LOD评估节点，生成地形索引表
*/
#pragma once
#include "Common.h"
#include "ZFrustum.h"

class ZQuadTree
{
public:
	//enum CornerType{CORNER_TL,CORNER_TR,CORNER_BL,CORNER_BR};    // 四叉树保存的4个角的常数值
	//enum {EDGE_UP,EDGE_DN,EDGE_LT,EDGE_RT};                      // 邻近节点处理用的常数值
	enum QuadLocation                            // 四叉树和平截台体的关系
	{
		FRUSTUM_OUT = 0,                        // 完全脱离平截台体
		FRUSTUM_PARTIALLY_IN = 1,               // 部分包含在平截台体
		FRUSTUM_COMPLETELY_IN = 2,              // 完全包含在平截台体
		FRUSTUM_UNKNOWN = -1
	};

private:
	typedef struct node
	{
		int		ID;                           // 节点ID
		int		ParentID;                     // 父节点ID
		int		ChildID[4];                   // 子节点ID
		int		NeighborID[4];                // 邻近节点ID
		int		iCenter;                      // 节点中心值
		int		iCorner[4];                   // 节点四角值
		bool	bCulled;                      // 节点剔除标志
		float	fRadius;                      // 边界球体半径
	}TreeNode;
	TreeNode*	TreeList;                     // 树节点列表（顺序储存方式）
	int			MaxNodeNum;                   // 节点总数
	static int	TotalTreeID;                  // 记录并分配节点ID

	//ZQuadTree* m_pChild[4];                      // 四叉树的4个子节点
	//ZQuadTree* m_pParent;                        // 防止龟裂(Popping)的产生
	//ZQuadTree* m_pNeighbor[4];                   // 防止龟裂(Popping)的产生
	//int m_iCenter;                               // 四叉树保存的第一个值
	//int m_iCorner[4];                            // 四叉树保存的第二个值
                                                 //   TopLeft(TL)       TopRight(TR)
	                                             //             0-------1
                                                 //             |       |
	                                             //             |       |
	                                             //             2-------3
	                                             // BottomLeft(BL)      BottomRight(BR)

	//bool m_bCulled;                              // 平截台体剔除的节点
	//float m_fRadius;                             // 包含节点的边界球体(bounding sphere)的半径

private:
	// 初始化节点
	int CreateNode(int ParentID,int nCornerTL,int nCornerTR,int nCornerBL,int nCornerBR);
	// 使用4个下层对四叉树进行再分割(subdivide)
	bool _SubDivide(int ID);
	// 创建输出多边形的索引
	int _GenTriIndex(int ID,int nTris,void* pIndex,LandVertex* pHeightMap,ZFrustum* pFrustum,float fLodRatio);
	// 求得两个点的距离
	float _GetDistance(D3DXVECTOR3 pv1,D3DXVECTOR3 pv2)
	{
		return D3DXVec3Length(&(pv2 - pv1));
	}
	// 以摄像机与当前节点的距离为标准求得LOD值
	int _GetLODLevel(int ID,LandVertex* pHeightMap,D3DXVECTOR3* pCamera,float fLODRatio)
	{
		float d = _GetDistance(*(D3DXVECTOR3*)(pHeightMap + TreeList[ID].iCenter),*pCamera);
		return max((int)(d * fLODRatio),1);
	}
	// 当前节点包含在平截台体?
	int _IsInFrustum(int ID,LandVertex* pHeightMap,ZFrustum* pFrustum);
	// 根据_IsInFrustum()函数的结果对平截台体进行剔除
	void _FrustumCull(int ID,LandVertex* pHeightMap,ZFrustum* pFrustum);
	// 创建四叉树(在Build()函数中)
	bool _BuildQuadTree(LandVertex* pHeightMap,int ID);
	// 检索四叉树,查询与四个角值一致的节点
	int _FindNode(int _0,int _1,int _2,int _3);
	// 设置各节点的邻近节点
	void _BuildNeighborNode();
	// 重置每个节点
	void _Reset();

	// 判断是否为2的N次方
	//bool IsTowPow(int value);
	// 计算树的节点总数
	//int CalcNodeNum(int max,int min);
	// 添加子节点
	//ZQuadTree* _AddChild(int nCornerTL,int nCornerTR,int nCornerBL,int nCornerBR);
	// 设置4个角值
	//bool _SetCorners(int nCornerTL,int nCornerTR,int nCornerBL,int nCornerBR);
	// 在存储器中删除四叉树
	//void _Destroy();

public:
	// 最初循环节点构造函数
	ZQuadTree(int cx,int cy);
	// 下层子节点构造函数
	//ZQuadTree(ZQuadTree* pParent);
	// 析构函数
	~ZQuadTree(void);

	// 从LOD等级来看,当前节点是可以输出的节点吗?
	bool IsVisible(int ID,LandVertex* pHeightMap,D3DXVECTOR3* pCamera,float fLODRatio)
	{
		return ((TreeList[ID].iCorner[1] - TreeList[ID].iCorner[0]) <= _GetLODLevel(ID,pHeightMap,pCamera,fLODRatio));
	}
	// 创建四叉树
	bool Build(LandVertex* pHeightMap);
	// 创建三角形索引,返回输出三角形的个数
	int GenerateIndex(void* pIndex,LandVertex* pHeightMap,ZFrustum* pFrustum,float fLODRatio);

	// 得到角的索引值
	//void GetCorner(int& _0,int& _1,int& _2,int& _3)
	//{
	//	_0 = m_iCorner[0];
	//	_1 = m_iCorner[1];
	//	_2 = m_iCorner[2];
	//	_3 = m_iCorner[3];
	//}
};

