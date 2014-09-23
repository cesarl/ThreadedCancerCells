#pragma once

#include "Cell.hpp"
#include <algorithm>
#include <cassert>
#include "Display.hpp"
#include "Globals.hpp"

namespace TCC
{
	class GridBuffer
	{
		CellType *_buffer1;
		CellType *_buffer2;
		CellType *_read;
		CellType *_write;
		int _width;
		int _height;
		int _total;
	public:
		GridBuffer(int width, int height)
			: _width(width)
			, _height(height)
			, _total(width * height)
		{
			_buffer1 = new CellType[_total];
			_buffer2 = new CellType[_total];
			memset(_buffer1, None, sizeof(CellType) * _total);
			memset(_buffer2, None, sizeof(CellType) * _total);
			_read = _buffer1;
			_write = _buffer2;
		}
		~GridBuffer()
		{
			delete[] _buffer1;
			delete[] _buffer2;
		}

		void swap()
		{
			auto so = sizeof(CellType);
			memcpy(_read, _write, _total * so);
			std::swap(_read, _write);
		}

		void fill(CellType t)
		{
			TCC::Counter[t] += _total;
			memset((void*)_write, (int)t, sizeof(CellType) * _total);
		}

		bool isCell(int x, int y, CellType t)
		{
			if (x >= _width || y >= _height)
				return false;
			return _read[y * _width + x] == t;
		}

		void setCell(int x, int y, CellType t)
		{
			--TCC::Counter[_read[y * _width + x]];
			++TCC::Counter[t];
			_write[y * _width + x] = t;
		}

		void setCellWithRule(int x, int y, CellType t)
		{
			if (x >= _width || y >= _height || x < 0 || y < 0)
				return;
			if (t == Medecine && _read[y * _width + x] == Cancer)
			{
				--TCC::Counter[Cancer];
				++TCC::Counter[Healthy];
				_write[y * _width + x] = Healthy;
				return;
			}
			--TCC::Counter[_read[y * _width + x]];
			++TCC::Counter[t];
			_write[y * _width + x] = t;
		}

		void setCell(int i, CellType t)
		{
			_write[i] = t;
			--TCC::Counter[_read[i]];
			++TCC::Counter[t];
		}

		void fillDisplay(Display &display)
		{
			auto so = sizeof(unsigned char);
			for (unsigned int i = 0; i < _total; ++i)
			{
				memcpy((void*)(&display._buffer[i * 3]), (void*)(&Style[(std::size_t)(_write[i] * 3)]), so * 3);
			}
		}

		bool hasNeighbour(CellType type, int x, int y)
		{
			auto yw = 0;

			if (y > 0)
			{
				yw = ((y - 1) * _width) + x - 1;
				if (x > 0 && _read[yw] == type)
					return true;
				if (_read[++yw] == type)
					return true;
				if (x < _width - 1 && _read[++yw] == type)
					return true;
			}
			yw = ((y) * _width) + x - 1;
			if (x > 0 && _read[yw] == type)
				return true;
			if (x < _width - 1 && _read[yw + 2] == type)
				return true;
			if (y < _height - 1)
			{
				yw = ((y + 1) * _width) + x - 1;
				if (x > 0 && _read[yw] == type)
					return true;
				if (_read[++yw] == type)
					return true;
				if (x < _width - 1 && _read[++yw] == type)
					return true;
			}
			return false;
		}

		bool countNeighbours(CellType type, const int x, const int y, unsigned short min)
		{
			short t = 8 - min + 1;
			auto yw = 0;

			yw = ((y - 1) * _width) + x - 1;
			t -= _read[yw] != type;
			if (t <= 0) return false;			
			t -= _read[++yw] != type;
			if (t <= 0) return false;
			t -= _read[++yw] != type;
			if (t <= 0) return false;
			yw = ((y) * _width) + x - 1;
			t -= _read[yw] != type;
			if (t <= 0) return false;			
			t -= _read[yw + 2] != type;
			if (t <= 0) return false;
			yw = ((y + 1)* _width) + x - 1;
			t -= _read[yw] != type;
			if (t <= 0) return false;			
			t -= _read[++yw] != type;
			if (t <= 0) return false;			
			t -= _read[++yw] != type;
			if (t <= 0) return false;			
			return true;
		}

