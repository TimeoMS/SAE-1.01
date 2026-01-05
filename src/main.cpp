#include <ctime>
#include <iostream>
#include <random>

#if USE_LIBTERM
#include "Term/term.h"
#endif
#include "GameModes/gamemodes.h"

#ifndef NDEBUG
unsigned AllocationCount(0);

void *operator new(size_t size) {
	++AllocationCount;
	return malloc(size);
}
#endif

int main(int argc, const char *argv[]) {
	srand(time(0));

#if USE_LIBTERM
	term::init();
	term::set_interactive(true);

	term::screen screen;

	term::window win_main(screen, "Fenetre principal", 0, 0, 1, 1);
	win_main.set_maximized(true);
	win_main.set_padding(2, 1);
	win_main.print(" CandyCrush ", A_BOLD, CPAIR(COLOR_WHITE, COLOR_RED));

	term::window win_menu(screen, "Menu", 2, 4, 60, 20);
	win_menu.set_padding(2, 1);
	win_menu.props.color_pair = CPAIR(COLOR_WHITE, COLOR_BLACK);

	term::button btn_start;
	btn_start.text   = "Commencer une partie";
	btn_start.x      = 0;
	btn_start.y      = 0;
	btn_start.width  = btn_start.text.size();
	btn_start.height = 1;

	win_menu.widgets.push_back(std::make_shared<term::button>(btn_start));

	term::button btn_infinite;
	btn_infinite.text   = "Mode infini";
	btn_infinite.x      = 0;
	btn_infinite.y      = 2;
	btn_infinite.width  = btn_infinite.text.size();
	btn_infinite.height = 1;

	win_menu.widgets.push_back(std::make_shared<term::button>(btn_infinite));

	term::button btn_versus;
	btn_versus.text   = "Mode versus";
	btn_versus.x      = 0;
	btn_versus.y      = 4;
	btn_versus.width  = btn_versus.text.size();
	btn_versus.height = 1;

	win_menu.widgets.push_back(std::make_shared<term::button>(btn_versus));

	screen.window_list.push_back(win_menu);
	screen.window_list.push_back(win_main);

	{
		term::window &win_term = screen.window_list[0];
		term::window &win_main = screen.window_list[1];

		for (; screen.update();) {
			switch (win_term.get_pressed()) {
			default:
				screen.render();
				break;
			case 0:
				term::fini();
				GameModes::normal(20);
				break;
			case 1:
				term::fini();
				GameModes::infinite();
			case 2:
				term::fini();
				GameModes::versuslaunch(10);
			}
		}
	}

	term::fini();

	return 0;
#else
	if (argc == 2 && std::string(argv[1]) == "-h") {
		std::cout << "Usage: \n"
				  << "  ./candycrush [nb_shot](def: 4)                      "
					 "Start the game\n"
				  << "  ./candycrush -infinite                              "
					 "Start the infinte game mode\n"
				  << "  ./candycrush -versus [nb_shot](def: 4 per player)   "
					 "Start the versus game mode\n"
				  << "  ./candycrush -h                                     "
					 "Show this help message\n";
		return 0;
	} else if (argc == 2 && std::string(argv[1]) == "-versus") {
		GameModes::versuslaunch(argv[2] ? std::stoi(argv[2]) : 10);
	} else if (argc == 2 && std::string(argv[1]) == "-infinite") {
		GameModes::infinite();
	} else {
		GameModes::normal(argv[1] ? std::stoi(argv[1]) : 20);
	}

	return 0;
#endif
}
