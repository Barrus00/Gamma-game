/** @file
 * Implementacja klasy przechowującej stan gry gamma
 *
 * @author Bartosz Ruszewski <b.ruszewski@student.uw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 14.04.2020
 */

#include <stdlib.h>
#include <string.h>
#include "gamma.h"
#include "pairs.h"
#include "stack_pairs.h"
#include <stdio.h>
/** @brief Struktura pola na planszy.
 *  Przechowuje informacje o jednym polu na planszy,
 *  numer gracza do którego należy dane pole,
 *  współrzędne pola z którym jest połaczone oraz
 *  rangę, czyli wysokość w naszym grafie find & union <br>
 *  (Dokładniej na ilu głębokościach znajdują się pola które
 *  są z danym polem połączone)
 */
typedef struct square {
    uint32_t player;    ///< Numer właściciela (0 dla wolnego pola),
    pair parent;        ///< Współrzędne pola do którego należy,
    uint32_t rank;      ///< Ranga pola
} square;

/** @brief Struktura całej planszy.
 *  Przechowuje informacje o aktualnym stanie gry,
 *  zaiwera dwuwymiarową tablicę pól, rozmiar tablicy
 *  czyli jej wysokość i szerokość, maksymalną ilość rozłącznych obszarów
 *  zajętych przez każdego z graczy osobno, ilość graczy, oraz informacje o nich. <br>
 *  Dla każdego z graczy przechowujemy: <br>
 *  1) ilość rozłącznych obszarów <br>
 *  2) ilość pól zajętych <br>
 *  3) informacje czy wykonał już swój złoty ruch <br>
 *  Dodatkowo zawiera stos który pomaga pomocniczy do poruszać się po planszy,
 *  oraz tablicę @p visited, która określa odwiedzone dotychczas pola.
 */
struct gamma {
    square** board;         /**< @brief Opis planszy.
                            * Dwuwymiarowa tablica pól, o wymiarach
                            * [@p width][@p height], w której komórka
                            * [ @p x] [@p y] zawiera pole o współrzędnych
                            * (@p x, @p y). Numer gracza który zajmuje to pole
                            * jest równy 0, dla wolnego pola. <br> Indeksowane od (0,0).
                            */

    uint32_t* player_areas; /**< @brief Pamięta liczbę obszarów każdego z graczy.
                            * Wskaźnik na pierwszy element tablicy, w której
                            * element o indeksie @p i określa ilość rozłącznych
                            * obszarów które należą do gracza
                            * o numerze @p i @p + @p 1.
                            */
    uint64_t* player_fields; /**< @brief Zawiera liczbę pól które należą do graczy.
                             * Wskaźnik na pierwszy element tablicy, w której
                             * element o indeksie @p i określa ilość pól
                             * które należą do gracza o numerze @p i @p + @p 1.
                             */
    bool* player_gold_move; /**< @brief Przechowuje informacje o złotym ruchu gracza.
                            * Wskaźnik na pierwszy element tablicy, w której
                            * element o indeksie @p i określa czy gracz
                            * o numerze @p i @p + @p 1 wykonał już złoty ruch.
                            */
    uint32_t number_of_players; ///< Ilość graczy.

    uint32_t areas;         ///< Maksymalna ilosć rozłącznych obszarów.
    uint32_t width;         ///< Liczba kolumn w planszy.
    uint32_t height;        ///< Liczba wierszy w planszy.

    stack* stk;             ///< Stos na ktorym bedziemy wywolywac bfs.
    bool* visited;          /**< @brief Tablica odwiedzonych pól.
                            * Jednowymiarowa tablica wartości logicznych,
                            * określająca pola które już odwiedziliśmy
                            * w danym wywołaniu funkcji.
                            * Niech @p k oznacza całkowitą częśc dzielenia
                            * @p i @p / @p wysokość_planszy.
                            * Wtedy pozycja @p i w tablicy visited określa
                            * pole o współrzędnych (k, i - k * wysokość_planszy).
                            */
};

uint32_t gamma_how_many_players(gamma_t *g) {
    return g->number_of_players;
}

uint32_t gamma_width(gamma_t *g) {
    return g->width;
}

uint32_t gamma_height(gamma_t *g) {
    return g->height;
}




/** @brief Sprawdza czy z danej pozycji możemy pójść w górę.
 * Określa czy pole, które znajduje się w wierszu wyżej niż pole o współrzędnych
 * @p a mieści się na planszy, i czy należy do tego samego gracza.
 * @param[in] g         - wskaźnik na strukturę planszy.
 * @param[in] player    - numer gracza który zajmuje pole z którego się poruszamy,
 *                        liczba dodatnia
 * @param[in] a         - współrzędne pola z którego chcemy się poruszyć.
 * @return Wartość @p true, jeżeli pole w wierszu wyżej jest dobrze określone oraz
 * należy do tego samego gracza co pole wyjściowe. @p false w przeciwnym wypadku.
 */
