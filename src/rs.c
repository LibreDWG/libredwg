/*             rs.c        */
/* This program is an encoder/decoder for Reed-Solomon codes. Encoding is in
   systematic form, decoding via the Berlekamp iterative algorithm.
   In the present form , the constants mm, nn, tt, and kk=nn-2tt must be
   specified  (the double letters are used simply to avoid clashes with
   other n,k,t used in other programs into which this was incorporated!)
   Also, the irreducible polynomial used to generate GF(2**mm) must also be
   entered -- these can be found in Lin and Costello, and also Clark and Cain.

   The representation of the elements of GF(2**m) is either in index form,
   where the number is the power of the primitive element alpha, which is
   convenient for multiplication (add the powers modulo 2**m-1) or in
   polynomial form, where the bits represent the coefficients of the
   polynomial representation of the number, which is the most convenient form
   for addition.  The two forms are swapped between via lookup tables.
   This leads to fairly messy looking expressions, but unfortunately, there
   is no easy alternative when working with Galois arithmetic.

   The code is not written in the most elegant way, but to the best
   of my knowledge, (no absolute guarantees!), it works.
   However, when including it into a simulation program, you may want to do
   some conversion of global variables (used here because I am lazy!) to
   local variables where appropriate, and passing parameters (eg array
   addresses) to the functions  may be a sensible move to reduce the number
   of global variables and thus decrease the chance of a bug being introduced.

   This program does not handle erasures at present, but should not be hard
   to adapt to do this, as it is just an adjustment to the Berlekamp-Massey
   algorithm. It also does not attempt to decode past the BCH bound -- see
   Blahut "Theory and practice of error control codes" for how to do this.

              Simon Rockliff, University of Adelaide   21/9/89

   26/6/91 Slight modifications to remove a compiler dependent bug which hadn't
           previously surfaced. A few extra comments added for clarity.
           Appears to all work fine, ready for posting to net!

                  Notice
                 --------
   This program may be freely modified and/or given to whoever wants it.
   A condition of such distribution is that the author's contribution be
   acknowledged by his name being left in the comments heading the program,
   however no responsibility is accepted for any financial or other loss which
   may result from some unforseen errors or malfunctioning of the program
   during use.
                                 Simon Rockliff, 26th June 1991
*/

#include <math.h>
#include <stdio.h>
#include "config.h"
#include "rs.h"

#define mm 8   /* RS code over GF(2**4) - change to suit */
#define nn 255 /* nn=2**mm -1   length of codeword */
//#define tt 16  /* number of errors that can be corrected */
#define kk0 251 /* kk = nn-2*tt  */ /* Degree of g(x) = 2*tt. tt=2 data_block */
#define kk1 239 /* kk = nn-2*tt  */ /* Degree of g(x) = 2*tt. tt=8 system_block */

static int data_block_initialized = 0; // kk0
static int system_block_initialized = 0; // kk1

/* specify irreducible polynomial coeffts */
int pp0[mm] = { 1, 0, 1, 1, 1, 0, 0, 0 };
int pp1[mm] = { 1, 0, 0, 1, 0, 1, 1, 0 };
int alpha_to[nn + 1], index_of[nn + 1], gg[nn - kk1 + 1];
int recd[nn], data[kk0], bb[nn - kk1];

static void generate_gf (int *pp);
static void gen_poly (int kk);
static void encode_rs (int kk);
static void decode_rs (int kk);

