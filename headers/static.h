#pragma once

#define MAKE_STATIC_CLASS(cls) \
private: \
	static cls *instance; \
public: \
	__forceinline static cls *Create() \
	{ \
	if(!instance) \
	instance = new cls(); \
	\
	return instance; \
	} \
	\
	__forceinline static void Destroy() \
	{ \
	if(instance) \
		{ \
		delete instance; \
		instance = nullptr; \
		} \
	} \
	__forceinline static cls *GetInstance() { if(instance == nullptr) Create(); return instance; }

#define STATIC_IMPLEMENT(cls) \
	cls *cls::instance = nullptr;