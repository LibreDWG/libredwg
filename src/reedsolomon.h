/*
 * reedsolomon.h: Reed-Solomon (255,239) en-,decoding of r2007+ system pages.
 * See reedsolomon.c for the code parameters.
 */

/*
 * Decode a 255-byte block in place: 239 data bytes and 16 parity bytes.
 * Returns the number of corrected errors (at most 8) if fix is set, 0 if
 * there is no error, or -1 in case of unfixable errors, or of any error
 * without fix.
 */
int rs_decode_block (unsigned char *blk, int fix);

/*
 * Encode a block of count (at most 239) data bytes. Only the trailing 16
 * parity bytes are computed, in a buffer which the caller preallocates.
 */
void rs_encode_block (unsigned char *parity, unsigned char *src, int count);
