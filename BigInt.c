#include "BigInt.h"
#ifndef max
#define max(a,b) (((a) > (b)) ? a : b)
#endif

void BigInt_free(BigInt x) {
  free(x.digits);
}

void BigInt_shrink(BigInt *x) {
  // removes pre-trailing zeroes
  for (ssize_t i = 0; i < x->len; i++) {
    if (x->digits[i] != 0) {
      // found non-zero, remove everything before it
      if (i != 0) {
        x->len = x->len - i;
        memmove(x->digits, x->digits + i, x->len * sizeof(u32));
      }
      return;
    }
  }
  // only zeroes
  free(x->digits);
  x->len = 1;
  x->digits = malloc(sizeof(u32));
  x->digits[0] = 0;
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
BigInt BigInt_from_binary_string(char *s) {
  BigInt x;
  x.sign = true;
  x.len = 1 + strlen(s) / 32;
  x.digits = calloc(x.len, sizeof(u32));

  ssize_t current_digit = x.len - 1;
  u32 current_digit_digit = 1;
  for (char *c = s + strlen(s) - 1; c != s - 1; --c) {
    if (*c == '-') {
      x.sign = false;
    } else if (*c == '0') {
      current_digit_digit <<= 1;
    } else if (*c == '1') {
      x.digits[current_digit] |= current_digit_digit;
      current_digit_digit <<= 1;
    }
    if (current_digit_digit == 0) {
      current_digit--;
      current_digit_digit = 1;
    }
  }
  BigInt_shrink(&x);
  return x;
}

BigInt BigInt_add(BigInt x, BigInt y) {
  // if one of them is negative and the other isn't, this is subtraction
  if (x.sign != y.sign) {
    y.sign = !y.sign;
    return BigInt_sub(x, y);
  }
  // make sure x.len >= y.len
  if (y.len > x.len) {
    return BigInt_add(y, x);
  }
  BigInt z;
  z.len = x.len + 1;
  z.sign = x.sign; // equal to y's sign
  z.digits = calloc(z.len, sizeof(u32));
  bool carry = false;
  for (ssize_t i = 0; i < z.len; ++i) {
    u32 a = 0, b = 0;
    if (i < x.len) {
      a = x.digits[x.len - i - 1];
    }
    if (i < y.len) {
      b = y.digits[y.len - i - 1];
    }
    u32 c = a + b + carry;
    if (c < a || (carry && c == a)) {
      // overflowed
      carry = true;
    } else {
      carry = false;
    }
    z.digits[z.len - i - 1] = c;
    
  }
  BigInt_shrink(&z);
  return z;
}

BigInt BigInt_sub(BigInt x, BigInt y) {
  // if the signs are opposite, this is addition
  if (x.sign != y.sign) {
    y.sign = !y.sign;
    return BigInt_add(x, y);
  }
  // make sure x.len >= y.len
  if (y.len > x.len) {
    BigInt z = BigInt_sub(y, x);
    z.sign = !z.sign;
    return z;
  }
  // copy x with extra zeroes before it
  BigInt z = BigInt_clone(x);
  // start subtraction
  for (ssize_t i = 0; i < z.len; i++) {
    u32 a = 0, b = 0;
    if (i < z.len) {
      a = z.digits[z.len - i - 1];
    }
    if (i < y.len) {
      b = y.digits[y.len - i - 1];
    }
    u32 c = a - b;
    if (c <= a) {
      // no overflow
      z.digits[z.len - i - 1] = c;
    } else {
      // take 1 from the next digit in z
      bool found_nonzero = false;
      for (ssize_t j = i + 1; j < z.len; ++j) {
        u32 d = z.digits[z.len - j - 1];
        if (d != 0) {
          found_nonzero = true;
          z.digits[z.len - j - 1] = z.digits[z.len - j - 1] - 1;
          // set everything that was tried before it to 1...1
          for (ssize_t k = j - 1; k > i; --k) {
            z.digits[z.len - k - 1] -= 1;
          }
          break;
        }
      }
      if (found_nonzero) {
        z.digits[z.len - i - 1] = c;
      } else {
        z.digits[z.len - i - 1] = -c;
        z.sign = false;
      }
    }
  }
  BigInt_shrink(&z);
  return z;
}

BigInt BigInt_shiftleft(BigInt x, ssize_t shift) {
  if (shift == 0) {
    return BigInt_clone(x);
  }
  BigInt y;
  y.sign = x.sign;
  y.len = x.len + ((shift + 31) / 32);
  y.digits = calloc(y.len, sizeof(u32));
  u32 inner_shift = shift % 32;
  u32 between_shift = (shift + 31) / 32;
  for (ssize_t i = 0; i < x.len - 1; ++i) {
    y.digits[i + between_shift] = (x.digits[i] << inner_shift) + (x.digits[i + 1] >> (32 - inner_shift));
  }
  y.digits[between_shift + x.len - 1] = x.digits[x.len - 1] << inner_shift;
  return y;
}

BigInt BigInt_mul(BigInt x, BigInt y) {
  BigInt z;
  z.sign = x.sign == y.sign;
  z.len = x.len + y.len;
  z.digits = calloc(z.len, sizeof(u32));
  for (u32 i = y.len - 1; i <= y.len; --i) {
    u32 digit = y.digits[i];
    for (u32 j = 0; j < 32; ++j) {
      u32 bits = 1 << (31 - j);
      u32 binary_digit = digit & bits;
      if (binary_digit) {
        ssize_t shift = 32 * i + (31 - j);
        BigInt shifted = BigInt_shiftleft(x, shift);
        BigInt next_z = BigInt_add(shifted, z);
        BigInt_free(z);
        BigInt_free(shifted);
        z = next_z;
      }
    }
  }
  BigInt_shrink(&z);
  return z;
}
