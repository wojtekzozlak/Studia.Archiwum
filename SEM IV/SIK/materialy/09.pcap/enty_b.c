#include <pcap.h>
#include <stdlib.h>

/* Funkcja, ktora przy przechodzeniu po pliku nic nie robi. */
static void*
nothing (u_char *user, const struct pcap_pkthdr *hdr, const u_char *data)
{
  return NULL;
}

int
main (int argc, char *argv[])
{
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t *descriptor;
  struct pcap_pkthdr header;
  int count;

  if (argc != 3)
    {
      printf ("Zla liczba argumentow.\n"
	      "Podaj nazwe pliku zrzutu i liczbe pakietow do opuszczenia.\n");
      return 0;
    }

  descriptor = pcap_open_offline (argv[1], errbuf);
  if (descriptor == NULL)
    {
      fprintf (stderr, "%s\n", errbuf);
      exit (EXIT_FAILURE);
    }

  /* Pomin pakiety. */
  count = pcap_dispatch (descriptor, atoi (argv[2]), (pcap_handler) nothing,
		         (u_char*) "Zgadnij, gdzie sie pojawie?");
  if (count < 0)
    {
      pcap_perror (descriptor, "pcap_dispatch");
      pcap_close (descriptor);
      exit (EXIT_FAILURE);
    }
  else if (count == 0)
    {
      fprintf (stderr, "(1) W pliku %s nie ma tylu pakietow.\n", argv[1]);
      pcap_close (descriptor);
      exit (EXIT_FAILURE);
    }
  else
    {
      printf ("Liczba pominietych pakietów: %d\n", count);
    }

  /* Wypisz kolejny pakiet. */
  if (pcap_next (descriptor, &header) == NULL)
    {
      fprintf (stderr, "(2) W pliku %s nie ma tylu pakietow.\n", argv[1]);
      pcap_close (descriptor);
      exit (EXIT_FAILURE);
    }

  printf ("--- Pakiet nr %d ---\n", count + 1);
  printf ("Znacznik czasu: %ld.%ld\n", header.ts.tv_sec, header.ts.tv_usec);
  printf ("Tyle oktetow znajduje sie w pliku: %u\n", header.caplen);
  printf ("Tyle oktetow zawieral oryginalny pakiet: %u\n", header.len);

  pcap_close (descriptor);
  return 0;
}