		void computeCancer(const int x, const int y)
		{
			auto a = 0;
			auto index = y * _width + x;
			// H -> C
			auto &r = _read[index];
			if (r == Healthy && !(y == 0 || y == _height - 1 || x == 0 || x == _width - 1) && countNeighbours(Cancer, x, y, 6))
			{
				--TCC::Counter[r];
				++TCC::Counter[Cancer];
				_write[index] = Cancer;
				return;
			}
			// C -> H
			if (r == Cancer && !(y == 0 || y == _height - 1 || x == 0 || x == _width - 1) && countNeighbours(Medecine, x, y, 6))
			{
				auto yw = 0;

				yw = ((y - 1) * _width) + x - 1;

				_write[yw] = _read[yw] == Medecine ? Healthy : _read[yw];
				--TCC::Counter[_read[yw]];
				++TCC::Counter[_write[yw]];

				++yw;
				_write[yw] = _read[yw] == Medecine ? Healthy : _read[yw];
				--TCC::Counter[_read[yw]];
				++TCC::Counter[_write[yw]];

				++yw;
				_write[yw] = _read[yw] == Medecine ? Healthy : _read[yw];
				--TCC::Counter[_read[yw]];
				++TCC::Counter[_write[yw]];

				yw = ((y)* _width) + x - 1;

				_write[yw] = _read[yw] == Medecine ? Healthy : _read[yw];
				--TCC::Counter[_read[yw]];
				++TCC::Counter[_write[yw]];

				++yw;
				_write[yw] = Healthy;
				--TCC::Counter[_read[yw]];
				++TCC::Counter[_write[yw]];

				++yw;
				_write[yw] = _read[yw] == Medecine ? Healthy : _read[yw];
				--TCC::Counter[_read[yw]];
				++TCC::Counter[_write[yw]];

				yw = ((y + 1)* _width) + x - 1;

				_write[yw] = _read[yw] == Medecine ? Healthy : _read[yw];
				--TCC::Counter[_read[yw]];
				++TCC::Counter[_write[yw]];

				++yw;
				_write[yw] = _read[yw] == Medecine ? Healthy : _read[yw];
				--TCC::Counter[_read[yw]];
				++TCC::Counter[_write[yw]];

				++yw;
				_write[yw] = _read[yw] == Medecine ? Healthy : _read[yw];
				--TCC::Counter[_read[yw]];
				++TCC::Counter[_write[yw]];

				return;
			}
			// E -> M
			if (r == None && hasNeighbour(Medecine, x, y))
			{
				_write[index] = Medecine;
				return;
			}
		}

		void inject(unsigned int cx, unsigned int cy)
		{
			auto center = TCC::Position(cx, cy);
			if (TCC::injectionRadius == 1 && TCC::injectionThickness == 1)
			{
				setCellWithRule(center.x, center.y, Medecine);
				return;
			}
			for (auto i = 0; i < TCC::injectionThickness; ++i)
			{
				auto radius = TCC::injectionRadius + i;

				setCellWithRule(center.x, center.y + radius, Medecine);
				setCellWithRule(center.x, center.y - radius, Medecine);
				setCellWithRule(center.x + radius, center.y, Medecine);
				setCellWithRule(center.x - radius, center.y, Medecine);

				int f = 1 - radius;
				int fx = 0;
				int fy = -2 * radius;
				int x = 0;
				int y = radius;

				while (x < y)
				{
					if (f >= 0)
					{
						--y;
						fy += 2;
						f += fy;
					}
					++x;
					fx += 2;
					f += fx + 1;
					setCellWithRule(center.x + x, center.y + y, Medecine);
					setCellWithRule(center.x - x, center.y + y, Medecine);
					setCellWithRule(center.x + x, center.y - y, Medecine);
					setCellWithRule(center.x - x, center.y - y, Medecine);
					setCellWithRule(center.x + y, center.y + x, Medecine);
					setCellWithRule(center.x - y, center.y + x, Medecine);
					setCellWithRule(center.x + y, center.y - x, Medecine);
					setCellWithRule(center.x - y, center.y - x, Medecine);
				}
			}
		}
		void randomFill()
		{
			fill(None);

			{
				unsigned int goal = _total * TCC::cancerPercent / 100;
				unsigned int ratio = 0;
				unsigned int x, y;
				auto t = Cancer;
				while (ratio < goal)
				{
					x = rand() % _width;
					y = rand() % _height;

					if (_write[y * _width + x] != t)
					{
						setCell(x, y, t);
						++ratio;
					}
				}
			}
			{
				unsigned int goal = _total * TCC::healthyPercent / 100;
				unsigned int ratio = 0;
				unsigned int x, y;
				auto t = Healthy;
				while (ratio < goal)
				{
					x = rand() % _width;
					y = rand() % _height;

					if (_write[y * _width + x] != t)
					{
						setCell(x, y, t);
						++ratio;
					}
				}
			}
			swap();
		}
	};
}