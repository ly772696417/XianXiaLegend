#include "Heightmap.h"


CHeightmap::CHeightmap(void)
: mCellSpacing(0.0f)
{
}

CHeightmap::CHeightmap(int m, int n, float cellSpacing)
{
	create(m, n, cellSpacing);
}

CHeightmap::CHeightmap(int m, int n, float cellSpacing, 
					 const string& filename, float heightScale)
{
	loadRAW(m, n, cellSpacing, filename, heightScale);
}

CHeightmap::CHeightmap(const CHeightmap& rhs)
{
	*this = rhs;
}

CHeightmap& CHeightmap::operator=(const CHeightmap& rhs)
{
	if( this == &rhs )
		return *this;

	mHeightMapFilename = rhs.mHeightMapFilename;
	mHeightMap         = rhs.mHeightMap;
	mNormalMap         = rhs.mNormalMap;
	mCellSpacing       = rhs.mCellSpacing;
	mHeightScale       = rhs.mHeightScale;

	// Allow chain assignments x = y = z = w = ...
	return *this;
}

CHeightmap::~CHeightmap()
{
}

void CHeightmap::create(int m, int n, float cellSpacing)
{
	mCellSpacing       = cellSpacing;
	mHeightMapFilename = "DefaultFilename";
	mHeightScale       = 1.0f;
	mHeightMap.resize(m, n, 0.0f);
}

void CHeightmap::loadRAW(int m, int n, float cellSpacing, 
						const string& filename, float heightScale)
{
	mCellSpacing       = cellSpacing;
	mHeightMapFilename = filename;
	mHeightScale       = heightScale;

	// A height for each vertex
	std::vector<unsigned char> in( m * n );

	std::ifstream inFile;
	inFile.open(filename.c_str(), ios_base::binary);

	assert(inFile != 0);

	inFile.read((char*)&in[0], (streamsize)in.size());

	inFile.close();

	mHeightMap.resize(m, n, 0);
	for(int i = 0; i < m; ++i)
	{
		for(int j = 0; j < n; ++j)
		{
			int k = i * n + j;
			mHeightMap(i, j) = in[k] * heightScale;
		}
	}

	calcNormalMap();
}

void CHeightmap::saveRAW(const string& filename)
{
	ofstream outFile;

	outFile.open(filename.c_str(), ios_base::binary);

	assert(outFile != 0);

	int m = mHeightMap.numRows();
	int n = mHeightMap.numCols();

	vector<unsigned char> data(m*n);
	for(int i = 0; i < m; ++i)
		for(int j = 0; j < n; ++j)
			data[i * n + j] = (unsigned char)(mHeightMap(i, j) * 255.0f);

	outFile.write((char*)&data[0], (streamsize)data.size());

	outFile.close();
}

void CHeightmap::filter3x3()
{
	Table<float> temp(mHeightMap.numRows(), mHeightMap.numCols());

	for(int i = 0; i < mHeightMap.numRows(); ++i)
		for(int j = 0; j < mHeightMap.numCols(); ++j)
			temp(i,j) = sampleHeight3x3(i,j);

	mHeightMap = temp;
}

void CHeightmap::calcNormalMap()
{
	int m = mHeightMap.numRows();
	int n = mHeightMap.numCols();

	mNormalMap.resize(m, n);

	for(int i = 0; i < m; ++i)
		for(int j = 0; j < n; ++j)
			mNormalMap(i,j) = calcNormal(i, j);
}

D3DXVECTOR3& CHeightmap::normal(int i, int j)
{
	assert(mNormalMap.numRows() != 0 && mNormalMap.numCols() != 0);
	return mNormalMap(i, j);
}

int CHeightmap::numRows()
{
	return mHeightMap.numRows();
}

int CHeightmap::numCols()
{
	return mHeightMap.numCols();
}

float& CHeightmap::operator()(int i, int j)
{
	return mHeightMap(i, j);
}

bool CHeightmap::inBounds(int i, int j)
{
	return 
		i >= 0 && 
		i < (int)mHeightMap.numRows() && 
		j >= 0 && 
		j < (int)mHeightMap.numCols();
}

D3DXVECTOR3 CHeightmap::quadNormal(int i, int j)
{
	// Function computes the normal for the ijth quad.
	// We take the quad normal as the average of the two
	// triangles that make up the quad.
	//
	//       u
	// h0*-------*h1
	//   |      /|
	//  v|    /  |t
	//   |  /    |
	//   |/      |
	// h2*-------*h3
	//       s

	float h0 = mHeightMap(i,   j);
	float h1 = mHeightMap(i,   j+1); 
	float h2 = mHeightMap(i+1, j);  
	float h3 = mHeightMap(i+1, j+1);

	D3DXVECTOR3 u = D3DXVECTOR3(mCellSpacing, h1-h0, 0.0f);
	D3DXVECTOR3 v = D3DXVECTOR3(0.0f, h2-h0, -mCellSpacing);

	D3DXVECTOR3 s = D3DXVECTOR3(-mCellSpacing, h2-h3, 0.0f);
	D3DXVECTOR3 t = D3DXVECTOR3(0.0f, h1-h3, mCellSpacing);

	D3DXVECTOR3 n1(0.0f, 1.0f, 0.0f);
	D3DXVec3Cross(&n1, &u, &v);
	D3DXVec3Normalize(&n1, &n1);

	D3DXVECTOR3 n2(0.0f, 1.0f, 0.0f);
	D3DXVec3Cross(&n2, &s, &t);
	D3DXVec3Normalize(&n2, &n2);

	return (n1 + n2) * 0.5f;
}

D3DXVECTOR3 CHeightmap::calcNormal(int i, int j)
{
	// The ijth vertex normal is found by averaging the normals
	// of the four quads that surround the vertex.

	D3DXVECTOR3 avg(0.0f, 0.0f, 0.0f);
	float num = 0.0f;

	int numRows = (int)mHeightMap.numRows();
	int numCols = (int)mHeightMap.numCols();
	for(int m = i-1; m <= i; ++m)
	{
		for(int n = j-1; n <= j; ++n)
		{
			// Vertices on heightmap boundaries do not have surrounding quads
			// in some directions, so we just average in a normal vector
			// that is axis aligned with the y-axis.
			if(m < 0 || n < 0 || m == numRows-1 || n == numCols-1)
			{
				avg += D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				num += 1.0f;
			}
			else
			{
				avg += quadNormal(m, n);
				num += 1.0f;
			}
		}
	}

	avg /= num;
	D3DXVec3Normalize(&avg, &avg);

	return avg;
}

float CHeightmap::sampleHeight3x3(int i, int j)
{
	// Function computes the average height of the ij element.
	// It averages itself with its eight neighbor pixels.  Note
	// that if a pixel is missing neighbor, we just don't include it
	// in the average--that is, edge pixels don't have a neighbor pixel.
	//
	// ----------
	// | 1| 2| 3|
	// ----------
	// |4 |ij| 6|
	// ----------
	// | 7| 8| 9|
	// ----------

	float avg = 0.0f;
	float num = 0.0f;

	for(int m = i-1; m <= i+1; ++m)
	{
		for(int n = j-1; n <= j+1; ++n)
		{
			if( inBounds(m,n) )
			{
				avg += mHeightMap(m,n);
				num += 1.0f;
			}
		}
	}

	return avg / num;
}
