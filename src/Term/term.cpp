#include "term.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <cstring>

/* source : https://stackoverflow.com/a/43924525 */
#define KEY_CTRL(c) ((c) & 0x1f)

namespace term
{
	void init()
	{
		initscr();
		noecho();
	}

	void set_interactive(bool interactive)
	{
		if (interactive)
		{
			raw();
			nonl();
			keypad(stdscr, true);
			nodelay(stdscr, true);
		}
		else
		{
			cbreak();
			nl();
			keypad(stdscr, false);
			nodelay(stdscr, false);
		}
	}

	void fini() { endwin(); }

	void screen_render() {}

	window::window(std::string name, int x, int y, unsigned width, unsigned height,
				   unsigned padding_x, unsigned padding_y, bool maximized,
				   bool bordered)
		: name(name), x(x), y(y), width(width), height(height),
		  padding_x(padding_x), padding_y(padding_y), maximized(maximized),
		  bordered(bordered)
	{
		assert(width > 0 && height > 0);
	}

	void window::move_cursor_to(size_t cell)
	{
		size_t tmp_width;
		size_t tmp_height;

		tmp_width = width - padding_x * 2;
		tmp_height = height - padding_y * 2;

		cell = std::min(cell, tmp_width * tmp_height - 1);
		cursor_x = unsigned(padding_x + cell % tmp_width);
		cursor_y = unsigned(padding_y + cell / tmp_width);
	}

	void window::move_cursor_to(unsigned x, unsigned y)
	{
		x = std::min(x, width - 1);
		y = std::min(y, height - 1);

		this->cursor_x = x;
		this->cursor_y = y;
	}

	void window::move_cursor_by(ptrdiff_t cells)
	{
		ptrdiff_t tmp_width;
		ptrdiff_t tmp_height;
		ptrdiff_t tmp_curx;
		ptrdiff_t tmp_cury;
		size_t i;

		if (padding_x >= width / 2 || padding_y >= height / 2)
		{
			return;
		}

		tmp_width = width - padding_x * 2;
		tmp_height = height - padding_y * 2;
		tmp_curx = cursor_x - padding_x;
		tmp_cury = cursor_y - padding_y;

		i = size_t(std::clamp(tmp_cury * tmp_width + tmp_curx + cells, ptrdiff_t(0),
							  tmp_width * tmp_height - 1));

		cursor_x = unsigned(padding_x + i % tmp_width);
		cursor_y = unsigned(padding_y + i / tmp_width);
	}

	void window::clear() { buffer.clear(); }

	void window::print(std::string const &s, attr_t attrs, short color_pair)
	{
		for (char ch : s)
		{
			buffer.push_back(cell(ch, attrs, color_pair));
		}
		move_cursor_by(s.size());
	}

	screen::screen()
	{
		int w;
		int h;

		getmaxyx(stdscr, h, w);
		width = w;
		height = h;
		buffer = new cell[width * height];
	}

	screen::~screen() { delete[] buffer; }

	bool screen::update()
	{
		int ch;
		int w;
		int h;

		getmaxyx(stdscr, h, w);

		if (w != width || h != height)
		{
			delete[] buffer;

			width = w;
			height = h;

			buffer = new cell[width * height];
		}

		ch = getch();

		switch (ch)
		{
		default:
			break;
		case 'q':
		case KEY_CTRL('c'):
			return false;
		}

		return true;
	}

	static cell render_cell(window const &win, int x, int y)
	{
		chtype const corners[4] = {ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER,
								   ACS_LRCORNER};

		std::vector<cell> win_buffer;

		size_t i;

		int left;
		int top;
		int right;
		int bottom;
		int offset;

		win_buffer = win.get_buffer();

		left = int(win.get_position_x());
		top = int(win.get_position_y());
		right = left + int(win.get_width()) - 1;
		bottom = top + int(win.get_height()) - 1;

		offset = win.is_bordered();

		if (offset > 0)
		{
			bool cond_horiz;
			bool cond_vert;

			cond_horiz = y < top || y > bottom;
			cond_vert = x < left || x > right;

			if (cond_horiz)
			{
				if (cond_vert)
				{
					return cell(corners[(x > right) | ((y > bottom) << 1)]);
				}
				return cell(ACS_HLINE);
			}
			else if (cond_vert)
			{
				return cell(ACS_VLINE);
			}
		}

		i = (y - top) * win.get_width() + (x - left);
		if (i < win_buffer.size())
		{
			return win_buffer[i];
		}
		else
		{
			return cell(' ');
		}
	}

	void screen::render()
	{
		size_t i;

		std::fill_n(buffer, width * height, cell(' '));

		for (window const &win : window_list)
		{
			int left = int(win.get_position_x());
			int top = int(win.get_position_y());
			int right = left + int(win.get_width()) - 1;
			int bottom = top + int(win.get_height()) - 1;

			int offset = win.is_bordered();

			for (int y = std::max(top - offset, 0);
				y <= std::min(bottom + offset, int(height) - 1);
				++y)
			{
				for (int x = std::max(left - offset, 0);
					x <= std::min(right + offset, int(width) - 1);
					++x)
				{
					buffer[y * width + x] = render_cell(win, x, y);
				}
			}
		}

		move(0, 0);
		for (i = 0; i < width * height; ++i)
			addch(buffer[i]);

		refresh();
	}
} // namespace term
