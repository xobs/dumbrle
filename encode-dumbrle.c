#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

static int done;

static int get_next_char(int infd, char *c) {
    int ret;

    ret = read(infd, c, 1);
    if (ret != 1) {
        if (ret == -1)
            perror("Unable to read");
        else
            fprintf(stderr, "Done.\n");
        done = 1;
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    int infd = open("hdmi_phy_test1_wrapper.bin", O_RDONLY);
    int outfd = open("hdmi_phy_test1_wrapper.bin.rle", O_WRONLY | O_CREAT | O_TRUNC, 0777);
    char buffer[65536];
    int buffer_size;
    char c;

    if (get_next_char(infd, &c))
        done = 1;

    while (!done) {

        buffer_size = 1;
        if (c == 0) {
            while( (get_next_char(infd, &c) == 0)
                && (c == 0)
                && (buffer_size < 32767))
                buffer_size++;
            uint16_t outpkt = 0x8000 | buffer_size;
            write(outfd, &outpkt, sizeof(outpkt));
        }
        else {
            buffer[0] = c;
            while( (get_next_char(infd, &c) == 0)
                && (c != 0)
                && (buffer_size < 32767))
                buffer[buffer_size++] = c;
            uint16_t outpkt = buffer_size;
            write(outfd, &outpkt, sizeof(outpkt));
            write(outfd, buffer, buffer_size);
        }
    }

    close(infd);
    close(outfd);
    return 0;
}
