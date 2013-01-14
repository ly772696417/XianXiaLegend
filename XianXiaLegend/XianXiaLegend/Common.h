#pragma once

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#ifndef V
#define V(x)           { hr = x; }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return hr; } }
#endif

// windows
#include <windows.h>
#include <iostream>

// D3D
#pragma comment(lib,"d3d9.lib")
#include "d3d9.h"
#pragma comment(lib,"d3dx9.lib")
#include "d3dx9.h"
#pragma comment(lib,"d3dxof.lib")
#include <dxfile.h>
#pragma comment(lib,"dxguid.lib")
#include <initguid.h>

#pragma comment(lib,"Winmm.lib")

// STL
#include <string>
#include <vector>
#include <list>
#include <map>
using namespace std;

const float INFINITY = FLT_MAX;
const float EPSILON  = 0.001f;

// 结构
struct LandVertex
{
	float x,y,z;	
	D3DXVECTOR3 normal;
	float u,v;
	float tu,tv;
};

struct BILL_VERT
{
	float x,y,z;
	DWORD iColor;
	float tu,tv;
};
#define BILL_FVF (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

struct Ray
{
	D3DXVECTOR3 origin;  // 起点
	D3DXVECTOR3 dir;     // 方向
};

// 常量
const int ELAPSEDTIME_COUNT = 50;       // 帧链表计数量
const float TERRAIN_SLOPE = 0.5f;    // 地形的斜率(弧度)

const D3DXCOLOR WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR RED(1.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const D3DXCOLOR BLUE(0.0f, 0.0f, 1.0f, 1.0f);

struct Mtrl
{
	Mtrl()
		:ambient(WHITE), diffuse(WHITE), spec(WHITE), specPower(8.0f){}
	Mtrl(const D3DXCOLOR& a, const D3DXCOLOR& d, 
		const D3DXCOLOR& s, float power)
		:ambient(a), diffuse(d), spec(s), specPower(power){}

	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR spec;
	float specPower;
};

struct DirLight
{
	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR spec;
	D3DXVECTOR3 dirW;
};
