#pragma once

// a simple bidirectional map
// doesn't support operators or anything fancy
// but it does the job needed
template<class K, class V>
struct bimap_t
{
	//std::map<K, V> _Map;
	//std::map<V *, K *> _IMap;
	std::map<K, V> _Map;
	std::map<V, K> _IMap;

	inline const V &operator[](K key)
	{
		return const_cast<V>(_Map[key]);
	}

	V GetValueByKey(K key)
	{
		return _Map[key];
	}
	K GetKeyByValue(V value)
	{
		return _IMap[value];
	}

	inline std::map<K, V> &ThisMap() { return this->_Map; }

	inline bool Insert(K key, V value)
	{
		std::pair<K, V> p(key, value);
		std::pair<V, K> ip(value, key);
		if(_Map.find(key) == _Map.end())
		{
			_Map.insert(p);
			_IMap.insert(ip);
			return true;
		}

		return false;
	}

	inline bool Insert(std::pair<K, V> p)
	{
		std::pair<V, K> ip(p.second, p.first);
		if(_Map.find(p.first) == _Map.end())
		{
			_Map.insert(p);
			_IMap.insert(ip);
			return true;
		}

		return false;
	}

	inline bool Insert(std::pair<V, K> ip)
	{
		std::pair<K, V> p(ip.second, ip.first);
		if(_Map.find(p.first) == _Map.end())
		{
			_Map.insert(p);
			_IMap.insert(ip);
			return true;
		}

		return false;
	}

	inline void Clear()
	{
		_Map.clear();
		_IMap.clear();
	}
};