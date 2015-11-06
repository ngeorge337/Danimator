#include "libs.h"
#include "util.h"
#include "static.h"
#include "file.h"
#include "configfile.h"
#include "console.h"
#include "ccmd.h"
#include "token.h"
#include "cvar.h"

STATIC_IMPLEMENT(CvarManager)

bool BaseCvar::m_UseCallback = true;

BaseCvar::BaseCvar(std::string name, unsigned int flags, void (*callback)(BaseCvar &))
{
	BaseCvar *var;

	var = Cvars->GetCvar(name);

	m_Callback = callback;
	Flags = 0;
	Name = name;

	if(var != nullptr)
	{
		eCvarType type;
		CVarValue_u value;

		value = var->GetActual(&type);
		SetRep(value, type);

		if (var->Flags & CVAR_AUTO)
		{
			delete var;
		}
		else
			var->~BaseCvar();

		Flags = flags;
	}
	else
	{
		Flags = flags | CVAR_ISDEFAULT;
	}
}

bool BaseCvar::ToBool(CVarValue_u value, eCvarType type)
{
	switch(type)
	{
	case CVAR_Bool:
		return value.Bool;

	case CVAR_Int:
		return !!value.Int;

	case CVAR_Float:
		return value.Float != 0.f;

	case CVAR_String:
		if(CompNoCase(value.String, "true"))
			return true;
		else if(CompNoCase(value.String, "false"))
			return false;
		else
			return !!strtol(value.String, NULL, 0);

	default:
		return false;
	}
}

int BaseCvar::ToInt(CVarValue_u value, eCvarType type)
{
	int res;
	float tmp;

	switch(type)
	{
	case CVAR_Bool:			res = (int)value.Bool; break;
	case CVAR_Int:			res = value.Int; break;
	case CVAR_Float:		tmp = value.Float; res = (int)tmp; break;
	case CVAR_String:		
		{
			if(stricmp(value.String, "true") == 0)
				res = 1;
			else if(stricmp(value.String, "false") == 0)
				res = 0;
			else
				res = strtol(value.String, NULL, 0); 
			break;
		}
	default:				res = 0; break;
	}
	return res;
}

float BaseCvar::ToFloat(CVarValue_u value, eCvarType type)
{
	switch(type)
	{
	case CVAR_Bool:
		return (float)value.Bool;

	case CVAR_Int:
		return (float)value.Int;

	case CVAR_Float:
		return value.Float;

	case CVAR_String:
		return (float) strtod (value.String, NULL);

	default:
		return 0.f;
	}
}

static char truestr[] = "true";
static char falsestr[] = "false";

std::string BaseCvar::ToString(CVarValue_u value, eCvarType type)
{
	std::string ret = "";
	char cstrbuf[40];
	switch(type)
	{
	case CVAR_Bool:
		ret += value.Bool ? truestr : falsestr;
		break;

	case CVAR_String:
		ret += value.String;
		break;

	case CVAR_Int:
		sprintf(cstrbuf, "%i", value.Int);
		ret += cstrbuf;
		break;

	case CVAR_Float:
		sprintf(cstrbuf, "%f", value.Float);
		ret += cstrbuf;
		break;

	default:
		strcpy (cstrbuf, "<Unknown>");
		ret += cstrbuf;
		break;
	}
	return ret;
}

CVarValue_u BaseCvar::FromBool(bool value, eCvarType type)
{
	CVarValue_u ret;

	switch(type)
	{
	case CVAR_Bool:
		ret.Bool = value;
		break;

	case CVAR_Int:
		ret.Int = value;
		break;

	case CVAR_Float:
		ret.Float = value;
		break;

	case CVAR_String:
		ret.String = value ? truestr : falsestr;
		break;

	default:
		break;
	}

	return ret;
}

CVarValue_u BaseCvar::FromInt(int value, eCvarType type)
{
	CVarValue_u ret;
	char cstrbuf[40];

	switch(type)
	{
	case CVAR_Bool:
		ret.Bool = value != 0;
		break;

	case CVAR_Int:
		ret.Int = value;
		break;

	case CVAR_Float:
		ret.Float = (float)value;
		break;

	case CVAR_String:
		sprintf(cstrbuf, "%i", value);
		ret.String = cstrbuf;
		break;

	default:
		break;
	}

	return ret;
}

