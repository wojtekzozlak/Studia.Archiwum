program zliczanie;
{ Program, ktory wczytuje stringa ze standardowego wejscia, zlicza ilosc wystapien
  poszczegolnych znakow, ale tylko tych o numerach ASCII pomiedzy 48 oraz 122,
  a nastepnie wypisuje fajna sume. Fajna suma to wyartosc 1*a_48 + 2*a_49 + ...,
  gdzie a_i to licza wystapien i-tego znaku.
  
  Program jest swiadomie napisany w nie najlepszym stylu. Prosze tak nie pisac
  swoich programow. :)
}

procedure zliczaj(s: string); forward;

procedure wczytaj_i_policz;
var s: string;
begin
    writeln('wczytuje');
    readln(s);
    writeln('licze');
    zliczaj(s);
end;

procedure zliczaj2;
begin
    writeln('TODO');
    halt;
end;

procedure zliczaj(s: string);
const ILOSC_LITER: integer = 75;
      PIERWSZA_LITERA: integer = ord('0');
var a: array[1..75] of longint;
    i, wynik, znak: integer;
begin
    for i := 1 to ILOSC_LITER do
        a[i] := 0;
    for i := 1 to length(s) do begin
        znak := ord(s[i]);
        if (znak >= PIERWSZA_LITERA)
                and (znak < PIERWSZA_LITERA+ILOSC_LITER) then
            a[znak] := a[znak] + 1;
    end;
    wynik := 0;
    for i := 1 to ILOSC_LITER do
        wynik := wynik + i*a[i];
    writeln('fajna suma wynosi ', wynik);
end;
 
begin
    writeln('poczatek');
    wczytaj_i_policz;
    writeln('koniec');
end.