static bool is_north_valid(gamma_t *g, uint32_t player, pair a) {
    uint32_t x = a.fst;
    uint32_t y = a.snd;

    return (y < g->height - 1 && g->board[x][y + 1].player == player);
}

/** @brief Sprawdza czy z danej pozycji możemy pójść w dół.
 * Określa czy pole, które znajduje się w wierszu niżej niż pole o współrzędnych
 * @p a mieści się na planszy, i czy należy do tego samego gracza.
 * @param[in] g         - wskaźnik na strukturę planszy.
 * @param[in] player    - numer gracza który zajmuje pole z którego się poruszamy,
 *                        liczba dodatnia
 * @param[in] a         - współrzędne pola z którego chcemy się poruszyć.
 * @return Wartość @p true, jeżeli pole w wierszu niżej jest dobrze określone oraz
 * należy do tego samego gracza co pole wyjściowe. @p false w przeciwnym wypadku.
 */
static bool is_south_valid(gamma_t *g, uint32_t player, pair a) {
    uint32_t x = a.fst;
    uint32_t y = a.snd;

    return (y > 0 && g->board[x][y - 1].player == player);
}

/** @brief Sprawdza czy z danej pozycji możemy pójść w prawo.
 * Określa czy pole, które znajduje się w kolumnie na prawo od pola o
 * współrzędnych @p a mieści się na planszy, i czy należy do tego samego gracza.
 * @param[in] g         - wskaźnik na strukturę planszy.
 * @param[in] player    - numer gracza który zajmuje pole z którego się poruszamy,
 *                        liczba dodatnia
 * @param[in] a         - współrzędne pola z którego chcemy się poruszyć.
 * @return Wartość @p true, jeżeli pole w kolumnie na prawo jest dobrze określone oraz
 * należy do tego samego gracza co pole wyjściowe. @p false w przeciwnym wypadku.
 */
static bool is_east_valid(gamma_t *g, uint32_t player, pair a) {
    uint32_t x = a.fst;
    uint32_t y = a.snd;

    return (x < g->width - 1 && g->board[x + 1][y].player == player);
}

/** @brief Sprawdza czy z danej pozycji możemy pójść w lewo.
 * Określa czy pole, które znajduje się w kolumnie na lewo od pola o współrzędnych
 * @p a mieści się na planszy, i czy należy do tego samego gracza.
 * @param[in] g         - wskaźnik na strukturę planszy.
 * @param[in] player    - numer gracza który zajmuje pole z którego się poruszamy,
 *                        liczba dodatnia
 * @param[in] a         - współrzędne pola z którego chcemy się poruszyć.
 * @return Wartość @p true, jeżeli pole w kolumnie na lewo jest dobrze określone oraz
 * należy do tego samego gracza co pole wyjściowe. @p false w przeciwnym wypadku.
 */
static bool is_west_valid(gamma_t *g, uint32_t player, pair a) {
    uint32_t x = a.fst;
    uint32_t y = a.snd;

    return (x > 0 && g->board[x - 1][y].player == player);
}

/** @brief Przesuwa współrzędne do wiersza wyżej.
 * Dla danej pary współrzędnych @p a, zwraca współrzędne określające
 * pole które znajduje się we wierszu wyżej. Funkcja wywołująca powinna
 * najpierw sprawdzić, czy daną parę można przesunąć bez wychodzenia poza
 * określony obszar.
 * @param a             - para nieujemnych współrzędnych opisująca położenie
 *                        pola które chcemy przesunąć.
 * @return Parę współrzędnych powstałą po przesunięciu danej pary do wiersza wyżej.
 */
static pair move_pair_north(pair a) {
    return make_pair(a.fst, a.snd + 1);
}

/** @brief Przesuwa współrzędne do wiersza niżej.
 * Dla danej pary współrzędnych @p a, zwraca współrzędne określające
 * pole które znajduje się we wierszu niżej. Funkcja wywołująca powinna
 * najpierw sprawdzić, czy daną parę można przesunąć bez wychodzenia poza
 * określony obszar.
 * @param a             - para nieujemnych współrzędnych opisująca położenie
 *                        pola które chcemy przesunąć.
 * @return Parę współrzędnych powstałą po przesunięciu danej pary do wiersza nizej.
 */
static pair move_pair_south(pair a) {
    return make_pair(a.fst, a.snd - 1);
}

/** @brief Przesuwa współrzędne do kolumny na prawo.
 * Dla danej pary współrzędnych @p a, zwraca współrzędne określające
 * pole które znajduje się w kolumnie na prawo. Funkcja wywołująca powinna
 * najpierw sprawdzić, czy daną parę można przesunąć bez wychodzenia poza
 * określony obszar.
 * @param a             - para nieujemnych współrzędnych opisująca położenie
 *                        pola które chcemy przesunąć.
 * @return Parę współrzędnych powstałą po przesunięciu danej pary do kolumny
 * na prawo.
 */
static pair move_pair_east(pair a) {
    return make_pair(a.fst + 1, a.snd);
}

