DumbRLE
=======

A super stupid RLE implementation, useful for compressing large FPGA bitstreams
down into small size.


Format
------

The format involves a 16-bit header, followed by data.  The header is one
bit of "type" followed by 15 bits of "size".

If "type" is 0, then the "size" indicates the number of zeroes.  The next
header follows immediately afterwards.

If "type" is 1, then the "size" indicates the number of bytes of data
that follow.  After the data, the next header follows.

Note that if there is a run of more than 32768 bytes of data or zeroes, then
it is divided among multiple packets.
