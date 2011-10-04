Wątki
-----

1. Co to są wątki?
   ---------------

   Wątki są często nazywane lekkimi procesami, ponieważ są podobne do procesów uniksowych.

   Aby wyjaśnić różnicę między procesem i wątkiem należy zdefiniować pojęcie procesu.
   Proces zawiera wykonywalny program (kod programu), grupę zasobów takich jak 
   deskryptory plików, zarezerwowane bloki pamięci etc. oraz kontekst czyli pozycję
   wskaźnika stosu oraz wartość licznika rozkazów. 

   Wątki mogą współdzielić zasoby w obrębie jednego procesu. Mogą korzystać z tych samych 
   obszarów pamięci, pisać do tych samych plików. Jedyne co charakteryzuje wątek to jego
   kontekst. Na program napisany z użyciem wątków można spojrzeć jak na grupę zasobów 
   dzieloną przez kilka mini-procesów.

   W związku z tym, że wątki są "małe" w porównaniu do procesów, ich obsługa jest mniej 
   kosztowna dla systemu operacyjnego. Mniej pracochłonne jest utworzenie nowego wątku,
   bo nie trzeba przydzielać dla niego zasobów, mniej czasu zabiera też zakończenie
   wątku, bo system operacyjny nie musi po nim "sprzątać". Jednak główny zysk polega 
   na zmniejszeniu kosztu przełączania między wątkami w obrębie jednego procesu. 

   Przełączenie kontekstu, które normalnie odbywa się wiele razy na sekundę w jądrze 
   systemu operacyjnego, jest operacją kosztowną. Wiąże się ono ze zmianą struktur 
   systemowych, wczytaniem odpowiednich wartości do rejestrów procesora. Dlatego właśnie
   wymyślono mechanizm wątków, aby zredukować koszt pojedynczego przełączenia. 


2. Implementacja wątków
   --------------------

   Ze względu na sposób implementacji, wyróżniamy dwa typy wątków: wątki poziomu 
   użytkownika (user-space threads) i wątki poziomu jądra (kernel-space threads).

   Wątki poziomu użytkownika
   -------------------------

   Wątki poziomu użytkownika rezygnują z zarządzania wykonaniem przez jądro i robią 
   to same. Często jest to nazywane "wielowątkowością spółdzielczą", gdyż proces 
   definiuje zbiór wykonywalnych procedur, które są "wymieniane" przez operacje 
   na wskaźniku stosu. Zwykle każdy wątek "rezygnuje" z procesora przez bezpośrednie 
   wywołanie żądania wymiany (wysłanie sygnału i zainicjowanie mechanizmu zarządzającego)
   albo przez odebranie sygnału zegara systemowego. 

   Implementacje wątków poziomu użytkownika napotykają na liczne problemy, które trudno 
   obejść, np.:

      - problem "kradzenia" czasu wykonania innych wątków przez jeden wątek, 
      - brak sposobu wykorzystania możliwości SMP (ang. symmetric multiprocessing, 
        czyli obsługa wielu procesorów), 
      - oczekiwanie jednego z wątków na zakończenie blokującej operacji wejścia/wyjścia 
        powoduje, że inne wątki tego procesu też tracą swój czas wykonania. 

   Obecnie wątki poziomu użytkownika mogą szybciej dokonać wymiany niż wątki poziomu jądra,
   ale ciągły rozwój tych drugich powoduje, że ta różnica jest bardzo mała.

   Wątki poziomu jądra
   -------------------

   Wątki poziomu jądra są często implementowane poprzez dołączenie do każdego procesu 
   tabeli/listy jego wątków. W tym rozwiązaniu system zarządza każdym wątkiem wykorzystując 
   kwant czasu przyznany dla jego procesu-rodzica.

   Zaletą takiej implementacji jest zniknięcie zjawiska "kradzenia" czasu wykonania innych 
   wątków przez "zachłanny" wątek, bo zegar systemowy tyka niezależnie i system wydziedzicza
   "niesforny" wątek. Także blokowanie operacji wejścia/wyjścia nie jest już problemem.
   Ponadto przy poprawnym zakodowaniu programu, proces może automatycznie wyciągnąć korzyści 
   z istnienia SMP przyśpieszając swoje wykonanie przy każdym dodatkowym procesorze.


