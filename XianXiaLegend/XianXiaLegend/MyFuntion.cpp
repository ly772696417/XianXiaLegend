#include "MyFuntion.h"

CMyFuntion::CMyFuntion(void)
{
}

CMyFuntion::~CMyFuntion(void)
{
}

HRESULT CMyFuntion::CopyName(LPCSTR Name,LPSTR *pNewName)
{
    UINT cbLength;

    if( Name != NULL )
    {
        cbLength = (UINT)strlen(Name) + 1;
        *pNewName = new CHAR[cbLength];
        if (*pNewName == NULL)
            return E_OUTOFMEMORY;
        memcpy( *pNewName,Name,cbLength*sizeof(CHAR) );
    }
    else
    {
        *pNewName = NULL;
    }
    return S_OK;
}

D3DXVECTOR3 CMyFuntion::DirToRot(D3DXVECTOR3 vDir,D3DXVECTOR3 vOrigRot)
{
	D3DXVECTOR3 vIn,vOut;
	vIn = vDir;
	vIn.y = 0;
	D3DXVec3Normalize(&vIn,&vIn);

	D3DXVECTOR3 ZDir = D3DXVECTOR3(0,0,-1);
	float Yaw = D3DXVec3Dot(&ZDir,&vIn);
	Yaw = acos(Yaw);
	if (vIn.x <= 0)
	{
		vOut = D3DXVECTOR3(0,Yaw,0);
		vOut += vOrigRot;
	}
	else
	{
		vOut = D3DXVECTOR3(0,2 * D3DX_PI - Yaw,0);
		vOut += vOrigRot;
	}
	return vOut;
}

Ray CMyFuntion::CalculateRay(HWND hWnd,IDirect3DDevice9* pDevice)
{
	Ray ray;
	// 获取客户区鼠标坐标
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hWnd,&pt);
	// 获得视口
	D3DVIEWPORT9 vp;
	pDevice->GetViewport(&vp);
	// 获得投影变换矩阵
	D3DXMATRIX proj;
	pDevice->GetTransform(D3DTS_PROJECTION,&proj);
	// 直接逆转换为视图空间中的点
	D3DXVECTOR3 viewPos;
	viewPos.x = ((2.0f * pt.x) / vp.Width - 1.0f) / proj._11;
	viewPos.y = ((-2.0f * pt.y) / vp.Height + 1.0f) / proj._22;
	viewPos.z = 1.0f;

	//--------------转换到世界空间中的射线----------------
	ray.origin = D3DXVECTOR3(0,0,0);
	ray.dir = viewPos;
	// 获得视图变换矩阵的逆矩阵
	D3DXMATRIX matView,matInvView;
	pDevice->GetTransform(D3DTS_VIEW,&matView);
	D3DXMatrixInverse(&matInvView,NULL,&matView);
	// 转换到世界空间
	D3DXVec3TransformCoord(&ray.origin,&ray.origin,&matInvView);
	D3DXVec3TransformNormal(&ray.dir,&ray.dir,&matInvView);
	D3DXVec3Normalize(&ray.dir,&ray.dir);

	return ray;
}

/*bool CMyFuntion::IntersectTriangle(const Ray& ray,D3DXVECTOR3& v0,D3DXVECTOR3& v1,
					   D3DXVECTOR3& v2,float* t,float* u,float* v)
{
	D3DXVECTOR3 orig = ray.origin;
	D3DXVECTOR3 dir = ray.dir;
	// 算出两个边的向量
	D3DXVECTOR3 edge1 = v1 - v0;
	D3DXVECTOR3 edge2 = v2 - v0;

	D3DXVECTOR3 pvec;
	D3DXVec3Cross(&pvec,&dir,&edge2);

	float det = D3DXVec3Dot(&edge1,&pvec);

	D3DXVECTOR3 tvec;
	if (det > 0)
	{
		tvec = orig - v0;
	}
	else
	{
		/*tvec = v0 - orig;
		det = -det;
		return false;
	}
	if (det < 0.0001f)
		return false;
	// 计算u并测试是否合法（在三角形内）
	*u = D3DXVec3Dot(&tvec,&pvec);
	if (*u < 0.0f || *u > det)
		return false;

	D3DXVECTOR3 qvec;
	D3DXVec3Cross(&qvec,&tvec,&edge1);
	//计算u并测试是否合法（在三角形内）
	*v = D3DXVec3Dot(&dir,&qvec);
	if (*v < 0.0f || *u + *v > det)
		return false;

	*t = D3DXVec3Dot(&edge2,&qvec);
	float fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	if(*t < 0)
		return FALSE;

	return true;
}*/

