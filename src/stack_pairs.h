/** @file
 * Interfejs stosu par.
 *
 * @author Bartosz Ruszewski <b.ruszewski@student.uw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 14.04.2020
 */

#ifndef GAMMA_STACK_PAIRS_H
#define GAMMA_STACK_PAIRS_H

#include <stdlib.h>
#include "pairs.h"

/** @brief Struktura opisująca stos par.*/
typedef struct stack {
    uint64_t maxsize;       ///< Maksymalny rozmiar stosu
    uint64_t top;           ///< Indeks elementu na samej górze stosu
    pair* items;            ///< Tablica elementów na stosie.
} stack;

/** @brief Tworzy nowy stos.
 * Inicjalizuje nowy stos, o rozmairze który określa @p capacity. <br>
 * Funkcja wywołująca powinna usunąć ten stos z pamięci.
 * @param[in] capacity  - rozmiar tworzonego stosu, liczba dodatnia.
 * @return wskaźnik na utworzoną stukture, lub NULL jeżeli nie udało
 * się zaalokować pamięci.
 */
stack* new_stack(uint32_t capacity);

/** @brief Zwraca ilość elementów na stosie.
 * @param[in] ptr   - zawiera wskaźnik do struktury, której rozmiar chcemy odczytać
 * @return ilość elementów na stosie.
 */
uint64_t stack_size(stack *ptr);

/** @brief Sprawdza czy stos nie jest pusty.
 * @param[in] ptr    - zawiera wskaźnik do struktury, którą chcemy sprawdzić
 * @return Wartość @p true, jeżeli na stosie nie ma już żadnych elementów
 * lub @p false, jeżeli dany stos nie jest pusty.
 */
bool is_stack_empty(stack *ptr);

/** @brief Wrzuca parę na stos.
 * Aktualizuje dany stos wrzucając na jego szczyt element @p a.
 * Jednocześnie aktualizuje aktualną ilość elementów na stosie.
 * @param[in,out] ptr   - wskaźnik do struktury, którą aktualizujemy
 * @param[in] a         - element który chcemy wrzucić na szczyt, para nieujemnych liczb
 */
void push(stack* ptr, pair a);

/** @brief Zwraca i usuwa parę ze szczytu niepustego stosu.
 * Aktualizuje stos @p ptr, usuwając z niego najwyżej położony element. <br>
 * Jednocześnie aktualizuje aktualną ilość elementów na danym stosie.
 * @param[in,out] ptr       - wskaźnik do aktualizowanej struktury
 * @return Parę która znajduje się na szczycie stosu.
 */
pair pop(stack *ptr);

/** @brief Usuwa stos.
 * Zwalnia z pamięci strukturę, którą wskazuje @p ptr. <br>
 * Nic nie robi, jeżeli wskaźnik ma wartość NULL.
 * @param[in,out] ptr       - wskaźnik na usuwaną strukturę.
 */
void free_stack(stack* ptr);

#endif //GAMMA_STACK_PAIRS_H