/** @brief Przesuwa współrzędne do kolumny na lewo.
 * Dla danej pary współrzędnych @p a, zwraca współrzędne określające
 * pole które znajduje się w kolumnie na prawo. Funkcja wywołująca powinna
 * najpierw sprawdzić, czy daną parę można przesunąć bez wychodzenia poza
 * określony obszar.
 * @param a             - para nieujemnych współrzędnych opisująca położenie
 *                        pola które chcemy przesunąć.
 * @return Parę współrzędnych powstałą po przesunięciu danej pary do kolumny na prawo.
 */
static pair move_pair_west(pair a) {
    return make_pair(a.fst - 1, a.snd);
}

/** @brief Zwraca adres pola o danych współrzędnych.
 * Zwraca adres pola które znajduje się na parze współrzędnych
 * @p coordinates znajdujące się na planszy wskazywanej przez wskaźnik @p g.
 * Funkcja wywołujaca powinna uważać, czy wskaźnik na planszę nie jest pusty.
 * @param[in] g             - wskaźnik na planszę
 * @param[in] cooridnates   - para nieujemnych współrzędnych opisująca położenie pola
 * @return Adres pola wskazywane przez daną parę współrzędnych.
 */
static square* get_field(gamma_t *g, pair cooridnates) {
    return &(g->board[cooridnates.fst][cooridnates.snd]);
}

/** @brief Zwraca numer gracza do którego należy pole.
 * Zwraca numer gracza do którego należy pole o współrzędnych @p coordinates
 * na planszy @p g. Funkcja wywołująca musi uważać na to, czy dane pole 
 * jest dobrze określone dla danej planszy, 
 * i czy wskaźnik na planszę nie jest pusty.
 * @param[in] g             - wskaźnik na planszę 
 * @param[in] coordinates   - para nieujemnych współrzędnych opisująca położenie pola
 * @return Numer gracza do którego należy dane pole.
 */
static uint32_t get_player(gamma_t *g, pair coordinates) {
    return get_field(g, coordinates)->player;
}

uint32_t gamma_player(gamma_t *g, uint32_t x, uint32_t y) {
    pair coordinates = make_pair(x, y);

    return get_player(g, coordinates);
}

/** @brief Określa głównego rodzica danego pola.
 * Funkcja rekurencyjna która znajduje korzeń do którego podłączone
 * jest pole o wspołrzędnych @p coordinates na planszy @p g.
 * Jest to część implementacji struktury FIND & UNION,
 * funkcja podczas szukania korzenia, kompresuje też każdą ścieżkę,
 * podłączając każde pole po drodze bezpośrednio do korzenia.
 * Należy uważać czy wskaźnik planszy nie jest pusty,
 * oraz na to czy współrzędne są na niej dobrze określone.
 * @param[in,out] g         - wskaźnik na planszę.
 * @param[in] coordinates   - para nieujemnych współrzędnych opisująca położenie pola.
 * @return Parę współrzędnych określających położenie korzenia, czyli reprezentanta
 * obszaru do którego należy dane pole.
 */
static pair find_ancestor(gamma_t *g, pair coordinates) {
    pair actual;

    actual = get_field(g, coordinates)->parent;

    if (compare_pairs(actual, coordinates) != 0) {
        return get_field(g, coordinates)->parent = find_ancestor(g, actual);
    }
    else {
        return actual;
    }
}

/** @brief Łączy dwa dane pola, z możliwością aktualizowania obszarów graczy.
 * Funkcja będącą częścią implementacji struktury FIND & UNION.
 * Łączy dwa dane pola (@p x, i @p y) znajdujące się na planszy
 * która zawiera wskaźnik @p g. Podłącza korzeń jednego pola do drugiego,
 * przy okazji kompresując ścieżki. Przy łączeniu korzeni, zwraca uwagę
 * na ich rangę, zawsze podłączamy korzeń o mniejszej randze, do tego o większej.
 * Jeżeli rangi są równe, to podłączamy korzeń drzewa do którego należy
 * pierwsze pole, do korzenia do którego należy pole nr. 2. Dodatkowo
 * zwiększamy wtedy jego rangę. Należy uważać czy wskaźnik na pole nie jest pusty,
 * oraz czy współrzędne są dobrze określone. Dodakowo możemy
 * określić (@p if_need_to_count), czy przy łączeniu rozłącznych obszarów mamy
 * aktualizować liczbę obszarów gracza do którego należą pola.
 * @param[in,out] g             - wskaźnik na planszę.
 * @param[in] x                 - para nieujemnych współrzędnych opisująca
 *                                pierwsze pole
 * @param[in] y                 - para nieujemnych współrzędnych opisująca
 *                                drugie pole
 * @param[in] if_need_to_count  - zmienna logiczna, określa
 *                                czy należy aktualizować obszary które łączymy.
 *                                (A dokładniej ilości obszarów gracza do którego
 *                                należą pola). Dla @p true, aktualizujemy,
 *                                w przeciwnym wypadku ignorujemy łaczenia.
 */
