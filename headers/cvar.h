#pragma once

union CVarValue_u
{
	bool Bool;
	int Int;
	float Float;
	char *String;
};

enum
{
	CVAR_ARCHIVE		= 1,	// set to cause it to be saved to config
	CVAR_SECRET			= 2,	// don't allow change from console at all
	CVAR_ISDEFAULT		= 4,	// is cvar unchanged since creation?
	CVAR_NOINITCALL		= 8,	// don't call callback at game start
	CVAR_NOMCALL		= 16,	// don't call callback when value is changed
	CVAR_AUTO			= 32,
};

enum eCvarType
{
	CVAR_Bool,
	CVAR_Int,
	CVAR_Float,
	CVAR_String,
	CVAR_Dummy,		// just redirects to another cvar
};

//==================
// Base Cvar Class
//==================
class BaseCvar
{
public:
	BaseCvar(std::string name, unsigned int flags, void (*callback)(BaseCvar &));
	//virtual ~BaseCvar();

	inline void Callback() { if(m_Callback) m_Callback(*this); }

	inline std::string GetName() const { return Name; }
	inline int GetFlags() const { return Flags; }

	void ResetToDefault();

	void SetRep(CVarValue_u value, eCvarType type);

	virtual CVarValue_u Get(eCvarType type) const = 0;
	virtual CVarValue_u GetActual(eCvarType *type) const = 0;
	virtual CVarValue_u GetDefault(eCvarType type) const = 0;
	virtual CVarValue_u GetActualDefault(eCvarType *type) const = 0;

	virtual void SetDefault(CVarValue_u value, eCvarType type) = 0;

	virtual eCvarType GetRealType() const = 0;

	// todo: on enabling callbacks, have CvarManager cycle through cvars and run callbacks
	static void EnableCallbacks() { m_UseCallback = true; }
	static void DisableCallbacks() { m_UseCallback = false; }

	static bool ToBool(CVarValue_u value, eCvarType type);
	static int ToInt(CVarValue_u value, eCvarType type);
	static float ToFloat(CVarValue_u value, eCvarType type);
	static std::string ToString(CVarValue_u value, eCvarType type);

	static CVarValue_u FromBool(bool value, eCvarType type);
	static CVarValue_u FromInt(int value, eCvarType type);
	static CVarValue_u FromFloat(float value, eCvarType type);
	static CVarValue_u FromString(const char *value, eCvarType type);

	static bool UsingCallbacks() { return m_UseCallback; }

	virtual std::string GetDescription() const { return desc; }

	unsigned int Flags;

protected:
	BaseCvar() { DisableCallbacks(); }

	std::string Name;
	std::string desc;
	eCvarType Type;

	virtual void SetValue(CVarValue_u value, eCvarType type) = 0;

private:
	BaseCvar(const BaseCvar &var);
	BaseCvar(const char *name, unsigned int flags);

	static bool m_UseCallback;

	void (*m_Callback)(BaseCvar &);
};

//==================
// Bool Cvar
//==================
class BoolCvar : public BaseCvar
{
public:
	BoolCvar(std::string name, bool def, int _min, int _max, unsigned int flags, std::string _desc, void (*callback)(BoolCvar &)=NULL);

	virtual CVarValue_u Get(eCvarType type) const;
	virtual CVarValue_u GetActual (eCvarType *type) const;
	virtual CVarValue_u GetDefault(eCvarType type) const;
	virtual CVarValue_u GetActualDefault(eCvarType *type) const;

	virtual void SetDefault(CVarValue_u value, eCvarType type);

	virtual eCvarType GetRealType() const;

	inline bool operator =(bool boolval) { CVarValue_u val; val.Bool = boolval; SetRep(val, CVAR_Bool); return boolval; }
	inline operator bool() const { return Value; }
	inline bool operator *() const { return Value; }

protected:
	virtual void SetValue(CVarValue_u value, eCvarType type);

	bool Value;
	bool DefaultValue;
};

//==================
// Int Cvar
//==================
class IntCvar : public BaseCvar
{
public:
	IntCvar(std::string name, int def, int _min, int _max, unsigned int flags, std::string _desc, void (*callback)(IntCvar &)=NULL);

	virtual CVarValue_u Get(eCvarType type) const;
	virtual CVarValue_u GetActual (eCvarType *type) const;
	virtual CVarValue_u GetDefault(eCvarType type) const;
	virtual CVarValue_u GetActualDefault(eCvarType *type) const;

	virtual void SetDefault(CVarValue_u value, eCvarType type);

