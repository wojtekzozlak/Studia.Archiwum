W±tki
-----

1. Co to s± w±tki?
   ---------------

   W±tki s± czêsto nazywane lekkimi procesami, poniewa¿ s± podobne do procesów uniksowych.

   Aby wyja¶niæ ró¿nicê miêdzy procesem i w±tkiem nale¿y zdefiniowaæ pojêcie procesu.
   Proces zawiera wykonywalny program (kod programu), grupê zasobów takich jak 
   deskryptory plików, zarezerwowane bloki pamiêci etc. oraz kontekst czyli pozycjê
   wska¼nika stosu oraz warto¶æ licznika rozkazów. 

   W±tki mog± wspó³dzieliæ zasoby w obrêbie jednego procesu. Mog± korzystaæ z tych samych 
   obszarów pamiêci, pisaæ do tych samych plików. Jedyne co charakteryzuje w±tek to jego
   kontekst. Na program napisany z u¿yciem w±tków mo¿na spojrzeæ jak na grupê zasobów 
   dzielon± przez kilka mini-procesów.

   W zwi±zku z tym, ¿e w±tki s± "ma³e" w porównaniu do procesów, ich obs³uga jest mniej 
   kosztowna dla systemu operacyjnego. Mniej pracoch³onne jest utworzenie nowego w±tku,
   bo nie trzeba przydzielaæ dla niego zasobów, mniej czasu zabiera te¿ zakoñczenie
   w±tku, bo system operacyjny nie musi po nim "sprz±taæ". Jednak g³ówny zysk polega 
   na zmniejszeniu kosztu prze³±czania miêdzy w±tkami w obrêbie jednego procesu. 

   Prze³±czenie kontekstu, które normalnie odbywa siê wiele razy na sekundê w j±drze 
   systemu operacyjnego, jest operacj± kosztown±. Wi±¿e siê ono ze zmian± struktur 
   systemowych, wczytaniem odpowiednich warto¶ci do rejestrów procesora. Dlatego w³a¶nie
   wymy¶lono mechanizm w±tków, aby zredukowaæ koszt pojedynczego prze³±czenia. 


2. Implementacja w±tków
   --------------------

   Ze wzglêdu na sposób implementacji, wyró¿niamy dwa typy w±tków: w±tki poziomu 
   u¿ytkownika (user-space threads) i w±tki poziomu j±dra (kernel-space threads).

   W±tki poziomu u¿ytkownika
   -------------------------

   W±tki poziomu u¿ytkownika rezygnuj± z zarz±dzania wykonaniem przez j±dro i robi± 
   to same. Czêsto jest to nazywane "wielow±tkowo¶ci± spó³dzielcz±", gdy¿ proces 
   definiuje zbiór wykonywalnych procedur, które s± "wymieniane" przez operacje 
   na wska¼niku stosu. Zwykle ka¿dy w±tek "rezygnuje" z procesora przez bezpo¶rednie 
   wywo³anie ¿±dania wymiany (wys³anie sygna³u i zainicjowanie mechanizmu zarz±dzaj±cego)
   albo przez odebranie sygna³u zegara systemowego. 

   Implementacje w±tków poziomu u¿ytkownika napotykaj± na liczne problemy, które trudno 
   obej¶æ, np.:

      - problem "kradzenia" czasu wykonania innych w±tków przez jeden w±tek, 
      - brak sposobu wykorzystania mo¿liwo¶ci SMP (ang. symmetric multiprocessing, 
        czyli obs³uga wielu procesorów), 
      - oczekiwanie jednego z w±tków na zakoñczenie blokuj±cej operacji wej¶cia/wyj¶cia 
        powoduje, ¿e inne w±tki tego procesu te¿ trac± swój czas wykonania. 

   Obecnie w±tki poziomu u¿ytkownika mog± szybciej dokonaæ wymiany ni¿ w±tki poziomu j±dra,
   ale ci±g³y rozwój tych drugich powoduje, ¿e ta ró¿nica jest bardzo ma³a.

   W±tki poziomu j±dra
   -------------------

   W±tki poziomu j±dra s± czêsto implementowane poprzez do³±czenie do ka¿dego procesu 
   tabeli/listy jego w±tków. W tym rozwi±zaniu system zarz±dza ka¿dym w±tkiem wykorzystuj±c 
   kwant czasu przyznany dla jego procesu-rodzica.

   Zalet± takiej implementacji jest znikniêcie zjawiska "kradzenia" czasu wykonania innych 
   w±tków przez "zach³anny" w±tek, bo zegar systemowy tyka niezale¿nie i system wydziedzicza
   "niesforny" w±tek. Tak¿e blokowanie operacji wej¶cia/wyj¶cia nie jest ju¿ problemem.
   Ponadto przy poprawnym zakodowaniu programu, proces mo¿e automatycznie wyci±gn±æ korzy¶ci 
   z istnienia SMP przy¶pieszaj±c swoje wykonanie przy ka¿dym dodatkowym procesorze.