static void union_fields(gamma_t *g, pair x, pair y, bool if_need_to_count) {
    pair a = find_ancestor(g, x);
    pair b = find_ancestor(g, y);
    uint32_t owner = get_field(g, x)->player;
    square *ancestor_x, *ancestor_y;

    ancestor_x = get_field(g, a);
    ancestor_y = get_field(g, b);

    if (compare_pairs(a, b) != 0 && if_need_to_count)
        g->player_areas[owner - 1]--;

    if (ancestor_x->rank > ancestor_y->rank) {
        ancestor_y->parent = a;
    }
    else {
        ancestor_x->parent = b;
    }

    if (ancestor_x->rank == ancestor_y->rank)
        ancestor_x->rank++;
}
/** @brief Łączy wszystkie pola sąsiadujące.
 * Podłącza wszystkie pola, które są zajęte przez gracza o numerze @p player,
 * sąsiadujące z tym określonym przez parę współrzędnych @p central.
 * Podobnie jak w funkcji union_fields możemy określić za pomocą parametru
 * @p if_need_to_count to czy aktualizujemy obszary gracza przy łączeniu.
 * Należy uważać czy wskaźnik na struktrurę planszy nie jest pusty,
 * i czy współrzędna jest dobrze określona.
 * @param[in] g                 - wskaźnik na strukturę planszy
 * @param[in] player            - numer gracza do którego pole należy
 *                                liczba dodatnia
 * @param[in] central           - para nieujemnych współrzędnych pola do
 *                                którego będziemy podłączać sąsiadów
 * @param if_need_to_count      - zmienna logiczna, określa
 *                                czy należy aktualizować obszary które łączymy.
 *                                (A dokładniej ilości obszarów gracza do którego
 *                                należą pola). Dla @p true, aktualizujemy,
 *                                w przeciwnym wypadku ignorujemy łaczenia.
 */
static void union_neighbours(gamma_t *g, uint32_t player, pair central, bool if_need_to_count) {
    if (is_west_valid(g, player, central)) {
        union_fields(g, central, move_pair_west(central), if_need_to_count);
    }
    if (is_east_valid(g, player, central)) {
        union_fields(g, central, move_pair_east(central), if_need_to_count);
    }
    if (is_south_valid(g, player, central)) {
        union_fields(g, central, move_pair_south(central), if_need_to_count);
    }
    if (is_north_valid(g, player, central)) {
        union_fields(g, central, move_pair_north(central), if_need_to_count);
    }
}

/** @brief Sprawdza czy pole sąsiaduje z jakimś, które należy do danego gracza.
 * Funkcja określa czy pole znajdujące się na planszy którą wskazuje wskaźnik @p g
 * opisane przez współrzedne @p center, sąsiaduje z jakimś polem zajetym przez
 * gracza o numerze @p player @p + @p 1.
 * Należy zwracać uwagę na poprawność argumentów.
 * @param[in] g             - wskaźnik na planszę
 * @param[in] player        - numer gracza, do którego mają należeć sąsiedzi,
 *                            liczba dodatnia
 * @param[in] center        - para nieujemnych współrzędnych opisująca pole,
 *                            które sprawdzamy
 * @return Wartość @p true, jeżeli dane pole sąsiaduje już z jakimś zajętym
 * przez danego gracza. @p false w przeciwnym wypadku.
 */
static bool check_neighbours(gamma_t *g, uint32_t player, pair center) {
    if (is_north_valid(g, player, center) ||
        is_south_valid(g, player, center) ||
        is_west_valid(g, player, center) ||
        is_east_valid(g, player, center))
        return true;
    else
        return  false;
}
/** @brief Ustawia wszystkie wartości tablicy bool na false.
 * Funkcja dla danej tablicy bool @p arr, i jej rozmiaru jako
 * @p size. Ustawia jej wszystkie wartości na @p false. <br>
 * Należy zwracać uwagę na poprawność przekazywanych argumentów.
 * @param[in, out] arr    - wskaźnik na pierwszą wartość w tablicy
 * @param[in] size        - rozmiar tablicy, liczba dodatnia.
 */
static void clear_bool_arr(bool* arr, uint64_t size) {
    memset(arr, false, size * sizeof(bool));
}
/** @brief Sprawdza czy pamięć na planszę jest dobrze zaalokowana.
 * Funkcja sprawdza czy plansza na którą wskazuje wskaźńik @p g, jest poprawna.
 * Czyli czy udało się zaalokować wystarczającą ilosć pamięci.
 * @param[in] g                 - wskaźnik na strukturę planszy
 * @return Wartość logiczna @p true, gdy poprawnie zaalokowano pamięć.
 * @p false w przeciwnym wypadku.
 */
static bool check_if_all_ok(gamma_t *g) {
    if (g == NULL || g->board == NULL)
        return false;

    for (uint32_t i = 0; i < g->width; i++) {
        if (g->board[i] == NULL) {
            return false;
        }
    }

    if (g->player_fields == NULL ||
        g->player_gold_move == NULL ||
        g->player_areas == NULL ||
        g->stk == NULL ||
        g->visited == NULL)
        return false;

    return true;
}

gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {
    if (width < 1 || height < 1 || players < 1 || areas < 1)
        return NULL;

    gamma_t* new_object = (gamma_t *)malloc(sizeof(gamma_t));

    if (new_object == NULL)
        return NULL;

    new_object->board = (struct square **)calloc(width, sizeof(struct square));

    if (new_object->board != NULL) {
        for (uint32_t i = 0; i < width; i++) {
            new_object->board[i] = (struct square *) calloc(height, sizeof(struct square));
        }
    }

    new_object->player_areas = calloc(players, sizeof(uint32_t));
    new_object->player_fields = calloc(players, sizeof(uint64_t));

    new_object->player_gold_move = calloc(players, sizeof(bool));
    clear_bool_arr(new_object->player_gold_move, players);

    new_object->number_of_players = players;

    new_object->areas = areas;
    new_object->width = width;
    new_object->height = height;

    new_object->stk = new_stack(width * height);
    new_object->visited = (bool *)malloc(width * height * sizeof(bool));

    if (!check_if_all_ok(new_object)){
        gamma_delete(new_object);
        return NULL;
    }

    return new_object;
}

void gamma_delete(gamma_t *g) {
    if (g != NULL) {

        if (g->board != NULL) {
            for (uint32_t i = 0; i < g->width; i++)
                free(g->board[i]);

        }

        free(g->board);
        free(g->player_areas);
        free(g->player_gold_move);
        free(g->player_fields);
        free_stack(g->stk);
        free(g->visited);
        free(g);
    }
}

/** @brief Sprawdza poprawność pary współrzędnych.
 * Funkcja sprawdza czy dana para współrzędnych @p coordinates,
 * jest dobrze określona na planszy wskazywanej przez wskaźnik
 * @p g. Należy dbać o to, żeby argumenty były poprawne
 * @param[in] g             - wskaźnik na strukturę planszy
 * @param[in] coordinates   - para liczb nieujemnych opisująca położenie pola
 * @return Wartość @p true, jeżeli współrzędne są poprawne, (tzn. mieszczą
 * się na planszy), lub @p false w przeciwnym wypadku.
 */
static bool check_coordinates(gamma_t *g, pair coordinates) {
    uint32_t x = coordinates.fst;
    uint32_t y = coordinates.snd;

    return (x < g->width && y < g->height);
}

/** @brief Sprawdza poprawność numeru gracza.
 * Funkcja sprawdza czy @p id poprawnie określa indeks gracza,
 * na planszy wskazywanej przez wskaźnik @p g.
 * Należy dbać o to, żeby argumenty były poprawne
 * @param[in] g             - wskaźnik na strukturę planszy
 * @param[in] id            - indeks gracza, liczba dodatnia
 * @return Wartość @p true, jeżeli indeks jest poprawny,
 * lub @p false w przeciwnym wypadku.
 */
static bool check_player(gamma_t *g, uint32_t id) {
    return id > 0 && id <= g->number_of_players;
}

/** @brief Sprawdza czy ruch jest poprawnie określony.
 * Funkcja sprawdza, czy gracz o indeksie @p player,
 * może wykonać ruch na planszy wskazywanej przez wskaźnik @p g
 * na miejsce o współrzędnych @p center.
 * Należy dbać o poprawność argumentów.
 * @param[in] g             - wskaźnik na strukturę planszy
 * @param[in] player        - indeks gracza, liczba dodatnia
 * @param[in] center        - para nieujemnych wspołrzędnych określająca pole
 * @return Wartość @p true, jezeli gracz może wykonać ruch
 * @p false w przeciwnym wypadku.
 */
static bool is_move_valid (gamma_t *g, uint32_t player, pair center) {

    if ((!check_player(g, player) ||
        !check_coordinates(g, center) ||
        get_player(g, center) != 0) ||
        (g->player_areas[player - 1] >= g->areas &&
        !check_neighbours(g, player, center)))
        return false;

    return true;
}
/** @brief Sprawdza wstępnie czy złoty ruch jest poprawnie określony.
* Funkcja wstępnie sprawdza, czy gracz o indeksie @p player,
* może wykonać złoty ruch na planszy wskazywanej przez wskaźnik @p g
* na miejsce o współrzędnych @p center.
* Należy dbać o poprawność argumentów.
* @param[in] g             - wskaźnik na strukturę planszy
* @param[in] player        - indeks gracza, liczba dodatnia
* @param[in] center        - para nieujemnych wspołrzędnych określająca pole
* @return Wartość @p true, jezeli gracz może wykonać ruch,
 * lub @p false w przeciwnym wypadku.
* (UWAGA: Funkcja nie sprawdza czy ruch ten rozspójni obszary co doprowadzi
* ostatecznie do tego, że ruch ten będzie niemożliwy)
*/
static bool is_golden_move_valid (gamma_t *g, uint32_t player, pair center) {

    if ((!check_player(g, player) ||
         !check_coordinates(g, center)||
         get_player(g, center) == 0) ||
         (g->player_areas[player - 1] == g->areas &&
         !check_neighbours(g, player, center)))
        return false;

    return true;
}

bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (g == NULL)
        return false;

    pair this_field = make_pair(x, y);

    if (!is_move_valid(g, player, this_field))
        return false;

    g->player_fields[player - 1]++;
    g->player_areas[player - 1]++;

    get_field(g, this_field)->player = player;
    get_field(g, this_field)->parent = this_field;

    union_neighbours(g, player, this_field, true);

    return true;
}

/** @brief Sprawdza, czy dane pole zostało już odwiedzone i odwiezda je.
 * Funkcja sprawdza, czy pole określone przez parę współrzędnych @p a,
 * zostało już odwiedzone (Czy wartość na pozycji @p a w tablicy bool @p arr,
 * jest true, czy false). Dodatkowo aktualizuje ją i zaznacza odwiedzone wartości.
 * Należy dbać o poprawność argumentów
 * @param[in,out] arr       - wskaźnik na pierwszą wartość w tablicy 2d
 * @param[in] height        - liczba wierszy tablicy, liczba dodatnia
 * @param[in] a             - para nieujemnych współrzędnych określająca pole
 * @return Wartość @p true, jeżeli pole nie zostało jescze odwiedzone
 * i zmienia wartość w tablicy na danym indeksie na true
 * W przeciwnym wypadku zwraca @p false.
 */
static bool check_and_visit (bool* arr, uint32_t height, pair a) {
    if (!*(arr + height * a.fst + a.snd)) {
        *(arr + height * a.fst + a.snd) = true;
        return false;
    }
    return true;
}

/** @brief Określa czy dane pole jest reprezentantem obszaru.
 * Funkcja sprawdza czy pole na planszy wskazywanej przez wskaźnik @p g
 * opisane parą współrzędnych @p coordinates, jest reprezentantem obszaru
 * do którego należy.
 * Należy dbać o poprawność argumentów funkcji.
 * @param[in] g             - wskaźnik na planszę
 * @param[in] coordinates   - para nieujemnych współrzędnych opisująca położenie pola
 * @return Wartość @p true, jeżeli dane pole jest reprezentantem obszaru,
 * do którego należy (korzeniem), lub @p false w przeciwnym wypadku.
 */
static bool is_his_own_parent(gamma_t *g, pair coordinates) {
    if (compare_pairs (coordinates, find_ancestor(g, coordinates)))
        return true;

    return false;
}

/** @brief Przywraca pole do wartości bazowych.
 * Funkcja aktualizuje pole, znajdujące się na planszy wskazywanej przez @p g,
 * o parze współrzędnych @p a. Przwyraca je do wartości bazowych,
 * czyli usuwa numer gracza który je posiada, resetuje rangę pola i ustawia
 * je na swojego reprezentanta.
 * Należy dbać o poprawność argumentów funkcji.
 * @param[in,out] g         - wskaźnik na planszę
 * @param[in] a             - para nieujemnych współrzędnych opisująca położenie pola
 */
static void reset_field(gamma_t *g, pair a) {
    square* this_field = get_field(g, a);

    this_field->player = 0;
    this_field->parent = a;
    this_field->rank = 0;
}

/** @brief Przywraca reprezentanta pola do wartości bazowych.
 * Funkcja aktualizuje pole, znajdujące się na planszy wskazywanej przez @p g,
 * o parze współrzędnych @p a. Ustawia to pole na swojego reprezentanta i
 * resetuje jego rangę. Zostawiając przy tym właściciela pola.
 * Należy dbać o poprawność argumentów funkcji.
 * @param[in,out] g         - wskaźnik na planszę
 * @param[in] a             - para nieujemnych współrzędnych opisująca położenie pola
 */
static void reset_parent(gamma_t *g, pair a){
    square* this_field = get_field(g, a);

    this_field->parent = a;
    this_field->rank = 0;
}

/** @brief Ustawia wszystkie pola na spójnym obszarze na swoich reprezentantów.
 * Funkcja zmienia reprezentantów każdego pola, na planszy którą wskazuje @p g,
 * które należy do spójnego obszaru (czyli należy do tego samego gracza)
 * wraz z polem o współrzędnych @p center. Zachowuje się jak BFS, zbiera
 * wszystkie sąsiadujące pola na stos, i po kolei resetuje reprezentantów
 * każdego z nich.
 * (Zatem każde pole na obszarze jest teraz swoim własnym reprezentantem)
 * @param[in,out] g          - wskaźnik na planszę
 * @param[in] player         - indeks gracza do którego należą pola na obszarze,
 *                             liczba dodatnia
 * @param[in] center         - para nieujemnych współrzędnych pola od którego
 *                             zaczniemy wywołania,
 */
