/*
 * reedsolomon.c: Reed-Solomon (255,239) en-,decoding
 * written by Alex Papazoglou
 */

/*
 * Decode a 255-byte block in place. Returns number of corrected errors,
 * if fix is set, or -1 in case of unfixed errors.
 */
int rs_decode_block (unsigned char *blk, int fix);

/*
 * Encode a block. Only the trailing 16 parity bytes are computed in
 * a buffer which caller preallocates.
 */
void rs_encode_block (unsigned char *parity, unsigned char *src, int count);
