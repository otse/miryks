#pragma once

#include <vector>

template <typename T>
bool VectorAdd(T t, std::vector<T> &v)
{
	typename std::vector<T>::iterator has = std::find(v.begin(), v.end(), t);
	if (has == v.end())
	{
		// We don't have this yet, add it
		v.push_back(t);
		return true;
	}
	return false;
}

template <typename T>
bool VectorRemove(T t, std::vector<T> &v)
{
	typename std::vector<T>::iterator has = std::find(v.begin(), v.end(), t);
	if (has != v.end())
	{
		// We have this, erase it
		v.erase(has);
		return true;
	}
	return false;
}