3. Różne implementacje wątków
   --------------------------

   Na zajęciach omówimy wątki zgodne ze standardem POSIX, czyli tak zwane "pthreads"
   (POSIX Threads). Standard ten w Linuksie jest zaimplementowany w bibliotekach
   LinuxThreads i NPTL (Native POSIX Threads Library). Obie implementują wątki na poziomie
   jądra. Aby sprawdzić, jaką implementację dostarcza nasz system, można wywołać

     getconf GNU_LIBPTHREAD_VERSION

   Programy używające wątków należy kompilować z opcją -pthread.

 * man pthreads

4. Jak utworzyć wątek?
   -------------------

   Do tworzenia wątków służy funkcja pthread_create.

   int pthread_create(pthread_t *thread, pthhread_attr_t *attr,
                      void * (* func)(void *), void *arg);
 * man pthread_create

   Funkcja ta tworzy wątek, którego kod wykonywalny znajduje się w funkcji podanej jako
   argument func. Wątek jest uruchamiany z parametrem arg, a informacja o wątku 
   (deskryptor wątku) jest umieszczana w strukturze thread.

   Podczas wywołania funkcji pthread_create tworzony jest wątek, który działa do chwili
   gdy funkcja będąca treścią wątku zostanie zakończona przez return, wywołanie funkcji
   pthread_exit lub po prostu zostanie wykonana ostatnia instrukcja funkcji.

   Argument attr zawiera atrybuty z jakimi chcemy stworzyć wątek. Do obsługi atrybutów
   wątku służą funkcje pthread_attr_*. Są to między innymi:

   int pthread_attr_init(pthread_attr_t *attr);
   int ptrehad_attr_destroy(pthread_attr_t *attr);
           Inicjowanie obiektu attr domyślnymi wartościami oraz niszczenie obiektu attr.

   int pthread_attr_setdetachstate(pthread_attr_t *attr, int state);
           Ustawianie atrybutu "detachstate". Możliwe wartości to PTHREAD_CREATE_JOINABLE
	   (domyślna) i PTHREAD_CREATE_DETACHED. Atrybut ten określa, czy można czekać
	   na zakończenie wątku.

 * man pthread_attr_setdetachstate

5. Jak czekać na zakończenie wątku?
   --------------------------------

   Podobnie jak w przypadku procesów program główny może oczekiwać na zakończenie się 
   wątków, które stworzył - służy do tego funkcja pthread_join.

   int pthread_join(pthread_t thread, void **status);

 * man pthread_join
   
   Co najwyżej jeden wątek może czekać na zakończenie jakiegoś wątku. Wywołanie join dla
   wątku, na który ktoś już czeka, zakończy się błędem.
   
6. Na jakie wątki można czekać?
   ----------------------------

   Tak jak powiedzieliśmy, czekać można na wątki, które zostały stworzone z atrybutem
   detachstate ustawionym na PTHREAD_CREATE_JOINABLE.

   Po uruchomieniu wątku można jeszcze zmienić wartość tego atrybutu poprzez wywołanie
   funkcji pthread_detach, która powoduje, że na wątek nie można będzie oczekiwać. Taki 
   wątek będziemy nazywać "odłączonym". 

   int pthread_detach(pthread_t thread);

 * man pthread_detach

   Jeśli wątek przejdzie w stan odłączony to zaraz po zakończeniu tego wątku czyszczone
   są wszystkie związane z nim struktury w pamięci. Jeśli jednak wątek nie przejdzie 
   do stanu odłączony to informacja o jego zakończeniu pozostanie w pamięci do czasu, 
   aż inny wątek wykona funkcję pthread_join dla zakończonego wątku.

 * Przeanalizuj program threads.c. Spróbuj wywołać go z parametrem i bez parametru, 
   zaobserwuj co stanie się jeśli wątki zostaną utworzone jako odłączone.