static void reset_parents_area(gamma_t *g, uint32_t player, pair center) {
    uint32_t width = g->width;
    uint32_t height = g->height;
    bool* visited;
    stack *stk = g->stk;
    pair curr;

    clear_bool_arr(g->visited, width * height);
    visited = g->visited;
    push(stk, center);

    while (!is_stack_empty(stk)) {
        curr = pop(stk);

        reset_parent(g, curr);

        if (is_north_valid(g, player, curr) &&
            !check_and_visit(visited, height, move_pair_north(curr))) {

            push(stk, move_pair_north(curr));
        }
        if (is_south_valid(g, player, curr) &&
            !check_and_visit(visited, height, move_pair_south(curr))) {

            push(stk, move_pair_south(curr));
        }
        if (is_west_valid(g, player, curr) &&
            !check_and_visit(visited, height, move_pair_west(curr))) {

            push(stk, move_pair_west(curr));
        }
        if (is_east_valid(g, player, curr) &&
            !check_and_visit(visited, height, move_pair_east(curr))) {

            push(stk, move_pair_east(curr));
        }
    }
}

/** @brief Łączy zresetowany obszar pól w jeden spójny obszar.
 * Funkcja operuje na obszarze, na planszy wskazywanej przez @p g,
 * który został wcześniej zresetowany (Każde pole jest swoim własnym reprezentantem).
 * Po kolei łączy każde pole na spójnym obszarze gracza @p player z polem centralnym
 * o współrzędnych @p central.
 * @param[in,out] g          - wskaźnik na planszę
 * @param[in] player         - indeks gracza do którego należą pola na obszarze,
 *                             liczba dodatnia
 * @param[in] central        - para nieujemnych współrzędnych pola od którego
 *                             zaczniemy wywołania,
 */
static void update_unions_on_area(gamma_t *g, uint32_t player, pair central) {
    uint32_t height = g->height;
    uint32_t width = g->width;
    bool* visited;
    stack *stk = g->stk;
    pair curr;

    clear_bool_arr(g->visited, width * height);
    visited = g->visited;
    push(stk, central);

    while (!is_stack_empty(stk)) {
        curr = pop(stk);

        union_fields(g, central, curr, false);

        if (is_north_valid(g, player, curr) &&
            !check_and_visit(visited, height, move_pair_north(curr))) {

            push(stk, move_pair_north(curr));
        }
        if (is_south_valid(g, player, curr) &&
            !check_and_visit(visited, height, move_pair_south(curr))) {

            push(stk, move_pair_south(curr));
        }
        if (is_west_valid(g, player, curr) &&
            !check_and_visit(visited, height, move_pair_west(curr))) {

            push(stk, move_pair_west(curr));
        }
        if (is_east_valid(g, player, curr) &&
            !check_and_visit(visited, height,move_pair_east(curr))) {

            push(stk, move_pair_east(curr));
        }
    }
}

/** @brief Łączy kolejne spójne obszary sąsiadów i liczy ile było takich rozłącznych.
 * Funkcja łączy na nowo, zresetowane już obszary sąsiadujące z polem @p central,
 * które należą do gracza o indeksie @p player. Przy tym sprawdza ilu sąsiadów tego
 * pola jest rozłącznych
 * @param[in,out] g         - wskaźnik na planszę
 * @param[in] player        - numer indeksu gracza do którego należy pole
 *                            liczba dodatnia
 * @param[in] central       - para nieujemnych współrzędnych określających
 *                            położenie pola
 * @return Liczbę rozłącznych obszarów które powstaną po usunięciu danego pola.
 */
static int update_neighbours_and_count_them(gamma_t *g, uint32_t player, pair central) {
    int out = 0;

    if (is_north_valid(g, player, central) &&
        !is_his_own_parent(g, move_pair_north(central))) {

        out++;
        update_unions_on_area(g, player, move_pair_north(central));
    }
    if (is_south_valid(g, player, central) &&
        !is_his_own_parent(g, move_pair_south(central))) {

        out++;
        update_unions_on_area(g, player, move_pair_south(central));
    }
    if (is_west_valid(g, player, central) &&
        !is_his_own_parent(g, move_pair_west(central))) {

        out++;
        update_unions_on_area(g, player, move_pair_west(central));
    }
    if (is_east_valid(g, player, central) &&
        !is_his_own_parent(g, move_pair_east(central))) {

        out++;
        update_unions_on_area(g, player, move_pair_east(central));
    }

    return out;
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    int field_owner;
    pair this_field = make_pair(x, y);

    if (g == NULL || !is_golden_move_valid(g, player, this_field) ||
        !gamma_golden_possible(g, player) ||
        get_player(g, this_field) == player){

        return false;
    }

    field_owner = get_player(g, this_field);

    reset_parents_area(g, field_owner, this_field);
    reset_field(g, this_field);

    g->player_areas[field_owner - 1] +=
            update_neighbours_and_count_them(g, field_owner, this_field) - 1;

    g->player_fields[field_owner - 1]--;

    if (g->player_areas[field_owner - 1] > g->areas ||
        !gamma_move(g, player, x, y)) {

        gamma_move(g, field_owner, x, y);
        return false;
    }

    g->player_gold_move[player - 1] = true;

    return true;
}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
    if (g == NULL || !check_player(g, player))
        return 0;

    uint64_t out = 0;
    struct square** board = g->board;

    if (g->player_areas[player - 1] < g->areas) {
        out = g->width * g->height;

        for (uint32_t i = 0; i < g->number_of_players; i++) {
            out -= g->player_fields[i];
        }
    }
    else {
        for (uint32_t row = 0; row < g->height; row++) {
            for (uint32_t column = 0; column < g->width; column++) {
                if (board[column][row].player == 0)
                    if (check_neighbours(g, player, make_pair(column, row)))
                        out++;
            }
        }
    }

    return out;
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
    if (g == NULL || !check_player(g, player))
        return 0;

    return g->player_fields[player - 1];
}