/* generate GF(2**mm) from the irreducible polynomial p(X) in pp[0]..pp[mm]
   lookup tables:  index->polynomial form   alpha_to[] contains j=alpha**i;
                   polynomial form -> index form  index_of[j=alpha**i] = i
   alpha=2 is the primitive element of GF(2**mm)
*/
static void
generate_gf (int *pp)
{
  int i, mask;

  mask = 1;
  alpha_to[mm] = 0;
  for (i = 0; i < mm; i++)
    {
      alpha_to[i] = mask;
      index_of[alpha_to[i]] = i;
      if (pp[i] != 0)
        alpha_to[mm] ^= mask;
      mask <<= 1;
    }
  index_of[alpha_to[mm]] = mm;
  mask >>= 1;
  for (i = mm + 1; i < nn; i++)
    {
      if (alpha_to[i - 1] >= mask)
        alpha_to[i] = alpha_to[mm] ^ ((alpha_to[i - 1] ^ mask) << 1);
      else
        alpha_to[i] = alpha_to[i - 1] << 1;
      index_of[alpha_to[i]] = i;
    }
  index_of[0] = -1;
}

/* Obtain the generator polynomial of the tt-error correcting, length
  nn=(2**mm -1) Reed Solomon code  from the product of (X+alpha**i), i=1..2*tt
*/
static void
gen_poly (int kk)
{
  int i, j;

  gg[0] = 2; /* primitive element alpha = 2  for GF(2**mm)  */
  gg[1] = 1; /* g(x) = (X+alpha) initially */
  for (i = 2; i <= nn - kk; i++)
    {
      gg[i] = 1;
      for (j = i - 1; j > 0; j--)
        if (gg[j] != 0)
          gg[j] = gg[j - 1] ^ alpha_to[(index_of[gg[j]] + i) % nn];
        else
          gg[j] = gg[j - 1];
      gg[0] = alpha_to[(index_of[gg[0]] + i) % nn]; /* gg[0] can never be zero */
    }
  /* convert gg[] to index form for quicker encoding */
  for (i = 0; i <= nn - kk; i++)
    gg[i] = index_of[gg[i]];
}

static void
encode_rs (int kk)
/* take the string of symbols in data[i], i=0..(k-1) and encode systematically
   to produce 2*tt parity symbols in bb[0]..bb[2*tt-1]
   data[] is input and bb[] is output in polynomial form.
   Encoding is done by using a feedback shift register with appropriate
   connections specified by the elements of gg[], which was generated above.
   Codeword is   c(X) = data(X)*X**(nn-kk)+ b(X)          */
{
  register int i, j;
  int feedback;

  for (i = 0; i < nn - kk; i++)
    bb[i] = 0;
  for (i = kk - 1; i >= 0; i--)
    {
      feedback = index_of[data[i] ^ bb[nn - kk - 1]];
      if (feedback != -1)
        {
          for (j = nn - kk - 1; j > 0; j--)
            if (gg[j] != -1)
              bb[j] = bb[j - 1] ^ alpha_to[(gg[j] + feedback) % nn];
            else
              bb[j] = bb[j - 1];
          bb[0] = alpha_to[(gg[0] + feedback) % nn];
        }
      else
        {
          for (j = nn - kk - 1; j > 0; j--)
            bb[j] = bb[j - 1];
          bb[0] = 0;
        };
    };
};

