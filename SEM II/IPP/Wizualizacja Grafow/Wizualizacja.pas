//////////////////////////////////////////////////////////////////
// Modul Wizualizacji - Wojciech Zoltak
//
// modul odpowiada za rysowanie reprezentacji grafu na komponencie
// klasy TPaintBox32
//////////////////////////////////////////////////////////////////
unit Wizualizacja;

interface

  uses
    Windows, Dialogs, SysUtils, Forms, GR32, GR32_Image, Grafy;

  type
    AkcjaMyszy = (NIC, DRAG, NOWYW, USUNW, NOWAK, USUNK, ZETYK);
    AkcjaPlotno = record
      mysz: akcjaMyszy;
      a: TWierzcholek
    end;

  type
    Wektor = ^WektorDef;
    WektorDef = record
      x, y: Integer;
    end;

  type
    PlotnoWsk = ^TPaintBox32;

  type
    TWizW = class
    private
      plotno: PlotnoWsk;
      offset: Integer;
      ramka: Integer;
      akt: TColor32;
      nieakt: TColor32;
    public
      constructor Create(plotno: PlotnoWsk; ramka: Integer);
      procedure rysujWierzcholek(w: TWierzcholek);
      function dajOffset() : Integer;
      function czyKlik(w: TWierzcholek; wek: Wektor) : Boolean;
    end;

  type
    TWizK = class
    private
      plotno: PlotnoWsk;
      ramka: Integer;
      kolor: TColor32;
    public
      constructor Create(plotno: PlotnoWsk; ramka: Integer);
      procedure rysujKrawedz(k: Krawedz);
    end;

  type
    TWizE = class
    private
      ramka: Integer;
      plotno: PlotnoWsk;
      offset: Integer;
      kolor: TColor32;
    public
      constructor Create(plotno: PlotnoWsk; ramka: Integer; offset: Integer);
      procedure rysujEtykiete(w: TWierzcholek);
      procedure ustawOffset(offset: Integer);
    end;


  type
    TWizualizator = class
    private
      graf: GrafWsk;
      plotno: PlotnoWsk;
      tlo: TColor32;
      ramka: Integer;
      function dajKlikniety(p: Wektor) : TWierzcholek;
    public
      akcja: AkcjaPlotno;
      modulW: TWizW;
      modulK: TWizK;
      modulE: TWizE;
      etykietyRysuj: Boolean;
      etykietyPytaj: Boolean;
      constructor Create(graf: GrafWsk; plotno: PlotnoWsk);
      destructor Destroy(); virtual;
      procedure wizualizuj();
      procedure myszDol(p: Wektor);
      procedure myszGora(p: Wektor);
      procedure ruchMyszy(p: Wektor);
      procedure anulujAkcje();
    end;

    function nowyWektor(x: Integer; y: Integer) : Wektor;

