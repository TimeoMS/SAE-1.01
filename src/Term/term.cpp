#include "term.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

/* source : https://stackoverflow.com/a/43924525 */
#define KEY_CTRL(c) ((c) & 0x1f)

namespace term {
static bool _term_fin = false;

void init() {
	_term_fin = false;
	initscr();
	noecho();

	if (has_colors()) {
		start_color();
		use_default_colors();

		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				init_pair(CPAIR(i, j), i, j);
			}
		}
	}
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

void fini() {
	if (!_term_fin) {
		_term_fin = true;
		set_interactive(false);
		echo();
		endwin();
	}
}

window::window(screen const &scr, std::string name, int x, int y,
               unsigned width, unsigned height)
	: scr(scr), name(name), x(x), y(y), width(width), height(height) {
	assert(width > 0 && height > 0);
}

void window::move_cursor_to(size_t cell) {
	size_t tmp_width;
	size_t tmp_height;

	tmp_width  = get_inner_width();
	tmp_height = get_inner_height();

	cell     = std::min(cell, tmp_width * tmp_height - 1);
	cursor_x = unsigned(cell % tmp_width);
	cursor_y = unsigned(cell / tmp_width);
}

void window::move_cursor_to(unsigned x, unsigned y) {
	x = std::min(x, get_inner_width() - 1);
	y = std::min(y, get_inner_height() - 1);

	this->cursor_x = x;
	this->cursor_y = y;
}

void window::move_cursor_by(ptrdiff_t cells) {
	ptrdiff_t tmp_width;
	ptrdiff_t tmp_height;
	size_t    i;

	tmp_width  = get_inner_width();
	tmp_height = get_inner_height();

	i = size_t(std::clamp(cursor_x * tmp_width + cursor_y + cells, ptrdiff_t(0),
	                      tmp_width * tmp_height - 1));

	cursor_x = unsigned(i % tmp_width);
	cursor_y = unsigned(i / tmp_width);
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

	if (_term_fin) {
		return false;
	}

	getmaxyx(stdscr, h, w);

	if (w != width || h != height) {
		glyph_buf = std::vector<cell>(width * height, cell(' '));
	}

	ch = getch();

	window &focused = this->window_list[0];

	switch (ch) {
	default:
		break;
	case KEY_UP:
	case 'w':
		focused.move_cursor_to(focused.get_cursor_x(),
		                       focused.get_cursor_y() - 1);
		break;
	case KEY_DOWN:
	case 's':
		focused.move_cursor_to(focused.get_cursor_x(),
		                       focused.get_cursor_y() + 1);
		break;
	case KEY_LEFT:
	case 'a':
		focused.move_cursor_to(focused.get_cursor_x() - 1,
		                       focused.get_cursor_y());
		break;
	case KEY_RIGHT:
	case 'd':
		focused.move_cursor_to(focused.get_cursor_x() + 1,
		                       focused.get_cursor_y());
		break;
	case KEY_ENTER:
	case ' ': {
		size_t i;

		for (i = 0; i < focused.widgets.size(); ++i) {
			std::shared_ptr<widget> ptr = focused.widgets[i];

			auto w = ptr.get();
			int  l, t, r, b;
			int  curx, cury;

			l = w->x, t = w->y;
			r = l + w->width - 1, b = t + w->height - 1;
			curx = focused.get_cursor_x(), cury = focused.get_cursor_y();

			if (curx >= l && curx <= r && cury >= t && cury <= b) {
				focused.press(i);
				break;
			}
		}
		break;
	}
	case KEY_CTRL('c'):
		return false;
	}

	return true;
}

static cell render_glyph(window const &win, int x, int y) {
	chtype const corners[4] = {ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER,
	                           ACS_LRCORNER};

	std::vector<cell> win_buffer;
	size_t            i;

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
	win_buffer.resize(win.get_inner_width() * win.get_inner_height(),
	                  cell(' ', win.props));

	for (std::shared_ptr<widget> ptr : win.widgets) {
		unsigned u, v;
		widget  *w;

		w = ptr.get();

		std::vector<cell> buf(w->width * w->height, cell(' '));
		w->render(buf);

		for (v = 0; v < w->height; ++v) {
			for (u = 0; u < w->width; ++u) {
				size_t j;
				size_t k;

				j = (w->y + v) * win.get_inner_width() + (w->x + u);
				k = v * w->width + u;

				if (j < win_buffer.size() && k < buf.size()) {
					win_buffer[j] = buf[k];
				}
			}
		}
	}

	if (x == (win.get_inner_x() + win.get_cursor_x()) &&
	    y == (win.get_inner_y() + win.get_cursor_y())) {
		short fg, bg;
		fg = CPAIR_FG(win_buffer[i].color_pair);
		bg = CPAIR_BG(win_buffer[i].color_pair);

		win_buffer[i].color_pair = CPAIR(bg, fg);
	}

	return win_buffer[i];
}

void screen::render() {
	size_t i;

	std::fill(glyph_buf.begin(), glyph_buf.end(), cell(' '));

	for (auto win = window_list.rbegin(); win != window_list.rend(); ++win) {
		int l, t, r, b;

		l = win->get_outer_x();
		t = win->get_outer_y();
		r = win->get_outer_x() + int(win->get_outer_width() - 1);
		b = win->get_outer_y() + int(win->get_outer_height() - 1);

		for (int y = t; y <= b; ++y) {
			for (int x = l; x <= r; ++x) {
				glyph_buf[y * width + x] = render_glyph(*win, x, y);
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

void widget::clear() { glyph_buf.clear(); }

void widget::print(std::string const &s, attr_t attrs, short color_pair) {
	for (char ch : s) {
		glyph_buf.push_back(cell(ch, attrs, color_pair));
	}
}

void label::render(std::vector<cell> &output) {
	this->clear();
	print(text, props.attrs, props.color_pair);
	std::copy(glyph_buf.begin(), glyph_buf.end(), output.begin());
}

void button::render(std::vector<cell> &output) {
	this->clear();
	print(text, props.attrs, props.color_pair);
	std::copy(glyph_buf.begin(), glyph_buf.end(), output.begin());
}
} // namespace term