static void
decode_rs (int kk)
/* assume we have received bits grouped into mm-bit symbols in recd[i],
   i=0..(nn-1),  and recd[i] is index form (ie as powers of alpha).
   We first compute the 2*tt syndromes by substituting alpha**i into rec(X) and
   evaluating, storing the syndromes in s[i], i=1..2tt (leave s[0] zero) .
   Then we use the Berlekamp iteration to find the error location polynomial
   elp[i].   If the degree of the elp is >tt, we cannot correct all the errors
   and hence just put out the information symbols uncorrected. If the degree of
   elp is <=tt, we substitute alpha**i , i=1..n into the elp to get the roots,
   hence the inverse roots, the error location numbers. If the number of errors
   located does not equal the degree of the elp, we have more than tt errors
   and cannot correct them.  Otherwise, we then solve for the error value at
   the error location and correct the error.  The procedure is that found in
   Lin and Costello. For the cases where the number of errors is known to be
   too large to correct, the information symbols as received are output (the
   advantage of systematic encoding is that hopefully some of the information
   symbols will be okay and that if we are in luck, the errors are in the
   parity part of the transmitted codeword).  Of course, these insoluble cases
   can be returned as error flags to the calling routine if desired.   */
{
  int i, j, u, q;
  int elp[nn - kk + 2][nn - kk], d[nn - kk + 2], l[nn - kk + 2],
      u_lu[nn - kk + 2], s[nn - kk + 1];
  const int tt = (nn - kk) / 2;
  int count = 0, syn_error = 0, root[tt], loc[tt], z[tt + 1], err[nn],
      reg[tt + 1];

  /* first form the syndromes */
  for (i = 1; i <= nn - kk; i++)
    {
      s[i] = 0;
      for (j = 0; j < nn; j++)
        if (recd[j] != -1)
          s[i] ^= alpha_to[(recd[j] + i * j) % nn]; /* recd[j] in index form */
      /* convert syndrome from polynomial form to index form  */
      if (s[i] != 0)
        syn_error = 1; /* set flag if non-zero syndrome => error */
      s[i] = index_of[s[i]];
    };

  if (syn_error) /* if errors, try and correct */
    {
      /* compute the error location polynomial via the Berlekamp iterative
         algorithm, following the terminology of Lin and Costello :   d[u] is
         the 'mu'th discrepancy, where u='mu'+1 and 'mu' (the Greek letter!) is
         the step number ranging from -1 to 2*tt (see L&C),  l[u] is the degree
         of the elp at that step, and u_l[u] is the difference between the step
         number and the degree of the elp.
      */
      /* initialise table entries */
      d[0] = 0;      /* index form */
      d[1] = s[1];   /* index form */
      elp[0][0] = 0; /* index form */
      elp[1][0] = 1; /* polynomial form */
      for (i = 1; i < nn - kk; i++)
        {
          elp[0][i] = -1; /* index form */
          elp[1][i] = 0;  /* polynomial form */
        }
      l[0] = 0;
      l[1] = 0;
      u_lu[0] = -1;
      u_lu[1] = 0;
      u = 0;

      do
        {
          u++;
          if (d[u] == -1)
            {
              l[u + 1] = l[u];
              for (i = 0; i <= l[u]; i++)
                {
                  elp[u + 1][i] = elp[u][i];
                  elp[u][i] = index_of[elp[u][i]];
                }
            }
          else
            /* search for words with greatest u_lu[q] for which d[q]!=0 */
            {
              q = u - 1;
              while ((d[q] == -1) && (q > 0))
                q--;
              /* have found first non-zero d[q]  */
              if (q > 0)
                {
                  j = q;
                  do
                    {
                      j--;
                      if ((d[j] != -1) && (u_lu[q] < u_lu[j]))
                        q = j;
                    }
                  while (j > 0);
                };

              /* have now found q such that d[u]!=0 and u_lu[q] is maximum */
              /* store degree of new elp polynomial */
              if (l[u] > l[q] + u - q)
                l[u + 1] = l[u];
              else
                l[u + 1] = l[q] + u - q;

              /* form new elp(x) */
              for (i = 0; i < nn - kk; i++)
                elp[u + 1][i] = 0;
              for (i = 0; i <= l[q]; i++)
                if (elp[q][i] != -1)
                  elp[u + 1][i + u - q]
                      = alpha_to[(d[u] + nn - d[q] + elp[q][i]) % nn];
              for (i = 0; i <= l[u]; i++)
                {
                  elp[u + 1][i] ^= elp[u][i];
                  elp[u][i]
                      = index_of[elp[u][i]]; /*convert old elp value to index*/
                }
            }
          u_lu[u + 1] = u - l[u + 1];

          /* form (u+1)th discrepancy */
          if (u < nn - kk) /* no discrepancy computed on last iteration */
            {
              if (s[u + 1] != -1)
                d[u + 1] = alpha_to[s[u + 1]];
              else
                d[u + 1] = 0;
              for (i = 1; i <= l[u + 1]; i++)
                if ((s[u + 1 - i] != -1) && (elp[u + 1][i] != 0))
                  d[u + 1] ^= alpha_to[(s[u + 1 - i] + index_of[elp[u + 1][i]])
                                       % nn];
              d[u + 1] = index_of[d[u + 1]]; /* put d[u+1] into index form */
            }
        }
      while ((u < nn - kk) && (l[u + 1] <= tt));

      u++;
      if (l[u] <= tt) /* can correct error */
        {
          /* put elp into index form */
          for (i = 0; i <= l[u]; i++)
            elp[u][i] = index_of[elp[u][i]];

          /* find roots of the error location polynomial */
          for (i = 1; i <= l[u]; i++)
            reg[i] = elp[u][i];
          count = 0;
          for (i = 1; i <= nn; i++)
            {
              q = 1;
              for (j = 1; j <= l[u]; j++)
                if (reg[j] != -1)
                  {
                    reg[j] = (reg[j] + j) % nn;
                    q ^= alpha_to[reg[j]];
                  };
              if (!q) /* store root and error location number indices */
                {
                  root[count] = i;
                  loc[count] = nn - i;
                  count++;
                };
            };
          if (count == l[u]) /* no. roots = degree of elp hence <= tt errors */
            {
              /* form polynomial z(x) */
              for (i = 1; i <= l[u]; i++) /* Z[0] = 1 always - do not need */
                {
                  if ((s[i] != -1) && (elp[u][i] != -1))
                    z[i] = alpha_to[s[i]] ^ alpha_to[elp[u][i]];
                  else if ((s[i] != -1) && (elp[u][i] == -1))
                    z[i] = alpha_to[s[i]];
                  else if ((s[i] == -1) && (elp[u][i] != -1))
                    z[i] = alpha_to[elp[u][i]];
                  else
                    z[i] = 0;
                  for (j = 1; j < i; j++)
                    if ((s[j] != -1) && (elp[u][i - j] != -1))
                      z[i] ^= alpha_to[(elp[u][i - j] + s[j]) % nn];
                  z[i] = index_of[z[i]]; /* put into index form */
                };

              /* evaluate errors at locations given by error location numbers
               * loc[i] */
              for (i = 0; i < nn; i++)
                {
                  err[i] = 0;
                  if (recd[i] != -1) /* convert recd[] to polynomial form */
                    recd[i] = alpha_to[recd[i]];
                  else
                    recd[i] = 0;
                }
              for (i = 0; i < l[u];
                   i++) /* compute numerator of error term first */
                {
                  err[loc[i]] = 1; /* accounts for z[0] */
                  for (j = 1; j <= l[u]; j++)
                    if (z[j] != -1)
                      err[loc[i]] ^= alpha_to[(z[j] + j * root[i]) % nn];
                  if (err[loc[i]] != 0)
                    {
                      err[loc[i]] = index_of[err[loc[i]]];
                      q = 0; /* form denominator of error term */
                      for (j = 0; j < l[u]; j++)
                        if (j != i)
                          q += index_of[1 ^ alpha_to[(loc[j] + root[i]) % nn]];
                      q = q % nn;
                      err[loc[i]] = alpha_to[(err[loc[i]] - q + nn) % nn];
                      recd[loc[i]] ^= err[loc[i]]; /*recd[i] must be in
                                                      polynomial form */
                    }
                }
            }
          else /* no. roots != degree of elp => >tt errors and cannot solve */
            for (i = 0; i < nn; i++) /* could return error flag if desired */
              if (recd[i] != -1)     /* convert recd[] to polynomial form */
                recd[i] = alpha_to[recd[i]];
              else
                recd[i] = 0; /* just output received codeword as is */
        }
      else /* elp has degree has degree >tt hence cannot solve */
        for (i = 0; i < nn; i++) /* could return error flag if desired */
          if (recd[i] != -1)     /* convert recd[] to polynomial form */
            recd[i] = alpha_to[recd[i]];
          else
            recd[i] = 0; /* just output received codeword as is */
    }
  else /* no non-zero syndromes => no errors: output received codeword */
    for (i = 0; i < nn; i++)
      if (recd[i] != -1) /* convert recd[] to polynomial form */
        recd[i] = alpha_to[recd[i]];
      else
        recd[i] = 0;
}