3. Ró¿ne implementacje w±tków
   --------------------------

   Na zajêciach omówimy w±tki zgodne ze standardem POSIX, czyli tak zwane "pthreads"
   (POSIX Threads). Standard ten w Linuksie jest zaimplementowany w bibliotekach
   LinuxThreads i NPTL (Native POSIX Threads Library). Obie implementuj± w±tki na poziomie
   j±dra. Aby sprawdziæ, jak± implementacjê dostarcza nasz system, mo¿na wywo³aæ

     getconf GNU_LIBPTHREAD_VERSION

   Programy u¿ywaj±ce w±tków nale¿y kompilowaæ z opcj± -pthread.

 * man pthreads

4. Jak utworzyæ w±tek?
   -------------------

   Do tworzenia w±tków s³u¿y funkcja pthread_create.

   int pthread_create(pthread_t *thread, pthhread_attr_t *attr,
                      void * (* func)(void *), void *arg);
 * man pthread_create

   Funkcja ta tworzy w±tek, którego kod wykonywalny znajduje siê w funkcji podanej jako
   argument func. W±tek jest uruchamiany z parametrem arg, a informacja o w±tku 
   (deskryptor w±tku) jest umieszczana w strukturze thread.

   Podczas wywo³ania funkcji pthread_create tworzony jest w±tek, który dzia³a do chwili
   gdy funkcja bêd±ca tre¶ci± w±tku zostanie zakoñczona przez return, wywo³anie funkcji
   pthread_exit lub po prostu zostanie wykonana ostatnia instrukcja funkcji.

   Argument attr zawiera atrybuty z jakimi chcemy stworzyæ w±tek. Do obs³ugi atrybutów
   w±tku s³u¿± funkcje pthread_attr_*. S± to miêdzy innymi:

   int pthread_attr_init(pthread_attr_t *attr);
   int ptrehad_attr_destroy(pthread_attr_t *attr);
           Inicjowanie obiektu attr domy¶lnymi warto¶ciami oraz niszczenie obiektu attr.

   int pthread_attr_setdetachstate(pthread_attr_t *attr, int state);
           Ustawianie atrybutu "detachstate". Mo¿liwe warto¶ci to PTHREAD_CREATE_JOINABLE
	   (domy¶lna) i PTHREAD_CREATE_DETACHED. Atrybut ten okre¶la, czy mo¿na czekaæ
	   na zakoñczenie w±tku.

 * man pthread_attr_setdetachstate

5. Jak czekaæ na zakoñczenie w±tku?
   --------------------------------

   Podobnie jak w przypadku procesów program g³ówny mo¿e oczekiwaæ na zakoñczenie siê 
   w±tków, które stworzy³ - s³u¿y do tego funkcja pthread_join.

   int pthread_join(pthread_t thread, void **status);

 * man pthread_join
   
   Co najwy¿ej jeden w±tek mo¿e czekaæ na zakoñczenie jakiego¶ w±tku. Wywo³anie join dla
   w±tku, na który kto¶ ju¿ czeka, zakoñczy siê b³êdem.
   
