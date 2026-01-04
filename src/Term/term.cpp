#include "term.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

/* source : https://stackoverflow.com/a/43924525 */
#define KEY_CTRL(c) ((c) & 0x1f)

namespace term {
void init() {
	initscr();
	noecho();
}

void set_interactive(bool interactive) {
	if (interactive) {
		raw();
		nonl();
		keypad(stdscr, true);
		nodelay(stdscr, true);
	} else {
		cbreak();
		nl();
		keypad(stdscr, false);
		nodelay(stdscr, false);
	}
}

void fini() { endwin(); }

window::window(screen const &scr, std::string name, int x, int y,
               unsigned width, unsigned height)
	: scr(scr), name(name), x(x), y(y), width(width), height(height) {
	assert(width > 0 && height > 0);
}

void window::move_cursor_to(size_t cell) {
	size_t tmp_width;
	size_t tmp_height;

	tmp_width  = width - padding_x * 2;
	tmp_height = height - padding_y * 2;

	cell     = std::min(cell, tmp_width * tmp_height - 1);
	cursor_x = unsigned(padding_x + cell % tmp_width);
	cursor_y = unsigned(padding_y + cell / tmp_width);
}

void window::move_cursor_to(unsigned x, unsigned y) {
	x = std::min(x, width - 1);
	y = std::min(y, height - 1);

	this->cursor_x = x;
	this->cursor_y = y;
}

void window::move_cursor_by(ptrdiff_t cells) {
	ptrdiff_t tmp_width;
	ptrdiff_t tmp_height;
	ptrdiff_t tmp_curx;
	ptrdiff_t tmp_cury;
	size_t    i;

	if (padding_x >= width / 2 || padding_y >= height / 2) {
		return;
	}

	tmp_width  = width - padding_x * 2;
	tmp_height = height - padding_y * 2;
	tmp_curx   = cursor_x - padding_x;
	tmp_cury   = cursor_y - padding_y;

	i = size_t(std::clamp(tmp_cury * tmp_width + tmp_curx + cells, ptrdiff_t(0),
	                      tmp_width * tmp_height - 1));

	cursor_x = unsigned(padding_x + i % tmp_width);
	cursor_y = unsigned(padding_y + i / tmp_width);
}

void window::clear() { glyph_buf.clear(); }

void window::print(std::string const &s, attr_t attrs, short color_pair) {
	for (char ch : s) {
		glyph_buf.push_back(cell(ch, attrs, color_pair));
	}
	move_cursor_by(s.size());
}

screen::screen() {
	int w;
	int h;

	getmaxyx(stdscr, h, w);
	dirty_l = 0, dirty_t = 0, dirty_r = w - 1, dirty_b = h - 1;

	if ((has_color = has_colors())) {
		start_color();
		use_default_colors();
	}

	width  = w;
	height = h;

	glyph_buf = std::vector<cell>(w * h, cell(' '));
}

unsigned screen::get_width() const {
	int w, h;
	getmaxyx(stdscr, h, w);
	return w;
}

unsigned screen::get_height() const {
	int w, h;
	getmaxyx(stdscr, h, w);
	return h;
}

bool screen::update() {
	int ch;
	int w;
	int h;

	getmaxyx(stdscr, h, w);

	if (w != width || h != height) {
		glyph_buf = std::vector<cell>(width * height, cell(' '));
	}

	ch = getch();

	switch (ch) {
	default:
		break;
	case 'q':
	case KEY_CTRL('c'):
		return false;
	}

	return true;
}

static cell render_glyph(window const &win, int x, int y) {
	chtype const corners[4] = {ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER,
	                           ACS_LRCORNER};

	std::vector<cell> win_buffer;

	size_t i;

	int pad_x  = win.get_padding_x();
	int pad_y  = win.get_padding_y();
	int border = win.is_bordered();

	int left   = win.get_outer_x();
	int top    = win.get_outer_y();
	int right  = left + int(win.get_outer_width()) - 1;
	int bottom = top + int(win.get_outer_height()) - 1;

	if (border > 0) {
		bool cond_horiz;
		bool cond_vert;

		cond_horiz = y <= top || y >= bottom;
		cond_vert  = x <= left || x >= right;

		if (cond_horiz) {
			if (cond_vert) {
				return cell(corners[(x >= right) | ((y >= bottom) << 1)],
				            win.props);
			}
			return cell(ACS_HLINE, win.props);
		} else if (cond_vert) {
			return cell(ACS_VLINE, win.props);
		}
	}

	left   = win.get_inner_x();
	top    = win.get_inner_y();
	right  = win.get_inner_x() + int(win.get_inner_width() - 1);
	bottom = win.get_inner_y() + int(win.get_inner_height() - 1);

	if (x < left || x > right || y < top || y > bottom)
		return cell(' ', win.props);

	i = (y - top) * (win.get_inner_width()) + (x - left);

	win_buffer = win.get_buffer();

	if (i < win_buffer.size()) {
		return win_buffer[i];
	} else {
		return cell(' ', win.props);
	}
}

void screen::render() {
	size_t i;

	std::fill(glyph_buf.begin(), glyph_buf.end(), cell(' '));

	dirty_l = std::max(dirty_l, 0u);
	dirty_t = std::max(dirty_t, 0u);
	dirty_r = std::min(dirty_r, width - 1);
	dirty_b = std::min(dirty_b, height - 1);

	for (window const &win : window_list) {
		int l, t, r, b;
		dirty_l = 0, dirty_t = 0, dirty_r = width - 1, dirty_b = height - 1;

		int border = win.is_bordered();
		int pad_x  = win.get_padding_x();
		int pad_y  = win.get_padding_y();

		if (win.is_maximized()) {
			l = dirty_l, t = dirty_t, r = dirty_r, b = dirty_b;
		} else {
			l = std::max(win.get_outer_x(), dirty_l);
			t = std::max(win.get_outer_y(), dirty_t);
			r = std::min(win.get_outer_x() + int(win.get_outer_width()) - 1,
			             dirty_r);
			b = std::min(win.get_outer_y() + int(win.get_outer_height()) - 1,
			             dirty_b);
		}

		for (int y = t; y <= b; ++y) {
			for (int x = l; x <= r; ++x) {
				glyph_buf[y * width + x] = render_glyph(win, x, y);
			}
		}
	}

	move(0, 0);

	for (i = 0; i < glyph_buf.size(); ++i) {
		chtype ch;
		ch = glyph_buf[i].glyph | glyph_buf[i].attrs |
		     COLOR_PAIR(glyph_buf[i].color_pair);
		addch(ch);
	}

	refresh();
}
} // namespace term