bool gamma_golden_possible(gamma_t *g, uint32_t player) {
    if (g == NULL || !check_player(g, player))
        return false;

    if (g->player_gold_move[player - 1]) return false;

    for (uint32_t i = 0; i < g->number_of_players; i++) {
        if (g->player_fields[i] > 0 && i != player - 1)
            return true;
    }

    return false;
}

/** @brief Liczy ilość cyfr danej liczby.
 * Dla danej liczby @p x, liczy z ilu cyfr sie składa.
 * @param[in] x         - liczba którą sprawdzamy, nieujemna
 * @return Liczba określająca ilość cyfr w liczbie.
 */
uint16_t how_many_digits(uint32_t x) {
    uint16_t out = 0;

    if (x == 0)
        return 1;

    while (x != 0) {
        out++;
        x = x / 10;
    }

    return out;
}
/** @brief Określa szerokość każdego pola w napisie.
 * Funkcja sprawdza ilość cyfr w największym indeksie gracza który,
 * występuje na planszy wskazywanej przez @p g. Dla liczb więcej niż dwucyfrowych
 * rezerwuje szerokość o jeden większą, dla zwiększonej czytelności.
 * @param[in] g         - wskaźnik na planszę
 * @return Szerokość każdego miejsca opisującego numer gracza na planszy.
 */
static uint16_t size_needed(gamma_t *g) {
    uint32_t out = 0;

    for (uint32_t i = 0; i < g->number_of_players; i++) {
        if (g->player_fields[i] > 0)
            out = i;
    }

    out = how_many_digits(out);

    if (out > 1) out++;

    return out;
}

char* gamma_board(gamma_t *g) {
    if (g == NULL)
        return NULL;

    char *board;
    uint32_t width = g->width;
    uint32_t height = g->height;
    pair i;
    uint64_t start_index, end_index = 0;
    uint64_t size_of_board, player_id;
    int elem_width = size_needed(g);

    size_of_board = elem_width * width * height + height + 1;
    board = (char *) malloc(size_of_board * sizeof(char));

    if (board == NULL)
        return NULL;

    for (uint32_t row = 0; row < height; row++) {
        for (uint32_t column = 0; column < width; column++) {
            i = make_pair(column, height - row - 1);
            start_index = end_index;
            end_index += elem_width - 1;
            player_id = get_player(g, i);

            if (player_id == 0) {
                board[end_index] = '.';
                end_index--;
            }

            while (player_id > 0) {
                board[end_index] = (char) (player_id % 10 + '0');
                end_index--;
                player_id /= 10;
            }

            end_index++;

            while (end_index > start_index) {
                end_index--;
                board[end_index] = ' ';
            }
            end_index += elem_width;
        }
        board[end_index++] = '\n';
    }
    board[end_index] = '\0';

    return board;
}

char* gamma_board_max(gamma_t *g) {
    if (g == NULL)
        return NULL;

    char *board;
    uint32_t width = g->width;
    uint32_t height = g->height;
    pair i;
    uint64_t start_index, end_index = 0;
    uint64_t size_of_board, player_id;
    int elem_width = how_many_digits(g->number_of_players) + 1;

    size_of_board = elem_width * width * height + height + 1;
    board = (char *) malloc(size_of_board * sizeof(char));

    if (board == NULL)
        return NULL;

    for (uint32_t row = 0; row < height; row++) {
        for (uint32_t column = 0; column < width; column++) {
            i = make_pair(column, height - row - 1);
            start_index = end_index;
            end_index += elem_width - 1;
            player_id = get_player(g, i);

            if (player_id == 0) {
                board[end_index] = '.';
                end_index--;
            }

            while (player_id > 0) {
                board[end_index] = (char) (player_id % 10 + '0');
                end_index--;
                player_id /= 10;
            }

            end_index++;

            while (end_index > start_index) {
                end_index--;
                board[end_index] = ' ';
            }
            end_index += elem_width;
        }
        board[end_index++] = '\n';
    }
    board[end_index] = '\0';

    return board;
}