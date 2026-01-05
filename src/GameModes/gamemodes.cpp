#include "Matrix/constants.h"
#include "Matrix/matrix.h"
#include "gamemodes.h"
#include <sstream>

bool readInput(size_t &x, size_t &y, char &direction) {
	std::string line;
	if (!std::getline(std::cin, line))
		return false;

	std::istringstream iss(line);

	if (iss >> x >> y >> direction) {
		return true;
	}
	return false;
}

void GameModes::infinite() {
	Matrix   m(5);
	unsigned score(0);
	for (;;) {
		size_t x(0);
		size_t y(0);
		char   direction('d');

		m.refill();
		m.displayGrid();

		std::cout
			<< "x y direction (u d l r, q pour quitter) (votre score est de "
			<< score << "): ";

		if (!readInput(x, y, direction)) {
			casali::couleur(KRouge);
			std::cerr << "Erreur de saisie\n";
			casali::couleur(KReset);
			continue;
		}

		if (direction == 'q')
			break;

		if (direction != 'u' && direction != 'd' && direction != 'r' &&
		    direction != 'l')
			std::cerr << "Direction n est pas bien utilise\n";

		m.makeAMove({x, y}, direction);

		bool comboFound;

		do {
			casali::maPosition pos{0, 0};
			comboFound = false;

			for (size_t col(0); col < m.n; ++col) {
				unsigned howMany(0);
				if (m.atLeastThreeInAColumnFrom(col, pos, howMany)) {
					score += howMany;
					m.removalInColumn(pos, howMany);
					comboFound = true;
				}
			}

			for (size_t row(0); row < m.n; ++row) {
				unsigned howMany(0);
				if (m.atLeastThreeInARowFrom(row, pos, howMany)) {
					score += howMany;
					m.removalInRow(pos, howMany);
					comboFound = true;
				}
			}

			if (comboFound)
				std::cout << "Combo ! Score actuel: " << score << "\n";

		} while (comboFound);
	}
}

void GameModes::normal(int nbMoves) {
	Matrix   m(5);
	unsigned score(0);
	for (unsigned i(0); i < nbMoves; i += 1) {
		size_t x(0);
		size_t y(0);
		char   direction('d');

		m.displayGrid();

		std::cout
			<< "x y direction (u d l r, q pour quitter) (votre score est de "
			<< score << "): ";

		if (!readInput(x, y, direction)) {
			casali::couleur(KRouge);
			std::cerr << "Erreur de saisie\n";
			casali::couleur(KReset);
			continue;
		}

		if (direction == 'q')
			break;

		if (direction != 'u' && direction != 'd' && direction != 'r' &&
		    direction != 'l')
			std::cerr << "Direction n est pas bien utilise\n";

		m.makeAMove({x, y}, direction);

		bool comboFound;

		do {
			casali::maPosition pos{0, 0};
			comboFound = false;

			for (size_t col(0); col < m.n; ++col) {
				unsigned howMany(0);
				if (m.atLeastThreeInAColumnFrom(col, pos, howMany)) {
					score += howMany;
					m.removalInColumn(pos, howMany);
					comboFound = true;
				}
			}

			for (size_t row(0); row < m.n; ++row) {
				unsigned howMany(0);
				if (m.atLeastThreeInARowFrom(row, pos, howMany)) {
					score += howMany;
					m.removalInRow(pos, howMany);
					comboFound = true;
				}
			}

			if (comboFound)
				std::cout << "Combo ! Score actuel: " << score << "\n";

		} while (comboFound);
	}
}

void versus(Matrix &m, unsigned &score, int player) {
	size_t x(0);
	size_t y(0);
	char   direction('d');

	m.displayGrid();

	std::cout
		<< "j" << player
		<< ": x y direction (u d l r, q pour quitter) (votre score est de "
		<< score << "): ";

	if (!readInput(x, y, direction)) {
		casali::couleur(KRouge);
		std::cerr << "Erreur de saisie\n";
		casali::couleur(KReset);
		return;
	}

	if (direction == 'q')
		exit(0);

	if (direction != 'u' && direction != 'd' && direction != 'r' &&
	    direction != 'l')
		std::cerr << "Direction n est pas bien utilise\n";

	m.makeAMove({x, y}, direction);

	bool comboFound;

	do {
		casali::maPosition pos{0, 0};
		comboFound = false;

		for (size_t col(0); col < m.n; ++col) {
			unsigned howMany(0);
			if (m.atLeastThreeInAColumnFrom(col, pos, howMany)) {
				score += howMany;
				m.removalInColumn(pos, howMany);
				comboFound = true;
			}
		}

		for (size_t row(0); row < m.n; ++row) {
			unsigned howMany(0);
			if (m.atLeastThreeInARowFrom(row, pos, howMany)) {
				score += howMany;
				m.removalInRow(pos, howMany);
				comboFound = true;
			}
		}

		if (comboFound)
			std::cout << "Combo ! Score actuel: " << score << "\n";

	} while (comboFound);
}

void GameModes::versuslaunch(int nbMoves) {
	Matrix   j1(5);
	Matrix   j2 = j1;
	unsigned scorej1(0);
	unsigned scorej2(0);

	for (unsigned i(0); i < nbMoves * 2; i += 1) {
		bool Player1Turn = true;
		if (Player1Turn)
			Player1Turn = false;
		versus(j1, scorej1, 1);
		if (!Player1Turn)
			Player1Turn = true;
		versus(j2, scorej2, 2);
	}
}
