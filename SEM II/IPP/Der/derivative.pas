{ Moduł Derivative - Wojciech Żółtak - 08.03.2010
  Zawiera funkcję różniczkującą proste wyrażenia algebraiczne w formie drzewa
  oraz obsługuje operacje wejścia/wyjścia obiektów tego typu }
unit
  Derivative;



interface

  type
    { typ wariantowy charakteru wierzchołka }
    Character = (Scalar, UnaryOperator, BinaryOperator, Variable);

    { typ drzewa wyrażenia }
    ExprTree = ^ExprNode;
    ExprNode = record
      v : String; { wartość (x, sin, cos, 20 itp.) }
      ch : Character; { charakter }
      l, r : ExprTree; { synowie }
      links : Integer; { licznik referencji }
    end;

    { typ wyrażenia }
    Expression = record
      size : Integer; { wielkość wyrażenia w wierzchołkach drzewa }
      t : ExprTree; { drzewo wyrażenia }
    end;


  procedure load_expr(var e : Expression);
  { load_expr[e] wczytuje wyrażenie w postaci drzewa i zapisuje w [e]
    - czas liniowy względem rozmiaru wyrazenia
    - pamięć proporcjonalna do wysokości drzewa }

  function copy_expr(e : Expression) : Expression;
  { copy_expr[e] zwraca wyrażenie będące kopią [e]
    - czas/pamięć stała }

  procedure expr_derivative(var e : Expression);
  { expr_derivative[e] zamienia [e] na drzewo będące jego pochodną, wypisując
    przy tym informacje o ilości wierzchołków na stderr
    - czas liniowy względem rozmiaru wyrażenia 
    - pamięć proporcjonalna do wyrażenia wynikowego }

  procedure print_expr(e : Expression);
  { print_expr[e] wypisuje wyrażenie [e] na standardowe wyjście 
    - czas liniowy względem rozmiaru wyrazenia
    - pamięć proporcjonalna do wysokości drzewa }

  procedure clear_expr(var e : Expression);
  { clear_expr[e] zamienia [e] w wyrażenie puste, usuwając drzewo z pamięci 
    - czas liniowy względem rozmiaru wyrazenia
    - pamięć proporcjonalna do wysokości drzewa }



