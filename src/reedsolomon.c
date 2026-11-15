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
 * reedsolomon.c: Reed-Solomon (255,239) and (255,251) en-, decoding, as used
 * for the system and the data pages of r2007 DWGs. Written from scratch from
 * the textbook algorithms: syndromes, Berlekamp-Massey, Chien search and
 * Forney's formula.
 * written by Reini Urban, with LLM assistance
 *
 * The codes are over GF(2^8), with X as the primitive element alpha, and
 * the generator roots alpha^1 ... alpha^nroots. The system pages have 16
 * parity bytes (corrects 8 errors) and the primitive polynomial
 *
 *     X^8 + X^6 + X^5 + X^3 + 1      (0x169)
 *
 * the data pages have 4 parity bytes (corrects 2 errors) and
 *
 *     X^8 + X^4 + X^3 + X^2 + 1      (0x11d)
 *
 * A block is the data bytes followed by the parity bytes, 255 in all. Its
 * byte i is the coefficient of X^i of the code word, so the code word is
 * data(X) + X^k * parity(X), and the parity is X^nroots * data(X) modulo the
 * generator. A shorter data part is padded with zeros.
 */

#include "config.h"
#include <string.h>
#include "reedsolomon.h"

#define RS_N 255 /* code word length */
#define RS_FCR 1 /* first root of the generator is alpha^RS_FCR */
#define RS_MAXPARITY 16

typedef struct
{
  unsigned poly;
  int nroots;
  unsigned char exp[2 * RS_N];
  unsigned char log[RS_N + 1];
  unsigned char gen[RS_MAXPARITY + 1]; /* generator, low degree first */
  int ready;
} Rs_Code;

static Rs_Code rs_system = { 0x169, 16, { 0 }, { 0 }, { 0 }, 0 };
static Rs_Code rs_data = { 0x11d, 4, { 0 }, { 0 }, { 0 }, 0 };

static unsigned char
gf_mul (const Rs_Code *restrict c, const unsigned char a, const unsigned char b)
{
  if (!a || !b)
    return 0;
  return c->exp[c->log[a] + c->log[b]];
}

static unsigned char
gf_inv (const Rs_Code *restrict c, const unsigned char a)
{
  /* a != 0 */
  return c->exp[RS_N - c->log[a]];
}

/* The tables are small and constant, so the lazy init is idempotent and
   benign if it races. */
static void
rs_init (Rs_Code *restrict c)
{
  int i, j;
  unsigned x = 1;

  if (c->ready)
    return;
  for (i = 0; i < RS_N; i++)
    {
      c->exp[i] = (unsigned char)x;
      c->log[x] = (unsigned char)i;
      x <<= 1;
      if (x & 0x100)
        x ^= c->poly;
    }
  for (i = RS_N; i < 2 * RS_N; i++)
    c->exp[i] = c->exp[i - RS_N];
  /* g(X) = product of (X - alpha^(FCR+i)) */
  memset (c->gen, 0, sizeof (c->gen));
  c->gen[0] = 1;
  for (i = 0; i < c->nroots; i++)
    {
      const unsigned char root = c->exp[(RS_FCR + i) % RS_N];
      for (j = i + 1; j > 0; j--)
        c->gen[j] = (unsigned char)(c->gen[j - 1] ^ gf_mul (c, c->gen[j], root));
      c->gen[0] = gf_mul (c, c->gen[0], root);
    }
  c->ready = 1;
}

/* Evaluate p[0..len-1] (low degree first) at x with Horner's rule. */
static unsigned char
poly_eval (const Rs_Code *restrict c, const unsigned char *p, const int len,
           const unsigned char x)
{
  unsigned char r = 0;
  int i;
  for (i = len - 1; i >= 0; i--)
    r = (unsigned char)(gf_mul (c, r, x) ^ p[i]);
  return r;
}

static void
rs_encode (Rs_Code *restrict c, unsigned char *parity,
           const unsigned char *src, int count)
{
  const int np = c->nroots;
  int i, j;

  rs_init (c);
  memset (parity, 0, np);
  if (count > RS_N - np)
    count = RS_N - np;
  /* Long division of X^nroots * data(X) by g(X), highest degree first. The
     remainder is kept low degree first in parity[]. */
  for (i = count - 1; i >= 0; i--)
    {
      const unsigned char lead = (unsigned char)(parity[np - 1] ^ src[i]);
      for (j = np - 1; j > 0; j--)
        parity[j] = (unsigned char)(parity[j - 1] ^ gf_mul (c, lead, c->gen[j]));
      parity[0] = gf_mul (c, lead, c->gen[0]);
    }
}