int rs_decode_data_block (unsigned char *blk, int fix)
{
  if (!data_block_initialized)
    {
      generate_gf (pp0);
      gen_poly (kk0);
      data_block_initialized = 1;
      system_block_initialized = 0;
    }
  // TODO
  decode_rs (kk0);
  return 0;
}

int rs_decode_system_block (unsigned char *blk, int fix)
{
  if (!system_block_initialized || data_block_initialized)
    {
      generate_gf (pp1);
      gen_poly (kk1);
      system_block_initialized = 1;
      data_block_initialized = 0;
    }
  // TODO
  decode_rs (kk1);
  return 0;
}
void rs_encode_data_block (unsigned char *parity, unsigned char *src, int count)
{
  if (!data_block_initialized || system_block_initialized)
    {
      generate_gf (pp0);
      gen_poly (kk0);
      data_block_initialized = 1;
      system_block_initialized = 0;
    }
  // TODO
  encode_rs (kk0);
}

void rs_encode_system_block (unsigned char *parity, unsigned char *src, int count)
{
  if (!system_block_initialized || data_block_initialized)
    {
      generate_gf (pp1);
      gen_poly (kk1);
      system_block_initialized = 1;
      data_block_initialized = 0;
    }
  // TODO
  encode_rs (kk1);
}

