#pragma once

class Serializable
{
public:
	virtual void Serialize(File &file) = 0;
	virtual void Deserialize(File &file) = 0;
};