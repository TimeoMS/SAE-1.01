#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ncurses.h>

#define CPAIR(f, b) (1 + (f) * 8 + (b))
#define CPAIR_FG(cpair) (((cpair) - 1) / 8)
#define CPAIR_BG(cpair) (((cpair) - 1) % 8)

namespace term {
struct rect {
	int left;
	int top;
	int right;
	int bottom;
};

struct cell_props {
	attr_t attrs      = A_NORMAL;
	short  color_pair = 0;

	cell_props() : attrs(A_NORMAL), color_pair(0) {}
	cell_props(attr_t attrs, short color_pair)
		: attrs(attrs), color_pair(color_pair) {}
};

struct cell {
	chtype glyph      = ' ';
	attr_t attrs      = A_NORMAL;
	short  color_pair = 0;

	cell(chtype glyph)
		: glyph(glyph & A_CHARTEXT), attrs(glyph & A_ATTRIBUTES) {}
	cell(chtype glyph, cell_props props)
		: glyph(glyph), attrs(props.attrs), color_pair(props.color_pair) {}
	cell(chtype glyph, attr_t attrs, short color_pair)
		: glyph(glyph), attrs(attrs), color_pair(color_pair) {}
};

struct widget {
  protected:
	std::vector<cell> glyph_buf;

	void clear();
	void print(std::string const &s, attr_t attrs, short color_pair);

  public:
	int        x;
	int        y;
	unsigned   width;
	unsigned   height;
	cell_props props;

	virtual ~widget() = default;

	virtual void render(std::vector<cell> &output) = 0;
};

class window;
class screen {
  private:
	unsigned width;
	unsigned height;

	std::vector<cell> glyph_buf;

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

	unsigned padding_x = 0;
	unsigned padding_y = 0;
	bool     maximized = false;
	bool     bordered  = true;

	unsigned cursor_x = 0;
	unsigned cursor_y = 0;

	int pressed_widget = -1;

	std::vector<cell> glyph_buf;

  public:
	cell_props props;

	std::vector<std::shared_ptr<widget>> widgets;

	window(screen const &scr, std::string name, int x, int y, unsigned width,
	       unsigned height);

	std::string get_name() const { return name; }

	int      get_outer_x() const { return !maximized * x; }
	int      get_outer_y() const { return !maximized * y; }
	unsigned get_outer_width() const {
		return maximized ? scr.get_width() : width;
	}
	unsigned get_outer_height() const {
		return maximized ? scr.get_height() : height;
	}

	int get_inner_x() const { return padding_x + bordered + get_outer_x(); }
	int get_inner_y() const { return padding_y + bordered + get_outer_y(); }
	unsigned get_inner_width() const {
		return std::max(get_outer_width(), (padding_x + bordered) * 2) -
		       (padding_x + bordered) * 2;
	}
	unsigned get_inner_height() const {
		return std::max(get_outer_height(), (padding_y + bordered) * 2) -
		       (padding_y + bordered) * 2;
	}

	unsigned get_padding_x() const { return padding_x; }
	unsigned get_padding_y() const { return padding_y; }
	bool     is_maximized() const { return maximized; }
	bool     is_bordered() const { return bordered; }

	unsigned get_cursor_x() const { return cursor_x; }
	unsigned get_cursor_y() const { return cursor_y; }

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
	           short color_pair = 0);

	int  get_pressed() { return pressed_widget; }
	void press(int widget_index) { pressed_widget = widget_index; }
};

struct label : public widget {
	std::string text;

	void render(std::vector<cell> &output) override;
};

struct button : public widget {
	std::string text;

	button() { props.color_pair = CPAIR(COLOR_BLACK, COLOR_WHITE); }

	void render(std::vector<cell> &output) override;
};

void init();
void set_interactive(bool interactive);
void fini();
} // namespace term
