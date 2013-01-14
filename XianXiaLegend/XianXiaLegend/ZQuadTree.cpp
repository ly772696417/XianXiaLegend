#include "ZQuadTree.h"

int ZQuadTree::TotalTreeID = 0;

ZQuadTree::ZQuadTree(int cx,int cy)
{
	int Width = cx - 1,Depth = 0;
	while (Width >= 1)
	{
		Width = Width >> 1;
		Depth++;
	}
	MaxNodeNum = (pow(4,Depth) - 1) / (4 - 1);
	TreeList = new TreeNode[MaxNodeNum];
	TreeList[0].ID = 0;
	TreeList[0].ParentID = 0;
	for (int i = 0;i < 4;i++)
	{
		TreeList[0].ChildID[i] = -1;
		TreeList[0].NeighborID[i] = -1;
	}
	TreeList[0].iCorner[0] = 0;
	TreeList[0].iCorner[1] = cx - 1;
	TreeList[0].iCorner[2] = cx * (cy - 1);
	TreeList[0].iCorner[3] = cx * cy - 1;
	TreeList[0].iCenter = (TreeList[0].iCorner[0] + TreeList[0].iCorner[1] + 
							TreeList[0].iCorner[2] + TreeList[0].iCorner[3]) >> 2;
	TreeList[0].bCulled = false;
	TreeList[0].fRadius = 0.0f;
}

//ZQuadTree::ZQuadTree(int cx,int cy)
//{
//	m_pParent = NULL;
//	for (int i = 0;i < 4;i++)
//	{
//		m_pChild[i] = NULL;
//		m_pNeighbor[i] = NULL;
//	}
//	m_iCorner[CORNER_TL] = 0;
//	m_iCorner[CORNER_TR] = cx - 1;
//	m_iCorner[CORNER_BL] = cx * (cy - 1);
//	m_iCorner[CORNER_BR] = cx * cy - 1;
//	m_iCenter = (m_iCorner[CORNER_TL] + m_iCorner[CORNER_TR] + m_iCorner[CORNER_BL] + m_iCorner[CORNER_BR]) >> 2;
//	m_bCulled = false;
//	m_fRadius = 0.0f;
//}

//ZQuadTree::ZQuadTree(ZQuadTree* pParent)
//{
//	m_pParent = pParent;
//	for (int i = 0;i < 4;i++)
//	{
//		m_pChild[i] = NULL;
//		m_pNeighbor[i] = NULL;
//		m_iCorner[i] = 0;
//	}
//	m_iCenter = 0;
//	m_bCulled = false;
//	m_fRadius = 0.0f;
//}

ZQuadTree::~ZQuadTree(void)
{
	SAFE_DELETE_ARRAY(TreeList);
}

int ZQuadTree::CreateNode(int ParentID,int nCornerTL,int nCornerTR,int nCornerBL,int nCornerBR)
{
	TotalTreeID++;
	TreeList[TotalTreeID].ID = TotalTreeID;
	TreeList[TotalTreeID].ParentID = ParentID;
	for (int i = 0;i < 4;i++)
	{
		TreeList[TotalTreeID].ChildID[i] = -1;
		TreeList[TotalTreeID].NeighborID[i] = -1;
	}
	TreeList[TotalTreeID].iCorner[0] = nCornerTL;
	TreeList[TotalTreeID].iCorner[1] = nCornerTR;
	TreeList[TotalTreeID].iCorner[2] = nCornerBL;
	TreeList[TotalTreeID].iCorner[3] = nCornerBR;
	TreeList[TotalTreeID].iCenter = (nCornerTL + nCornerTR + nCornerBL + nCornerBR) >> 2;
	TreeList[TotalTreeID].bCulled = false;
	TreeList[TotalTreeID].fRadius = 0.0f;
	return TotalTreeID;
}

// 创建四叉树
bool ZQuadTree::Build(LandVertex* pHeightMap)
{
	if (_BuildQuadTree(pHeightMap,0))
	{
		_BuildNeighborNode();
		return true;
	}
	return false;
}

// 创建三角形索引,返回输出三角形的个数
int ZQuadTree::GenerateIndex(void* pIndex,LandVertex* pHeightMap,ZFrustum* pFrustum,float fLODRatio)
{
	_Reset();
	_FrustumCull(0,pHeightMap,pFrustum);
	return _GenTriIndex(0,0,pIndex,pHeightMap,pFrustum,fLODRatio);
}

