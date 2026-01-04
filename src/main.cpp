#include "Matrix/matrix.h"
#if USE_LIBTERM
#include "Term/term.h"
#endif
#include <iostream>

#ifndef NDEBUG
unsigned AllocationCount(0);

void *operator new(size_t size) {
	++AllocationCount;
	return malloc(size);
}
#endif

int main(int argc, const char *argv[]) {
#if USE_LIBTERM
	term::init();
	term::set_interactive(true);

	term::screen screen;

	term::window testwin(screen, "test window", 4, 4, 12, 9);
	testwin.set_maximized(false);
	testwin.set_padding(2, 1);
	testwin.print("hello world");

	term::window testwin2(screen, "test window 2", 10, 9, 15, 7);
	testwin2.print("mafieux");

	screen.window_list.push_back(testwin);
	screen.window_list.push_back(testwin2);

	for (; screen.update();) {
		screen.render();
	}

	term::fini();

	return 0;
#else
	srand(time(0));

	bool exit(false);

	for (; !exit;) {
		Matrix             m(6);
		casali::maPosition pos{0, 0};
		unsigned           count(0);
		for (size_t x(0); x < 6; ++x)
			if (m.atLeastThreeInARowFrom(x, pos, count) && count >= 5) {
				exit = true;
				std::cout << "---------------------\n";
				m.displayGrid();
				std::cout << "---------------------\n";
				m.removalInRow(pos, count);
				m.displayGrid();
			}
	}
#endif

#ifndef NDEBUG
	std::cout << "----------------\nNombre allocation: " << AllocationCount
			  << '\n';
#endif

	return 0;
}
