class Random
{
public:
	Random() { seed = 0; srand(seed); }
	Random(unsigned _seed) { seed = _seed; srand(seed); }
	int operator()(int _min, int _max)
	{
		return _min + (rand() % _max);
	}
	float operator()(float _min, float _max, int places = 2)
	{
		if(places == 0)
			return (int) _min + (rand() % (int) _max);
		float r = (int) _min + (rand() % (int) _max);
		r = _min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(_max-_min)));
		int divisor = std::pow(10, places);
		r = floorf(r * divisor) / divisor;
		return r;
	}

	inline float Deviation(float mid, float dev)
	{
		return this->operator() (mid-dev, mid+dev);
	}
	inline int Deviation(int mid, int dev)
	{
		return this->operator() (mid-dev, mid+dev);
	}
	inline sf::Vector2f Circle(sf::Vector2f point, float radius)
	{
		if(radius <= 0.f)
			return point;

		float pAng = this->operator()(0.f, 360.f);
		float pRad = radius * std::sqrt(this->operator()(0.f, 1.f));
		sf::Vector2f radiusVector;
		radiusVector.x = pRad * std::cos(pAng);
		radiusVector.y = pRad * std::sin(pAng);
		return point + radiusVector;
	}

	inline void SetSeed(unsigned newseed)
	{
		seed = newseed;
		srand(seed);
	}
	inline unsigned int GetSeed()
	{
		return seed;
	}

private:
	unsigned int seed;
};