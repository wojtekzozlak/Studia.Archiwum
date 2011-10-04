unit Bst;


interface

type
  drzewo = ^wezel;
  wezel = record
    w : integer;
    lsyn,psyn : drzewo;
  end;


procedure insert(var d : drzewo; v : integer);
// wstawia element do drzewa

procedure printAll(d:drzewo);
// wypisuje elementy w drzewie

procedure removeAll(var d : drzewo);
// usuwa z pamięci drzewo 

function removeMin(var d : drzewo) : integer;
// usuwa minimalną wartość z drzewa i zwraca ją

procedure remove(var d : drzewo; v : integer);
// usuwa element z drzewa




implementation

function node(v : integer; l : drzewo; p : drzewo) : drzewo;
var
  d : drzewo;
begin
  New(d);
  d^.w := v;
  d^.lsyn := l;
  d^.psyn := p;
  node := d;
end;


procedure insert(var d : drzewo; v : integer);
begin
  if (d = nil) then d := node(v, nil, nil)
  else if (d^.w <> v) then begin
    if d^.w > v then insert(d^.lsyn, v)
    else insert(d^.psyn, v);
  end;
end;


procedure printAll(d : drzewo);
begin
  if d <> nil then begin
    printAll(d^.lsyn);
    writeLn(d^.w);
    printAll(d^.psyn);
  end;
end;


// usuwa z pamięci drzewo 
procedure removeAll(var d : drzewo);
begin
  if d <> nil then begin
    removeAll(d^.lsyn);
    removeAll(d^.psyn);
    dispose(d);
    d := nil;
  end;
end;


function removeMin(var d : drzewo) : integer;
var
  it : ^drzewo;
  p : drzewo;
begin
  if d = nil then begin
    removeMin := 0;
    exit;
  end;

  it := addr(d);
  while it^^.lsyn <> nil do it := addr(it^^.lsyn);

  removeMin := it^^.w;
  p := it^^.psyn;
  dispose(it^);
  it^ := p;
end;


procedure remove(var d : drzewo; v : integer);
var
  it : ^drzewo;
  b : drzewo;
begin
  // przeszukiwanie
  it := addr(d);
  while (it^ <> nil) and (it^^.w <> v) do
    if it^^.w > v then it := addr(it^^.lsyn)
    else it := addr(it^^.psyn);

  if it^ = nil then exit; 

  // właściwe usunięcie
  if it^^.psyn = nil then begin
    b := it^^.lsyn;
    dispose(it^);
    it^ := b;
  end else it^^.w := removeMin(it^^.psyn);
end;


begin
end. 