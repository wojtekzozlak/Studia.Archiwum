program wizualizacjaGrafow;



uses
  Forms,
  Main in 'Main.pas' {glowneOkno},
  Grafy in 'Grafy.pas',
  Symulacja in 'Symulacja.pas',
  Wizualizacja in 'Wizualizacja.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TglowneOkno, glowneOkno);
  Application.Run;
end.
