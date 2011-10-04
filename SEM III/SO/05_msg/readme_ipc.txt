Komunikacja miedzyprocesowa
===========================

Literatura
----------
  + M. K. Johnson, E. W. Troan
    "Oprogramowanie uzytkowe w systemie Linux", rozdz.

  + W. R. Stevens 
    "Programowanie w srodowisku systemu UNIX", rozdz. 15.6

  + M. J. Bach "Budowa systemu operacyjnego UNIX", rozdz.

  + man do poszczególnych funkcji


1. Wprowadzenie do komunikacji miedzyprocesowej w systemie Linux
   -------------------------------------------------------------

Mianem komunikacji miedzyprocesowej (InterProcess Communication lub
IPC) okresla sie zbior mechanizmow, ktore pozwalaja na wspoldzialanie
procesow, w szczegolnosci tych niespokrewnionych miedzy soba. Oprocz
poznanych juz laczy (nazwanych i nienazwanych), sa to m.in. kolejki
komunikatow, semafory oraz wspoldzielone segmenty pamieci. Jak to
czesto w Linuksie i innych systemach *xowych, jednoczesnie sa dostepne
rozwiazania nieco przestarzale, ale za to obecne na wszystkich
platformach (pakiet IPC Systemu V), jak tez dobrze przemyslane, ale
nie zawsze dostepne mechanizmy POSIX-owe.

Poniewaz te ostatnie, choc wygodniejsze i podobniejsze w obsludze do
znanych juz rozwiazan opartych o system plikow (lacza), maja nieco
mniejsze mozliwosci oraz sa, poki co, rzadziej uzywane, przedstawimy
na razie mechanizmy IPC Systemu V (SysV IPC). Wszystkie mechanizmy 
(kolejki, semafory i pamiec dzielona) charakteryzuja sie podobnym 
sposobem obslugi oraz przestrzenia nazw.

1.1 Wspolne cechy mechanizmow SysV IPC
    ----------------------------------

Kazdy obiekt IPC (kolejka komunikatow, zbior semaforow badz segment
pamieci dzielonej) jest identyfikowany przez jadro za pomoca pewnej
nieujemnej liczby naturalnej. Jej znajomosc jest konieczna i
wystarczajaca do przeprowadzenia dowolnej operacji, np. przeslania
komunikatu. Poniewaz dla wszystkich procesow, operujacych na danym
obiekcie, identyfikator jest wspolny, pelni on role analogiczna do
indeksu w globalnej tablicy otwartych plikow.

Jadro nadaje identyfikator obiektowi IPC w momencie tworzenia go,
wybierajac sposrod dostepnych jeszcze wartosci. Oznacza to, ze
identyfikator moze zalezec od tego ile obiektow IPC utworzono
wczesniej. Wspolpracujace ze soba procesy potrzebuja wiec innego
sposobu nazywania obiektow IPC, do ktorych chcialyby sie odwolywac. W
tym celu z kazdym obiektem jest zwiazany jego klucz (key_t key),
ktory, rozszerzajac nasze porownanie z systemem plikow, pelni role
unikatowej nazwy pliku. Obecnie typ 'key_t' jest zdefiniowany jako typ
'int'.

Niezaleznosc mechanizmow IPC od systemu plikow powoduje, ze nie ma
potrzeby istnienia "deskryptorow" do obiektow IPC i faktycznie ich nie
ma.

Informacje o istniejacych w pamieci obiektach IPC, do ktorych mamy
uprawnienia odczytu, mozna uzyskac wykonujac z linii polecen polecenie
'ipcs'. Wywolanie go z opcja -l pokazuje ustawione w systemie
operacyjnym limity zwiazane z obiektami IPC.

*****************************************
**** Wykonaj polecenia ipcs oraz ipcs -l
*****************************************

1.2 Dostep do mechanizmow pakietu IPC --- rodzina funkcji 'get'
    -----------------------------------------------------------

Aby uzyskac identyfikator dowolnego obiektu IPC (i ewentualnie go
stworzyc, jesli obiekt o zadanym kluczu jeszcze nie istnieje), nalezy
posluzyc sie jedna z funkcji z rodziny funkcji 'get'. W zaleznosci od
tego, czy chodzi o kolejke komunikatow, zbior semaforow czy tez
segment pamieci dzielonej, bedzie to funkcja 'msgget', 'semget' lub
'shmget'.

Wszystkie funkcje z tej rodziny maja deklaracje nastepujacej postaci:

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/???.h>

int ???get(key_t key, [dodatkowy argument, ] int flags);

gdzie '???' jest jednym ze skrotow: 'msg', 'sem' lub 'shm', w
zaleznosci od tego o jakiego rodzaju obiekt IPC chodzi. Argument 'key'
jest kluczem i pelni wspomniana juz role niezmiennej nazwy dla
tworzonego obiektu, a kolejne bity argumentu 'flags' opisuja
zachowanie funkcji w przypadku istnienia/nieistnienia obiektu o
zadanym kluczu oraz okreslaja nadawane prawa dostepu do tworzonego
obiektu.

Dodatkowy argument wystepuje tylko w funkcjach 'semget' i 'shmget' i
okresla, odpowiednio, liczbe semaforow w zestawie i rozmiar
dzielonego segmentu pamieci.

Omowimy teraz dokladniej dzialanie rodziny funkcji 'get'.

Jesli przekazany w argumencie klucz odpowiada juz istniejacemu
(uprzednio stworzonemu) obiektowi IPC danego typu oraz w przekazanej
fladze ('flags') nie jest ustawiony bit IPC_EXCL, to funkcja zwraca
identyfikator tej struktury.

