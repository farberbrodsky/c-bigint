#include "BigInt.h"

struct {
  BigInt a;
  BigInt b;
} typedef Fraction;

void Fraction_free(Fraction);
Fraction Fraction_simplify(Fraction);
Fraction Fraction_clone(Fraction);
char *Fraction_to_decimal_string(Fraction);
Fraction Fraction_add(Fraction, Fraction);
