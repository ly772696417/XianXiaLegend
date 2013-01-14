#pragma once

#include "Common.h"
#include "Table.h"
#include "d3dx9math.h"
#include <fstream>

class CHeightmap
{
public:
	CHeightmap();
	CHeightmap(int m, int n, float cellSpacing);
	CHeightmap(int m, int n, float cellSpacing, 
		const std::string& filename, float heightScale);
	CHeightmap(const CHeightmap& rhs);
	CHeightmap& operator=(const CHeightmap& rhs);
	~CHeightmap(void);

	void create(int m, int n, float cellSpacing);
	void loadRAW(int m, int n, float cellSpacing,
		const std::string& filename, float heightScale);
	void saveRAW(const std::string& filename);
	void filter3x3();
	void calcNormalMap();
	D3DXVECTOR3& normal(int i, int j);

	int numRows();
	int numCols();

	float& operator()(int i, int j);

private:
	bool        inBounds(int i, int j);
	D3DXVECTOR3 quadNormal(int i, int j);
	D3DXVECTOR3 calcNormal(int i, int j);
	float       sampleHeight3x3(int i, int j);

private:
	std::string        mHeightMapFilename;
	
	Table<D3DXVECTOR3> mNormalMap;
	float              mCellSpacing;
	float              mHeightScale;

public:
	Table<float>       mHeightMap;
};

