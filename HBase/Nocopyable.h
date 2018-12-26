#pragma once 

class Nocopyable
{
public:
	Nocopyable(const Nocopyable&) = delete;
	void operator=(const Nocopyable&) = delete;

protected:
	Nocopyable() = default;
	~Nocopyable() = default;
};
