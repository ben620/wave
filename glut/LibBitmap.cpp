#include "LibBitmap.h"
#include <stdio.h>
#include <stdint.h>

LibBitmap::LibBitmap()
{
}


LibBitmap::~LibBitmap()
{
	Clear();
}

void LibBitmap::Clear()
{
	if (_data != nullptr)
	{
		delete[] _data;
		_data = nullptr;
		_width = 0;
		_height = 0;
	}
}

#pragma pack(push)
#pragma pack(2)

struct BitmapFileHeader 
{
	std::uint16_t    bfType;
	std::uint32_t    bfSize;
	std::uint16_t    bfReserved1;
	std::uint16_t    bfReserved2;
	std::uint32_t    bfOffBits;
};

struct BitmapInfoHeader 
{
	std::uint32_t      biSize;
	std::int32_t       biWidth;
	std::int32_t       biHeight;
	std::uint16_t      biPlanes;
	std::uint16_t      biBitCount;
	std::uint32_t      biCompression;
	std::uint32_t      biSizeImage;
	std::int32_t       biXPelsPerMeter;
	std::int32_t       biYPelsPerMeter;
	std::uint32_t      biClrUsed;
	std::uint32_t      biClrImportant;
};

#pragma pack(pop)

bool LibBitmap::ReadFile(const char *path)
{
	FILE *file = fopen(path, "rb");
	if (file == nullptr)
	{
		return false;
	}

	BitmapFileHeader bmpFH;
	fread(&bmpFH, sizeof(BitmapFileHeader), 1, file);
	BitmapInfoHeader bmpIH;
	fread(&bmpIH, sizeof(BitmapInfoHeader), 1, file);

	_width = bmpIH.biWidth;
	_height = bmpIH.biHeight;

	const int paddingRowSize = ((_width * 3 + 3) >> 2) << 2;
	const int actualRowSize = _width * 3;
	_data = new unsigned char[_width * _height * 3];
	int paddingData = 0;
	for (int ii = 0; ii < _height; ++ii)
	{
		fread(&_data[(_height - 1 - ii) * actualRowSize], actualRowSize, 1, file);
		fread(&paddingData, 1, paddingRowSize - actualRowSize, file);
	}

	fclose(file);
	return true;
}


void LibBitmap::CreateEmpty(int width, int height)
{
	_width = width;
	_height = height;
	_data = new unsigned char[width * height * 3];
}

bool LibBitmap::WriteFile(const char *name)
{
	if (_data == nullptr)
	{
		return false;
	}
	FILE *file = fopen(name, "wb");
	if (file == nullptr)
	{
		return false;
	}

	const int rowSize = ((_width * 3 + 3) >> 2) << 2;
	{
		BitmapFileHeader bmpFH;
		bmpFH.bfType = 0x4D42;
		bmpFH.bfReserved1 = 0;
		bmpFH.bfReserved2 = 0;
		bmpFH.bfOffBits = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
		bmpFH.bfSize = bmpFH.bfOffBits + rowSize * _height;
		fwrite(&bmpFH, sizeof(BitmapFileHeader), 1, file);
	}
	{
		BitmapInfoHeader bmpIH;
		bmpIH.biSize = sizeof(BitmapInfoHeader);
		bmpIH.biWidth = _width;
		bmpIH.biHeight = _height;
		bmpIH.biPlanes = 1;
		bmpIH.biBitCount = 24;
		bmpIH.biCompression = 0;
		bmpIH.biSizeImage = 0;
		bmpIH.biXPelsPerMeter = 0;
		bmpIH.biYPelsPerMeter = 0;
		bmpIH.biClrUsed = 0;
		bmpIH.biClrImportant = 0;
		fwrite(&bmpIH, sizeof(BitmapInfoHeader), 1, file);
	}
	const int origRowSize = _width * 3;
	const int padingSize = rowSize - origRowSize;
	const int padding = 0;
	for (int ii = _height - 1; ii >= 0; --ii)
	{
		fwrite(&_data[ii * origRowSize], origRowSize, 1, file);
		fwrite(&padding, 1, padingSize, file);
	}

	fclose(file);
	return true;
}