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

static int get_next_type(int infd, uint16_t *c) {
    int ret;

    ret = read(infd, c, 2);
    if (ret != 2) {
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
    int infd = open("hdmi_phy_test1_wrapper.bin.rle", O_RDONLY);
    int outfd = open("hdmi_phy_test1_wrapper.bin.rle.dec", O_WRONLY | O_CREAT | O_TRUNC, 0777);
    char c;
    uint16_t type;

    if (get_next_type(infd, &type))
        done = 1;

    while (!done) {

        if (type & 0x8000) {
            type &= ~0x8000;
            int i;
            c = 0;
            for (i = 0; i < type; i++)
                write(outfd, &c, 1);
        }
        else {
            int i;
            for (i = 0; (i < type) && !done; i++) {
                get_next_char(infd, &c);
                write(outfd, &c, 1);
            }
        }
        get_next_type(infd, &type);
    }

    close(infd);
    close(outfd);
    return 0;
}
