/** @file
 * Impelementacja struktury pary.
 *
 * @author Bartosz Ruszewski <b.ruszewski@student.uw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 14.04.2020
 */
#include "pairs.h"

int compare_pairs(pair a, pair b) {
    if (a.fst > b.fst) {
        return 1;
    } else if (b.fst > a.fst) {
        return -1;
    } else {
        if (a.snd > b.snd)
            return 1;
        else if (b.snd > a.snd)
            return -1;
        else
            return 0;
    }
}

pair make_pair (uint32_t x, uint32_t y) {
    pair new;

    new.fst = x;
    new.snd = y;

    return  new;
}