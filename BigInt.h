#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef u_int32_t u32;

struct {
  bool sign;
  ssize_t len;
  u32* digits; // 2**32 would be [1,0], 2**64 would be [1,0,0]
} typedef BigInt;

void BigInt_free(BigInt);
void BigInt_shrink(BigInt *);
BigInt BigInt_clone(BigInt);
char  *BigInt_to_binary_string(BigInt);
BigInt BigInt_from_binary_string(char *);
BigInt BigInt_add(BigInt, BigInt);
BigInt BigInt_sub(BigInt, BigInt);
BigInt BigInt_shiftleft(BigInt, ssize_t);
BigInt BigInt_mul(BigInt, BigInt);
