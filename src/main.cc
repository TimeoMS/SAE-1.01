#include "term.hh"

int main() {
	term::init();
	term::set_interactive(true);

	term::screen screen;

	term::window testwin("test window", 5, 4, 12, 7);
	testwin.print("hello world");

	term::window testwin2("test window 2", 15, 9, 15, 4);
	testwin2.print("mafieux");

	screen.window_list.push_back(testwin);
	screen.window_list.push_back(testwin2);

	for (; screen.update();) {
		screen.render();
	}

	term::fini();

	return 0;
}