7. Synchronizacja wątków
   ---------------------

   Do synchronizacji wątków niezbyt dobrze nadają się dotychczas poznane mechanizmy, 
   dlatego istnieją osobne dla nich mechanizmy synchronizacyjne: muteksy i zmienne
   warunkowe.

   Muteksy
   -------

   Następujące funkcje są dostępne dla muteksów:

   int pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr);
   int pthread_mutex_destroy(pthread_mutex_t *mutex);
           Inicjowanie muteksa i niszczenie go.

   int pthread_mutex_lock(pthread_mutex_t *mutex);
   int pthread_mutex_trylock(pthread_mutex_t *mutex);
   int pthread_mutex_unlock(pthread_mutex_t *mutex);
           Semaforowe operacje na muteksie, objaśnione poniżej.

   Muteks może być w dwóch stanach: może być wolny albo może być w posiadaniu 
   pewnego wątku. Proces, który chce zdobyć muteks wykonuje operację lock. Jeśli muteks 
   jest wolny, to operacja kończy się powodzeniem i wątek staje się właścicielem muteksa. 
   Jeśli jednak muteks jest w posiadaniu innego wątku, to operacja powoduje wstrzymanie 
   wątku aż do momentu kiedy muteks będzie wolny. Przypomina to zatem operacje wejścia
   do monitora.

   Operacja trylock działa podobnie, z wyjątkiem tego, że zwraca błąd w sytuacji, gdy
   wątek miałby być wstrzymany.

   Wątek, który jest w posiadaniu muteksa może go oddać wykonując operację unlock.
   Powoduje to obudzenie pewnego wątku oczekującego na operacji lock, jeśli taki jest i 
   przyznanie mu muteksa. Jeśli nikt nie czeka, to muteks staje się wolny. 
   Przypomina to zatem operację wyjścia z monitora. Zwróćmy uwagę, że 
   unlock może wykonać tylko ten proces, który posiada muteks.
   
   Implementacja w Linuksie nie narzuca powyższego wymogu, ale używanie operacji lock
   i unlock w inny sposób powoduje nieprzenośność programu. 

 * man pthread_mutex_lock

   Biblioteka LinuxThreads udostępnia trzy rodzaje muteksów (rodzaj wybieramy ustawiając
   odpowiedni atrybut podczas tworzenia wątku), które różnie reagują w sytuacji, gdy
   wątek wykonuje lock na muteksie, który już posiada. Po szczegóły odsyłamy do

 * man ptrhead_mutexattr_settype

   Zmienne warunkowe
   -----------------

   Zmienne warunkowe to odpowiednik kolejek w monitorze. Operacje wait() i signal()
   to odpowiednio pthread_cond_wait i pthread_cond_signal.
   
   Aby zasymulować działanie monitora należy używać muteksa broniącego wejścia do
   monitora. Operacja wejścia do monitora odpowiada operacji lock na muteksie, 
   a operacja wyjścia - wykonaniu unlock. Zwróć uwagę, że jest to zgodne z tym,
   że unlock może wykonać jedynie proces, który jest w posiadaniu muteksa (czyli jest
   w monitorze). Operacje na zmiennych warunkowych monitora wykonuje się za pomocą 
   pthread_cond_wait oraz pthread_cond_signal. 

   int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *cond_attr);
   int pthread_cond_destroy(pthread_cond_t *cond);
           Inicjowanie zmiennej warunkowej (argument cond_attr jest zawsze NULL)
           i niszczenie jej.
   
   int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
           Atomowo zwalnia mutex (który musiał być wcześniej w posiadaniu wątku) i zawiesza
	   się na kolejce zmiennej warunkowej cond (chwilowe wyjście z monitora).

   int pthread_cond_signal(pthread_cond_t *cond);
           Jeśli nikt nie czekał na kolejce cond, to nic się nie dzieje. W przeciwnym
	   wypadku jeden z czekających wątków jest budzony.

 * man pthread_cond_wait

   Standardowy scenariusz użycia zmiennej warunkowej wygląda następująco:

   pthread_mutex_lock(&mut);
   ...
   while (warunek) 
       pthread_cond_wait(&cond, &mut);
   ...
   pthread_mutex_unlock(&mut);

   Zwróć uwagę na użycie pętli zamiast warunku! Jest to spowodowane tym, że 
   monitory gwarantują zwalnianie procesów oczekujących na zmiennych warunkowych
   przed wpuszczeniem nowych procesów do monitorów - dlatego instrukcja if jest
   wystarczająca. W przypadku muteksów takiej gwarancji nie ma, więc po zwolnieniu 
   muteksa do monitora może wejść inny proces, "zepsuć" warunek, na który oczekiwał 
   budzony proces. Obudzony proces musi zatem ponownie sprawdzić, czy warunek jest 
   spełniony - stąd użycie pętli.

 * Przeanalizuj program prod_kon.c. Zwróć uwagę na funkcje put() i get() w których odbywa
   się synchronizacja między wątkami producenta i konsumenta.

   Oprócz powyższych operacji dostępne są również dwie dodatkowe:
   pthread_cond_timedwait - czeka tylko przez określony czas,
   pthread_cond_broadcast - budzi wszystkie wątki czekające w kolejce.
   
 * Przeanalizuj program timeout.c zwróć uwagę na użycie funkcji pthread_cond_timedwait().

