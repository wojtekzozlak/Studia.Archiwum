{ Różniczkowanie wyrażeń (projekt z IPP) - Wojciech Żółtak - 08.03.2010
  Program wczytuje wyrażenie algebraiczne w postaci drzewa, które następnie
  różniczkuje i wypisuje na standardowe wyjście.
  Dodatkowo na stderr zostaje wysłana informacja o ilości wierzchołków w drzewie
  przed różniczkowaniem, w trakcie (przed usunięciem zbędnych) oraz po }
program
  zad2;

uses
  Derivative;

var
  e : Expression;
begin
  Derivative.load_expr(e);
  Derivative.expr_derivative(e);
  Derivative.print_expr(e);
  Derivative.clear_expr(e);
end.
