#pragma once

#include <string>

class LibBitmap
{
public:
	LibBitmap();
	~LibBitmap();

public:
	bool ReadFile(const char *name);
	bool WriteFile(const char *name);
	void Clear();
	int Width() const 
	{
		return _width;
	}
	int Height() const
	{
		return _height;
	}

	unsigned char* Data()
	{
		return _data;
	}

	void FromMemory(int width, int height, unsigned char *data);

private:
	int _width;
	int _height;
	unsigned char *_data;
};