6. Na jakie w±tki mo¿na czekaæ?
   ----------------------------

   Tak jak powiedzieli¶my, czekaæ mo¿na na w±tki, które zosta³y stworzone z atrybutem
   detachstate ustawionym na PTHREAD_CREATE_JOINABLE.

   Po uruchomieniu w±tku mo¿na jeszcze zmieniæ warto¶æ tego atrybutu poprzez wywo³anie
   funkcji pthread_detach, która powoduje, ¿e na w±tek nie mo¿na bêdzie oczekiwaæ. Taki 
   w±tek bêdziemy nazywaæ "od³±czonym". 

   int pthread_detach(pthread_t thread);

 * man pthread_detach

   Je¶li w±tek przejdzie w stan od³±czony to zaraz po zakoñczeniu tego w±tku czyszczone
   s± wszystkie zwi±zane z nim struktury w pamiêci. Je¶li jednak w±tek nie przejdzie 
   do stanu od³±czony to informacja o jego zakoñczeniu pozostanie w pamiêci do czasu, 
   a¿ inny w±tek wykona funkcjê pthread_join dla zakoñczonego w±tku.

 * Przeanalizuj program threads.c. Spróbuj wywo³aæ go z parametrem i bez parametru, 
   zaobserwuj co stanie siê je¶li w±tki zostan± utworzone jako od³±czone.

7. Synchronizacja w±tków
   ---------------------

   Do synchronizacji w±tków niezbyt dobrze nadaj± siê dotychczas poznane mechanizmy, 
   dlatego istniej± osobne dla nich mechanizmy synchronizacyjne: muteksy i zmienne
   warunkowe.

   Muteksy
   -------

   Nastêpuj±ce funkcje s± dostêpne dla muteksów:

   int pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr);
   int pthread_mutex_destroy(pthread_mutex_t *mutex);
           Inicjowanie muteksa i niszczenie go.

   int pthread_mutex_lock(pthread_mutex_t *mutex);
   int pthread_mutex_trylock(pthread_mutex_t *mutex);
   int pthread_mutex_unlock(pthread_mutex_t *mutex);
           Semaforowe operacje na muteksie, obja¶nione poni¿ej.

   Muteks mo¿e byæ w dwóch stanach: mo¿e byæ wolny albo mo¿e byæ w posiadaniu 
   pewnego w±tku. Proces, który chce zdobyæ muteks wykonuje operacjê lock. Je¶li muteks 
   jest wolny, to operacja koñczy siê powodzeniem i w±tek staje siê w³a¶cicielem muteksa. 
   Je¶li jednak muteks jest w posiadaniu innego w±tku, to operacja powoduje wstrzymanie 
   w±tku a¿ do momentu kiedy muteks bêdzie wolny. Przypomina to zatem operacje wej¶cia
   do monitora.

   Operacja trylock dzia³a podobnie, z wyj±tkiem tego, ¿e zwraca b³±d w sytuacji, gdy
   w±tek mia³by byæ wstrzymany.

   W±tek, który jest w posiadaniu muteksa mo¿e go oddaæ wykonuj±c operacjê unlock.
   Powoduje to obudzenie pewnego w±tku oczekuj±cego na operacji lock, je¶li taki jest i 
   przyznanie mu muteksa. Je¶li nikt nie czeka, to muteks staje siê wolny. 
   Przypomina to zatem operacjê wyj¶cia z monitora. Zwróæmy uwagê, ¿e 
   unlock mo¿e wykonaæ tylko ten proces, który posiada muteks.
   
   Implementacja w Linuksie nie narzuca powy¿szego wymogu, ale u¿ywanie operacji lock
   i unlock w inny sposób powoduje nieprzeno¶no¶æ programu. 

 * man pthread_mutex_lock

   Biblioteka LinuxThreads udostêpnia trzy rodzaje muteksów (rodzaj wybieramy ustawiaj±c
   odpowiedni atrybut podczas tworzenia w±tku), które ró¿nie reaguj± w sytuacji, gdy
   w±tek wykonuje lock na muteksie, który ju¿ posiada. Po szczegó³y odsy³amy do

 * man ptrhead_mutexattr_settype

   Zmienne warunkowe
   -----------------

   Zmienne warunkowe to odpowiednik kolejek w monitorze. Operacje wait() i signal()
   to odpowiednio pthread_cond_wait i pthread_cond_signal.
   
   Aby zasymulowaæ dzia³anie monitora nale¿y u¿ywaæ muteksa broni±cego wej¶cia do
   monitora. Operacja wej¶cia do monitora odpowiada operacji lock na muteksie, 
   a operacja wyj¶cia - wykonaniu unlock. Zwróæ uwagê, ¿e jest to zgodne z tym,
   ¿e unlock mo¿e wykonaæ jedynie proces, który jest w posiadaniu muteksa (czyli jest
   w monitorze). Operacje na zmiennych warunkowych monitora wykonuje siê za pomoc± 
   pthread_cond_wait oraz pthread_cond_signal. 

   int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *cond_attr);
   int pthread_cond_destroy(pthread_cond_t *cond);
           Inicjowanie zmiennej warunkowej (argument cond_attr jest zawsze NULL)
           i niszczenie jej.
   
   int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
           Atomowo zwalnia mutex (który musia³ byæ wcze¶niej w posiadaniu w±tku) i zawiesza
	   siê na kolejce zmiennej warunkowej cond (chwilowe wyj¶cie z monitora).

   int pthread_cond_signal(pthread_cond_t *cond);
           Je¶li nikt nie czeka³ na kolejce cond, to nic siê nie dzieje. W przeciwnym
	   wypadku jeden z czekaj±cych w±tków jest budzony.

 * man pthread_cond_wait

   Standardowy scenariusz u¿ycia zmiennej warunkowej wygl±da nastêpuj±co:

   pthread_mutex_lock(&mut);
   ...
   while (warunek) 
       pthread_cond_wait(&cond, &mut);
   ...
   pthread_mutex_unlock(&mut);

   Zwróæ uwagê na u¿ycie pêtli zamiast warunku! Jest to spowodowane tym, ¿e 
   monitory gwarantuj± zwalnianie procesów oczekuj±cych na zmiennych warunkowych
   przed wpuszczeniem nowych procesów do monitorów - dlatego instrukcja if jest
   wystarczaj±ca. W przypadku muteksów takiej gwarancji nie ma, wiêc po zwolnieniu 
   muteksa do monitora mo¿e wej¶æ inny proces, "zepsuæ" warunek, na który oczekiwa³ 
   budzony proces. Obudzony proces musi zatem ponownie sprawdziæ, czy warunek jest 
   spe³niony - st±d u¿ycie pêtli.

 * Przeanalizuj program prod_kon.c. Zwróæ uwagê na funkcje put() i get() w których odbywa
   siê synchronizacja miêdzy w±tkami producenta i konsumenta.

   Oprócz powy¿szych operacji dostêpne s± równie¿ dwie dodatkowe:
   pthread_cond_timedwait - czeka tylko przez okre¶lony czas,
   pthread_cond_broadcast - budzi wszystkie w±tki czekaj±ce w kolejce.
   
 * Przeanalizuj program timeout.c zwróæ uwagê na u¿ycie funkcji pthread_cond_timedwait().