// 使用4个下层对四叉树进行再分割(subdivide)
bool ZQuadTree::_SubDivide(int ID)
{
	if (TreeList[ID].iCorner[1] - TreeList[ID].iCorner[0] <= 1)
		return false;
	// 计算四边的中点
	int nCornerTM = (TreeList[ID].iCorner[0] + TreeList[ID].iCorner[1]) >> 1;
	int nCornerLM = (TreeList[ID].iCorner[0] + TreeList[ID].iCorner[2]) >> 1;
	int nCornerRM = (TreeList[ID].iCorner[1] + TreeList[ID].iCorner[3]) >> 1;
	int nCornerBM = (TreeList[ID].iCorner[2] + TreeList[ID].iCorner[3]) >> 1;
	TreeList[ID].ChildID[0] = CreateNode(ID,TreeList[ID].iCorner[0],nCornerTM,nCornerLM,TreeList[ID].iCenter);
	TreeList[ID].ChildID[1] = CreateNode(ID,nCornerTM,TreeList[ID].iCorner[1],TreeList[ID].iCenter,nCornerRM);
	TreeList[ID].ChildID[2] = CreateNode(ID,nCornerLM,TreeList[ID].iCenter,TreeList[ID].iCorner[2],nCornerBM);
	TreeList[ID].ChildID[3] = CreateNode(ID,TreeList[ID].iCenter,nCornerRM,nCornerBM,TreeList[ID].iCorner[3]);
	return true;
}

// 创建输出多边形的索引
int ZQuadTree::_GenTriIndex(int ID,int nTris,void* pIndex,LandVertex* pHeightMap,ZFrustum* pFrustum,float fLodRatio)
{
	/*if (TreeList[ID].bCulled == false)
	{*/
		if (IsVisible(ID,pHeightMap,pFrustum->GetPos(),fLodRatio))
		{
			DWORD* p = ((DWORD*)pIndex) + nTris * 3;
			
			// 判断四个方向的邻近节点LOD等级与当前节点关系（注意是当前单元格数与邻近LOD比较，非LOD与LOD比较）
			int iRepair[4] = {0,0,0,0};
			int NeighborLod;
			for (int i=0;i<4;i++)
			{
				if (TreeList[ID].ChildID[0] != -1 && TreeList[ID].NeighborID[i] != -1)
				{
					NeighborLod = _GetLODLevel(TreeList[ID].NeighborID[i],pHeightMap,pFrustum->GetPos(),fLodRatio);
					if (TreeList[ID].iCorner[1] - TreeList[ID].iCorner[0] > NeighborLod)
						iRepair[i] = 1;
				}
			}
			// 四个方向相同
			if ((iRepair[0] + iRepair[1] + iRepair[2] + iRepair[3]) == 0)
			{
				// 左上方三角形    
				*p++ = TreeList[ID].iCorner[0];   
				*p++ = TreeList[ID].iCorner[1];   
				*p++ = TreeList[ID].iCorner[2];   
				nTris++;   
				// 右下方三角形    
				*p++ = TreeList[ID].iCorner[2];   
				*p++ = TreeList[ID].iCorner[1];   
				*p++ = TreeList[ID].iCorner[3];
				nTris++;   
			}
			else
			{
				// 上方补三角
				if (iRepair[0] == 1)
				{
					int nCornerTM = (TreeList[ID].iCorner[0] + TreeList[ID].iCorner[1]) >> 1;
					*p++ = TreeList[ID].iCorner[0];
					*p++ = nCornerTM;
					*p++ = TreeList[ID].iCenter;
					nTris++;
					*p++ = nCornerTM;
					*p++ = TreeList[ID].iCorner[1];
					*p++ = TreeList[ID].iCenter;
					nTris++;	
				}
				else
				{
					*p++ = TreeList[ID].iCorner[0];
					*p++ = TreeList[ID].iCorner[1];
					*p++ = TreeList[ID].iCenter;
					nTris++;
				}
				// 下方补三角
				if (iRepair[1] == 1)
				{
					int nCornerBM = (TreeList[ID].iCorner[2] + TreeList[ID].iCorner[3]) >> 1;
					*p++ = TreeList[ID].iCorner[2];
					*p++ = TreeList[ID].iCenter;
					*p++ = nCornerBM;
					nTris++;
					*p++ = nCornerBM;
					*p++ = TreeList[ID].iCenter;
					*p++ = TreeList[ID].iCorner[3];
					nTris++;
				}
				else
				{
					*p++ = TreeList[ID].iCorner[2];
					*p++ = TreeList[ID].iCenter;
					*p++ = TreeList[ID].iCorner[3];
					nTris++;
				}
				// 左方补三角
				if (iRepair[2] == 1)
				{
					int nCornerLM = (TreeList[ID].iCorner[0] + TreeList[ID].iCorner[2]) >> 1;
					*p++ = nCornerLM;
					*p++ = TreeList[ID].iCorner[0];
					*p++ = TreeList[ID].iCenter;
					nTris++;
					*p++ = nCornerLM;
					*p++ = TreeList[ID].iCenter;
					*p++ = TreeList[ID].iCorner[2];
					nTris++;
				}
				else
				{
					*p++ = TreeList[ID].iCorner[0];
					*p++ = TreeList[ID].iCenter;
					*p++ = TreeList[ID].iCorner[2];
					nTris++;
				}
				// 右方补三角
				if (iRepair[3] == 1)
				{
					int nCornerRM = (TreeList[ID].iCorner[1] + TreeList[ID].iCorner[3]) >> 1;
					*p++ = TreeList[ID].iCenter;
					*p++ = TreeList[ID].iCorner[1];
					*p++ = nCornerRM;
					nTris++;
					*p++ = TreeList[ID].iCenter;
					*p++ = nCornerRM;
					*p++ = TreeList[ID].iCorner[3];
					nTris++;
				}
				else
				{
					*p++ = TreeList[ID].iCorner[3];
					*p++ = TreeList[ID].iCenter;
					*p++ = TreeList[ID].iCorner[1];
					nTris++;
				}
			}
			return nTris;
		}
		if (TreeList[ID].ChildID[0] != -1 && TreeList[TreeList[ID].ChildID[0]].bCulled != true)
			nTris = _GenTriIndex(TreeList[ID].ChildID[0],nTris,pIndex,pHeightMap,pFrustum,fLodRatio);   
		if (TreeList[ID].ChildID[1] != -1 && TreeList[TreeList[ID].ChildID[1]].bCulled != true)
			nTris = _GenTriIndex(TreeList[ID].ChildID[1],nTris,pIndex,pHeightMap,pFrustum,fLodRatio);   
		if (TreeList[ID].ChildID[2] != -1 && TreeList[TreeList[ID].ChildID[2]].bCulled != true)
			nTris = _GenTriIndex(TreeList[ID].ChildID[2],nTris,pIndex,pHeightMap,pFrustum,fLodRatio);   
		if (TreeList[ID].ChildID[3] != -1 && TreeList[TreeList[ID].ChildID[3]].bCulled != true)
			nTris = _GenTriIndex(TreeList[ID].ChildID[3],nTris,pIndex,pHeightMap,pFrustum,fLodRatio);   
		return nTris;
	//}
	//else
	//	return nTris;
}

