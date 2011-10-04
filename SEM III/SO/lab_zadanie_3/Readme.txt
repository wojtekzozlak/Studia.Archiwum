Symulator pamięci wirtualnej ze stronicowaniem
==============================================

Readme w przeciwieństwie do kodu w języku polskim, dla sprawniejszego
wyrażania myśli.

1. Sposób użycia
----------------

   1.1 Kompilacja
  
       Aby skompilować bibliotekę użyj  `make {strategia}`, gdzie {strategia}
       to `lfu` bądź `fifo`. Make wywołany bez parametrów skompiluje bibliotekę
       ze strategią FIFO.
     
       Wynikiem operacji make powinna być biblioteka dynamiczna `libpagesim.so`.

       `make clean` usuwa wszystkie wyprodukowane pliki.

    1.2 Praca z biblioteką

        * Biblioteka udostępnia operacje opisane w pliku `pagesim.h`, służące
        do obsługi symulowanej pamięci.

        * Pierwszą wywołaną funkcją biblioteki powinno być `page_sim_init`

        * Funkcja `page_sim_end` nie posiada gwarancji poprawnego działania,
        jeśli trwa operacja `get`, bądź `set` i może doprowadzić do
        nieodwracalnego popsucia biblioteki. W razie napotkania błędu podczas
        obsługi `page_sim_end`, jeśli jesteśmy pewni, że zakończyły się już
        wszystkie (choć nie musiały być zakończone w momencie powstania błędu)
        operacje `get`/`set`, można spróbować wywołać kolejno
        `page_sim_end`, `page_sim_mem_clean`, `page_sim_init` i jeśli ten
        ostatni nie zwrócił żadnych błędów, biblioteka nadaje się powtórnie do
        użytku.