bool CMyFuntion::CollisonTestAABB(D3DXVECTOR3 AMin,D3DXVECTOR3 AMax,D3DXVECTOR3 BMin,D3DXVECTOR3 BMax)
{
	if (AMin.x < BMax.x && AMax.x > BMin.x && 
		AMin.y < BMax.y && AMax.y > BMin.y && 
		AMin.z < BMax.z && AMax.z > BMin.z)
		return true;
	return false;
}

bool CMyFuntion::CollisonTestPoint(D3DXVECTOR3 A,D3DXVECTOR3 BMin,D3DXVECTOR3 BMax)
{
	if (A.x < BMax.x && A.x > BMin.x && 
		A.y < BMax.y && A.y > BMin.y && 
		A.z < BMax.z && A.z > BMin.z)
		return true;
	return false;
}

D3DXVECTOR3 CMyFuntion::GetVec3FromStr(char* str)
{
	char X[32],Y[32],Z[32];
	sscanf(str,"%[^,;],%[^,;],%[^,;]",X,Y,Z);
	D3DXVECTOR3 v;
	v.x = atof(X);
	v.y = atof(Y);
	v.z = atof(Z);
	return v;
}

void CMyFuntion::DrawLine(LPDIRECT3DDEVICE9 pDevice,
						  D3DXVECTOR3 &v1,
						  D3DXVECTOR3 &v2,
						  D3DCOLOR color)
{
	struct D3D_VERTEX
	{
		D3DXVECTOR3 pos;
		D3DCOLOR color;
	};

	D3D_VERTEX point[2];

	point[0].pos = v1;
	point[0].color = color;
	point[1].pos = v2;
	point[1].color = color;

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pDevice->SetTransform(D3DTS_WORLD,&matWorld);
	DWORD data;
	pDevice->GetRenderState(D3DRS_LIGHTING,&data);
	pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

	pDevice->SetFVF( D3DFVF_XYZ|D3DFVF_DIFFUSE );
	pDevice->DrawPrimitiveUP(D3DPT_LINELIST,1,point,sizeof(D3D_VERTEX));

	pDevice->SetRenderState(D3DRS_LIGHTING,data);
}

float CMyFuntion::CalcSlope(D3DXVECTOR3 start,D3DXVECTOR3 end)
{
	float Y = end.y - start.y;
	float Z = D3DXVec3Length(&(end - start));
	float Slope = asin(Y / Z);
	return Slope;
}

// returns a random number
FORCEINLINE float asm_rand()
{

#if _MSC_VER == 1300

	static unsigned __int64 q = time( NULL );

	_asm {
		movq mm0, q

			// do the magic MMX thing
			pshufw mm1, mm0, 0x1E
			paddd mm0, mm1

			// move to integer memory location and free MMX
			movq q, mm0
			emms
	}

	return float( q );
#else
	// VC6 does not support pshufw
	return float( rand() );
#endif
}

// returns the maximum random number
FORCEINLINE float asm_rand_max()
{

#if _MSC_VER == 1300

	//	return std::numeric_limits< unsigned __int64 >::max();
	return 9223372036854775807.0f;
#else
	// VC6 does not support unsigned __int64
	return float( RAND_MAX );
#endif

}

float CMyFuntion::UnitRandom()
{
	return asm_rand() / asm_rand_max();
}

float CMyFuntion::RangeRandom(float fLow, float fHigh)
{
	return (fHigh-fLow)*UnitRandom() + fLow;
}

float CMyFuntion::SymmetricRandom()
{
	return 2.f * UnitRandom() - 1.f;
}