#ifdef TEST
void test (int *pp, int kk)
{
  int i;
  const int tt = (nn - kk) / 2;

  /* generate the Galois Fields GF(2**mm) */
  generate_gf (pp);
  printf ("Look-up tables for GF(2**%d)\n", mm);
  printf ("  i   alpha_to[i]  index_of[i]\n");
  for (i = 0; i <= nn; i++)
    printf ("%3d      %3d          %3d\n", i, alpha_to[i], index_of[i]);
  printf ("\n\n");

  /* compute the generator polynomial for this RS code */
  gen_poly (kk);

  /* for known data, stick a few numbers into a zero codeword. Data is in
     polynomial form.
  */
  for (i = 0; i < kk; i++)
    data[i] = 0;

  /* for example, say we transmit the following message (nothing special!) */
  data[0] = 8;
  data[1] = 6;
  data[2] = 8;
  data[3] = 1;
  data[4] = 2;
  data[5] = 4;
  data[6] = 15;
  data[7] = 9;
  data[8] = 9;

  /* encode data[] to produce parity in bb[].  Data input and parity output
     is in polynomial form
  */
  encode_rs (kk);

  /* put the transmitted codeword, made up of data plus parity, in recd[] */
  for (i = 0; i < nn - kk; i++)
    recd[i] = bb[i];
  for (i = 0; i < kk; i++)
    recd[i + nn - kk] = data[i];

  /* if you want to test the program, corrupt some of the elements of recd[]
     here. This can also be done easily in a debugger. */
  /* Again, lets say that a middle element is changed */
  data[nn - nn / 2] = 3;

  for (i = 0; i < nn; i++)
    recd[i] = index_of[recd[i]]; /* put recd[i] into index form */

  /* decode recv[] */
  decode_rs (kk); /* recd[] is returned in polynomial form */

  /* print out the relevant stuff - initial and decoded {parity and message} */
  printf ("Results for Reed-Solomon code (n=%d, k=%d, t=%d)\n\n", nn, kk,
          tt);
  printf (
      "  i  data[i]   recd[i](decoded)   (data, recd in polynomial form)\n");
  for (i = 0; i < nn - kk; i++)
    printf ("%3d    %3d      %3d\n", i, bb[i], recd[i]);
  for (i = nn - kk; i < nn; i++)
    printf ("%3d    %3d      %3d\n", i, data[i - nn + kk], recd[i]);
}

int main (void) {
  test(pp0, kk0);
  test(pp1, kk1);
}
#endif
