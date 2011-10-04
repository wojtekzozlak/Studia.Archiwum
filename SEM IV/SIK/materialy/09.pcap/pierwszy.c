#include <pcap.h>


int main (int argc, char** argv)
{
  char ebuf[PCAP_ERRBUF_SIZE];
  pcap_t* plik=NULL;
  struct pcap_pkthdr naglowek;

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

  if (!(pcap_next(plik, &naglowek)))
	 {
		fprintf(stderr, "W pliku %s nie ma pakietow\n", argv[1]);
		exit(1);
	 }
  printf( "--- Pierwszy pakiet ---\n" );
  printf( "Znacznik czasu:%ld.%ld \n", naglowek.ts.tv_sec, 
                                       naglowek.ts.tv_usec); 
  printf( "Tyle oktetow pakietu znajduje sie w pliku: %d\n", naglowek.caplen);
  printf( "Tyle oktetow zawieral oryginalny pakiet: %d\n", naglowek.len);

  pcap_close(plik);
  return 0;
}
