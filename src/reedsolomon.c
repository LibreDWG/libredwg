/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2026 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * reedsolomon.c: Reed-Solomon (255,239) en-, decoding, as used for the
 * system pages of r2007+ DWGs. Written from scratch from the textbook algorithms:
 * syndromes, Berlekamp-Massey, Chien search and Forney's formula.
 * written by Reini Urban, with LLM assistance
 *
 * The code is over GF(2^8) with the primitive polynomial
 *
 *     X^8 + X^6 + X^5 + X^3 + 1      (0x169)
 *
 * and X as the primitive element alpha. The 16 generator roots are
 * alpha^1 ... alpha^16. A block is 239 data bytes followed by 16 parity
 * bytes. Its byte i is the coefficient of X^i of the code word, so the code
 * word is data(X) + X^239 * parity(X), and the parity is X^16 * data(X)
 * modulo the generator. A shorter data part is padded with zeros.
 */

#include "config.h"
#include <string.h>
#include "reedsolomon.h"

#define RS_N 255 /* code word length */
#define RS_PARITY 16
#define RS_POLY 0x169
#define RS_FCR 1 /* first root of the generator is alpha^RS_FCR */

static unsigned char gf_exp[2 * RS_N];
static unsigned char gf_log[RS_N + 1];
static unsigned char gf_gen[RS_PARITY + 1]; /* generator, low degree first */
static int gf_ready = 0;

static unsigned char
gf_mul (const unsigned char a, const unsigned char b)
{
  if (!a || !b)
    return 0;
  return gf_exp[gf_log[a] + gf_log[b]];
}

static unsigned char
gf_inv (const unsigned char a)
{
  /* a != 0 */
  return gf_exp[RS_N - gf_log[a]];
}

/* The tables are small and constant, so the lazy init is idempotent and
   benign if it races. */
static void
gf_init (void)
{
  int i, j;
  unsigned x = 1;

  if (gf_ready)
    return;
  for (i = 0; i < RS_N; i++)
    {
      gf_exp[i] = (unsigned char)x;
      gf_log[x] = (unsigned char)i;
      x <<= 1;
      if (x & 0x100)
        x ^= RS_POLY;
    }
  for (i = RS_N; i < 2 * RS_N; i++)
    gf_exp[i] = gf_exp[i - RS_N];
  /* g(X) = product of (X - alpha^(FCR+i)) */
  memset (gf_gen, 0, sizeof (gf_gen));
  gf_gen[0] = 1;
  for (i = 0; i < RS_PARITY; i++)
    {
      const unsigned char root = gf_exp[(RS_FCR + i) % RS_N];
      for (j = i + 1; j > 0; j--)
        gf_gen[j] = (unsigned char)(gf_gen[j - 1] ^ gf_mul (gf_gen[j], root));
      gf_gen[0] = gf_mul (gf_gen[0], root);
    }
  gf_ready = 1;
}

/* Evaluate p[0..len-1] (low degree first) at x with Horner's rule. */
static unsigned char
poly_eval (const unsigned char *p, const int len, const unsigned char x)
{
  unsigned char r = 0;
  int i;
  for (i = len - 1; i >= 0; i--)
    r = (unsigned char)(gf_mul (r, x) ^ p[i]);
  return r;
}

/*
 * Encode a block. Only the trailing 16 parity bytes are computed in
 * a buffer which caller preallocates. count is the number of data bytes,
 * at most 239.
 */
void
rs_encode_block (unsigned char *parity, unsigned char *src, int count)
{
  int i, j;

  gf_init ();
  memset (parity, 0, RS_PARITY);
  if (count > RS_N - RS_PARITY)
    count = RS_N - RS_PARITY;
  /* Long division of X^16 * data(X) by g(X), highest degree first. The
     remainder is kept low degree first in parity[]. */
  for (i = count - 1; i >= 0; i--)
    {
      const unsigned char lead = (unsigned char)(parity[RS_PARITY - 1] ^ src[i]);
      for (j = RS_PARITY - 1; j > 0; j--)
        parity[j] = (unsigned char)(parity[j - 1] ^ gf_mul (lead, gf_gen[j]));
      parity[0] = gf_mul (lead, gf_gen[0]);
    }
}

/*
 * Decode a 255-byte block in place. Returns the number of corrected errors
 * if fix is set, or -1 in case of unfixed errors (or any error without fix).
 * 0 means no error.
 */