// 当前节点包含在平截台体?
int ZQuadTree::_IsInFrustum(int ID,LandVertex* pHeightMap,ZFrustum* pFrustum)
{
	bool b[4];
	bool bInSphere;
	// 在边界球体内
	bInSphere = pFrustum->IsInSphere((D3DXVECTOR3*)(pHeightMap + TreeList[ID].iCenter),TreeList[ID].fRadius);
	// 不在边界球体内的话,可以省略点单位的平截台体的测试.
	if (!bInSphere)
		return FRUSTUM_OUT;
	// 四叉树的4组边界平截台体测试
	b[0] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap + TreeList[ID].iCorner[0]));
	b[1] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap + TreeList[ID].iCorner[1]));
	b[2] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap + TreeList[ID].iCorner[2]));
	b[3] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap + TreeList[ID].iCorner[3]));
	// 4个都在平截台体内部
	if ((b[0] + b[1] + b[2] + b[3]) == 4)
		return FRUSTUM_COMPLETELY_IN;
	// 一部分在平截台体内部
	return FRUSTUM_PARTIALLY_IN;
}

// 根据_IsInFrustum()函数的结果对平截台体进行剔除
void ZQuadTree::_FrustumCull(int ID,LandVertex* pHeightMap,ZFrustum* pFrustum)
{
	int ret;
	ret = _IsInFrustum(ID,pHeightMap,pFrustum);
	switch (ret)
	{
	case FRUSTUM_COMPLETELY_IN:
		TreeList[ID].bCulled = false;
		return;
	case FRUSTUM_PARTIALLY_IN:
		TreeList[ID].bCulled = false;
		break;
	case FRUSTUM_OUT:
		TreeList[ID].bCulled = true;
		return;
	}
	if (TreeList[ID].ChildID[0] != -1)
		_FrustumCull(TreeList[ID].ChildID[0],pHeightMap,pFrustum);
	if (TreeList[ID].ChildID[1] != -1)
		_FrustumCull(TreeList[ID].ChildID[1],pHeightMap,pFrustum);
	if (TreeList[ID].ChildID[2] != -1)
		_FrustumCull(TreeList[ID].ChildID[2],pHeightMap,pFrustum);
	if (TreeList[ID].ChildID[3] != -1)
		_FrustumCull(TreeList[ID].ChildID[3],pHeightMap,pFrustum);
}

