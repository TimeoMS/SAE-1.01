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

/* formules :
 * https://en.wikipedia.org/wiki/Y%E2%80%B2UV#Conversion_to/from_RGB */

col_rgb::operator col_yuv() const {
	col_yuv yuv;
	double  _r;
	double  _g;
	double  _b;

	_r = double(r), _g = double(g), _b = double(b);

	yuv.y = 0.299 * _r + 0.587 * _g + 0.114 * _b;
	yuv.u = -0.14713 * _r - 0.28886 * _g + 0.436 * _b;
	yuv.v = 0.615 * _r - 0.51499 * _g - 0.10001 * _b;

	return yuv;
}

col_yuv::operator col_rgb() const {
	col_rgb rgb;

	rgb.r = uint8_t(std::round(std::clamp(y + 1.13983 * v, 0.0, 1.0) * 255.0));
	rgb.g = uint8_t(
		std::round(std::clamp(y - 0.39465 * u - 0.5806 * v, 0.0, 1.0) * 255.0));
	rgb.b = uint8_t(std::round(std::clamp(y + 2.03211 * u, 0.0, 1.0) * 255.0));

	return rgb;
}

void cell_renderer::render(std::vector<chtype> &output) {
	std::vector<col_yuv> yuv_colors;

	size_t i;

	auto op_colors    = [](cell &c) { return c.fg; };
	auto cmp_distance = [](col_yuv const &a, col_yuv const &b) {
		return (a.y * a.y + a.u * a.u + a.v * a.v) <
		       (b.y * b.y + b.u * b.u + b.v * b.v);
	};

	yuv_colors.resize(glyph_buf.size());
	std::transform(glyph_buf.begin(), glyph_buf.end(), yuv_colors.begin(),
	               op_colors);
	std::sort(yuv_colors.begin(), yuv_colors.end(), cmp_distance);
	yuv_colors.erase(std::unique(yuv_colors.begin(), yuv_colors.end()),
	                 yuv_colors.end());

	if (yuv_colors.size() > COLORS) {
		col_yuv tmp[COLORS];

		for (i = 0; i < COLORS; ++i) {
			tmp[i] = yuv_colors[size_t(
				std::round(i * yuv_colors.size() / double(COLORS)))];
		}

		yuv_colors = std::vector<col_yuv>(tmp, tmp + COLORS);
	}

	std::vector<std::tuple<short, short, short>> curses_colors;

	auto op_curses = [](col_yuv &yuv) {
		col_rgb rgb = yuv;

		return std::tuple<short, short, short>(
			std::round(1000.0 * rgb.r / 255.0),
			std::round(1000.0 * rgb.g / 255.0),
			std::round(1000.0 * rgb.b / 255.0));
	};

	curses_colors.resize(yuv_colors.size());
	std::transform(yuv_colors.begin(), yuv_colors.end(), curses_colors.begin(),
	               op_curses);

	for (i = 1; i < curses_colors.size(); ++i) {
		std::cerr << std::get<0>(curses_colors[i]) << ' '
				  << std::get<1>(curses_colors[i]) << ' '
				  << std::get<2>(curses_colors[i]) << std::endl;
		init_color(i, std::get<0>(curses_colors[i]),
		           std::get<1>(curses_colors[i]),
		           std::get<2>(curses_colors[i]));
	}
	std::cerr << std::endl;

	for (i = 0; i < curses_colors.size(); ++i) {
		size_t j;
		for (j = 0; j < curses_colors.size(); ++j) {
			init_pair(short(i * curses_colors.size() + j), i, j);
		}
	}

	auto op_cell = [i, &curses_colors, &yuv_colors, &cmp_distance](cell &c) {
		chtype ch;
		short  fg;
		short  bg;

		ch = c.glyph;

		auto find_nearest = [&yuv_colors](col_yuv a) {
			double dist;
			size_t found;
			size_t i;

			for (i = 0; i < yuv_colors.size(); ++i) {
				col_yuv b;
				double  tmp;

				b   = yuv_colors[i];
				tmp = (b.y - a.y) * (b.y - a.y) + (b.u - a.u) * (b.u - a.u) +
				      (b.v - a.v) * (b.v - a.v);

				if (dist < tmp) {
					dist  = tmp;
					found = i;
				}
			}

			return found;
		};

		fg = find_nearest(c.fg);
		bg = find_nearest(c.bg);

		return ch | c.attrs | COLOR_PAIR(bg * curses_colors.size() + fg);
	};

	std::transform(glyph_buf.begin(), glyph_buf.end(), output.begin(), op_cell);
}