implementation

  const
    { tablice stałych z operacjami, które są obsługiwane }
    U_OPER_SIZE = 4;
    U_OPER : array [1..U_OPER_SIZE] of String[3] = ('exp', 'ln', 'sin', 'cos');
    B_OPER_SIZE = 4;
    B_OPER : array [1..B_OPER_SIZE] of String[1] = ('+', '-', '*', '/');


  { is_u_oper[s] sprawdza, czy [s] reprezentuje jedną z operacji
    jednoargumentowych }
  function is_u_oper(s : String) : Boolean;
  var
    i : Integer;
  begin
    is_u_oper := true;
    for i := 1 to U_OPER_SIZE do
      if s = U_OPER[i] then exit;
    is_u_oper := false
  end;


  { is_u_oper[s] sprawdza, czy [s] reprezentuje jedną z operacji
    dwuargumentowych }
  function is_b_oper(s : String) : Boolean;
  var
    i : Integer;
  begin
    is_b_oper := true;
    for i := 1 to B_OPER_SIZE do
      if s = B_OPER[i] then exit;
    is_b_oper := false
  end;


  { make_node[v, ch, l, r] zwraca wskaźnik na wierzchołek o zadanych
    właściwościach, inkrementując liczniki referencji drzew trafiających na
    pozycje synów
    Ważne!
    Funkcja zakłada, że wskaźnik na węzeł zostanie gdzieś podpięty (licznik
    referencji domyślnie 1. Oraz, że synowie są już istniejącymi węzłami
    (zwiększa im licznik o 1). Dlatego make_node[..] nie powinno być
    argumentem [l] ani [r] }
  function make_node(v : String; ch : Character;
  l : ExprTree; r : ExprTree) : ExprTree;
  var
    t : ExprTree;
  begin
    new(t);
    t^.v := v;
    t^.ch := ch;
    t^.l := l;
    t^.r := r;
    t^.links := 1;
    if l <> NIL then inc(l^.links);
    if r <> NIL then inc(r^.links);
    make_node := t;
  end;

  { zero[] zwraca wierzchołek ze stałą 0 }
  function zero() : ExprTree;
  begin
    zero := make_node('0', Scalar, NIL, NIL);
  end;


  { zero[] zwraca wierzchołek ze stałą 1 }
  function one() : ExprTree;
  begin
    one := make_node('1', Scalar, NIL, NIL);
  end;


  { links_modify[t, i] zwiększa licznik referencji [t] o [i] }
  procedure links_modify(var t : ExprTree; i : Integer);
  begin
    t^.links := t^.links + i;
  end;

  { copy_expr[e] zwraca wyrażenie będące kopią [e] }
  function copy_expr(e : Expression) : Expression;
  begin
    copy_expr.size := e.size;
    copy_expr.t := e.t;
    links_modify(e.t, 1);
  end;

  { load_expr[e] wczytuje wyrażenie w postaci drzewa i zapisuje w [e] }
  procedure load_expr(var e : Expression);

    function expr(var c : Integer) : ExprTree;
    var
      s : String;
    begin
      readln(s);
      if is_u_oper(s) then begin
        expr := make_node(s, UnaryOperator, NIL, NIL);
        expr^.l := expr(c);
      end else if is_b_oper(s) then begin
        expr := make_node(s, BinaryOperator, NIL, NIL);
        expr^.l := expr(c);
        expr^.r := expr(c);
      end else if s = 'x' then expr := make_node(s, Variable, NIL, NIL)
      else if s = '0' then expr := zero()
      else if s = '1' then expr := one()
      else expr := make_node(s, Scalar, NIL, NIL);
      inc(c);
    end;

  begin
    e.size := 0;
    e.t := expr(e.size);
  end;


  { print_expr[e] wypisuje wyrażenie [e] na standardowe wyjście }
  procedure print_expr(e : Expression);

    procedure print_tree(t : ExprTree);
    begin
      if t = NIL then exit;
      writeln(t^.v);
      print_tree(t^.l);
      print_tree(t^.r);    
    end;

  begin
    print_tree(e.t);
  end;


  { clean_tree[t, c] usuwa z [t] niepotrzebne wierzchołki, zmniejszając [c]
    o ilość usniętych węzłów }
  procedure clean_tree(var t : ExprTree; var c : Integer);
  begin
    if t = NIL then exit;
    if t^.links = 0 then begin
      if t^.l <> NIL then begin
        links_modify(t^.l, -1);
        clean_tree(t^.l, c);
      end;
      if t^.r <> NIL then begin
        links_modify(t^.r, -1);
        clean_tree(t^.r, c);
      end;
      dispose(t);
      dec(c);
      t := NIL;
    end;
  end;


  { clear_expr[e] zamienia [e] w wyrażenie puste, usuwając drzewo z pamięci }
  procedure clear_expr(var e : Expression);
  begin
    if e.t = NIL then exit;
    links_modify(e.t, -1); { sugeruję korzeniowi, że nie jest potrzebny }
    clean_tree(e.t, e.size); { i teraz czyszczenie usuwa wszystko }
  end;


  { expr_derivative[e] zamienia [e] na drzewo będące jego pochodną, wypisując
    przy tym informacje o ilości wierzchołków na stderr }
  procedure expr_derivative(var e : Expression);
  var
    b : ExprTree;

    procedure der(var t : ExprTree; var c : Integer);
    var
      bt : ExprTree;
    begin
      bt := t;  { odwiązanie drzewa na bok, żeby nie stracić wskaźnika } 
      links_modify(bt, -1); { zmniejszam, bo prawie zawsze zastąpię }
      { wszystkie przez analogię, więc tylko ogólny schemat:
        - nadpisuję [t] przez nowe wierzchołki zależnie od funkcji
        - poprawiam licznik rozmiaru drzewa o nowe wierzchołki
        - rekurencyjnie różniczkuję odpowiednie gałęzie	
        Ważne!
        Za poprawianie liczników referencji odpowiada samo make_node[]
        (wyjaśnione w komentarzach do niego), dlatego czasem jest użyte
        z NILami, które po chwili zostają nadpisane }
      case t^.ch of
        Scalar : begin
          t := zero();
          inc(c)
        end;
        Variable : begin
          t := one();
          inc(c)
        end;
        UnaryOperator : begin
          if t^.v = 'exp' then begin
            t := make_node('*', BinaryOperator, bt, bt^.l);
            c := c + 1;
            der(t^.r, c);
          end else if t^.v = 'ln' then begin
            t := make_node('*', BinaryOperator, NIL, bt^.l);
            t^.l := make_node('/', BinaryOperator, NIL, bt^.l);
            (t^.l)^.l := one();
            c := c + 3;
            der(t^.r, c);
          end else if t^.v = 'sin' then begin
            t := make_node('*', BinaryOperator, NIL, bt^.l);
            t^.l := make_node('cos', UnaryOperator, bt^.l, NIL);
            c := c + 2;
            der(t^.r, c);
          end else if t^.v = 'cos' then begin
            t := make_node('*', BinaryOperator, NIL, bt^.l);
            t^.l := make_node('-', BinaryOperator, NIL, NIL);
            (t^.l)^.l := zero();
            (t^.l)^.r := make_node('sin', UnaryOperator, bt^.l, NIL);
            c := c + 4;
            der(t^.r, c);
          end;
        end;
        BinaryOperator : begin
          if t^.v = '+' then begin
            t := make_node('+', BinaryOperator, bt^.l, bt^.r);
            inc(c);
            der(t^.l, c);
            der(t^.r, c);
          end else if t^.v = '-' then begin
            t := make_node('-', BinaryOperator, bt^.l, bt^.r);
            inc(c);
            der(t^.l, c);
            der(t^.r, c);
          end else if t^.v = '*' then begin
            t := make_node('+', BinaryOperator, NIL, NIL);
            t^.l := make_node('*', BinaryOperator, bt^.l, bt^.r);
            t^.r := make_node('*', BinaryOperator, bt^.l, bt^.r);
            c := c + 3;
            der((t^.l)^.l, c);
            der((t^.r)^.r, c);
          end else if t^.v = '/' then begin
            t := make_node('/', BinaryOperator, NIL, NIL);
            t^.r := make_node('*', BinaryOperator, bt^.r, bt^.r);
            t^.l := make_node('-', BinaryOperator, NIL, NIL);
            (t^.l)^.l := make_node('*', BinaryOperator, bt^.l, bt^.r);
            (t^.l)^.r := make_node('*', BinaryOperator, bt^.l, bt^.r);
            c := c + 5;
            der(((t^.l)^.l)^.l, c);
            der(((t^.l)^.r)^.r, c);
          end;
        end;
      end;
    end;

  begin
    b := e.t; { odwiązanie drzewa na bok, żeby nie stracić wskaźnika } 
    writeln(stderr, e.size); { rozmiar przed }
    der(e.t, e.size);
    writeln(stderr, e.size); { rozmiar w trakcie }
    clean_tree(b, e.size);
    writeln(stderr, e.size); { rozmiar po }
  end;

end.
