#pragma once

#include <vector>
#include "Globals.hpp"
#include <glm/glm.hpp>

namespace TCC
{
	typedef bool CellMed;
	class MedecineManager
	{
		CellMed *_buffer1;
		CellMed *_buffer2;
		CellMed *_read;
		CellMed *_write;
		unsigned int _counter = 0;
		glm::vec2 _center;

		unsigned int _width;
		unsigned int _height;
		unsigned int _total;
	public:
		MedecineManager(unsigned int width, unsigned int height)
			: _width(width)
			, _height(height)
			, _total(width * height)
		{
			_buffer1 = new CellMed[_total]();
			_buffer2 = new CellMed[_total]();
			_read = _buffer1;
			_write = _buffer2;
		}
		~MedecineManager()
		{
			delete[] _buffer1;
			delete[] _buffer2;
		}

		unsigned short set(bool tof, int x, int y)
		{
			if (x >= _width || x < 0 || y >= _height || y < 0)
				return 0;
			_write[x + _width * y] = tof;
			return 1;
		}

		void inject()
		{
			if (_counter > 0)
				return;
			memset((void*)_write, false, sizeof(CellMed) * _total);
			auto center = TCC::Position(TCC::mouse_x, TCC::windowHeight - TCC::mouse_y);
			_center = center;
			for (auto i = 0; i < TCC::injectionThickness; ++i)
			{
				auto radius = TCC::injectionRadius + i;

				_counter += set(true, center.x, center.y + radius);
				_counter += set(true, center.x, center.y - radius);
				_counter += set(true, center.x + radius, center.y);
				_counter += set(true, center.x - radius, center.y);

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
					_counter += set(true, center.x + x, center.y + y);
					_counter += set(true, center.x - x, center.y + y);
					_counter += set(true, center.x + x, center.y - y);
					_counter += set(true, center.x - x, center.y - y);
					_counter += set(true, center.x + y, center.y + x);
					_counter += set(true, center.x - y, center.y + x);
					_counter += set(true, center.x + y, center.y - x);
					_counter += set(true, center.x - y, center.y - x);
				}
			}
		}

		bool progress(unsigned int f, glm::uvec2 &p)
		{
			auto from = glm::vec2(f % _width, f / _width);
			auto inidist = from - _center;
			auto to = (from + inidist * 2.0f);
			int DX = to.x - from.y;
			int DY = to.y - from.y;
			int N = 0;
			if (abs(DX) > abs(DY)) N = abs(DX);
			else N = abs(DY);
			float SX = (float)DX / (float)N;
			float SY = (float)DY / (float)N;
			for (int jj = 0; jj < N; jj++)
			{
				from.x += SX;
				from.y += SY;
				p = from;
				if (p.x >= _width || p.x < 0 || p.y >= _height || p.y < 0)
					return false;
				return true;
				//setPixel(PX, PY);
			}
			return true;
		}

		void update()
		{
			std::swap(_read, _write);
			memset((void*)_write, false, sizeof(CellMed) * _total);
			for (unsigned int i = 0; i < _total; ++i)
			{
				if (_read[i])
				{
					glm::uvec2 n;
					if (progress(i, n))
					{
						//TCC::displayBuffer->drawPixel(TCC::Position(n.x, n.y), TCC::Color(123, 23, 122));
						writeBuf->setCell(n.x, n.y, Medecine);
						_write[n.x + n.y * _width] = true;
					}
					else
						--_counter;
				}
			}
		}
	private:
	};
}