8. Jak wymusiæ zakoñczenie w±tku?
   ------------------------------

   Aby wymusiæ zakoñczenie innego w±tku nale¿y wywo³aæ funkcjê pthread_cancel. W±tek
   mo¿e zignorowaæ nasze ¿±danie, zakoñczyæ siê natychmiast lub od³o¿yæ zakoñczenie na
   bardziej dogodny moment.
   
   int pthread_cancel(pthread_t thread);
           Wymuszenie zakoñczenia w±tku thread. Ewentualne zakoñczenie w±tku bêdzie
	   równowa¿ne wykonaniu pthread_exit(PTHREAD_CANCELED).

   int pthread_setcancelstate(int state, int *oldstate);
           Ustawienie reakcji na ¿±danie zakoñczenia w±tku: PTHREAD_CANCEL_ENABLE (domy¶lnie)
           lub PTHREAD_CANCEL_DISABLE (ignorowanie ¿±dañ zakoñczenia).

   int pthread_setcanceltype(int type, int *oldtype);
           Ustawienie trybu koñczenia w±tku: PTHREAD_CANCEL_ASYNCHRONOUS (asynchronicznie),
	   lub PTHREAD_CANCEL_DEFERRED (tryb "deferred", domy¶lnie).

 * man pthread_cancel

   Je¶li w±tek jest w stanie, w którym nie mo¿na go przerwaæ to ¿±danie przerwania go
   funkcj± pthread_cancel jest wstrzymywane do czasu, kiedy bêdzie mo¿na zakoñczyæ 
   w±tek.