CVarValue_u BaseCvar::FromFloat(float value, eCvarType type)
{
	CVarValue_u ret;
	char cstrbuf[40];

	switch(type)
	{
	case CVAR_Bool:
		ret.Bool = value != 0.f;
		break;

	case CVAR_Int:
		ret.Int = (int)value;
		break;

	case CVAR_Float:
		ret.Float = value;
		break;

	case CVAR_String:
		sprintf (cstrbuf, "%f", value);
		ret.String = cstrbuf;
		break;

	default:
		break;
	}

	return ret;
}

static BYTE HexToByte(const char *hex)
{
	BYTE v = 0;
	for(int i = 0; i < 2; ++i)
	{
		v <<= 4;
		if (hex[i] >= '0' && hex[i] <= '9')
		{
			v += hex[i] - '0';
		}
		else if (hex[i] >= 'A' && hex[i] <= 'F')
		{
			v += hex[i] - 'A';
		}
		else // The string is already verified to contain valid hexits
		{
			v += hex[i] - 'a';
		}
	}
	return v;
}

CVarValue_u BaseCvar::FromString(const char *value, eCvarType type)
{
	CVarValue_u ret;
	//int i;

	switch(type)
	{
	case CVAR_Bool:
		if(CompNoCase(value, "true"))
			ret.Bool = true;
		else if(CompNoCase(value, "false"))
			ret.Bool = false;
		else
			ret.Bool = strtol(value, NULL, 0) != 0;
		break;

	case CVAR_Int:
		if (stricmp(value, "true") == 0)
			ret.Int = 1;
		else if(stricmp(value, "false") == 0)
			ret.Int = 0;
		else
			ret.Int = strtol(value, NULL, 0);
		break;

	case CVAR_Float:
		ret.Float = (float) strtod(value, NULL);
		break;

	case CVAR_String:
		ret.String = const_cast<char *>(value);
		break;

	default:
		break;
	}

	return ret;
}

void BaseCvar::SetRep(CVarValue_u value, eCvarType type)
{
	SetValue(value, type);

	if (m_UseCallback)
		Callback();

	Flags &= ~CVAR_ISDEFAULT;
}

void BaseCvar::ResetToDefault()
{
	if(!(Flags & CVAR_ISDEFAULT))
	{
		CVarValue_u val;
		eCvarType type;

		val = GetActualDefault(&type);
		SetRep(val, type);
		Flags |= CVAR_ISDEFAULT;
	}
}

//==================
// Bool Cvar
//==================

BoolCvar::BoolCvar(std::string name, bool def, int _min, int _max, unsigned int flags, std::string _desc, void (*callback)(BoolCvar &))
	: BaseCvar(name, flags, reinterpret_cast<void (*)(BaseCvar &)>(callback))
{
	desc = _desc + "\n";
	DefaultValue = def;
	if (Flags & CVAR_ISDEFAULT)
		Value = def;

	if(BaseCvar::UsingCallbacks())
		Callback();

	Cvars->StoreCvar(name, this);
}

eCvarType BoolCvar::GetRealType() const
{
	return CVAR_Bool;
}

CVarValue_u BoolCvar::Get(eCvarType type) const
{
	return FromBool(Value, type);
}

CVarValue_u BoolCvar::GetActual(eCvarType *type) const
{
	CVarValue_u ret;
	*type = CVAR_Bool;
	ret.Bool = Value;
	return ret;
}

CVarValue_u BoolCvar::GetDefault(eCvarType type) const
{
	return FromBool(DefaultValue, type);
}

CVarValue_u BoolCvar::GetActualDefault (eCvarType *type) const
{
	CVarValue_u ret;
	*type = CVAR_Bool;
	ret.Bool = DefaultValue;
	return ret;
}

void BoolCvar::SetDefault(CVarValue_u value, eCvarType type)
{
	DefaultValue = ToBool(value, type);
	if(Flags & CVAR_ISDEFAULT)
	{
		SetRep(value, type);
		Flags |= CVAR_ISDEFAULT;
	}
}

void BoolCvar::SetValue(CVarValue_u value, eCvarType type)
{
	Value = ToBool(value, type);
}

//==================
// Int Cvar
//==================
IntCvar::IntCvar(std::string name, int def, int _min, int _max, unsigned int flags, std::string _desc,  void (*callback)(IntCvar &))
	: BaseCvar(name, flags, reinterpret_cast<void (*)(BaseCvar &)>(callback))
{
	assert(!(_min >= _max && _min != NULL && _max != NULL));	// Minimum value can't be higher than maximum
	desc = _desc + "\n";
	MinValue = _min;
	MaxValue = _max;
	if(MinValue != 0 && MaxValue != 0) ClampTo(def, MinValue, MaxValue);
	DefaultValue = def;
	if(Flags & CVAR_ISDEFAULT)
		Value = def;

	if(BaseCvar::UsingCallbacks())
		Callback();

	Cvars->StoreCvar(name, this);
}