static int
rs_decode (Rs_Code *restrict c, unsigned char *blk, const int fix)
{
  const int np = c->nroots;
  const int t = np / 2;
  unsigned char syn[RS_MAXPARITY];
  unsigned char lambda[RS_MAXPARITY + 1]; /* error locator, low degree first */
  unsigned char prev[RS_MAXPARITY + 1];   /* the locator before the last change */
  unsigned char omega[RS_MAXPARITY];      /* error evaluator */
  int pos[RS_MAXPARITY];
  unsigned char val[RS_MAXPARITY];
  int i, j, r, nerr, deg, shift, have_err = 0;
  int len = 0;              /* current number of errors in the LFSR */
  unsigned char prev_d = 1; /* the discrepancy at the last change */

  rs_init (c);
  for (j = 0; j < np; j++)
    {
      /* S_j = r(alpha^(FCR+j)) */
      unsigned char s = 0;
      for (i = RS_N - 1; i >= 0; i--)
        s = (unsigned char)(gf_mul (c, s, c->exp[(RS_FCR + j) % RS_N]) ^ blk[i]);
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
  for (r = 0; r < np; r++)
    {
      unsigned char d = syn[r];
      for (i = 1; i <= len; i++)
        d ^= gf_mul (c, lambda[i], syn[r - i]);
      if (!d)
        {
          shift++;
          continue;
        }
      {
        unsigned char tmp[RS_MAXPARITY + 1];
        const unsigned char f = gf_mul (c, d, gf_inv (c, prev_d));
        memcpy (tmp, lambda, sizeof (tmp));
        /* lambda -= d/prev_d * X^shift * prev */
        for (i = 0; i + shift <= np; i++)
          lambda[i + shift] ^= gf_mul (c, f, prev[i]);
        if (2 * len <= r)
          {
            memcpy (prev, tmp, sizeof (prev));
            prev_d = d;
            len = r + 1 - len;
            shift = 1;
          }
        else
          shift++;
      }
    }
  if (len > t)
    return -1;
  deg = 0;
  for (i = 0; i <= np; i++)
    if (lambda[i])
      deg = i;
  if (deg != len)
    return -1;

  /* Chien search: lambda(alpha^-p) == 0 for an error at position p */
  nerr = 0;
  for (i = 0; i < RS_N; i++)
    {
      if (!poly_eval (c, lambda, len + 1, c->exp[(RS_N - i) % RS_N]))
        {
          if (nerr >= t)
            return -1;
          pos[nerr++] = i;
        }
    }
  if (nerr != len)
    return -1; /* not all roots are in the field, more errors than we can fix */

  /* omega(X) = S(X) * lambda(X) mod X^nroots */
  for (i = 0; i < np; i++)
    {
      unsigned char o = 0;
      for (j = 0; j <= i && j <= len; j++)
        o ^= gf_mul (c, lambda[j], syn[i - j]);
      omega[i] = o;
    }
  /* Forney: e = X^(1-FCR) * omega(X^-1) / lambda'(X^-1), X = alpha^pos */
  for (i = 0; i < nerr; i++)
    {
      const unsigned char xinv = c->exp[(RS_N - pos[i]) % RS_N];
      const unsigned char xinv2 = gf_mul (c, xinv, xinv);
      unsigned char num = poly_eval (c, omega, np, xinv);
      unsigned char den = 0, xp = 1; /* the derivative: odd terms only */
      for (j = 1; j <= len; j += 2)
        {
          /* xp is xinv^(j-1) */
          den ^= gf_mul (c, lambda[j], xp);
          xp = gf_mul (c, xp, xinv2);
        }
      if (!den)
        return -1;
      num = gf_mul (c, num, c->exp[((RS_N + 1 - RS_FCR) * pos[i]) % RS_N]);
      val[i] = gf_mul (c, num, gf_inv (c, den));
    }
  for (i = 0; i < nerr; i++)
    blk[pos[i]] ^= val[i];

  /* verify, a failed decode could "fix" to another code word, or not */
  for (j = 0; j < np; j++)
    if (poly_eval (c, blk, RS_N, c->exp[(RS_FCR + j) % RS_N]))
      {
        for (i = 0; i < nerr; i++)
          blk[pos[i]] ^= val[i]; /* undo */
        return -1;
      }
  return nerr;
}

/*
 * Encode a block of count (at most 239) data bytes. Only the trailing 16
 * parity bytes are computed, in a buffer which the caller preallocates.
 */
void
rs_encode_block (unsigned char *parity, unsigned char *src, int count)
{
  rs_encode (&rs_system, parity, src, count);
}

/*
 * Decode a 255-byte block in place: 239 data bytes and 16 parity bytes.
 * Returns the number of corrected errors (at most 8) if fix is set, 0 if
 * there is no error, or -1 in case of unfixable errors, or of any error
 * without fix.
 */
int
rs_decode_block (unsigned char *blk, int fix)
{
  return rs_decode (&rs_system, blk, fix);
}

/* The same for a data page block of 251 data bytes and 4 parity bytes, with
   at most 2 errors. */
void
rs_encode_data_block (unsigned char *parity, unsigned char *src, int count)
{
  rs_encode (&rs_data, parity, src, count);
}

int
rs_decode_data_block (unsigned char *blk, int fix)
{
  return rs_decode (&rs_data, blk, fix);
}
