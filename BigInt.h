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
BigInt BigInt_clone(BigInt);
char  *BigInt_to_binary_string(BigInt);
BigInt BigInt_from_binary_string(char *);
BigInt BigInt_add(BigInt x, BigInt y);
BigInt BigInt_sub(BigInt x, BigInt y);
