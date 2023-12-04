int rs_decode_data_block (unsigned char *blk, int fix);
int rs_decode_system_block (unsigned char *blk, int fix);
void rs_encode_data_block (unsigned char *parity, unsigned char *src, int count);
void rs_encode_system_block (unsigned char *parity, unsigned char *src, int count);
