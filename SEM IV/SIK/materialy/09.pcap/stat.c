#include <pcap.h>


int main (int argc, char** argv)
{
  char ebuf[PCAP_ERRBUF_SIZE];
  pcap_t* plik=NULL;
  int snaplen=-1;

  if (argc!=2)
	 {
		printf( "Zla liczba argumentow\n" );
		printf( "Podaj nazwe pliku zrzutu\n" );
		exit(0);
	 }

  if (!(plik=pcap_open_offline(argv[1], ebuf)))
	 {
		perror(ebuf);
		exit(1);
	 }

  printf( "Wersja pcap, z ktora zapisano plik: %d.%d\n", 
          pcap_major_version(plik),
          pcap_minor_version(plik));
  if (pcap_is_swapped(plik))
	 printf( "Plik ma odwrocony porzadek bajtow\n" );
  else
	 printf( "Plik ma porzadek bajtow taki jak w komputerze\n" );
  snaplen = pcap_snapshot(plik);
  printf( "Maksymalna dlugosc zapisywanych danych pakietu: %d\n", 
          snaplen);

  pcap_close(plik);
  return 0;
}
