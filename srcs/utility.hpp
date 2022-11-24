#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <ostream>
#include <vector>

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	os << "vector(capacity=" << vec.capacity() << ", size=" << vec.size() << ")"
	   << std::endl;

	if (vec.size())
	{
		typename std::vector<T>::const_iterator it;
		for (it = vec.begin(); it != vec.end(); ++it)
		{
			std::cout << *it << ", ";
		}
	}
	return os;
}

#endif // UTILITY_HPP
