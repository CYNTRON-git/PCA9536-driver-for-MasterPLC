#pragma once
class BaseDevice
{
public:
	BaseDevice();
	virtual ~BaseDevice();
};


class IPDevice : public BaseDevice
{
	// IP порт 
	unsigned short  _Port;
public:
	unsigned short  Port(void) 
	{
		return _Port;
	}
	void SetPort(unsigned short Port)
	{
		_Port = Port;
	}

	MPLCSHARE_API IPDevice() : _Port(0)
	{}
	MPLCSHARE_API virtual ~IPDevice()
	{}
};




