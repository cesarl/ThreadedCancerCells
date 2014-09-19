#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <GL/gl.h>
#include "Globals.hpp"

namespace TCC
{
	class GridBuffer;
	class Display
	{
	public:
		Display(unsigned int width, unsigned int height)
			: _width(width)
			, _height(height)
			, _buffer(nullptr)
		{
			_buffer = new unsigned char[_width * _height * 3]();
			memset((void*)(_buffer), 0, sizeof(unsigned char) * _width * height * 3);
		}

		~Display()
		{
			if (_buffer)
				delete[] _buffer;
		}

		void fill(const glm::uvec3& color)
		{
			auto so = sizeof(unsigned char) * 3;
			unsigned char c[3] = { color.x, color.y, color.z };
			auto t = _width * _height;
			for (auto i = 0; i < _width; ++i)
			{
				memcpy((void*)(&_buffer[i * 3]), (void*)(&c), so);
			}
			for (auto i = 1; i < _height; ++i)
			{
				memcpy((void*)(&_buffer[i * _width * 3]), (void*)(&_buffer[0]), _width * so);
			}
		}

		void randomFill()
		{
			for (auto i = 0; i < _width; ++i)
			{
				for (auto j = 0; j < _height; ++j)
				{
					drawPixel(glm::uvec2(i, j), glm::uvec3(rand() % 255, rand() % 255, rand() % 255));
				}
			}
		}

		void drawPixel(const glm::uvec2& position, const glm::uvec3& color)
		{
			if (position.x >= _width || position.y >= _height)
				return;
			_buffer[(position.y * (_width * 3)) + position.x * 3] = color.x;
			_buffer[(position.y * (_width * 3)) + position.x * 3 + 1] = color.y;
			_buffer[(position.y * (_width * 3)) + position.x * 3 + 2] = color.z;
		}

		void drawLine(const glm::uvec2& from, const glm::uvec2& to, const glm::uvec3& color)
		{
			auto dist = to - from;
			auto d = 2 * dist.y - dist.x;
			drawPixel(from, color);
			auto y = from.y;

			for (auto x = from.x + 1; x <= to.x; ++x)
			{
				if (d > 0)
				{
					y = y + 1;
					drawPixel(glm::uvec2(x, y), color);
					d = d + (2 * dist.y - 2 * dist.x);
				}
				else
				{
					drawPixel(glm::uvec2(x, y), color);
					d = d + (2 * dist.y);
				}
			}
		}

		void drawCircle(const glm::uvec2& center, unsigned int radius, const glm::uvec3& color)
		{
			drawPixel(glm::uvec2(center.x, center.y + radius), color);
			drawPixel(glm::uvec2(center.x, center.y - radius), color);
			drawPixel(glm::uvec2(center.x + radius, center.y), color);
			drawPixel(glm::uvec2(center.x - radius, center.y), color);

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
				drawPixel(Position(center.x + x, center.y + y), color);
				drawPixel(Position(center.x - x, center.y + y), color);
				drawPixel(Position(center.x + x, center.y - y), color);
				drawPixel(Position(center.x - x, center.y - y), color);
				drawPixel(Position(center.x + y, center.y + x), color);
				drawPixel(Position(center.x - y, center.y + x), color);
				drawPixel(Position(center.x + y, center.y - x), color);
				drawPixel(Position(center.x - y, center.y - x), color);
			}
		}

		void render()
		{
			glDrawPixels(_width, _height, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)_buffer);
		}

		Display() = delete;
		Display(const Display&) = delete;
		Display(Display&&) = delete;
		Display& operator=(const Display&) = delete;
		Display& operator=(Display&&) = delete;
	private:
		unsigned int _width;
		unsigned int _height;
		unsigned char* _buffer;
		friend class GridBuffer;
	};
}