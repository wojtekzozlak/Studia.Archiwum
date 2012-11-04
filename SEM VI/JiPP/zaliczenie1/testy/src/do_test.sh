#!/bin/bash
echo "" > log
#rm in/*.in
#rm outs/*.out


echo ""
echo "Rozpoczynam testy. Jesli chcesz zobaczyc ile zostalo wykonane wpisz 'tail log' w innym terminalu."
echo "Zobaczysz numer ostatniego wykonanego testu na 100, ktore zostana wykonane."
echo ""

for i in $(seq 1 1 100)
do
    TEST="in/test$i.in"
    OUT1="out/out1_$i.out"
    OUT2="out/out2_$i.out"
    ./bin/make_test > $TEST
    ./bin/sol < $TEST | ./bin/sorter > $OUT1
    ./prog < $TEST | ./bin/sorter > $OUT2
    echo "$TEST" >> log
    diff -w $OUT1 $OUT2 >> log
done

echo ""
echo "---------------------------"
echo "Testy zakonczone. Sprawdz plik 'log' - jesli zawiera tylko nazwy testow, to Twoje odpowiedzi zgadzaja sie z programem przykladowym"

exit 0
