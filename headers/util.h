#pragma once

#undef min
#undef max

inline bool IsDecimal(float v)
{
	if(floor(v) != v)
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline float Round(float x)
{
	if(x < 0.f)
		return std::ceil(x - 0.5f);

	return std::floor(x + 0.5f);
}

inline double Round(double x)
{
	if(x < 0.0)
		return std::ceil(x - 0.5f);

	return std::floor(x + 0.5f);
}

inline float RoundToPlace(float x, int place)
{
	int p = std::pow(10, place);
	return std::floor(x * (p)) / p;
}

inline double RoundToPlace(double x, int place)
{
	int p = std::pow(10, place);
	return std::floor(x * (p)) / p;
}

inline int FloatToByte(float in)
{
	float f2 = std::max(0.0f, std::min(1.0f, in));
	return floor(f2 == 1.0f ? 255 : f2 * 256.0f);
}

inline float ByteToFloat(int in)
{
	int b2 = std::max(0, std::min(255, in));
	return (b2 == 255 ? 1.0f : b2 / 256.0f);
}

inline sf::Vector2f Midpoint(sf::Vector2f &a, sf::Vector2f &b)
{
	return sf::Vector2f((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f);
}

template<class T> inline T Clamp (const T in, const T min, const T max)
{
	return in <= min ? min : in >= max ? max : in;
}

template<class T> inline void ClampTo (T &in, const T min, const T max)
{
	in = (in <= min ? min : in >= max ? max : in);
}

template<class T> inline void swapvalues (T &a, T &b)
{
	T temp = a; a = b; b = temp;
}

template<class T> inline void VectorErase(std::vector<T> &vect, T val)
{
	std::vector<T>::iterator it;
	it = std::find(vect.begin(), vect.end(), val);
	vect.erase(it);
}

template< typename T >
struct delete_ptr : public std::unary_function<bool,T>
{
	bool operator()(T*pT) const { delete pT; return true; }
};

template<class T, class Y>
inline void ClearPointersType(T &cnt)
{
	std::for_each(cnt.begin(), cnt.end(), delete_ptr<Y>());
}

template <typename T>
void RemoveElement(std::vector<T>& vec, size_t pos)
{
	std::vector<T>::iterator it = vec.begin();
	std::advance(it, pos);
	vec.erase(it);
}

#ifndef BIT
#define BIT( num ) ( 1 << ( num ) )
#endif

typedef unsigned long long QWORD;

inline float dotProduct(const sf::Vector2f& lhs, const sf::Vector2f& rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

inline sf::Vector2f unitVector(const sf::Vector2f& vector)
{
	return vector / std::sqrt(dotProduct(vector, vector));
}

inline sf::Vector2f perpendicularVector(const sf::Vector2f& vector)
{
	return{ -vector.y, vector.x };
}

inline float VectorLength(sf::Vector2f &vec)
{
	return std::sqrt(std::pow(vec.x, 2) + std::pow(vec.y, 2));
}

inline float VectorLength(sf::Vector2f &vec1, sf::Vector2f &vec2)
{
	float xd = vec2.x - vec1.x;
	float yd = vec2.y - vec1.y;
	return std::sqrt((xd * xd) + (yd * yd));
}