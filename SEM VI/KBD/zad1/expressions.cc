#include<iostream>

enum DataType { BOOL, INT, DOUBLE };

template <int T>
struct TypeTraits {};

template <>
struct TypeTraits<BOOL> {
  typedef bool T;
};

template<>
struct TypeTraits<INT> {
  typedef int T;
};

template<>
struct TypeTraits<DOUBLE> {
  typedef double T;
};



template<DataType T1, DataType T2, DataType T3>
struct PlusOperator {
  typename TypeTraits<T3>::T operator ()
      (typename TypeTraits<T1>::T a, typename TypeTraits<T2>::T b){
    return (typename TypeTraits<T3>::T)(a + b);
  }
};


template<DataType T1, DataType T2,
         template <DataType X, DataType Y, DataType Z> class Operation>
void operation(typename TypeTraits<T1>::T* a, 
               typename TypeTraits<T2>::T* b, int n) {
  Operation<T1, T2, T1> op;
  for(int i = 0; i < n; ++i) {
    a[i] = op(a[i], b[i]);
  }
}



int main(){
  int a[2] = {1, 2};
  int b[2] = {3, 4};
  int n = 2;

  operation<INT, INT, PlusOperator>(a, b, 2);
  for(int i = 0; i < n; ++i) std::cout << a[i] << std::endl;

  return 0;
}