//创建四叉树(在Build()函数中)
bool ZQuadTree::_BuildQuadTree(LandVertex* pHeightMap,int ID)
{
	D3DXVECTOR3 pv1 = *(D3DXVECTOR3*)(pHeightMap + TreeList[ID].iCenter);
	D3DXVECTOR3 pv2 = *(D3DXVECTOR3*)(pHeightMap + TreeList[ID].iCorner[0]);
	TreeList[ID].fRadius = _GetDistance(pv1,pv2); 
	if (_SubDivide(ID))
	{
		for (int i = 0;i < 4;i++)
		{
			_BuildQuadTree(pHeightMap,TreeList[ID].ChildID[i]);
		}
	}
	return true;
}

// 检索四叉树,查询与四个角值一致的节点
int ZQuadTree::_FindNode(int _0,int _1,int _2,int _3)
{
	for (int i = 0;i < MaxNodeNum;i++)
	{
		if (TreeList[i].iCorner[0] == _0 &&
			TreeList[i].iCorner[1] == _1 &&
			TreeList[i].iCorner[2] == _2 &&
			TreeList[i].iCorner[3] == _3)
			return i;
	}
	return -1;
}

// 设置各节点的邻近节点
void ZQuadTree::_BuildNeighborNode()
{
	int TL,TR,BL,BR;
	for (int i = 0;i < MaxNodeNum;i++)
	{
		// 上邻接点
		TL = 2 * TreeList[i].iCorner[0] - TreeList[i].iCorner[2]; 
		TR = 2 * TreeList[i].iCorner[1] - TreeList[i].iCorner[3]; 
		BL = TreeList[i].iCorner[0];
		BR = TreeList[i].iCorner[1];
		TreeList[i].NeighborID[0] = _FindNode(TL,TR,BL,BR);
		// 下邻接点
		TL = TreeList[i].iCorner[2];
		TR = TreeList[i].iCorner[3];
		BL = 2 * TreeList[i].iCorner[2] - TreeList[i].iCorner[0];
		BR = 2 * TreeList[i].iCorner[3] - TreeList[i].iCorner[1];
		TreeList[i].NeighborID[1] = _FindNode(TL,TR,BL,BR);
		// 左邻接点
		TL = 2 * TreeList[i].iCorner[0] - TreeList[i].iCorner[1];
		TR = TreeList[i].iCorner[0];
		BL = 2 * TreeList[i].iCorner[2] - TreeList[i].iCorner[3];
		BR = TreeList[i].iCorner[2];
		TreeList[i].NeighborID[2] = _FindNode(TL,TR,BL,BR);
		// 右邻接点
		TL = TreeList[i].iCorner[1];
		TR = 2 * TreeList[i].iCorner[1] - TreeList[i].iCorner[0];
		BL = TreeList[i].iCorner[3];
		BR = 2 * TreeList[i].iCorner[3] - TreeList[i].iCorner[2];
		TreeList[i].NeighborID[3] = _FindNode(TL,TR,BL,BR);
	}
}

// 重置节点
void ZQuadTree::_Reset()
{
	for (int i = 0;i < MaxNodeNum;i++)
	{
		TreeList[i].bCulled = false;
	}
}

// 判断是否为二的N次方
//bool ZQuadTree::IsTowPow(int value)
//{
//	int i,j;
//	i = (value - 1) | value;
//	j = (value << 1) - 1;
//	if (i == j)
//		return true;
//	return false;
//}


//int ZQuadTree::CalcNodeNum(int max,int min)
//{
//	int ctr = 0;
//	int var = 0;
//	while (var == 0)
//	{
//		ctr += max;
//		max = max / min;
//		if (max == 1)
//			var = 1;
//	}
//	ctr++;
//	return ctr;
//}

// 添加子节点
//ZQuadTree* ZQuadTree::_AddChild(int nCornerTL,int nCornerTR,int nCornerBL,int nCornerBR)
//{
//	ZQuadTree* newNode = new ZQuadTree(this);
//	newNode->_SetCorners(nCornerTL,nCornerTR,nCornerBL,nCornerBR);
//	return newNode;
//}

// 设置4个角值
//bool ZQuadTree::_SetCorners(int nCornerTL,int nCornerTR,int nCornerBL,int nCornerBR)
//{
//	m_iCorner[CORNER_TL] = nCornerTL;
//	m_iCorner[CORNER_TR] = nCornerTR;
//	m_iCorner[CORNER_BL] = nCornerBL;
//	m_iCorner[CORNER_BR] = nCornerBR;
//	m_iCenter = (m_iCorner[CORNER_TL] + m_iCorner[CORNER_TR] + m_iCorner[CORNER_BL] + m_iCorner[CORNER_BR]) >> 2;
//	return true;
//}

// 在存储器中删除四叉树
//void ZQuadTree::_Destroy()
//{
//	for (int i=0;i < 4;i++)
//	{
//		if (m_pChild[i] != NULL)
//			delete m_pChild[i];
//	}
//}