eCvarType IntCvar::GetRealType() const
{
	return CVAR_Int;
}

CVarValue_u IntCvar::Get(eCvarType type) const
{
	return FromInt(Value, type);
}

CVarValue_u IntCvar::GetActual(eCvarType *type) const
{
	CVarValue_u ret;
	*type = CVAR_Int;
	ret.Int = Value;
	return ret;
}

CVarValue_u IntCvar::GetDefault(eCvarType type) const
{
	return FromInt(DefaultValue, type);
}

CVarValue_u IntCvar::GetActualDefault (eCvarType *type) const
{
	CVarValue_u ret;
	*type = CVAR_Int;
	ret.Int = DefaultValue;
	return ret;
}

void IntCvar::SetDefault(CVarValue_u value, eCvarType type)
{
	DefaultValue = ToInt(value, type);
	if(Flags & CVAR_ISDEFAULT)
	{
		SetRep(value, type);
		Flags |= CVAR_ISDEFAULT;
	}
}

void IntCvar::SetValue(CVarValue_u value, eCvarType type)
{
	if(MinValue != 0 && MaxValue != 0)
		Value = Clamp(ToInt(value, type), MinValue, MaxValue);
	else Value = ToInt(value, type);
}

//==================
// Float Cvar
//==================
FloatCvar::FloatCvar(std::string name, float def, float _min, float _max, unsigned int flags, std::string _desc, void (*callback)(FloatCvar &))
	: BaseCvar(name, flags, reinterpret_cast<void (*)(BaseCvar &)>(callback))
{
	assert(!(_min >= _max && _min != NULL && _max != NULL));	// Minimum value can't be higher than maximum
	desc = _desc + "\n";
	MinValue = _min;
	MaxValue = _max;
	if(MinValue != 0 && MaxValue != 0) ClampTo(def, MinValue, MaxValue);
	DefaultValue = def;
	if (Flags & CVAR_ISDEFAULT)
		Value = def;

	if(BaseCvar::UsingCallbacks())
		Callback();

	Cvars->StoreCvar(name, this);
}

eCvarType FloatCvar::GetRealType() const
{
	return CVAR_Float;
}

CVarValue_u FloatCvar::Get(eCvarType type) const
{
	return FromFloat(Value, type);
}

CVarValue_u FloatCvar::GetActual(eCvarType *type) const
{
	CVarValue_u ret;
	*type = CVAR_Float;
	ret.Float = Value;
	return ret;
}

CVarValue_u FloatCvar::GetDefault(eCvarType type) const
{
	return FromFloat(DefaultValue, type);
}

CVarValue_u FloatCvar::GetActualDefault (eCvarType *type) const
{
	CVarValue_u ret;
	*type = CVAR_Float;
	ret.Float = DefaultValue;
	return ret;
}

void FloatCvar::SetDefault(CVarValue_u value, eCvarType type)
{
	DefaultValue = ToFloat(value, type);
	if(Flags & CVAR_ISDEFAULT)
	{
		SetRep(value, type);
		Flags |= CVAR_ISDEFAULT;
	}
}

void FloatCvar::SetValue(CVarValue_u value, eCvarType type)
{
	if(MinValue != 0 && MaxValue != 0)
		Value = Clamp(ToFloat(value, type), MinValue, MaxValue);
	else Value = ToFloat(value, type);
}

//==================
// String Cvar
//==================
StringCvar::StringCvar(std::string name, std::string def, int _min, int _max, unsigned int flags, std::string _desc, void (*callback)(StringCvar &))
	: BaseCvar(name, flags, reinterpret_cast<void (*)(BaseCvar &)>(callback))
{
	desc = _desc + "\n";
	DefaultValue = def;
	if (Flags & CVAR_ISDEFAULT)
		Value = def;

	if(BaseCvar::UsingCallbacks())
		Callback();

	Cvars->StoreCvar(name, this);
}

eCvarType StringCvar::GetRealType() const
{
	return CVAR_String;
}

CVarValue_u StringCvar::Get(eCvarType type) const
{
	return FromString(Value.c_str(), type);
}

CVarValue_u StringCvar::GetActual(eCvarType *type) const
{
	CVarValue_u ret = { 0 };
	*type = CVAR_String;
	strcpy(ret.String, Value.c_str());
	//ret.String = Value.c_str();
	return ret;
}