9. Dwa tryby koñczenia w±tków
   --------------------------

   Wywo³anie funkcji pthread_cancel powoduje ró¿n± reakcjê koñczonego w±tku w zale¿no¶ci
   od trybu zakoñczenia w±tku w jakim znajduje siê koñczony w±tek.

   W trybie asynchronicznym w±tek jest koñczony natychmiast w momencie wywo³ania 
   pthread_cancel przez inny w±tek.
   W trybie "deferred" w±tek nie jest koñczony od razu. Koñczy siê on dopiero w momencie
   dotarcia do tzw. "cancellation point". Cancellation point s± funkcje: pthread_join,
   pthread_cond_(timed)wait, pthread_testcancel, sem_wait, sigwait.
   Cancellation point jest równie¿ wiêkszo¶æ wywo³añ funkcji systemowych (w zasadzie
   wszystkie blokuj±ce), nale¿y jednak uwa¿aæ aby wykorzystywaæ zgodne z w±tkami 
   biblioteki.
   Funkcje, które na pewno s± przygotowane do pracy z w±tkami to: read, write, select,
   accept, sendto, recvfrom, connect.
   
   Uwaga: funkcja pthread_mutex_lock nie jest cancellation point.
   
   W±tek mo¿e te¿ sztucznie wej¶æ do cancellation point wywo³uj±c funkcjê 
   pthread_testcancel.

   void pthread_testcancel(void);

10. Bloki czyszcz±ce
    ----------------

   Aby nie dopu¶ciæ do blokad wynikaj±cych z przerwania w±tku, który zablokowa³
   jakie¶ zasoby (semafory, pliki itp.) wymy¶lono mechanizm bloków czyszcz±cych.
   Po zablokowaniu jakiego¶ zasobu rejestruje siê funkcjê, która powinna zostaæ 
   wykonana je¶li wykonanie w±tku zostanie zak³ócone. Robi siê to przy pomocy funkcji 
   pthread_cleanup_push.

   pthread_cleanup_push(void (*func)(void *), void *arg);

 * man pthread_cleanup_push
   
   Aby odrejestrowaæ funkcjê czyszcz±c± np. po odblokowaniu zasobu nale¿y u¿yæ 
   funkcji pthread_cleanup_pop.

   pthread_cleanup_pop(int exec);

   Standardowe u¿ycie bloków czyszcz±cych mog³oby wygl±daæ nastêpuj±co:

   pthread_mutex_lock(&mut);
   pthread_cleanup_push(pthread_mutex_unlock, &mut);
   ...
   pthread_mutex_unlock(&mut);
   pthread_cleanup_pop(FALSE);  

   Dwie ostatnie operacje mo¿na po³±czyæ w jedno wywo³anie funkcji pthread_cleanup_pop
   z argumentem TRUE. Takie wywo³anie oznacza, ¿e podczas odrejestrowywania blok 
   zostanie wykonany, zatem w tym przyk³adzie zostanie odblokowany semafor.

   pthread_mutex_lock(&mut);
   pthread_cleanup_push(pthread_mutex_unlock, &mut);
   ...
   //   pthread_mutex_unlock(&mut);
   pthread_cleanup_pop(TRUE);

   Bloki czyszcz±ce s± rejestrowane na stosie, wiêc nie ma przeszkód aby stosowaæ 
   zagnie¿d¿one bloki czyszcz±ce. W sytuacji za³amania w±tku bloki zostan± wykonane
   w kolejno¶ci odwrotnej do ich rejestrowania.

 * Przeanalizuj program cancel.c. Zwróæ uwagê na u¿ycie bloków czyszcz±cych oraz ¶wiadome
   wej¶cie do cancellation point przy u¿yciu pthread_testcancel.


-----
Ostatnia aktualizacja:
25.11.2008, Tomasz Idziaszek
