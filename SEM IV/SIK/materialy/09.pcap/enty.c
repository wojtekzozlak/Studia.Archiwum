#include <pcap.h>
#include <stdlib.h>

/* funkcja, ktora przy przechodzeniu po pliku nic nie robi */
void* nic(user, nagl, tekst)
{
  return NULL;
}

int main (int argc, char** argv)
{
  char ebuf[PCAP_ERRBUF_SIZE];
  pcap_t* plik=NULL;
  struct pcap_pkthdr naglowek;
  int ile=-1;

  if (argc!=3)
	 {
		printf( "Zla liczba argumentow\n" );
		printf( "Podaj nazwe pliku zrzutu i liczbe pakietow do opuszczenia\n" );
		exit(0);
	 }

  if (!(plik=pcap_open_offline(argv[1], ebuf)))
	 {
		perror(ebuf);
		exit(1);
	 }

  /* opusc pakiety */
  ile = atoi(argv[2]);
  if (pcap_dispatch(plik, ile, (pcap_handler)nic, 
                    "zgadnij, gdzie sie pojawie")!=ile) 
	 {
		fprintf(stderr, "(1)W pliku %s nie ma tyle pakietow\n", argv[1]);
		exit(1);
	 }

  /* wypisz pakiet */
  if (!(pcap_next(plik, &naglowek)))
	 {
		fprintf(stderr, "(2)W pliku %s nie ma tyle pakietow\n", argv[1]);
		exit(1);
	 }
  printf( "--- Pakiet nr %d ---\n", ile+1 );
  printf( "Znacznik czasu:%ld.%ld \n", naglowek.ts.tv_sec, 
                                       naglowek.ts.tv_usec);
  printf( "Tyle oktetow znajduje sie w pliku: %d\n", naglowek.caplen);
  printf( "Tyle oktetow zawieral oryginalny pakiet: %d\n", naglowek.len);

  pcap_close(plik);
  return 0;
}
