# coding=utf-8

from pypol import *
import math

def interpolacja_hermit(punkty):
  """Liczy wielomian interpolacyjny dla zadania Hermite'a (niezbyt wydajnie).

     Argument jest słownikiem z liczbami jako kluczami i listami liczb jako wartościami
     (klucz - współrzędna, wartość - wartości w kolejnych pochodnych). Np.

         { 0: [1, 0, 0], 1: [0, 0, 0] }
  """
  tab = []
  tab2 = []
  ret = []
  for k in sorted(punkty.iterkeys()):
    v = punkty[k]
    for x in v:
      tab.append(k)
      tab2.append(v[0])

  ret.append(tab2[0])
  for i in range(0, len(tab) - 1):
    for j in range(0, len(tab2) - 1 - i):
      if tab[j] != tab[j + i + 1]:
        tab2[j] = (tab2[j + 1] - tab2[j]) / (tab[j + i + 1] - tab[j])
      else:
        tab2[j] = punkty[tab[j]][i + 1] / math.factorial(i + 1)
    ret.append(tab2[0])


  # wynik w bazie potęgowej
  ret_w = ret[0]
  w = 1.0
  for i in range(1, len(ret)):
    if tab[i - 1] >= 0:
      pol = 'x - %d' % tab[i - 1]
    else:
      pol = 'x + %d' % abs(tab[i - 1])
    w = w * polynomial(pol)
    ret_w += w * ret[i]
  return ret_w

print "H_0,0 : %s " % interpolacja_hermit({ 0.0: [1.0, 0.0, 0.0], 1.0: [0.0, 0.0, 0.0] })
print "H_0,1 : %s " % interpolacja_hermit({ 0.0: [0.0, 1.0, 0.0], 1.0: [0.0, 0.0, 0.0] })
print "H_0,2 : %s " % interpolacja_hermit({ 0.0: [0.0, 0.0, 1.0], 1.0: [0.0, 0.0, 0.0] })
print "H_1,0 : %s " % interpolacja_hermit({ 0.0: [0.0, 0.0, 0.0], 1.0: [1.0, 0.0, 0.0] })
print "H_1,1 : %s " % interpolacja_hermit({ 0.0: [0.0, 0.0, 0.0], 1.0: [0.0, 1.0, 0.0] })
print "H_1,2 : %s " % interpolacja_hermit({ 0.0: [0.0, 0.0, 0.0], 1.0: [0.0, 0.0, 1.0] })
