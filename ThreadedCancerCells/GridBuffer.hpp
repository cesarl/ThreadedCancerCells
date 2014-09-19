#pragma once

#include "Cell.hpp"
#include <algorithm>
#include <cassert>
#include "Display.hpp"

namespace TCC
{
	class GridBuffer
	{
		Cell *_buffer;
		unsigned int _width;
		unsigned int _height;
		unsigned int _total;
	public:
		GridBuffer(unsigned int width, unsigned int height)
			: _width(width)
			, _height(height)
			, _total(width * height)
		{
			_buffer = new Cell[_total];
		}
		~GridBuffer()
		{
			delete[] _buffer;
		}

		void fill(CellType t)
		{
			memset((void*)_buffer, (int)t, sizeof(Cell) * _total);
		}

		bool isCell(unsigned int x, unsigned int y, CellType t)
		{
			if (x >= _width || y >= _height)
				return false;
			return _buffer[y * _width + x].type == t;
		}

		void setCell(unsigned int x, unsigned int y, CellType t)
		{
			_buffer[y * _width + x].type = t;
		}

		void setCell(unsigned int i, CellType t)
		{
			_buffer[i].type = t;
		}

		void fillDisplay(Display &display)
		{
			auto so = sizeof(unsigned char);
			for (unsigned int i = 0; i < _total; ++i)
			{
				memcpy((void*)(&display._buffer[i * 3]), (void*)(&Style[(std::size_t)(_buffer[i].type * 3)]), so * 3);
			}
		}

		CellType computeCancer(unsigned int x, unsigned int y)
		{
			auto index = y * _width + x;
			if (_buffer[index].type != Healthy || y == 0 || y == _height - 1 || x == 0 || x == _width - 1)
				return _buffer[index].type;
			unsigned short t = 0;
			auto yw = 0;
			yw = ((y - 1) * _width) + x - 1;
			t += _buffer[yw].type == Cancer;
			t += _buffer[++yw].type == Cancer;
			t += _buffer[++yw].type == Cancer;
			yw = ((y) * _width) + x - 1;
			t += _buffer[yw].type == Cancer;
			t += _buffer[yw + 2].type == Cancer;
			yw = ((y + 1)* _width) + x - 1;
			t += _buffer[yw].type == Cancer;
			t += _buffer[++yw].type == Cancer;
			t += _buffer[++yw].type == Cancer;
			if (t >= 6)
				return Cancer;
			return Healthy;
		}

		void randomFill(CellType t1, unsigned int percent, CellType tbg)
		{
			unsigned int p;
			CellType t;
			if (percent > 50)
			{
				fill(t1);
				p = 100 - percent;
				t = tbg;
			}
			else
			{
				fill(tbg);
				p = percent;
				t = t1;
			}
			unsigned int goal = _total * p / 100;
			unsigned int ratio = 0;
			unsigned int x, y;
			while (ratio < goal)
			{
				x = rand() % _width;
				y = rand() % _height;
				if (!isCell(x, y, t))
				{
					setCell(x, y, t);
					++ratio;
				}
			}
		}
	};
}