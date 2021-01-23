#include "BigInt.h"
#ifndef max
#define max(a,b) (((a) > (b)) ? a : b)
#endif

void BigInt_free(BigInt x) {
  free(x.digits);
}

BigInt BigInt_clone(BigInt x) {
  BigInt y;
  y.sign = x.sign;
  y.len = x.len;
  y.digits = malloc(x.len * sizeof(u32));
  memcpy(y.digits, x.digits, x.len * sizeof(u32));
  return y;
}

char *BigInt_to_binary_string(BigInt x) {
  char *result = malloc(x.len * 32 + 1);
  char *current_char = result;
  if (!x.sign) {
    *(current_char++) = '-';
  }
  bool found_nonzero_digit = false;
  for (ssize_t i = 0; i < x.len; ++i) {
    u32 digit = x.digits[i];
    for (u32 current_digit = 1 << 31; current_digit != 0; current_digit >>= 1) {
      if (digit & current_digit) {
        *(current_char++) = '1';
        found_nonzero_digit = true;
      } else if (found_nonzero_digit) {
        // don't show trailing zeros
        *(current_char++) = '0';
      }
    }
  }
  if (!found_nonzero_digit) {
    // this is zero
    *(current_char++) = '0';
  }
  *current_char = '\0';
  return result;
}

BigInt BigInt_add(BigInt x, BigInt y) {
  // if one of them is negative and the other isn't, this is subtraction
  if (y.sign && !x.sign) {
    return BigInt_sub(y, x);
  } else if (x.sign && !y.sign) {
    return BigInt_sub(x, y);
  }
  // make sure x.len >= y.len
  if (y.len > x.len) {
    return BigInt_add(y, x);
  }
  BigInt z;
  z.len = x.len;
  z.sign = x.sign; // equal to y's sign
  z.digits = malloc(z.len * sizeof(u32));
  memset(z.digits, 0, z.len * sizeof(u32));
  bool carry = false;
  for (ssize_t i = x.len - 1; i >= 0; --i) {
    u32 a = x.digits[x.len - i - 1];
    if (i < y.len) {
      u32 b = y.digits[x.len - i - 1];
      for (u32 current_digit = 1; current_digit != 1 << 31; current_digit <<= 1) {
        u32 digit_a = a & current_digit;
        u32 digit_b = b & current_digit;
        bool both = digit_a && digit_b;
        bool only_one = (digit_a && !digit_b) || (digit_b && !digit_a);
        if (carry) {
          if (both) {
            z.digits[z.len - i - 1] |= current_digit;
            carry = false;
          } else if (only_one) {
            carry = true;
          } else {
            z.digits[z.len - i - 1] |= current_digit;
            carry = false;
          }
        } else {
          if (both) {
            carry = true;
          } else if (only_one) {
            z.digits[z.len - i - 1] |= current_digit;
          }
        }
      }
    } else {
      for (u32 current_digit = 1; current_digit != 1 << 31; current_digit <<= 1) {
        u32 digit_a = a & current_digit;
        if (carry) {
          if (!digit_a) {
            z.digits[z.len - i - 1] |= current_digit;
          }
        } else if (digit_a) {
          z.digits[z.len - i - 1] |= current_digit;
        }
      }
    }
  }
  return z;
}

BigInt BigInt_sub(BigInt x, BigInt y) {
  return x;
}
