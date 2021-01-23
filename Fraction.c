#include "Fraction.h"

void Fraction_free(Fraction x) {
  BigInt_free(x.a);
  BigInt_free(x.b);
}

BigInt gcd(BigInt a, BigInt b) {
  BigInt c = BigInt_clone(a);
  BigInt d = BigInt_clone(b);
  while (!BigInt_equal(c, d)) {
    if (BigInt_greater(c, d)) {
      BigInt next_c = BigInt_sub(c, d);
      BigInt_free(c);
      c = next_c;
    } else {
      BigInt next_d = BigInt_sub(d, c);
      BigInt_free(d);
      d = next_d;
    }
  }
  BigInt_free(d);
  return c;
}

Fraction Fraction_simplify(Fraction x) {
  BigInt g = gcd(x.a, x.b);
  BigInt a;
  BigInt b;
  BigInt_div(x.a, g, &a, NULL);
  BigInt_div(x.b, g, &b, NULL);
  Fraction result;
  result.a = a;
  result.b = b;
  return result;
}

char *Fraction_to_decimal_string(Fraction x) {
  char *a = BigInt_to_decimal_string(x.a);
  char *b = BigInt_to_decimal_string(x.b);
  unsigned long alen = strlen(a);
  unsigned long blen = strlen(b);
  char *result = malloc(alen + blen + 2);
  memcpy(result, a, alen);
  result[alen] = '/';
  memcpy(result + alen + 1, b, blen);
  result[alen + blen + 2] = '\0';
  free(a);
  free(b);
  return result;
}

Fraction Fraction_add(Fraction x, Fraction y) {
  Fraction result;

  result.b = BigInt_mul(x.b, y.b);

  BigInt a1 = BigInt_mul(x.a, y.b);
  BigInt a2 = BigInt_mul(x.b, y.a);

  result.a = BigInt_add(a1, a2);

  BigInt_free(a1);
  BigInt_free(a2);
  
  Fraction final_result = Fraction_simplify(result);
  Fraction_free(result);

  return final_result;
}
