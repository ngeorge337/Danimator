#pragma once

template<class T>
class _base_clamped_t
{
protected:
	T val;
	T minn;
	T maxx;

public:
	inline _base_clamped_t() {}
	inline _base_clamped_t(T minimum, T maximum) : minn(minimum), maxx(maximum) {}
	inline operator T() const { return val; }
	inline T operator *() const { return val; }
	inline T operator= (T in) { val = Clamp(in, minn, maxx); return val; }
	inline T operator+= (T in) { val += in; val = Clamp(val, minn, maxx); return val; }
};

template<class T>
class clamped_t : public _base_clamped_t<T>
{
public:
	inline clamped_t() : _base_clamped_t<T>() {}
	inline clamped_t(T minimum, T maximum) : _base_clamped_t<T>(T minimum, T maximum) : minn(minimum), maxx(maximum) {}
};

template<>
class clamped_t<float> : public _base_clamped_t<float>
{
public:
	inline clamped_t() : _base_clamped_t<float>()
	{
		minn = 0.f;
		maxx = 1.f;
	}

	inline operator float() const { return val; }
	inline float operator *() const { return val; }
	inline float operator= (float in) { val = Clamp(in, minn, maxx); return val; }
	inline float operator+= (float in) { val += in; val = Clamp(val, minn, maxx); return val; }
};

template<>
class clamped_t<double> : public _base_clamped_t<double>
{
public:
	inline clamped_t() : _base_clamped_t<double>()
	{
		minn = 0.0;
		maxx = 1.0;
	}

	inline operator double() const { return val; }
	inline double operator *() const { return val; }
	inline double operator= (double in) { val = Clamp(in, minn, maxx); return val; }
	inline double operator+= (double in) { val += in; val = Clamp(val, minn, maxx); return val; }
};

template<>
class clamped_t<int> : public _base_clamped_t<int>
{
public:
	inline clamped_t() : _base_clamped_t<int>()
	{
		minn = 0;
		maxx = 255;
	}

	inline operator int() const { return val; }
	inline int operator *() const { return val; }
	inline int operator= (int in) { val = Clamp(in, minn, maxx); return val; }
	inline int operator+= (int in) { val += in; val = Clamp(val, minn, maxx); return val; }
};