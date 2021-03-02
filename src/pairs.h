/** @file
 * Interfejs struktury pary.
 *
 * @author Bartosz Ruszewski <b.ruszewski@student.uw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 14.04.2020
 */
#ifndef GAMMA_PAIRS_H
#define GAMMA_PAIRS_H

#include <stdbool.h>
#include <stdint.h>

/** @brief Struktura pary.
 *  Para ma postać (fst, snd) <br>
 *  Oba parametry są typu uint32_t <br>
 *  Parę będziemy traktować jako spójny zapis współrzędnych
 *  (x, y)
 *  */
typedef struct pair {
    uint32_t fst;   ///< współrzędna x, liczba nieujemna
    uint32_t snd;   ///< współrzędna y, liczba nieujemna
} pair;

/** @brief Porównuje dwie pary.
 * Funkcja porównuje wartośći par @p a oraz @p b, i określa która z nich jest
 * większa lub czy sa sobie równe. Porównujemy najpierw pierwsze wartości,
 * jeżeli są sobie równe to porównujemy drugi element z pary.
 * @param a             - pierwsza para do porównania
 * @param b             - druga para do porównania
 * @return
 * @p 1  - Jeżeli pierwsza para jest większa. <br>
 * @p 0  - Jeżeli pary są równe. <br>
 * @p -1 - Jeżeli druga para jest większa.
 */
int compare_pairs(pair a, pair b);


/** @brief Tworzy nowa parę.
 * Funkcja tworzy i zwraca nową parę z wartości @p x, @p y.
 * @param x             - określa pierwszą wartość nowej pary, liczba nieujemna
 * @param y             - określa drugą wartość nowej pary, liczba nieujemna
 * @return parę postaci (@p x, @p y)
 */
pair make_pair (uint32_t x, uint32_t y);

#endif //GAMMA_PAIRS_H
