#pragma once

#include <vector>

template <typename T>
bool vector_safe_add(T t, std::vector<T> &v)
{
	if (t == nullptr)
		return false;
	typename std::vector<T>::iterator has = std::find(v.begin(), v.end(), t);
	if (has == v.end())
	{
		// We don't have this yet, add it.
		v.push_back(t);
		return true;
	}
	return false;
}

template <typename T>
bool vector_safe_remove(T t, std::vector<T> &v)
{
	if (t == nullptr)
		return false;
	typename std::vector<T>::iterator has = std::find(v.begin(), v.end(), t);
	if (has != v.end())
	{
		// We have this, erase it.
		v.erase(has);
		return true;
	}
	return false;
}