int
rs_decode_block (unsigned char *blk, int fix)
{
  unsigned char syn[RS_PARITY];
  unsigned char lambda[RS_PARITY + 1]; /* error locator, low degree first */
  unsigned char prev[RS_PARITY + 1];   /* the locator before the last change */
  unsigned char omega[RS_PARITY];      /* error evaluator */
  int pos[RS_PARITY];
  unsigned char val[RS_PARITY];
  int i, j, r, nerr, deg, shift, have_err = 0;
  int len = 0;           /* current number of errors in the LFSR */
  unsigned char prev_d = 1; /* the discrepancy at the last change */

  gf_init ();
  for (j = 0; j < RS_PARITY; j++)
    {
      /* S_j = r(alpha^(FCR+j)) */
      unsigned char s = 0;
      for (i = RS_N - 1; i >= 0; i--)
        s = (unsigned char)(gf_mul (s, gf_exp[(RS_FCR + j) % RS_N]) ^ blk[i]);
      syn[j] = s;
      if (s)
        have_err = 1;
    }
  if (!have_err)
    return 0;
  if (!fix)
    return -1;

  /* Berlekamp-Massey */
  memset (lambda, 0, sizeof (lambda));
  memset (prev, 0, sizeof (prev));
  lambda[0] = prev[0] = 1;
  shift = 1; /* the distance since the last change of the length */
  for (r = 0; r < RS_PARITY; r++)
    {
      unsigned char d = syn[r];
      for (i = 1; i <= len; i++)
        d ^= gf_mul (lambda[i], syn[r - i]);
      if (!d)
        {
          shift++;
          continue;
        }
      if (2 * len <= r)
        {
          unsigned char tmp[RS_PARITY + 1];
          memcpy (tmp, lambda, sizeof (tmp));
          /* lambda -= d/prev_d * X^shift * prev */
          {
            const unsigned char f = gf_mul (d, gf_inv (prev_d));
            for (i = 0; i + shift <= RS_PARITY; i++)
              lambda[i + shift] ^= gf_mul (f, prev[i]);
          }
          memcpy (prev, tmp, sizeof (prev));
          prev_d = d;
          len = r + 1 - len;
          shift = 1;
        }
      else
        {
          const unsigned char f = gf_mul (d, gf_inv (prev_d));
          for (i = 0; i + shift <= RS_PARITY; i++)
            lambda[i + shift] ^= gf_mul (f, prev[i]);
          shift++;
        }
    }
  if (len > RS_PARITY / 2)
    return -1;
  deg = 0;
  for (i = 0; i <= RS_PARITY; i++)
    if (lambda[i])
      deg = i;
  if (deg != len)
    return -1;

  /* Chien search: lambda(alpha^-p) == 0 for an error at position p */
  nerr = 0;
  for (i = 0; i < RS_N; i++)
    {
      if (!poly_eval (lambda, len + 1, gf_exp[(RS_N - i) % RS_N]))
        {
          if (nerr >= RS_PARITY / 2)
            return -1;
          pos[nerr++] = i;
        }
    }
  if (nerr != len)
    return -1; /* not all roots are in the field, more errors than we can fix */

  /* omega(X) = S(X) * lambda(X) mod X^16 */
  for (i = 0; i < RS_PARITY; i++)
    {
      unsigned char o = 0;
      for (j = 0; j <= i && j <= len; j++)
        o ^= gf_mul (lambda[j], syn[i - j]);
      omega[i] = o;
    }
  /* Forney: e = X^(1-FCR) * omega(X^-1) / lambda'(X^-1), X = alpha^pos */
  for (i = 0; i < nerr; i++)
    {
      const unsigned char xinv = gf_exp[(RS_N - pos[i]) % RS_N];
      unsigned char num = poly_eval (omega, RS_PARITY, xinv);
      unsigned char den = 0, xp = 1; /* the derivative: odd terms only */
      for (j = 1; j <= len; j += 2)
        {
          /* xp is xinv^(j-1) */
          den ^= gf_mul (lambda[j], xp);
          xp = gf_mul (xp, gf_mul (xinv, xinv));
        }
      if (!den)
        return -1;
      num = gf_mul (num, gf_exp[((RS_N + 1 - RS_FCR) * pos[i]) % RS_N]);
      val[i] = gf_mul (num, gf_inv (den));
    }
  for (i = 0; i < nerr; i++)
    blk[pos[i]] ^= val[i];

  /* verify, a failed decode could "fix" to another code word, or not */
  for (j = 0; j < RS_PARITY; j++)
    if (poly_eval (blk, RS_N, gf_exp[(RS_FCR + j) % RS_N]))
      {
        for (i = 0; i < nerr; i++)
          blk[pos[i]] ^= val[i]; /* undo */
        return -1;
      }
  return nerr;
}
