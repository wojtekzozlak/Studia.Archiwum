/****************************************************************************
 * BuiltIn.h
 *
 * Funkcje wbudowane języka.
 *
 * Funkcje wbudowane języka są w całości eksportowane do pliku wynikowego,
 * dzięki czemu jest on niezależny od zewnętrznych bibliotek. Funkcja main()I
 * będącą punktem wejścia programu w Latte jest wywoływana z poziomu funkcji
 * typu main([Ljava/lang/string;)V będącej standardowym punktem wejścia
 * programów w Javie.
 ****************************************************************************/
#ifndef _JVM_BUILTIN_
#define _JVM_BUILTIN_

#include <iostream>

namespace JVM {

std::ostream& writeBuiltIns(std::ostream& stream, std::string name);

}

#endif


