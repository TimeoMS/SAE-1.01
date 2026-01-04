#pragma once

#include <string>
#include <vector>

#include <ncurses.h>

namespace term {
struct rect {
	int left;
	int top;
	int right;
	int bottom;
};

union col_rgb;

struct col_yuv {
	double y = 0.0;
	double u = 0.0;
	double v = 0.0;

	operator col_rgb() const;

	inline bool operator==(col_yuv const &yuv) const {
		return y == yuv.y && u == yuv.u && v == yuv.v;
	}

	inline bool operator!=(col_yuv const &yuv) const { return !(*this == yuv); }
};

union col_rgb {
	uint32_t rgb = 0;
	struct {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t unused;
	};

	operator col_yuv() const;

	inline bool operator==(col_rgb const &rgb) const {
		return r == rgb.r && g == rgb.g && b == rgb.b;
	}

	inline bool operator!=(col_rgb const &rgb) const { return !(*this == rgb); }
};

struct cell_props {
	attr_t attrs      = A_NORMAL;
	short  color_pair = 0;

	cell_props() : attrs(A_NORMAL), color_pair(0) {}
	cell_props(attr_t attrs, short color_pair)
		: attrs(attrs), color_pair(color_pair) {}
};

struct cell {
	chtype  glyph = ' ';
	attr_t  attrs = A_NORMAL;
	col_rgb fg;
	col_rgb bg;

	cell(chtype glyph)
		: glyph(glyph & A_CHARTEXT), attrs(glyph & A_ATTRIBUTES) {}
	cell(chtype glyph, attr_t attrs, col_rgb fg)
		: glyph(glyph), attrs(attrs), fg(fg) {}
	cell(chtype glyph, attr_t attrs, col_rgb fg, col_rgb bg)
		: glyph(glyph), attrs(attrs), fg(fg), bg(bg) {}
};

class cell_renderer {
  private:
	std::vector<double>  score_buffer;
	std::vector<col_yuv> yuv_buffer;

  public:
	std::vector<cell> glyph_buf;

	void render(std::vector<chtype> &output);
};

class window;
class screen {
  private:
	bool     has_color;
	unsigned width;
	unsigned height;

	/* un "dirty rectangle" permet de suivre les modifications de l'ecran */
	unsigned dirty_l = 0;
	unsigned dirty_t = 0;
	unsigned dirty_r = 0;
	unsigned dirty_b = 0;

	cell_renderer renderer;

  public:
	std::vector<window> window_list;

	screen();

	unsigned get_width() const;
	unsigned get_height() const;

	bool update();
	void render();
};

class window {
  private:
	screen const &scr;
	std::string   name;

	int      x;
	int      y;
	unsigned width;
	unsigned height;

	unsigned padding_x;
	unsigned padding_y;
	bool     maximized;
	bool     bordered;

	unsigned cursor_x;
	unsigned cursor_y;

	std::vector<cell> glyph_buf;

  public:
	attr_t  attrs = A_NORMAL;
	col_rgb fg;
	col_rgb bg;
	col_rgb border_fg;
	col_rgb border_bg;

	window(screen const &scr, std::string name, int x, int y, unsigned width,
	       unsigned height);

	std::string get_name() const { return name; }

	int get_position_x() const { return padding_x + bordered + !maximized * x; }
	int get_position_y() const { return padding_y + bordered + !maximized * y; }
	unsigned get_width() const {
		return std::max(maximized ? scr.get_width() : width,
		                padding_x + bordered) -
		       padding_x + bordered;
	}
	unsigned get_height() const {
		return std::max(maximized ? scr.get_height() : height,
		                padding_y + bordered) -
		       padding_y + bordered;
	}

	unsigned get_padding_x() const { return padding_x; }
	unsigned get_padding_y() const { return padding_y; }
	bool     is_maximized() const { return maximized; }
	bool     is_bordered() const { return bordered; }

	unsigned get_cursor_x() const { return cursor_x - padding_x; }
	unsigned get_cursor_y() const { return cursor_y - padding_y; }

	std::vector<cell> const &get_buffer() const { return glyph_buf; }

	void set_name(std::string name) { this->name = name; }

	void set_position(int x, int y) {
		this->x = x;
		this->y = y;
	}

	void set_padding(unsigned padding) {
		padding_x = padding;
		padding_y = padding;
	}

	void set_padding(unsigned x, unsigned y) {
		padding_x = x;
		padding_y = y;
	}

	void set_maximized(bool maximized) { this->maximized = maximized; }
	void set_bordered(bool bordered) { this->bordered = bordered; }

	void move_cursor_to(size_t cell);
	void move_cursor_to(unsigned x, unsigned y);
	void move_cursor_by(ptrdiff_t cells);

	void clear();
	void print(std::string const &s, attr_t attrs = A_NORMAL,
	           col_rgb fg = col_rgb(), col_rgb bg = col_rgb());
};

void init();
void set_interactive(bool interactive);
void fini();
} // namespace term
