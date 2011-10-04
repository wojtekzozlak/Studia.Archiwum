#include <pcap.h>
#include <stdlib.h>

int
main (int argc, char *argv[])
{
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t *descriptor;
  int snaplen;

  if (argc < 2)
    {
      printf ("Uzyj: %s plik_zrzutu\n", argv[0]);
      return 0;
    }

  descriptor = pcap_open_offline (argv[1], errbuf);
  if (descriptor == NULL)
    {
      fprintf (stderr, "%s\n", errbuf);
      exit (EXIT_FAILURE);
    }

  printf ("Wersja pcap, z ktora zapisano plik: %d.%d\n",
	  pcap_major_version (descriptor), pcap_minor_version (descriptor));

  if (pcap_is_swapped (descriptor))
    printf ("Plik ma odwrocony porzadek bajtow.\n");
  else
    printf ("Plik ma porzadek bajtow taki jak w komputerze.\n");

  snaplen = pcap_snapshot (descriptor);
  printf ("Maksymalna dlugosc zapisywanych danych pakietu: %d\n", snaplen);

  pcap_close (descriptor);
  return 0;
}
