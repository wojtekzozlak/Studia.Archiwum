//////////////////////////////////////////////////////////////////
// Modul Grafow - Wojciech Zoltak
//
// modul odpowiada za wczytywanie i zapisywanie grafu oraz
// obsluge operacji na nim
//////////////////////////////////////////////////////////////////
unit Grafy;

interface

  uses
  SysUtils, Forms, Classes;

  type
    Pozycja = ^PozycjaDef;
    PozycjaDef = record
      x: Single;
      y: Single
    end;

  type
    TWierzcholek = class;
    TWektorWierzcholkow = class
      private
        rozmiar: Integer;
        nastepny: Integer;
        wierzcholki: array of TWierzcholek;
        procedure init();
      public
        constructor Create(); overload;
        constructor Create(n: Integer); overload;
        destructor Destroy(); virtual;
        function czyJest(w: TWierzcholek) : Integer;
        function daj(n: Integer) : TWierzcholek;
        function dodaj(w: TWierzcholek) : Integer;
        procedure usun(w: TWierzcholek);
        function dajRozmiar() : Integer;
    end;
    TWierzcholek = class
      private
        sasiedzi: TWektorWierzcholkow;
      public
        poz: Pozycja;
        lp: Integer;
        etykieta: String;
        aktywny: Boolean;        
        constructor Create(e: String; p: Pozycja); overload;
        destructor Destroy(); virtual;
        procedure dodajKrawedz(w: TWierzcholek);
        procedure usunKrawedz(w: TWierzcholek);
        function wypisz() : String;
    end;

  type
    Krawedz = ^KrawedzDef;
    KrawedzDef = record
      a, b: TWierzcholek;
    end;

  type
    TWektorKrawedzi = class
      private
        rozmiar: Integer;
        nastepny: Integer;
        krawedzie: array of Krawedz;
        procedure init();
        function czyRowne(a: Krawedz; b: Krawedz) : boolean;
      public
        constructor Create(); overload;
        constructor Create(n: Integer); overload;
        destructor Destroy(); virtual;
        function czyJest(k: Krawedz) : Integer;
        function daj(n: Integer) : Krawedz;
        function dodaj(k: Krawedz) : Integer;
        procedure usun(k: Krawedz);
        function dajRozmiar() : Integer;
        procedure usunZawierajace(w: Twierzcholek);
  end;

  type
    GrafWsk = ^TGraf;
    TGraf = class
      private
        wierzcholki: TWektorWierzcholkow;
        krawedzie: TWektorKrawedzi;
        function losujPozycje() : Pozycja;
      public
        constructor Create(); overload;
        constructor Create(sciezka: String); overload;
        destructor Destroy(); virtual;
        procedure zapisz(sciezka: String);
        function dajWierzcholek(numer: Integer) : TWierzcholek;
        function dajKrawedz(numer: Integer) : Krawedz;
        function dodajWierzcholek(e: String; p: Pozycja) : Integer;
        procedure usunWierzcholek(w: TWierzcholek);
        function dajIloscWierzcholkow() : Integer;
        function dajIloscKrawedzi() : Integer;
        procedure polacz(a: Integer; b: Integer);
        procedure tnij(a: Integer; b: Integer);
        procedure wypisz();
  end;

  type
    NieMaPliku = class(Exception);
    ZleDaneWejsciowe = class(Exception);

  function nowaPozycja(x: Single; y: Single) : Pozycja;
  function nowaKrawedz(a: TWierzcholek; b: TWierzcholek) : Krawedz;