Jesli natomiast klucz odpowiada nieistniejacej strukturze oraz
ustawiono we fladze bit IPC_CREAT, odpowiednia struktura zostanie
utworzona a jej identyfikator przekazany jako wynik. W mysl tego, co
powiedziano wczesniej o fladze IPC_EXCL, ustawienie obu flag
IPC_CREAT|IPC_EXCL pozwala, w sposob atomowy, na sprawdzenie czy
obiekt IPC o danym kluczu juz istnieje i jesli nie, na stworzenie
nowego obiektu danego typu.

Istnieje tez klucz specjalny IPC_PRIVATE, ktorego przekazanie jako
argumentu oznacza prosbe o stworzenie nienazwanego obiektu IPC. W
takim przypadku jest wykorzystywana jedynie informacja o uprawnieniach
do dostepu do tworzonego obiektu. Aby inny proces mogl sie do takiego
obiektu odwolac, musi on w jakis sposob poznac identyfikator
zasobu (na przyklad dziedziczac go po procesie macierzystym). 
Jest to jedyny sposob, gdyz kazde wywolanie funkcji z rodziny
'get' z kluczem IPC_PRIVATE powoduje stworzenie kolejnego obiektu
danego typu, o innym identyfikatorze.

Prawa dostepu do tworzonego obiektu sa okreslone przez 9 najmniej
znaczacych bitow przekazywanej flagi i dokladnie odpowiadaja
znaczeniem odpowiednim bitom w uprawnieniach nadawanych plikom. W
szczegolnosci oznacza to, ze mozna uzywac zdefiniowanych w pliku
<sys/stat.h> nazw symbolicznych na okreslenie odpowiednich
uprawnien. Ze specyfiki IPC wynika przy tym, ze bity odpowiedzialne za
prawa do wykonywania nie sa interpretowane w przypadku obiektow IPC.

Jesli z jakiegos powodu operacja z rodziny 'get' nie bedzie mogla
zostac poprawnie wykonana, funkcje przekazuja w wyniku wartosc -1 oraz
w odpowiedni sposob ustawiaja zmienna systemowa 'errno'.

**************************
**** Przeczytaj man svipc
**************************

Jesli projektujac aplikacje zdecydujemy sie na tworzenie obiektow o
zadanym kluczu, staniemy przed problemem wyboru kluczy w taki sposob,
by nie wejsc w konflikt z innymi programami. Generalnie nie istnieje
zadne 100%-owo skuteczne rozwiazanie, ale pomocne moze byc zapoznanie
sie z funkcja 'ftok'.

1.3 Pozostale wspolne aspekty obslugi mechanizmow IPC
    -------------------------------------------------

Wykonywaniu wszelkich operacji administracyjnych na obiektach IPC
sluza funkcje z rodziny funkcji 'ctl'. Z kolei specyficzne operacje,
zwiazane z semantyka danego typu obiektow, sa wykonywane przez
wywolania innych funkcji, ktore omowimy w czesciach poswieconych
konkretnym mechanizmom. W kazdym z tych przypadkow odwolujemy sie do
obiektow poprzez ich identyfikatory.

Co ciekawe, obiektow IPC nie trzeba po uzyciu "zamykac" tak, jak
trzeba zamykac deskryptory do plikow. Co wiecej, poniewaz obiekty IPC
nie istnieja wewnatrz procesow, pozostana w systemie do momentu ich
jawnego usuniecia.

Poniewaz nieusuwane moglyby, w skrajnym przypadku, wyczerpac nalozone
w systemie limity i uniemozliwic utworzenie innych obiektow, nalezy je
usunac, gdy tylko stana sie zbedne. Mozliwe jest to zarowno za pomoca
polecenia 'ipcrm' powloki, jak i odpowiednich wywolan funkcji z
rodziny 'ctl'.

Semantyka operacji usuniecia zalezy od typu usuwanego obiektu. Jest to
konsekwencja faktu, ze mechanizmy IPC nie sa oparte na wirtualnym
systemie plikow, ktory, dzieki licznikom odwolan, czeka z fizycznym
usunieciem obiektu do czasu, gdy przestanie byc potrzebny, czyli gdy
licznik bedzie mial wartosc 0.

W przypadku obiektow IPC jest tak tylko w przypadku segmentow pamieci
dzielonej. Usuniecie kolejki komunikatow lub zbioru semaforow powoduje
natomiast natychmiastowe przerwanie wszelkich trwajacych operacji oraz
uniewaznienie identyfikatorow tych obiektow.

**************************
**** Przeczytaj man ipcrm
**************************

Inna negatywna konsekwencja tego, ze zasoby IPC nie istnieja w
systemie plikow jest niemozliwosc jednoczesnego czekania na spelnienie
ktoregos z blokujacych warunkow (pojawienie sie odpowiedniego
komunikatu w jednej z kilku kolejek), jak jest to mozliwe, dzieki
funkcjom 'select' i 'poll' dla deskryptorow plikow (powiemy o tym 
w kolejnych laboratoriach).

1.4 Krotka charakterystyka mechanizmow POSIX-owych
    ----------------------------------------------

Zdefiniowane przez standard mechanizmy oparte sa na specjalnym
systemie plikow, ktory zwykle montuje sie pod
/dev/nazwa_mechanizmu. Umozliwia to zarowno przeprowadzenie operacji
usuwania obiektu w bardziej naturalny sposob, jak i wykonanie wielu
charakterystycznych dla deskryptorow operacji, w szczegolnosci
jednoczesnego oczekiwania na "zmultipleksowanych" kolejkach (funkcja
'select').

Mimo tych niewatpliwych zalet, wciaz sa to rozwiazania rzadko uzywane.