window::window(screen const &scr, std::string name, int x, int y,
               unsigned width, unsigned height)
	: scr(scr), name(name), x(x), y(y), width(width), height(height) {
	assert(width > 0 && height > 0);

	fg.rgb = 0xffffff;
	std::cerr << fg.unused << std::endl;
	border_fg = fg;
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

void window::print(std::string const &s, attr_t attrs, col_rgb fg, col_rgb bg) {
	for (char ch : s) {
		glyph_buf.push_back(cell(ch, attrs, fg, bg));
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

	renderer.glyph_buf = std::vector<cell>(w * h, cell(' '));
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
		renderer.glyph_buf = std::vector<cell>(width * height, cell(' '));
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

static cell render_glyph(window const &win, int x, int y, int w, int h) {
	chtype const corners[4] = {ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER,
	                           ACS_LRCORNER};

	std::vector<cell> win_buffer;

	size_t i;

	int pad_x  = win.get_padding_x();
	int pad_y  = win.get_padding_y();
	int border = win.is_bordered();

	int left   = win.get_position_x();
	int top    = win.get_position_y();
	int right  = left + int(win.get_width()) - 1;
	int bottom = top + int(win.get_height()) - 1;

	if (border > 0) {
		bool cond_horiz;
		bool cond_vert;

		int l = left - pad_x;
		int t = top - pad_y;
		int r = right + pad_x;
		int b = bottom + pad_y;

		cond_horiz = y < t || y > b;
		cond_vert  = x < l || x > r;

		if (cond_horiz) {
			if (cond_vert) {
				return cell(corners[(x > r) | ((y > b) << 1)], win.attrs,
				            win.border_fg, win.border_bg);
			}
			return cell(ACS_HLINE, win.attrs, win.border_fg, win.border_bg);
		} else if (cond_vert) {
			return cell(ACS_VLINE, win.attrs, win.border_fg, win.border_bg);
		}
	}

	if (x < left || x > right || y < top || y > bottom)
		return cell(' ', win.attrs, win.fg, win.bg);

	i = (y - top) * (win.get_width() - win.is_bordered() - pad_x) + (x - left);

	win_buffer = win.get_buffer();

	if (i < win_buffer.size()) {
		return win_buffer[i];
	} else {
		return cell(' ', win.attrs, win.fg, win.bg);
	}
}

void screen::render() {
	size_t i;

	std::fill(renderer.glyph_buf.begin(), renderer.glyph_buf.end(), cell(' '));

	dirty_l = std::max(dirty_l, 0u);
	dirty_t = std::max(dirty_t, 0u);
	dirty_r = std::min(dirty_r, width - 1);
	dirty_b = std::min(dirty_b, height - 1);

	for (window const &win : window_list) {
		int l, t, r, b;

		int border = win.is_bordered();
		int pad_x  = win.get_padding_x();
		int pad_y  = win.get_padding_y();

		if (win.is_maximized()) {
			l = dirty_l, t = dirty_t, r = dirty_r, b = dirty_b;
		} else {
			l = std::max(win.get_position_x() - border - pad_x, int(dirty_l));
			t = std::max(win.get_position_y() - border - pad_y, int(dirty_t));
			r = std::min(win.get_position_x() + int(win.get_width()) + border +
			                 pad_x - 1,
			             int(dirty_r));
			b = std::min(win.get_position_y() + int(win.get_height()) + border +
			                 pad_y - 1,
			             int(dirty_b));
		}

		for (int y = t; y <= b; ++y) {
			for (int x = l; x <= r; ++x) {
				renderer.glyph_buf[y * width + x] =
					render_glyph(win, x, y, int(width), int(height));
			}
		}
	}

	std::vector<chtype> output(width * height, ' ');
	renderer.render(output);

	move(0, 0);

	for (i = 0; i < output.size(); ++i) {
		chtype ch;
		ch = output[i];
		addch(ch);
	}

	refresh();
}
} // namespace term
