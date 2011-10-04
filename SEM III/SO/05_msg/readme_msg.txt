Kolejki komunikatow w systemie Linux
====================================

Podstawowa literatura
---------------------

  + M.J.Rochkind
    "Programowanie w systemie Unix dla zaawansowanych": rozdz. 7.5

Nieco bardziej wyczerpujacy opis wraz ze strukturami uzywanymi przez jadro
mozna znalezc w:

  + W.R.Stevens:
    "Programowanie zastosowan sieciowych w systemie Unix": rozdz. 3.8, 3.9.

  + odpowiednie strony mana

Programy przykladowe i pliki ¼rod³owe
-------------------------------------

  a) 'msgtool' - bardzo prosty program umozliwiajacy wyslanie i odebranie
      komunikatu, stworzenie, skasowanie kolejki i zmiane praw dostepu.

      Przed uruchomieniem trzeba ustawic zmienna srodowiskowa MSGKEY np.:
         export MSGKEY=4321

  b) 'msgsrv' i 'msgcli' - programy umozliwiajace przesylanie plikow przez 
      kolejki komunikatow. 
      Nalezy uruchomic 'msgsrv' na jednej konsoli, a 'msgcli' na drugiej.
      Zakonczenie 'msgsrv': ctrl-C. 'msgcli' pobiera nazwe pliku z stdin.

      Programy te maja zdefiniowany klucz w pliku 'mesg.h'.

  c) 'limits' sprawdza pojemnosc kolejki: maksymalna wielkosc
      komunikatu i ilosc pamieci przeznaczona na komunikaty.


Pliki zawierajace zrodla programow przykladowych.

   err.c, err.h
      Obsluga bledow

   msgtool.c
      Program demonstrujacy operacje na kolejkach komunikatow

   mesg.h
      Plik naglowkowy dla 'msgsrv.c' i 'msgcli.c'. Definicja komunikatu
      i klucza kolejki.

   msgsrv.c
      Serwer sluzacy do wysylania plikow. W petli odbiera nazwy plikow 
      od klientow i przesyla te pliki klientom. Przechwytuje SIGINT, 
      ktorego wywolanie powoduje zakonczenie programu.

   msgcli.c
      Klient czyta z wejscia nazwe pliku i wysyla serwerowi. Odebrany
      plik wypisuje na stdout.

   limits.c
      Badanie ograniczen systemu: maksymalnego rozmiaru komunikatu
      i pojemnosci kolejki. Przyklad uzycia flagi IPC_PRIVATE.
      Ograniczenia te sa zawarte w Linuxie w pliku /usr/include/linux/msg.h

1. Kolejki komunikatow
   -------------------

   Podstawowe funkcje systemowe przeznaczone do obslugi kolejek to:
   - msgget --- sluzy do tworzenia/otwierania kolejki,
   - msgctl --- sluzy do sterowania kolejka komunikatow, w szczegolnosci
               pozwala zlikwidowac kolejke,
   - msgsnd --- sluzy do wysylania komunikatow,
   - msgrcv --- sluzy do odbierania komunikatow.

1.1 Funkcja msgget
    --------------

   Aby bylo mozliwe skorzystanie z kolejki komunikatow jest konieczne
   jej utworzenie albo uzyskanie dostepu do kolejki istniejacej.  Jak
   juz powiedzielismy w czesci poswieconej wspolnym cechom wszystkich
   mechanizmow IPC, s³uzy do tego funkcja 'msgget':

   #include <sys/types.h>
   #include <sys/ipc.h>
   #include <sys/msg.h>

   int msgget(key_t klucz, int flagi)

   Znaczenie kolejnych parametrow funkcji 'msgget' jest calkowicie
   zgodne z tym, co do tej pory powiedzielismy.


* Przeczytaj man msgget


1.2 Wysylanie i odbior komunikatow --- funkcje msgsnd i msgrcv
    ----------------------------------------------------------

   Do wysylania i odbierania komunikatow sluza funkcje 'msgsnd' i
   'msgrcv'.  Obie te funkcje, jako drugi argument, przyjmuja wskaznik
   na strukture dowolnego typu zawierajaca odpowiednio
   wysylany/odbierany komunikat.  Jedyne ograniczenie to koniecznosc
   wystepowania na poczatku tej struktury dlugiej liczby calkowitej
   wiekszej od zera, ktora nazywa sie rodzajem komunikatu.
   Wprowadzenie pojecia rodzaju komunikatu pozwala odbiorcy na wybor z
   kolejki komunikatow wlasciwego rodzaju oraz oczekiwanie na taki
   komunikat.

   int msgsnd(idk, buf, lbajt, flagi);    /* przeslij komunikat */
   int idk;                               /* identyfikator kolejki */
   struct msgbuf *buf;                    /* wskaznik do komunikatu */
   int lbajt;                             /* dlugosc komunikatu */
   int flagi;

   Funkcja zwraca -1 w przypadku bledu i 0 w przypadku sukcesu.
   Dlugosc komunikatu oznacza liczbe bajtow komunikatu bez pola
   okreslajacego rodzaj.

   Parametr 'flagi' ma zwykle wartosc 0. Powoduje to wstrzymanie
   procesu w przypadku przepelnienia kolejki. Po ustaniu przepelnienia
   proces jest kontynuowany. Wartosc IPC_NOWAIT flagi powoduje
   natychmiastowe zakonczenie funkcji z okreslonym kodem bledu
   ('errno' ustawiane na EAGAIN).