	virtual eCvarType GetRealType() const;

	virtual int GetMinValue() const { return MinValue; }
	virtual int GetMaxValue() const { return MaxValue; }

	int operator =(int intval) { CVarValue_u val; val.Int = intval; SetRep(val, CVAR_Int); return intval; }
	inline operator int() const { return Value; }
	inline int operator *() const { return Value; }

protected:
	virtual void SetValue(CVarValue_u value, eCvarType type);

	int Value;
	int DefaultValue;

	int MinValue;
	int MaxValue;
};

//==================
// Float Cvar
//==================
class FloatCvar : public BaseCvar
{
public:
	FloatCvar(std::string name, float def, float _min, float _max, unsigned int flags, std::string _desc, void (*callback)(FloatCvar &)=NULL);

	virtual CVarValue_u Get(eCvarType type) const;
	virtual CVarValue_u GetActual (eCvarType *type) const;
	virtual CVarValue_u GetDefault(eCvarType type) const;
	virtual CVarValue_u GetActualDefault(eCvarType *type) const;

	virtual void SetDefault(CVarValue_u value, eCvarType type);

	virtual eCvarType GetRealType() const;

	virtual float GetMinValue() const { return MinValue; }
	virtual float GetMaxValue() const { return MaxValue; }

	float operator =(float floatval) { CVarValue_u val; val.Float = floatval; SetRep(val, CVAR_Float); return floatval; }
	inline operator float() const { return Value; }
	inline float operator *() const { return Value; }

protected:
	virtual void SetValue (CVarValue_u value, eCvarType type);

	float Value;
	float DefaultValue;

	float MinValue;
	float MaxValue;
};

//==================
// String Cvar
//==================
class StringCvar : public BaseCvar
{
public:
	StringCvar(std::string name, std::string def, int _min, int _max, unsigned int flags, std::string _desc, void (*callback)(StringCvar &)=NULL);
	//~StringCvar();

	virtual CVarValue_u Get(eCvarType type) const;
	virtual CVarValue_u GetActual (eCvarType *type) const;
	virtual CVarValue_u GetDefault(eCvarType type) const;
	virtual CVarValue_u GetActualDefault(eCvarType *type) const;

	virtual void SetDefault(CVarValue_u value, eCvarType type);

	virtual eCvarType GetRealType() const;

	const char *operator =(const char *stringrep) { CVarValue_u val; val.String = const_cast<char *>(stringrep); SetRep(val, CVAR_String); return stringrep; }
	std::string operator =(std::string stringrep) { char *cstrbuf = new char[stringrep.length() + 1]; strcpy(cstrbuf, stringrep.c_str()); CVarValue_u val; val.String = cstrbuf; SetRep(val, CVAR_String); delete[] cstrbuf; return stringrep; }
	inline operator const char *() const { return Value.c_str(); }
	inline const char *operator *() const { return Value.c_str(); }
	inline operator std::string() const { return Value; }

protected:
	virtual void SetValue(CVarValue_u value, eCvarType type);

	std::string Value;
	std::string DefaultValue;
};

class CvarManager
{
	MAKE_STATIC_CLASS(CvarManager)
public:
	void WriteToConfig(ConfigFile &cfg);
	void ReadCVars(ConfigFile &cfg);
	std::string ReadCvarList();
	std::string ReadDefaultCvarList();

	bool FindCvar(std::string name) { if(CVars.find(name) == CVars.end()) return false; else return true; }
	BaseCvar *GetCvar(std::string name) { if(FindCvar(name)) return CVars[name]; else return nullptr; }
	std::vector<std::string> GetCvarListing() const { return cvarlist; }

	void StoreCvar(std::string name, BaseCvar *var);

private:
	CvarManager() {}
	CvarManager(const CvarManager &) {}
	//CvarManager operator =(const CvarManager &) {}
	~CvarManager() {}

	std::vector<std::string> cvarlist;
	std::map<std::string, BaseCvar *> CVars;
};

#define Cvars CvarManager::GetInstance()

#define CUSTOM_CVAR(type, name, def, minimum, maxiumum, desc, flags) \
	void cvarfunc_##name(##type##Cvar &); \
	type##Cvar name (#name, def, minimum, maxiumum, flags, desc, cvarfunc_##name); \
	void cvarfunc_##name(##type##Cvar &self)

#define CVAR(type, name, def, minimum, maxiumum, desc, flags) \
	type##Cvar name (#name, def, minimum, maxiumum, flags, desc)

#define EXTERN_CVAR(type, name) extern type##Cvar name

#include "externcvar.h"