#include <pcap.h>
#include <stdlib.h>

int
main (int argc, char *argv[])
{
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t *descriptor;
  struct pcap_pkthdr header;

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

  if (!pcap_next (descriptor, &header))
    {
      fprintf (stderr, "W pliku %s nie ma pakietow.\n", argv[1]);
      pcap_close (descriptor);
      exit (EXIT_FAILURE);
    }

  printf ("--- Pierwszy pakiet ---\n");
  printf ("Znacznik czasu: %ld.%ld\n", header.ts.tv_sec, header.ts.tv_usec);
  printf ("Tyle oktetow pakietu znajduje sie w pliku: %u\n", header.caplen);
  printf ("Tyle oktetow zawieral oryginalny pakiet: %u\n", header.len);

  pcap_close (descriptor);
  return 0;
}