2. Konstrukcja biblioteki

    2.1 Informacje ogólne

        * Wszystkie dane biblioteki przetrzymywane są w globalnej strukturze
        `data` `typu page_sim_data`.

        * Plik z zawartością przestrzeni adresowej tworzony jest w miejscu
        uruchomienia programu, z bezpiecznie generowaną nazwą postaci
        `tmppagesimXXXXXX`, gdzie `XXXXXX` to unikalny ciąg znaków.

        * Biblioteka do synchronizacji używa N + 2 zmiennych warunkowych,
        gdzie N jest rozmiarem przestrzeni adresowej w stronach. Dodatkowe
        dwie zmienne służą do blokowania zbyt dużej ilości równoległych
        operacji wejścia/wyjścia oraz do informowania wątków, że w pamięci
        operacyjnej znajduje się ramka, która może zostać usunięta.
        Całości towarzyszy jeden globalny mutex.

        * Do pracy z dyskiem używane są funkcje `aio_read`/`aio_write`

    2.2 Opis realizacji stronicowania

        Biblioteka tworzy tablicę stron rozmiaru przestrzeni adresowej,
        złożoną z obiektów typu `page`. Reprezentują one stronę oraz ramkę
        równocześnie, w zależności od kontekstu. Zawierają numery stron
        które reprezentują, zmienną warunkową niezbędną do synchronizacji
        wątków operujących na jednej stronie. Jeśli strona jest wczytana,
        to struktura reprezentuje równocześnie ramkę, która ją przechowuje,
        przetrzymując wskaźnik na pamięć w której znajduje się treść strony,
        numer ramki, która przetrzymuje stronę oraz wskaźniki na poprzedni
        i następny element listy ramek.

        Podczas inicjalizacja biblioteka tworzy stos struktur reprezentujących
        ramki gotowe do wykorzystania - jeśli strona ma być wczytana do
        pamięci operacyjnej, to zdejmuje taką strukturę ze stosu, przepisuje
        sobie jej numer, przepina wskaźnik do pamięci na treść strony
        oraz dopisuje się na końcu listy ramek.
        Jeśli strona została zapisana usunięta i ramka ją przechowująca ma
        zostać usunięta, następuje przepięcie pamięci z powrotem na element
        stosu, przepisanie z powrotem numeru i poprawienie licznika stosu.

        Lista ramek ma niezmiennik mówiący, że zawiera tylko ramki, które
        nie są w użyciu, tzn. strategie każdą z nich mogą wyznaczyć do
        usunięcia. Z tego powodu ramka usuwana zostaje odpięta od listy
        jeszcze przed zleceniem zapisu, a ramka wczytywana nie zostaje
        przypięta do listy przed zakończeniem odczytu. Jeśli operacja
        zapisu/odczytu nie uda się, ramka zostaje odpowiednio przywrócona
        na listę (choć nie koniecznie w tym samym miejscu, w jakim się
        wcześniej znajdowała), bądź odłożona z powrotem na stos bez
        przyłączania do listy.


    2.3 Opis realizacji synchronizacji

        Biblioteka rezygnuje z idei pełnego mechanizmu czytelników i pisarzy
        na rzecz znacznie prostszego. W efekcie, w danym momencie z pamięci
        operacyjnej może korzystać tylko jeden proces. Jednak utrata wydajności
        biblioteki jest przez to pomijalna, lub wcale jej nie ma, gdyż
        koszta synchronizacji przy podanej dziedzinie parametrów zadania
        mogłyby się okazać większe od ewentualnych zysków na równoległym
        odczycie.

        Dostęp do strony odbywa się jedno bądź dwuetapowo. Wątek, który
        zgłasza chęć pracy z daną stroną najpierw czeka na zmiennej warunkowej
        danej strony, aż przestanie ona być w użyciu. Jeśli doczeka się,
        i strona jest wczytana do ramki, to wykonuje co ma do zrobienia
        i operacja kończy się.
        
        Jeśli strony nie ma w pamięci, musi zostać wczytana. Jeśli wszystkie
        ramki są już zajęte, to `select_page` wyznacza ramkę do usunięcia.
        Jeśli taka ramka nie istnieje, wątek zasypia na zmiennej warunkowej
        `data.create_page_cond` czekając aż coś się zwolni. Jeśli można wczytać
        ramkę, (jeśli zachodzi taka potrzeba) ramka do usunięcia zostaje
        odpięta i zlecona do zapisu na dysk, następnie nowa strona zostaje
        zlecona do odczytu, a ramka ją zawierająca przypięta do listy.
        Jeśli wszystko się uda, wątek wykonuje operacje na ramce i całość
        kończy się pomyślnie.

        Tuż przed zleceniem operacji wejścia/wyjścia wątek sprawdza, czy jest
        jeszcze możliwość wykonania operacji I/O, czy też odbywa się ich
        maksymalna ilość. Jeśli nie ma możliwości wykonania operacji I/O,
        wątek czeka na zmiennej warunkowej i zostaje obudzony przez
        wątek, który operację I/O zakończył.

        Całość wykorzystuje tylko jeden globalny mutex, który jest
        odblokowywany tylko przy oczekiwaniu na zmienne warunkowe oraz
        operacjach asynchronicznego odczytu i zapisu. W związku z tym,
        w danym momencie na pamięci operacyjnej może pracować tylko jeden
        wątek. Z drugiej strony, atomizuje to operację odczytu/zapisu
        w przypadku, gdy strona jest już wczytana do ramki. Co za tym idzie
        ramka posiada status `PSIM_IN_USE` tylko jeśli trwa na niej zapis,
        bądź odczyt. Do tego wątek, który ustawił `PSIM_IN_USE` na danej
        stronie/ramce ma na nią gwarantowaną wyłączność do zakończenia
        wszystkich operacji potrzebnych do zakończenia zapytania.
        Użycie pojedynczego mutexu eliminuje również problemy z dostępem
        do różnego typu zasobów - jeśli wątek ma mutex, to może bez obawy
        robić co chce ze wszystkim. Dodatkowo, użycie jednego mutexu, powoduje,
        że opłacalniejszym sposobem budzenia w przypadku wątków czekających
        na gotowość strony jest 'broadcast'. W przeciwnym razie, signal
        musiałyby wykonywać wątki, które nie musiały zlecać operacji I/O,
        a ich działanie jest w pełni atomowe i niezauważalne z perspektywy
        pozostałych wątków.
