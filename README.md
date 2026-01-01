# ⚡ Optimisation & const & en C++

Dans mes projets C++, je ne **passe pas les types primitifs** (`int`, `unsigned`, `bool`, etc.) par `const &` car ils sont très légers et copiés directement dans le registre CPU. Utiliser une référence pour eux introduirait une **indirection inutile**, donc on **gagne en performance et en lisibilité** en les passant par valeur.
En revanche, pour les **objets lourds** comme `std::vector`, `std::string` ou ma `struct Matrix`, je passe par `const &` afin d’éviter des copies coûteuses et de maximiser les performances mémoire.

## 🟢 À propos de `Matrix`

Ma `struct Matrix` stocke une grille carrée sous forme de **1D vector contigu**, optimisée pour le cache CPU et l’accès rapide :

```cpp
struct Matrix {
    size_t n;
    std::vector<unsigned> m;

    unsigned &at(size_t x, size_t y);        // accès avec coordonnées
    unsigned &at(const maPosition &pos);    // accès via position
    void displayGrid() const;               // affichage console
    void makeAMove(const maPosition &pos, char direction); // déplacement sécurisé

};
```

- Le 1D vector permet un **accès rapide** avec `y*n+x` et évite les allocations multiples des vecteurs 2D.
- Les méthodes `at()` sont surchargées pour **plus de lisibilité** avec `maPosition`.

## Installation

Lancez cette commande adapté à votre OS pour installer les dépendances nécessaires

### MacOS

Vous aurez besoin de `homebrew`

```zsh
brew install ncurses
```

### Linux

```sh
apt install ncurses -y
```