8. Jak wymusić zakończenie wątku?
   ------------------------------

   Aby wymusić zakończenie innego wątku należy wywołać funkcję pthread_cancel. Wątek
   może zignorować nasze żądanie, zakończyć się natychmiast lub odłożyć zakończenie na
   bardziej dogodny moment.
   
   int pthread_cancel(pthread_t thread);
           Wymuszenie zakończenia wątku thread. Ewentualne zakończenie wątku będzie
	   równoważne wykonaniu pthread_exit(PTHREAD_CANCELED).

   int pthread_setcancelstate(int state, int *oldstate);
           Ustawienie reakcji na żądanie zakończenia wątku: PTHREAD_CANCEL_ENABLE (domyślnie)
           lub PTHREAD_CANCEL_DISABLE (ignorowanie żądań zakończenia).

   int pthread_setcanceltype(int type, int *oldtype);
           Ustawienie trybu kończenia wątku: PTHREAD_CANCEL_ASYNCHRONOUS (asynchronicznie),
	   lub PTHREAD_CANCEL_DEFERRED (tryb "deferred", domyślnie).

 * man pthread_cancel

   Jeśli wątek jest w stanie, w którym nie można go przerwać to żądanie przerwania go
   funkcją pthread_cancel jest wstrzymywane do czasu, kiedy będzie można zakończyć 
   wątek.

9. Dwa tryby kończenia wątków
   --------------------------

   Wywołanie funkcji pthread_cancel powoduje różną reakcję kończonego wątku w zależności
   od trybu zakończenia wątku w jakim znajduje się kończony wątek.

   W trybie asynchronicznym wątek jest kończony natychmiast w momencie wywołania 
   pthread_cancel przez inny wątek.
   W trybie "deferred" wątek nie jest kończony od razu. Kończy się on dopiero w momencie
   dotarcia do tzw. "cancellation point". Cancellation point są funkcje: pthread_join,
   pthread_cond_(timed)wait, pthread_testcancel, sem_wait, sigwait.
   Cancellation point jest również większość wywołań funkcji systemowych (w zasadzie
   wszystkie blokujące), należy jednak uważać aby wykorzystywać zgodne z wątkami 
   biblioteki.
   Funkcje, które na pewno są przygotowane do pracy z wątkami to: read, write, select,
   accept, sendto, recvfrom, connect.
   
   Uwaga: funkcja pthread_mutex_lock nie jest cancellation point.
   
   Wątek może też sztucznie wejść do cancellation point wywołując funkcję 
   pthread_testcancel.

   void pthread_testcancel(void);

10. Bloki czyszczące
    ----------------

   Aby nie dopuścić do blokad wynikających z przerwania wątku, który zablokował
   jakieś zasoby (semafory, pliki itp.) wymyślono mechanizm bloków czyszczących.
   Po zablokowaniu jakiegoś zasobu rejestruje się funkcję, która powinna zostać 
   wykonana jeśli wykonanie wątku zostanie zakłócone. Robi się to przy pomocy funkcji 
   pthread_cleanup_push.

   pthread_cleanup_push(void (*func)(void *), void *arg);

 * man pthread_cleanup_push
   
   Aby odrejestrować funkcję czyszczącą np. po odblokowaniu zasobu należy użyć 
   funkcji pthread_cleanup_pop.

   pthread_cleanup_pop(int exec);

   Standardowe użycie bloków czyszczących mogłoby wyglądać następująco:

   pthread_mutex_lock(&mut);
   pthread_cleanup_push(pthread_mutex_unlock, &mut);
   ...
   pthread_mutex_unlock(&mut);
   pthread_cleanup_pop(FALSE);  

   Dwie ostatnie operacje można połączyć w jedno wywołanie funkcji pthread_cleanup_pop
   z argumentem TRUE. Takie wywołanie oznacza, że podczas odrejestrowywania blok 
   zostanie wykonany, zatem w tym przykładzie zostanie odblokowany semafor.

   pthread_mutex_lock(&mut);
   pthread_cleanup_push(pthread_mutex_unlock, &mut);
   ...
   //   pthread_mutex_unlock(&mut);
   pthread_cleanup_pop(TRUE);

   Bloki czyszczące są rejestrowane na stosie, więc nie ma przeszkód aby stosować 
   zagnieżdżone bloki czyszczące. W sytuacji załamania wątku bloki zostaną wykonane
   w kolejności odwrotnej do ich rejestrowania.

 * Przeanalizuj program cancel.c. Zwróć uwagę na użycie bloków czyszczących oraz świadome
   wejście do cancellation point przy użyciu pthread_testcancel.


-----
Ostatnia aktualizacja:
25.11.2008, Tomasz Idziaszek