implementation

  // pomocnicze

  function nowaPozycja(x: Single; y: Single) : Pozycja;
  begin
    nowaPozycja := new(pozycja);
    nowaPozycja.x := x;
    nowaPozycja.y := y;
  end;

  function nowaKrawedz(a: TWierzcholek; b: TWierzcholek) : Krawedz;
  var
    k: Krawedz;
  begin
    k := new(Krawedz);
    k^.a := a;
    k^.b := b;
    nowaKrawedz := k;
  end;

  // ---------------------------------------------------------------------------

  // TWektorWierzcholkow

  procedure TWektorWierzcholkow.init();
  begin
    SetLength(wierzcholki, rozmiar);
    nastepny := 0;
  end;

  constructor TWektorWierzcholkow.Create();
  begin
    rozmiar := 4;
    init();
  end;

  constructor TWektorWierzcholkow.Create(n: Integer);
  begin
    rozmiar := n;
    init();
  end;

  destructor TWektorWierzcholkow.Destroy();
  begin
    wierzcholki := nil;
    inherited;
  end;

  function TWektorWierzcholkow.czyJest(w: TWierzcholek) : Integer;
  var
    i: Integer;
  begin
    czyJest := -1;
    for i:=0 to nastepny-1 do
      if wierzcholki[i] = w then begin
        czyJest := i;
        break;
      end;
  end;

  function TWektorWierzcholkow.daj(n: Integer) : TWierzcholek;
  begin
    if n < nastepny then daj := wierzcholki[n]
    else daj := NIL;
  end;

  function TWektorWierzcholkow.dodaj(w: TWierzcholek) : Integer;
  begin
    if czyJest(w) <> -1 then begin
      dodaj := -1;
      exit;
    end;
    if nastepny = rozmiar then begin
      rozmiar := rozmiar*2;
      SetLength(wierzcholki, rozmiar);
    end;
    wierzcholki[nastepny] := w;
    dodaj := nastepny;
    Inc(nastepny);
  end;

  procedure TWektorWierzcholkow.usun(w: TWierzcholek);
  var
    i: Integer;
  begin
    for i:=0 to nastepny-1 do
      if wierzcholki[i] = w then begin
        Dec(nastepny);
        wierzcholki[i] := wierzcholki[nastepny];
      end;
  end;

  function TWektorWierzcholkow.dajRozmiar() : Integer;
  begin
    dajRozmiar := nastepny;
  end;

  // ---------------------------------------------------------------------------
  
  // TWektorKrawedzi

  procedure TWektorKrawedzi.init();
  begin
    SetLength(krawedzie, rozmiar);
    nastepny := 0;
  end;

  function TWektorKrawedzi.czyRowne(a: Krawedz; b: Krawedz) : Boolean;
  begin
    // oto co znaczy "tak czy siak"
    czyRowne := ((a^.a = b^.a) and (a^.b = b^.b))
      or ((a^.b = b^.a) and (a^.a = b^.b));
  end;

  constructor TWektorKrawedzi.Create();
  begin
    rozmiar := 4;
    init();
  end;

  constructor TWektorKrawedzi.Create(n: Integer);
  begin
    rozmiar := n;
    init();
  end;

  destructor TWektorKrawedzi.Destroy();
  begin
    krawedzie := nil;
    inherited;
  end;

  function TWektorKrawedzi.czyJest(k: Krawedz) : Integer;
  var
    i: Integer;
  begin
    czyJest := -1;
    for i:=0 to nastepny-1 do
      if czyRowne(krawedzie[i], k) then begin
        czyJest := i;
        break;
      end;
  end;

  function TWektorKrawedzi.daj(n: Integer) : Krawedz;
  begin
    if n < nastepny then daj := krawedzie[n]
    else daj := NIL;
  end;

  function TWektorKrawedzi.dodaj(k: Krawedz) : Integer;
  begin
    if czyJest(k) <> -1 then begin
      dodaj := -1;
      exit;
    end;
    if nastepny = rozmiar then begin
      rozmiar := rozmiar*2;
      SetLength(krawedzie, rozmiar);
    end;
    krawedzie[nastepny] := k;
    dodaj := nastepny;
    Inc(nastepny);
  end;

  procedure TWektorKrawedzi.usun(k: Krawedz);
  var
    i: Integer;
  begin
    for i:=0 to nastepny-1 do
      if czyRowne(krawedzie[i], k) then begin
        Dec(nastepny);
        krawedzie[i] := krawedzie[nastepny];
      end;
  end;

  function TWektorKrawedzi.dajRozmiar() : Integer;
  begin
    dajRozmiar := nastepny;
  end;

  procedure TWektorKrawedzi.usunZawierajace(w: TWierzcholek);
  var
    i: Integer;
  begin
    for i:=nastepny-1 downto 0 do
      if (krawedzie[i].a = w) or (krawedzie[i].b = w) then usun(krawedzie[i]);
  end;

  // ---------------------------------------------------------------------------

  // TWierzcholek

    constructor TWierzcholek.Create(e: String; p: Pozycja);
    begin
      etykieta := e;
      poz := p;
      sasiedzi := TWektorWierzcholkow.Create();
      aktywny := false;
    end;

    destructor TWierzcholek.Destroy();
    begin
      dispose(poz);
      sasiedzi.Destroy();
      inherited;
    end;

    procedure TWierzcholek.dodajKrawedz(w: TWierzcholek);
    begin
      sasiedzi.dodaj(w);
    end;

    procedure TWierzcholek.usunKrawedz(w: TWierzcholek);
    begin
      sasiedzi.usun(w);
    end;

    function TWierzcholek.wypisz() : String;
    var
      i: Integer;
      buf: String;
    begin
      buf := Concat(etykieta, '|', intToStr(lp), ' (', floatToStr(poz^.x),', ', floatToStr(poz^.y) ,'): ');
      for i:=0 to sasiedzi.dajRozmiar()-1 do begin
        buf := Concat(buf, sasiedzi.daj(i).etykieta, ' ');
      end;
      wypisz := buf;
    end;

  // --------------------------------------------------------------------------

  // TGraf

  constructor TGraf.Create();
  begin
    wierzcholki := TWektorWierzcholkow.Create();
    krawedzie := TWektorKrawedzi.Create();
  end;

  function TGraf.losujPozycje() : Pozycja;
  var
    x, y: Single;
  begin
    Randomize();
    x := Random(99999)/99999.0;
    y := Random(99999)/99999.0;
    losujPozycje := nowaPozycja(x, y);
  end;

  constructor TGraf.Create(sciezka: String);
  var
    f: TextFile;
    n, i, a, b: Integer;
    x, y: Single;
    e: String;
    p: Pozycja;
  begin
    if FileExists(sciezka) then assignFile(f, sciezka)
    else raise NieMaPliku.Create(PChar(sciezka));
    reset(f);
    try
      // wlasciwe wczytywanie
      Read(f, n);
      wierzcholki := TWektorWierzcholkow.Create(n*2);
      for i:=1 to n do begin
        Read(f, x, y, e);
        if (x = -1) and (y = -1) then p := losujPozycje
        else if (x < 0) or (y < 0) or (x > 1) or (y > 1) then raise ZleDaneWejsciowe.Create('')
        else p := nowaPozycja(x, y);
        dodajWierzcholek(trim(e), p);
      end;
      Read(f, n);
      krawedzie := TWektorKrawedzi.Create(n*2);
      for i:=1 to n do begin
        Read(f, a, b);
        polacz(a, b);
      end;
    except
      raise ZleDaneWejsciowe.Create('');
    end;
    closeFile(f);
  end;

  destructor TGraf.Destroy();
  var
    i: Integer;
    k: Krawedz;
    w: TWierzcholek;
  begin
    // niszczymy krawedzie
    for i:=0 to krawedzie.dajRozmiar()-1 do begin
      k := krawedzie.daj(i);
      dispose(k);
    end;
    // niszczymy wektor krawedzi
    krawedzie.Destroy();
    // niszczymy wierzcholki
    for i:=0 to wierzcholki.dajRozmiar()-1 do begin
      w := wierzcholki.daj(i);
      w.Destroy();
    end;
    // i ich wektor
    wierzcholki.Destroy();
    inherited;    
  end;

  procedure TGraf.zapisz(sciezka: String);
  var
    f: TextFile;
    i: Integer;
    w: TWierzcholek;
    k: Krawedz;
  begin
    assignFile(f, sciezka);
    rewrite(f);
    writeln(f, wierzcholki.dajRozmiar());
    for i:=0 to wierzcholki.dajRozmiar()-1 do begin
      w := wierzcholki.daj(i);
      writeln(f, w.poz^.x, ' ', w.poz^.y, ' ', w.etykieta);
    end;
    writeln(f, krawedzie.dajRozmiar());
    for i:=0 to krawedzie.dajRozmiar()-1 do begin
      k := krawedzie.daj(i);
      writeln(f, k^.a.lp, ' ', k^.b.lp);
    end;
    closeFile(f);
  end;


  function TGraf.dajWierzcholek(numer: Integer) : TWierzcholek;
  begin
    dajWierzcholek := wierzcholki.daj(numer);
  end;

  function TGraf.dajKrawedz(numer: Integer) : Krawedz;
  begin
    dajKrawedz := krawedzie.daj(numer);
  end;

  function TGraf.dodajWierzcholek(e: String; p: Pozycja) : Integer;
  var
    w: TWierzcholek;
    i: Integer;
  begin
    w := TWierzcholek.Create(e, p);
    i := wierzcholki.dodaj(w);
    w.lp := i;
    dodajWierzcholek := i;
  end;

  procedure TGraf.usunWierzcholek(w: TWierzcholek);
  begin
    krawedzie.usunZawierajace(w);
    wierzcholki.usun(w);
  end;

  function TGraf.dajIloscWierzcholkow() : Integer;
  begin
    dajIloscWierzcholkow := wierzcholki.dajRozmiar();
  end;

  function TGraf.dajIloscKrawedzi() : Integer;
  begin
    dajIloscKrawedzi := krawedzie.dajRozmiar();
  end;


  procedure TGraf.polacz(a: Integer; b: Integer);
  begin
    wierzcholki.daj(a).dodajKrawedz(wierzcholki.daj(b));
    wierzcholki.daj(b).dodajKrawedz(wierzcholki.daj(a));
    krawedzie.dodaj(nowaKrawedz(wierzcholki.daj(a), wierzcholki.daj(b)));
  end;

  procedure TGraf.tnij(a: Integer; b: Integer);
  begin
    wierzcholki.daj(a).usunKrawedz(wierzcholki.daj(b));
    wierzcholki.daj(b).usunKrawedz(wierzcholki.daj(a));
    krawedzie.usun(nowaKrawedz(wierzcholki.daj(a), wierzcholki.daj(b)));
  end;

  procedure TGraf.wypisz();
  var
    i: Integer;
    str: String;
    k: Krawedz;
  begin
    str := '';
    for i:=0 to wierzcholki.dajRozmiar()-1 do str := concat(str,'|',wierzcholki.daj(i).wypisz(),'|', #10);
    str := str + #10;
    for i:=0 to krawedzie.dajRozmiar()-1 do begin
      k := krawedzie.daj(i);
      str := concat(str, k^.a.etykieta, ' ', k^.b.etykieta, #10);
    end;
    Application.MessageBox(PChar(str), 'Graf');
  end;

  // --------------------------------------------------------------------------

end.
