FILES="Parser.H Absyn.C Absyn.H Printer.H Printer.C"
for file in $FILES
do
  cp $file ../src/parser/
  echo "cp $file ../src/parser/"
done

FILES="latte.l latte.y"
for file in $FILES
do
  cp $file ../src/grammar/
  echo "cp $file ../src/grammar/"
done
