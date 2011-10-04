//////////////////////////////////////////////////////////////////
// Modul Podstawowy - Wojciech Zoltak
//
// modul odpowiada za interfejs oraz scalenie pozosta�ych modulow
// w jedna dzialajaca calosc
//////////////////////////////////////////////////////////////////
unit Main;

interface

uses
  Wizualizacja, Windows, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, GR32_Image, GR32, StdCtrls, Menus, ComCtrls, Grafy, Symulacja,
  ExtCtrls;

type
  TGlowneOkno = class(TForm)
    obszarRysowania: TPaintBox32;
    glowneMenu: TMainMenu;
    menuPlik: TMenuItem;
    gmOtworz: TMenuItem;
    gmZapisz: TMenuItem;
    dialogOtworz: TOpenDialog;
    dialogZapisz: TSaveDialog;
    hmZakoncz: TMenuItem;
    wyzwalacz: TTimer;
    menuUstawienia: TMenuItem;
    menuPomoc: TMenuItem;
    Jakkorzysta1: TMenuItem;
    menuWczytajPonownie: TMenuItem;
    obszarRysowaniaMenu: TPopupMenu;
    pmDodajW: TMenuItem;
    pmUsunW: TMenuItem;
    pmDodajK: TMenuItem;
    pmUsunK: TMenuItem;
    timerOdswierzanie: TTimer;
    pasekStanu: TStatusBar;
    menuSymulacja: TMenuItem;
    menuStart: TMenuItem;
    menuJedenKrok: TMenuItem;
    menuTemperaturaUp: TMenuItem;
    menuTemperaturaDown: TMenuItem;
    pmZmienEtykiete: TMenuItem;
    menuPokazujEtykiety: TMenuItem;
    menuPytajEtykiety: TMenuItem;
    menuZmienT: TMenuItem;
    menuZmienC: TMenuItem;
    menuNowy: TMenuItem;
    procedure gmOtworzClick(Sender: TObject);
    procedure gmZapiszClick(Sender: TObject);
    procedure hmZakonczClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure obszarRysowaniaPaintBuffer(Sender: TObject);
    procedure wyzwalaczTimer(Sender: TObject);
    procedure obszarRysowaniaMouseDown(Sender: TObject;
      Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure obszarRysowaniaMouseMove(Sender: TObject; Shift: TShiftState;
      X, Y: Integer);
    procedure obszarRysowaniaMouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure timerOdswierzanieTimer(Sender: TObject);
    procedure menuStartClick(Sender: TObject);
    procedure menuTemperaturaDownClick(Sender: TObject);
    procedure menuTemperaturaUpClick(Sender: TObject);
    procedure pmDodajWClick(Sender: TObject);
    procedure FormKeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure menuPokazujEtykietyClick(Sender: TObject);
    procedure menuPytajEtykietyClick(Sender: TObject);
    procedure menuZmienTClick(Sender: TObject);
    procedure menuZmienCClick(Sender: TObject);
    procedure pmUsunWClick(Sender: TObject);
    procedure pmDodajKClick(Sender: TObject);
    procedure pmUsunKClick(Sender: TObject);
    procedure pmZmienEtykieteClick(Sender: TObject);
    procedure menuWczytajPonownieClick(Sender: TObject);
    procedure menuNowyClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure menuJedenKrokClick(Sender: TObject);
    procedure Jakkorzysta1Click(Sender: TObject);
  private
    graf: TGraf;
    wizualizator: TWizualizator;
    roz: Rozmieszczenie;
    procedure poprawInformacje();
    procedure akcjaNowyWierzcholek();
    procedure akcjaUsunWierzcholek();
    procedure akcjaNowaKrawedz();
    procedure akcjaUsunKrawedz();
    procedure akcjaZmianaEtykiety();
    procedure noweT(s: String);
    procedure noweC(s: String);
  public
    { Public declarations }
  end;

var
  GlowneOkno: TGlowneOkno;

implementation

{$R *.dfm}

// wyzwalacze

procedure TGlowneOkno.wyzwalaczTimer(Sender: TObject);
begin
  if roz.nastepnaIteracja() then
  else begin
    wyzwalacz.Enabled := false;
  end;
end;
procedure TGlowneOkno.timerOdswierzanieTimer(Sender: TObject);
begin
  obszarRysowania.Refresh();
  poprawInformacje();
end;

// -----------------------------------------------------------------------------

// Zdarzenia glownego okna

procedure TGlowneOkno.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  graf.Destroy();
  roz.Destroy();
  wizualizator.Destroy();
end;

procedure TGlowneOkno.FormCreate(Sender: TObject);
begin
  graf := TGraf.Create();
  wizualizator := TWizualizator.Create(addr(graf), addr(obszarRysowania));
  wizualizator.etykietyRysuj := menuPokazujEtykiety.Checked;
  wizualizator.etykietyPytaj := menuPytajEtykiety.Checked;
  roz := Rozmieszczenie.Create(addr(graf), addr(obszarRysowania), 20, 0.8);
end;

procedure TGlowneOkno.FormKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
  case Integer(Key) of
    VK_ESCAPE: begin
      wizualizator.anulujAkcje();
    end;
    Integer('V'): begin
      if (Shift = [ssctrl]) then akcjaUsunWierzcholek()
      else akcjaNowyWierzcholek();
    end;
    Integer('E'): begin
      if (Shift = [ssctrl]) then akcjaUsunKrawedz()
      else akcjaNowaKrawedz();
    end;
    VK_F2: begin
      akcjaZmianaEtykiety();
    end;
  end;
end;

// -----------------------------------------------------------------------------

// Zdarzenia obszaru rysowania

procedure TGlowneOkno.obszarRysowaniaMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var
  p: Wizualizacja.Wektor;
begin
  p := Wizualizacja.nowyWektor(X, Y);
  if Button = mbLeft then wizualizator.myszDol(p);
  dispose(p);
end;

procedure TGlowneOkno.obszarRysowaniaMouseMove(Sender: TObject;
  Shift: TShiftState; X, Y: Integer);
var
  p: Wizualizacja.Wektor;
begin
  p := Wizualizacja.nowyWektor(X, Y);
  wizualizator.ruchMyszy(p);
  dispose(p);
end;

procedure TGlowneOkno.obszarRysowaniaMouseUp(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var
  p: Wizualizacja.Wektor;
begin
  p := Wizualizacja.nowyWektor(X, Y);
  wizualizator.myszGora(p);
  dispose(p);
end;

procedure TGlowneOkno.obszarRysowaniaPaintBuffer(Sender: TObject);
begin
  wizualizator.wizualizuj();
end;

// -----------------------------------------------------------------------------

// funkcje akcji interfejsu

procedure TGlowneOkno.akcjaNowyWierzcholek();
begin
  wizualizator.anulujAkcje();
  wizualizator.akcja.mysz := NOWYW;
  obszarRysowania.Cursor := -3;
end;

procedure TGlowneOkno.akcjaUsunWierzcholek();
begin
  wizualizator.anulujAkcje();
  wizualizator.akcja.mysz := USUNW;
  obszarRysowania.Cursor := -3;
end;

procedure TGlowneOkno.akcjaNowaKrawedz();
begin
  wizualizator.anulujAkcje();
  wizualizator.akcja.mysz := NOWAK;
  obszarRysowania.Cursor := -3;
end;

procedure TGlowneOkno.akcjaUsunKrawedz();
begin
  wizualizator.anulujAkcje();
  wizualizator.akcja.mysz := USUNK;
  obszarRysowania.Cursor := -3;
end;

procedure TGlowneOkno.akcjaZmianaEtykiety();
begin
  wizualizator.anulujAkcje();
  wizualizator.akcja.mysz := ZETYK;
  obszarRysowania.Cursor := -3;
end;

procedure TGlowneOkno.noweT(s: String);
var
  t: Single;
  err: Integer;
begin
  val(s, t, err);
  if (err <> 0) or (t < 0) or (t > 1000) then
    Application.MessageBox('B��d! Podana temperatura jest b��dna.', 'B��d')
  else roz.t := t;
end;

procedure TGlowneOkno.noweC(s: String);
var
  c: Single;
  err: Integer;
begin
  val(s, c, err);
  if (err <> 0) or (c <= 0) or (c >= 1) then
    Application.MessageBox('B��d! Podany wsp�czynnik jest b��dny.', 'B��d')
  else roz.c := c;
end;

procedure TGlowneOkno.poprawInformacje();
begin
  pasekStanu.Panels[0].Text := Concat('V: ', intToStr(graf.dajIloscWierzcholkow()));
  pasekStanu.Panels[1].Text := Concat('E: ', intToStr(graf.dajIloscKrawedzi()));
  pasekStanu.Panels[2].Text := Concat('t: ', copy(floatToStr(roz.t), 0, 6));
  pasekStanu.Panels[3].Text := Concat('c: ', copy(floatToStr(roz.c), 0, 4));
  if wyzwalacz.Enabled then pasekStanu.Panels[4].Text := 'symulacja w��czona'
  else pasekStanu.Panels[4].Text := 'symulacja wstrzymana';
  case wizualizator.akcja.mysz of
    NIC:
      pasekStanu.Panels[5].Text := '';
    NOWYW:
      pasekStanu.Panels[5].Text := 'Kliknij w wybranym miejscu aby utworzy� wierzcho�ek. Esc - aby przesta�.';
    USUNW:
      pasekStanu.Panels[5].Text := 'Kliknij w wybranym miejscu aby usun�� wierzcho�ek. Esc - aby przesta�.';
    NOWAK:
      if wizualizator.akcja.a = NIL then begin
        pasekStanu.Panels[5].Text := 'Nowa kraw�d�. Wybierz PIERWSZY wierzcho�ek. Esc - aby przesta�.';
      end else pasekStanu.Panels[5].Text := 'Nowa kraw�d�. Wybierz DRUGI wierzcho�ek. Esc - aby przesta�.';
    USUNK:
      if wizualizator.akcja.a = NIL then begin
        pasekStanu.Panels[5].Text := 'Usu� kraw�d�. Wybierz PIERWSZY wierzcho�ek. Esc - aby przesta�.';
      end else pasekStanu.Panels[5].Text := 'Usu� kraw�d�. Wybierz DRUGI wierzcho�ek. Esc - aby przesta�.';
    ZETYK:
      pasekStanu.Panels[5].Text := 'Wybierz wierzcho�ek do zmiany etykiety. Esc - aby przesta�.';
  end;
end;


// -----------------------------------------------------------------------------

// funkcje zwiazane z gornym menu

procedure TGlowneOkno.gmOtworzClick(Sender: TObject);
var
  graf: TGraf;
begin
  if dialogOtworz.Execute() then begin
    try
      graf := TGraf.Create(dialogOtworz.FileName);
      glowneOkno.graf.Destroy();
      glowneOkno.graf := graf;
      menuWczytajPonownie.Enabled := true;
    except
      Application.MessageBox('B��d wczytywania danych z pliku', 'B��d!');
    end;
  end;
end;

procedure TGlowneOkno.gmZapiszClick(Sender: TObject);
begin
  if dialogZapisz.Execute() then begin
    try
      glowneOkno.graf.zapisz(dialogZapisz.FileName);
    except
      Application.MessageBox('B��d zapisu do pliku', 'B��d!');
    end;
  end;
end;

procedure TGlowneOkno.hmZakonczClick(Sender: TObject);
begin
  glowneOkno.Close();
end;

procedure TGlowneOkno.menuStartClick(Sender: TObject);
begin
  if wyzwalacz.Enabled then begin
    wyzwalacz.Enabled := false;
    menuStart.Caption := 'Stop';
  end else begin
    wyzwalacz.Enabled := true;
    menuStart.Caption := 'Start';
  end;
end;

procedure TGlowneOkno.menuTemperaturaDownClick(Sender: TObject);
begin
  if roz.t - 10 < 0 then roz.t := 0
  else roz.t := roz.t - 10;
end;

procedure TGlowneOkno.menuTemperaturaUpClick(Sender: TObject);
begin
  if roz.t + 10 > 1000 then roz.t := 1000
  else roz.t := roz.t + 10;
end;

procedure TGlowneOkno.menuPokazujEtykietyClick(Sender: TObject);
begin
  if menuPokazujEtykiety.Checked then begin
    menuPokazujEtykiety.Checked := false;
    wizualizator.etykietyRysuj := false;
  end else begin
    menuPokazujEtykiety.Checked := true;
    wizualizator.etykietyRysuj := true;
  end;
end;

procedure TGlowneOkno.menuPytajEtykietyClick(Sender: TObject);
begin
  if menuPytajEtykiety.Checked then begin
    menuPytajEtykiety.Checked := false;
    wizualizator.etykietyPytaj := false;
  end else begin
    menuPytajEtykiety.Checked := true;
    wizualizator.etykietyPytaj := true;
  end;
end;

procedure TGlowneOkno.menuZmienTClick(Sender: TObject);
var
  s: String;
begin
  if InputQuery('Temperatura', 'Podaj now� temperatur� (liczba z zakresu [0..1000])', s) then begin
    noweT(s);
  end;
end;

procedure TGlowneOkno.menuZmienCClick(Sender: TObject);
var
  s: String;
begin
  if InputQuery('Wsp. och�odzenia', 'Podaj nowy wsp�czynnik (liczba z zakresu (0..1))', s) then begin
    noweC(s);
  end;
end;

procedure TGlowneOkno.menuWczytajPonownieClick(Sender: TObject);
var
  graf: TGraf;
begin
  try
    graf := TGraf.Create(dialogOtworz.FileName);
    glowneOkno.graf.Destroy();
    glowneOkno.graf := graf;
  except
    Application.MessageBox('B��d wczytywania danych z pliku', 'B��d!');
  end;
end;

procedure TGlowneOkno.menuNowyClick(Sender: TObject);
begin
  if 1 = MessageDlg('Tworz�c nowy graf utracisz niezapisane dane. Kontynuowa�?',
    mtConfirmation, mbOKCancel, 0) then begin
    graf.Destroy();
    graf := TGraf.Create();
    menuWczytajPonownie.Enabled := false;
  end;
end;

procedure TGlowneOkno.menuJedenKrokClick(Sender: TObject);
begin
  roz.nastepnaIteracja();
end;

procedure TGlowneOkno.Jakkorzysta1Click(Sender: TObject);
var
  str: String;
begin
  str := Concat('Program umo�liwia obliczenie "optymalnego" rozmieszczenia',
  ' grafu do prezenetacji korzystaj�c z prostego modelu fizycznego opartego',
  ' na odpychaj�cych si� cz�steczkach (wierzcho�ki) oraz przyci�gaj�cych je',
  ' spr�ynach (kraw�dzie).', #10, #10,
  'Temperatura uk�adu odpowiada za maksymalne przesuni�cie wierzcho�ka w',
  ' trakcie pojedynczej iteracji, wsp�czynnik och�odzenia okre�la',
  ' spadek temperatury pomi�dzy iteracjami', #10, #10, #10,
  'Skr�ty klawiszowe:', #10, #10,
  'Spacja', #09,'- start/pauza symulacji', #10,
  'Enter', #09,'- pojedyncza iteracja', #10,
  'T', #09,'- zmiana temperatury uk�adu (na konretn� warto��)', #10,
  'Y', #09,'- zmiana wsp. och�odzenia', #10,
  ',', #09,'- zmniejszenie temperatury o 10', #10,
  '.', #09,'- zwi�kszenie temperatury o 10', #10,
  'v', #09,'- tryb nowego wierzcho�ka', #10,
  'ctrl + v', #09,'- tryb usuwania wierzcho�k�w', #10,
  'e', #09,'- tryb nowej kraw�dzi', #10,
  'ctrl + e', #09,'- tryb usuwania kraw�dzi', #10,
  'F2', #09,'- tryb zmiany etykiety', #10,
  'esc', #09,'- wy��czenie aktywnego trybu');
  Application.MessageBox(PChar(str), 'Pomoc');
end;

// -----------------------------------------------------------------------------

// menu kontekstowe



procedure TGlowneOkno.pmDodajWClick(Sender: TObject);
begin
  akcjaNowyWierzcholek();
end;

procedure TGlowneOkno.pmUsunWClick(Sender: TObject);
begin
  akcjaUsunWierzcholek();
end;

procedure TGlowneOkno.pmDodajKClick(Sender: TObject);
begin
  akcjaNowaKrawedz();
end;

procedure TGlowneOkno.pmUsunKClick(Sender: TObject);
begin
  akcjaUsunKrawedz();
end;

procedure TGlowneOkno.pmZmienEtykieteClick(Sender: TObject);
begin
  akcjaZmianaEtykiety();
end;

end.
