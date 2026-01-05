#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <vector>

constexpr unsigned KReset(0);
constexpr unsigned KNoir(30);
constexpr unsigned KRouge(31);
constexpr unsigned KVert(32);
constexpr unsigned KJaune(33);
constexpr unsigned KBleu(34);
constexpr unsigned KMAgenta(35);
constexpr unsigned KCyan(36);
constexpr unsigned KMaxTimes(3);
constexpr unsigned KNbCandies(7);
constexpr unsigned KImpossible(KNbCandies);

constexpr unsigned colorMap[] = {
    KReset, KNoir, KRouge, KVert, KJaune, KBleu, KMAgenta, KCyan
};

#endif