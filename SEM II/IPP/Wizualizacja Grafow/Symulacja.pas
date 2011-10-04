//////////////////////////////////////////////////////////////////
// Modul Symulacji - Wojciech Zoltak
//
// modul odpowiada za rozmieszczanie grafu metoda prostej symulacji
// pseudofizycznej
//////////////////////////////////////////////////////////////////
unit Symulacja;

interface
  uses
    SysUtils, Grafy, GR32, GR32_Image, Forms;

  type
    Wektor = ^WektorDef;
    WektorDef = record
      x, y: Single;
    end;

  type
    PlotnoWsk = ^TPaintBox32;
    Rozmieszczenie = class
      private
        plotno: PlotnoWsk;
        graf: GrafWsk;
        k: Single;
        krok: Integer;
        function F0(x: Single) : Single;
        function FP(x: Single) : Single;
        procedure cool();
        function denormalizuj(p: Pozycja) : Wektor;
        procedure normalizuj(w: Wektor; p: Pozycja);
      public
        cykl: Integer;
        t: Single;
        c: Single;
        constructor Create(graf: GrafWsk; plotno: PlotnoWsk; t: Single; c: Single);
        function nastepnaIteracja() : Boolean;
    end;

implementation

uses Math;

  // pomocnicze

  function nowyWektor(x: Integer; y: Integer) : Wektor;
  var
    w: Wektor;
  begin
    w := new(Wektor);
    w^.x := x;
    w^.y := y;
    nowyWektor := w;
  end;

  function modul(w: Wektor) : Single;
  begin
    modul := max(sqrt(1.0*w^.x*w^.x + 1.0*w^.y*w^.y), 0.0001);
  end;

  // ---------------------------------------------------------------------------

  // Rozmieszczenie

  procedure Rozmieszczenie.cool();
  begin
    t := c*t;
  end;

  function Rozmieszczenie.denormalizuj(p: Pozycja) : Wektor;
  var
    w: Wektor;
  begin
    w := new(Wektor);
    w^.x := Round(plotno^.Width*p^.x);
    w^.y := Round(plotno^.Height*p^.y);
    denormalizuj := w;
  end;

  procedure Rozmieszczenie.normalizuj(w: Wektor; p: Pozycja);
  begin
    p^.x := (1.0*w^.x)/plotno^.Width;
    p^.y := (1.0*w^.y)/plotno^.Height;
  end;

  function Rozmieszczenie.F0(x: Single) : Single;
  begin
    if x = 0 then x := 0.00001;
    F0 := Sqr(k)/x;
  end;

  function Rozmieszczenie.FP(x: Single) : Single;
  begin
    FP := Sqr(x)/k;
  end;

  constructor Rozmieszczenie.Create(graf: GrafWsk; plotno: plotnoWsk; t: Single; c: Single);
  begin
    self.graf := graf;
    self.plotno := plotno;
    self.krok := 1;
    self.cykl := 1;
    self.t := t;
    self.c := c;
    self.k := 0.3*sqrt(plotno^.Width*plotno^.Height)/max(graf^.dajIloscWierzcholkow(), 1);
  end;


  // alogrytm rozmieszczenia grafu - implementacja pseudokodu z tresci zadania
  // dodano warunek stopu w formie bariery temperatury
  function Rozmieszczenie.nastepnaIteracja() : Boolean;
  var
    poz: array of Wektor;
    dpoz: array of Wektor;
    i, j: Integer;
    d: Wektor;
    kr: Krawedz;
    p: Pozycja;
    a, b: Integer;
  begin
    setLength(poz, graf^.dajIloscWierzcholkow());
    setLength(dpoz, graf^.dajIloscWierzcholkow());
    for i:=0 to graf^.dajIloscWierzcholkow()-1 do begin
      poz[i] := denormalizuj(graf^.dajWierzcholek(i).poz);
      dpoz[i] := nowyWektor(0, 0);
    end;
    d := nowyWektor(0, 0);
    for i:=0 to graf^.dajIloscWierzcholkow()-1 do begin
      for j:=0 to graf^.dajIloscWierzcholkow()-1 do
        if graf^.dajWierzcholek(i) <> graf^.dajWierzcholek(j) then begin
          d^.x := poz[i]^.x - poz[j]^.x;
          d^.y := poz[i]^.y - poz[j]^.y;
          dpoz[i]^.x := Round(dpoz[i]^.x + F0(modul(d))*d^.x/modul(d));
          dpoz[i]^.y := Round(dpoz[i]^.y + F0(modul(d))*d^.y/modul(d));
        end;
    end;
    for i:=0 to graf^.dajIloscKrawedzi()-1 do begin
      kr := graf^.dajKrawedz(i);
      a := kr^.a.lp;
      b := kr^.b.lp;
      d^.x := poz[a]^.x - poz[b]^.x;
      d^.y := poz[a]^.y - poz[b]^.y;
      dpoz[a]^.x := Round(dpoz[a]^.x - FP(modul(d))*d^.x/modul(d));
      dpoz[a]^.y := Round(dpoz[a]^.y - FP(modul(d))*d^.y/modul(d));
      dpoz[b]^.x := Round(dpoz[b]^.x + FP(modul(d))*d^.x/modul(d));
      dpoz[b]^.y := Round(dpoz[b]^.y + FP(modul(d))*d^.y/modul(d));
    end;
    for i:=0 to graf^.dajIloscWierzcholkow()-1 do begin
      poz[i]^.x := Round(poz[i]^.x + min(modul(dpoz[i]), t)*dpoz[i]^.x/(cykl*modul(dpoz[i])));
      poz[i]^.y := Round(poz[i]^.y + min(modul(dpoz[i]), t)*dpoz[i]^.y/(cykl*modul(dpoz[i])));
      if poz[i]^.x < 0 then poz[i]^.x := 0
      else if poz[i]^.x > plotno^.Width then poz[i]^.x := plotno^.Width;
      if poz[i]^.y < 0 then poz[i]^.y := 0
      else if poz[i]^.y > plotno^.Height then poz[i]^.y := plotno^.Height;
    end;
    for i:=0 to graf^.dajIloscWierzcholkow()-1 do begin
      p := graf^.dajWierzcholek(i).poz;
      normalizuj(poz[i], p);
    end;
    krok := (krok+1) mod cykl;
    if krok = 0 then cool();
    nastepnaIteracja := (t > 1);
  end;

end.
