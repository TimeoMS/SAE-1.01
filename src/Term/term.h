#pragma once

#include <string>
#include <vector>

#include <ncurses.h>

namespace term
{
	union cell
	{
		cell() : c(' ') {}
		cell(chtype c) : c(c) {}
		cell(char ch, attr_t attrs, short color_pair)
			: ch(ch), attrs(attrs), color_pair(color_pair) {}

		operator chtype() const { return c; }

		chtype c;
		struct
		{
			char ch;
			attr_t attrs;
			short color_pair;
		};
	};

	class window
	{
	private:
		std::string name;

		int x;
		int y;
		unsigned width;
		unsigned height;

		unsigned padding_x;
		unsigned padding_y;
		bool maximized;
		bool bordered;

		unsigned cursor_x;
		unsigned cursor_y;

		std::vector<cell> buffer;

	public:
		window(std::string name, int x, int y, unsigned width, unsigned height,
			   unsigned padding_x = 0, unsigned padding_y = 0,
			   bool maximized = false, bool bordered = true);

		std::string get_name() const { return name; }

		int get_position_x() const { return x; }
		int get_position_y() const { return y; }
		unsigned get_width() const { return width; }
		unsigned get_height() const { return height; }

		unsigned get_padding_x() const { return padding_x; }
		unsigned get_padding_y() const { return padding_y; }
		bool is_maximized() const { return maximized; }
		bool is_bordered() const { return bordered; }

		unsigned get_cursor_x() const { return cursor_x - padding_x; }
		unsigned get_cursor_y() const { return cursor_y - padding_y; }

		std::vector<cell> const &get_buffer() const { return buffer; }

		void set_name(std::string name) { this->name = name; }

		void set_position(int x, int y)
		{
			this->x = x;
			this->y = y;
		}

		void set_padding(unsigned padding)
		{
			padding_x = padding;
			padding_y = padding;
		}

		void set_padding(unsigned x, unsigned y)
		{
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
	};

	class screen
	{
	private:
		unsigned width;
		unsigned height;

		/* un "dirty rectangle" permet de suivre les modifications de l'ecran */
		unsigned dirty_l = 0;
		unsigned dirty_t = 0;
		unsigned dirty_r = 0;
		unsigned dirty_b = 0;

		cell *buffer;

	public:
		std::vector<window> window_list;

		screen();
		~screen();

		bool update();
		void render();
	};

	void init();
	void set_interactive(bool interactive);
	void fini();
} // namespace term
