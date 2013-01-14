#pragma  once

#ifndef TABLE_H
#define TABLE_H

#include <cassert>

template <typename T>
class Table
{
public:
	Table();
	Table(int m, int n);
	Table(int m, int n, const T& value);
	Table(const Table<T>& rhs);
	~Table();

	Table<T>& operator=(const Table& rhs);
	T&        operator()(int i, int j);

	typedef T* iterator;
	iterator begin();
	iterator end();

	int numRows()const;
	int numCols()const;

	void resize(int m, int n);
	void resize(int m, int n, const T& value);

private:
	void destroy();

private:
	int _m;
	int _n;
	T*  _matrix;
};

template <typename T>
Table<T>::Table<T>()
{
	_matrix = 0;
	_m      = 0;
	_n      = 0;
}

template <typename T>
Table<T>::Table<T>(int m, int n)
{
	_matrix = 0;
	_m      = 0;
	_n      = 0;
	resize(m, n, T());
}

template <typename T>
Table<T>::Table<T>(int m, int n, const T& value)
{
	_matrix = 0;
	_m      = 0;
	_n      = 0;
	resize(m, n, value);
}

template <typename T>
Table<T>::Table<T>(const Table<T>& rhs)
{
	_matrix = 0;
	_m      = 0;
	_n      = 0;
	*this = rhs;
}

template <typename T>
Table<T>::~Table<T>()
{
	//destroy();
}

template <typename T>
Table<T>& Table<T>::operator=(const Table& rhs)
{
	if( this == &rhs )
		return *this;

	resize(rhs._m, rhs._n);

	int p = _m * _n;
	for(int i = 0; i < _m; ++i)
		for(int j = 0; j < _n; ++j)
			_matrix[i * _n + j] = rhs._matrix[i * _n + j];

	return *this;

}

template <typename T>
T& Table<T>::operator()(int i, int j)
{
	assert( i >= 0 && i < _m && j >= 0 && j < _n);
	return _matrix[i * _n + j];
}

template <typename T>
typename Table<T>::iterator Table<T>::begin()
{
	return &_matrix[0];
}

template <typename T>
typename Table<T>::iterator Table<T>::end()
{
	return &_matrix[_m*_n];
}

template <typename T>
int Table<T>::numRows()const
{
	return _m;
}

template <typename T>
int Table<T>::numCols()const
{
	return _n;
}

template <typename T>
void Table<T>::resize(int m, int n)
{
	resize(m, n, T());
}

template <typename T>
void Table<T>::resize(int m, int n, const T& value)
{
	assert(m > 0 && n > 0);

	destroy(); // free old data.

	_m = m;
	_n = n;

	int p = _m * _n;
	_matrix = new T[p];

	for(int i = 0; i < p; ++i)
		_matrix[i] = value;
}

template <typename T>
void Table<T>::destroy()
{
	if( _matrix )
	{
		delete[] _matrix;
		_matrix = 0;
	}

	_m = 0;
	_n = 0;
}

#endif // TABLE_H