* Przeczytaj man msgsnd

   int msgrcv(idk, buf, lbajt, typkom, flagi)  /* odbierz komunikat */
   int idk; 
   struct msgbuf *buf;
   int lbajt;
   long typkom;
   int flagi;

   Funkcja zwraca liczbe otrzymanych bajtow lub -1 w przypadku bledu.
   Argument 'lbajt' okresla maksymalna liczbe bajtow, ktora mozna
   umiescic w buforze 'buf' (znowu bez bajtow okreslajacych rodzaj
   komunikatu.

   Za pomoca 'msgrcv' mozemy wybierac komunikaty z kolejki. I tak: 
   - jesli typkom = 0, to z kolejki jest odczytywany pierwszy komunikat
     dowolnego typu.  
   - jesli typkom > 0, to wtedy jest odczytywany pierwszy komunikat 
     o typie typkom. Ustawienie parametru 'flagi' na 0 powowduje 
     zawieszenie procesu w oczekiwaniu na (wlasciwy) komunikat. 
     Ustawienie wartosci IPC_NOWAIT powoduje ewentualne 
     zakonczenie funkcji z odpowiednim bledem. Istnieje ponadto mozliwosc 
     odczytania z kolejki komunikatow pierwszego komunikatu o rodzaju 
     roznym od wskazanego (opcja MSG_EXCEPT).
   - jesli typkom <0, wtedy jest wybierany komunikat o najmniejszym rodzaju 
     sposrod mniejszych lub rownych wartosci bezwzglednej z typkom 
     (wybor priorytetowy).

* Przeczytaj man msgrcv

1.3 Zarzadzanie kolejka --- funkcja msgctl
    --------------------------------------

   Kolejna funkcja zwiazana z komunikatami to 'msgctl'.  Umozliwia ona
   dostep do struktury jadra przechowujacej informacje o kolejce.
   Najwazniejsza z naszego punkt widzenia operacja mozliwa do
   przeprowadzenia za pomoca tej funkcji to zlikwidowanie kolejki.

   int msgctl(idk, polec, bufst) /* sterowanie kolejka */
   int idk;
   int polec;                    /* polecenie */
   struct msqid_s *bufst;        /* wskaznik do bufora stanu */

   Funkcja zwraca 0 w przypadku sukcesu i -1 w przypadku bledu.
   Polecenie usuwajace kolejke ma postac IPC_RMID.  Odpowiednia postac
   wywolania to:

         msgctl(idk, IPC_RMID, NULL)

   Jeszcze raz przypominamy, ze usuniecie kolejki powoduje
   natychmiastowe przerwanie wszystkich czekajacych na nadejscie
   komunikatu wywolañ funkcji 'msgrcv', ktore zwracaja wtedy -1 oraz
   ustawiaja zmienna 'errno' na EIDRM.

* Przeczytaj man msgctl

   Pliki naglowkowe do omawianych funkcji znajduja sie w zbiorze:
   <sys/msg.h>.

   W pliku <sys/ipc.h> zdefiniowana jest ponadto struktura ipc_perm, 
   w ktorej jadro przechowuje informacje o tworcy, wlascicielu, 
   trybach dostepu, numerze kolejnym i kluczu kolejki.

   Jest ona elementem struktury msqid_ds, ktora ponadto zawiera takie
   informacje jak wskazniki do pierwszego i ostatniego komunikatu
   znajdujacego sie aktualnie w kolejce, liczba bajtow w kolejce,
   liczba komunikatow w kolejce, pojemnosc kolejki, identyfikatory
   procesow, ktore ostatnio pisaly i czytaly do/z kolejki, czasy
   ostatniego wykonania tych operacji oraz czas ostatniego wywolania
   'msgctl'.

   Bezposredni dostep do pol tej struktury daje funkcja 'msgctl'.

2. Porzadki
   --------

   Pamietajmy, ze niepotrzebne kolejki wymagaja jawnego usuniecia. Na
   poziomie shella mozliwe jest wykorzystanie programu 'ipcs' do
   sprawdzenia, jakie kolejki w systemie istnieja oraz ewentualne
   wykorzystanie programu 'ipcrm' do usuniecia niepotrzebnych kolejek.

   Na poziomie programu wygodnym narzedziem do sprzatania utworzonych
   kolejek sa funkcje obslugi sygnalow.


3. Cwiczenie
   ---------

   Jak moze zauwazyles analizujac tresc programow serwera i klienta
   uruchomienie jednoczesne dwoch klientow 'msgcli' moze
   spowodowac 'przemieszanie sie' przesylanych plikow, poniewaz 
   wszyscy klienci czytaja komunikaty o tym samym identyfikatorze (rownym 2).

   Twoje zadanie polega na napisaniu serwera, ktory realizuje na zlecenie 
   proste dzialania arytmetyczne (dodawanie, odejmowanie, mnozenie) na 
   liczbach calkowitych oraz klienta, ktory korzysta z jego uslug.
   Serwer otrzymuje od klientow komunikaty zawierajace dwie liczby calkowite 
   oraz znak okreslajacy rodzaj dzialania a odsyla wynik. 
   Komunikacja powinna gwarantowac odeslanie wyniku wlasciwemu zleceniodawcy.

4. Kolejki POSIX-owe
   -----------------

Kolejki POSIX-owe, podobnie jak w przypadku mechanizmu SysV IPC,
pozwalaja na przesylanie komunikatow o okreslonych
priorytetach. Zawsze przy tym nastepuje odczyt komunikatu o najwyzszym
priorytecie, nie ma wiec, na przyklad, prostej mozliwosci odczytywania
wiadomosci przeznaczonych dla konkretnego procesu (poprzez odbior
komunikatow o typie rownym pidowi zainteresowanego procesu).

W odroznieniu od kolejek SysV IPC, POSIX-owe pozwalaja z kolei na
asynchroniczne oczekiwanie na nadejscie komunikatu lub jednoczesne
oczekiwanie na wielu kolejkach.

* Przeczytaj man mq_overview