implementation

  // tworzy nowy Wektor wodzacy o zadanych wspó³rzêdnych
  function nowyWektor(x: Integer; y: Integer) : Wektor;
  var
    w: Wektor;
  begin
    new(w);
    w^.x := x;
    w^.y := y;
    nowyWektor := w;
  end;

  // ---------------------------------------------------------------------------

  // TWizW

  // tworzy obiekt rysujacy wierzcholki na zadanym p³otnie, z zadana ramka
  constructor TWizW.Create(plotno: PlotnoWsk; ramka: Integer);
  begin
    self.plotno := plotno;
    self.ramka := ramka;
    self.
    offset := 6;
    akt := Color32(255, 0, 0);
    nieakt := Color32(0, 200, 0);
  end;

  // bierze wierzcholek i rysuje go na przechowywanym plotnie
  procedure TWizW.rysujWierzcholek(w: TWierzcholek);
  var
    p: Pozycja;
    wyp: TColor32;
  begin
    p := w.poz;
    if w.aktywny then wyp := akt
    else wyp := nieakt;
    plotno.Buffer.FillRectS(
      Round(p^.x * (plotno^.Width - 2*ramka))-offset+ramka,
      Round(p^.y * (plotno^.Height - 2*ramka))-offset+ramka,
      Round(p^.x * (plotno^.Width - 2*ramka))+offset+ramka,
      Round(p^.y * (plotno^.Height - 2*ramka))+offset+ramka,
      wyp);
  end;

  // zwraca [true] jesli dany wierzcholek znajduje sie na danej pozycji myszy
  function TWizW.czyKlik(w: TWierzcholek; wek: Wektor) : Boolean;
  var
    x, y: Integer;
  begin
    czyKlik := false;
    x := Round(w.poz^.x*(plotno^.Width-2*ramka))+ramka;
    y := Round(w.poz^.y*(plotno^.Height-2*ramka))+ramka;
    if (abs(wek^.x - x) <= offset) and (abs(wek^.y - y) <= offset) then
      czyKlik := true;
  end;

  // zwraca [offset] z atrybutow modulu
  function TWizW.dajOffset() : Integer;
  begin
    dajOffset := offset;
  end;

  // ---------------------------------------------------------------------------

  // TWizK

  // tworzy obiekt rysujacy krawedzie na zadanym plotnie, o zadanej ramce
  constructor TWizK.Create(plotno: PlotnoWsk; ramka: Integer);
  begin
    self.kolor := Color32(0, 0, 0);
    self.plotno := plotno;
    self.ramka := ramka;
  end;

  // rysuje krawedz [k]
  procedure TWizK.rysujKrawedz(k: Krawedz);
  var
    pa, pb: Pozycja;
  begin
    pa := k^.a.poz;
    pb := k^.b.poz;
    plotno^.Buffer.LineA(
      Round(pa^.x * (plotno^.Width - 2*ramka)) + ramka,
      Round(pa^.y * (plotno^.Height - 2*ramka)) + ramka,
      Round(pb^.x * (plotno^.Width - 2*ramka)) + ramka,
      Round(pb^.y * (plotno^.Height - 2*ramka)) + ramka,
      kolor);
  end;

  // ---------------------------------------------------------------------------

  // TWizE

  // tworzy obiekt rysujacy etykiety na zadanym plotnie, o zadanej ramce,
  // wiedzac o rozmiarze ksztaltu tworzacego wierzcholek
  constructor TWizE.Create(plotno: PlotnoWsk; ramka: Integer; offset: Integer);
  begin
    self.kolor := Color32(0, 0, 0);
    self.plotno := plotno;
    self.ramka := ramka;
    self.offset := offset;
  end;

  // rysuje etykiete wierzcholka [w]
  procedure TWizE.rysujEtykiete(w: TWierzcholek);
  var
    x, y: Integer;
  begin
    x := Round(w.poz^.x * (plotno.Width - 2*ramka)) -
      (strLen(PChar(w.etykieta))*3) +ramka;
    y := Round(w.poz^.y * (plotno.Height - 2*ramka)) - 20 + ramka;
    plotno^.Buffer.RenderText(x, y, w.etykieta, 0, kolor);
  end;

  // ustawia zapamietany [offset] (jak np. zmienimy modul rysujacy wierzcholki)
  procedure TWizE.ustawOffset(offset: Integer);
  begin
    self.offset := offset;
  end;

  // ---------------------------------------------------------------------------

  // TWizualizator

  // tworzy obiekt wizualizuajcy zadany graf na zadanym plotnie
  // uzywajac domyslnych modulow
  constructor TWizualizator.Create(graf: GrafWsk; plotno: PlotnoWsk);
  begin
    etykietyRysuj := true;
    etykietyPytaj := true;
    akcja.mysz := NIC;
    akcja.a := NIL;
    ramka := 20;
    self.graf := graf;
    self.plotno := plotno;
    modulW := TWizW.Create(plotno, ramka);
    modulK := TWizK.Create(plotno, ramka);
    modulE := TWizE.Create(plotno, ramka, modulW.dajOffset());
    tlo := Color32(255, 255, 255);
  end;

  // niszczy obiekt
  destructor TWizualizator.Destroy();
  begin
    modulW.Destroy();
    modulK.Destroy();
    modulE.Destroy();
    graf := nil;
    plotno := nil;
  end;

  // rysuje od nowa graf na plotnie, wykorzystujac podpiete moduly
  procedure TWizualizator.wizualizuj();
  var
    i: Integer;
    b: Integer;
  begin
    plotno^.Buffer.Clear(tlo);

    b := graf^.dajIloscKrawedzi() - 1;
    for i:=0 to b do begin
      modulK.rysujKrawedz(graf^.dajKrawedz(i));
    end;
    b := graf^.dajIloscWierzcholkow() - 1;
    for i:=0 to b do begin
      modulW.rysujWierzcholek(graf^.dajWierzcholek(i));
    end;
    if etykietyRysuj then for i:=0 to b do begin
      modulE.rysujEtykiete(graf^.dajWierzcholek(i));
    end;
  end;

  // na podstawie pozycji myszy zwraca wierzcholek nad ktorym sie ona znajduje
  // badz NIL gdy taki nie istnieje
  function TWizualizator.dajKlikniety(p: Wektor) : TWierzcholek;
  var
    i: Integer;
    w: TWierzcholek;
  begin
    dajKlikniety := NIL;
    for i:=0 to graf^.dajIloscWierzcholkow()-1 do begin
      w := graf^.dajWierzcholek(i);
      if modulW.czyKlik(w, p) then begin
        dajKlikniety := w;
        break;
      end;
    end;
  end;

  // procedura obsluguje zdarzenia myszy zwiazane z wciesnieciem przycisku
  // na podstawie pozycji myszy oraz stanu struktury mowiacej o obecnym
  // trybie w jakim znajduje sie interfejs
  procedure TWizualizator.myszDol(p: Wektor);
  var
    e: String;
    w: TWierzcholek;
  begin

    if akcja.mysz = NOWYW then begin
      if etykietyPytaj then
        if InputQuery('Wierzcholek', 'Podaj etykietê nowego wierzcho³ka', e) then
          graf.dodajWierzcholek(e, nowaPozycja(1.0*(p^.x-ramka)/(plotno.Width - 2*ramka),
           1.0*(p^.y-ramka)/(plotno.Height-2*ramka)))
        else
      else
        graf.dodajWierzcholek('Nowy', nowaPozycja(1.0*p^.x/plotno.Width, 1.0*p^.y/plotno.Height));
    end;

    w := dajKlikniety(p);

    if w = nil then Exit;

    case akcja.mysz of
      NIC: begin
        w.aktywny := true;
        akcja.mysz := DRAG;
        akcja.a := w;
      end;

      USUNW:
        graf^.usunWierzcholek(w);

      NOWAK:
        if akcja.a = NIL then begin
          akcja.a := w;
          w.aktywny := true;
        end else begin
          graf^.polacz(akcja.a.lp, w.lp);
          akcja.a.aktywny := false;
          akcja.a := NIL;
        end;

      USUNK:
        if akcja.a = NIL then begin
          akcja.a := w;
          w.aktywny := true;
        end else begin
          graf^.tnij(akcja.a.lp, w.lp);
          akcja.a.aktywny := false;
          akcja.a := NIL;
        end;

      ZETYK:
          if InputQuery('Wierzcholek', 'Podaj now¹ etykietê wierzcho³ka', e) then
            w.etykieta := e;
    end;
  end;

  // procedura obsluguje zdarzenia myszy zwiazane ze zwolnieniem przycisku
  procedure TWizualizator.myszGora(p: Wektor);
  begin
    if akcja.mysz = DRAG then begin
      akcja.mysz := NIC;
      akcja.a.aktywny := false;
      akcja.a := NIL;
    end;
  end;

  // funkcja poprawia znormalizowana pozycje wierzcholka
  function poprawZa(x: Single) : Single;
  begin
    if x < 0 then poprawZa := 0
    else if x > 1 then poprawZa := 1
    else poprawZa := x;
  end;

  // procedure obsluguje zdarzenia myszy zwiazane z przesunieciem
  procedure TWizualizator.ruchMyszy(p: Wektor);
  begin
    if akcja.mysz = DRAG then begin
      akcja.a.poz^.x := poprawZa(1.0*(p^.x-ramka)/(plotno^.Width-2*ramka));
      akcja.a.poz^.y := poprawZa(1.0*(p^.y-ramka)/(plotno^.Height-2*ramka));
      if akcja.a.poz^.x < 0 then akcja.a.poz^.x := 0;
    end;
  end;

  // procedura czyszsci strukture odpowiadajaca za tryb w jakim znajduje sie
  // interfejs
  procedure TWizualizator.anulujAkcje();
  begin
    akcja.mysz := NIC;
    akcja.a := NIL;
    plotno.Cursor := 0;
  end;
end.