CVarValue_u StringCvar::GetDefault(eCvarType type) const
{
	return FromString(DefaultValue.c_str(), type);
}

CVarValue_u StringCvar::GetActualDefault (eCvarType *type) const
{
	CVarValue_u ret = { 0 };
	*type = CVAR_String;
	strcpy(ret.String, DefaultValue.c_str());
	//ret.String = DefaultValue;
	return ret;
}

void StringCvar::SetDefault(CVarValue_u value, eCvarType type)
{
	DefaultValue = ToString(value, type);
	if(Flags & CVAR_ISDEFAULT)
	{
		SetRep(value, type);
		Flags |= CVAR_ISDEFAULT;
	}
}

void StringCvar::SetValue(CVarValue_u value, eCvarType type)
{
	Value = ToString(value, type);
}

//====================================
// Cvar Manager implementation
//====================================
void CvarManager::StoreCvar(std::string name, BaseCvar *var)
{
	if(FindCvar(name))
	{
		Console->LogMessage("Tried to register Cvar '%s' more than once", name);
		return;
	}

	cvarlist.push_back(name);
	CVars.insert(std::pair<std::string, BaseCvar *>(name, var));
}

void CvarManager::WriteToConfig(ConfigFile &cfg)
{
	BaseCvar *var;
	cfg.Clear();
	if(!cvarlist.empty())
	{
		for(int i = 0; i < cvarlist.size(); i++)
		{
			var = CVars[cvarlist.at(i)];
			if(var->Flags & CVAR_ARCHIVE)
			{
				cfg.Add(cvarlist.at(i) + " ");
				cfg.Add(var->ToString(var->Get(var->GetRealType()), var->GetRealType()));
				cfg.Add("\n");
			}
		}
	}
}

std::string CvarManager::ReadCvarList()
{
	BaseCvar *var;
	std::string ret = "";
	if(!cvarlist.empty())
	{
		for(int i = 0; i < cvarlist.size(); i++)
		{
			var = CVars[cvarlist.at(i)];
			if(var->Flags & CVAR_ARCHIVE)
			{
				ret += cvarlist.at(i) + " ";
				ret += var->ToString(var->Get(var->GetRealType()), var->GetRealType());
				ret +=  "\n";
			}
		}
	}
	return ret;
}

std::string CvarManager::ReadDefaultCvarList()
{
	BaseCvar *var;
	std::string ret = "";
	if(!cvarlist.empty())
	{
		for(int i = 0; i < cvarlist.size(); i++)
		{
			var = CVars[cvarlist.at(i)];
			if(var->Flags & CVAR_ARCHIVE)
			{
				ret += cvarlist.at(i) + " ";
				ret += var->ToString(var->GetDefault(var->GetRealType()), var->GetRealType());
				ret +=  "\n";
			}
		}
	}
	return ret;
}

void CvarManager::ReadCVars(ConfigFile &cfg)
{
	Parser parser;
	BaseCvar *var;
	std::string tok;

	if(!cfg.IsOpen())
		return;

	parser.OpenString(cfg.GetConfig());

	while(parser.GetState() == PS_GOOD)
	{
		tok = parser.GetToken();
		if(FindCvar(tok))
		{
			var = GetCvar(tok);
			switch(var->GetRealType())
			{
			case CVAR_Bool:
				if(CompNoCase(parser.PeekToken(), "true") || CompNoCase(parser.PeekToken(), "false"))
					var->SetRep(var->FromString(parser.GetToken().c_str(), var->GetRealType()), var->GetRealType());
				else
					var->SetRep(var->FromBool(parser.GetBool(), var->GetRealType()), var->GetRealType());
				break;

			case CVAR_Int:
				var->SetRep(var->FromInt(parser.GetInteger(), var->GetRealType()), var->GetRealType());
				break;

			case CVAR_Float:
				var->SetRep(var->FromFloat(parser.GetFloat(), var->GetRealType()), var->GetRealType());
				break;

			case CVAR_String:
				var->SetRep(var->FromString(parser.GetToken().c_str(), var->GetRealType()), var->GetRealType());
				break;
			}
		}
		else if(CCMDMan->FindCCMD(tok))
		{
			parser.SkipLine();
		}
		else
		{
			if(!parser.IsEOF())
			{
				Console->LogMessage("Unknown Cvar '%s'", tok);
				parser.SkipLine();
			}
		}
	}
}