program test_pierwszosci;
{ Program, ktory wczytuje ze standardowego wejscia liczbe 32-bitowa
  i mowi, czy jest liczba pierwsza.
  
  Program jest swiadomie napisany w nie najlepszym stylu. Prosze tak nie pisac
  swoich programow. :)
}

var m: integer;

function czy_pierwsza(b: longint): boolean;
var b_ori: single;
    i: longint;
begin
    czy_pierwsza := true;
    b_ori := b;
    i := 2;
    repeat
        if (b mod i) = 0 then begin
            czy_pierwsza := false;
            exit;
        end;
        inc(i);
    until i > sqrt(b_ori);
end;

begin
    readln(m);
    writeln(czy_pierwsza(m));
end.