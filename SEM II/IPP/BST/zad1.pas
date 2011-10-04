program zad1;

uses Bst;


var
  i : integer;
  d : drzewo;
begin
  d := nil; // tworzenie pustego drzewa

  while not eof do begin
    readln(i);
    insert(d, i);
  end;
  
  while d <> nil do writeLn(removeMin(d));
end. 
