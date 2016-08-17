#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

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

static int print_usage(const char *progname) {
    fprintf(stderr, "Usage: %s infile [outfile]\n", progname);
    return 0;
}

int main(int argc, char **argv) {
    char c;
    uint16_t type;

    if (argc < 1) {
        print_usage(argv[0]);
        return 1;
    }

    const char *infilename = argv[1];
    int infd = open(infilename, O_RDONLY);
    if (infd == -1) {
        perror("Unable to open source file");
        return 1;
    }

    char outfilename[1024];
    if (argc < 2)
        snprintf(outfilename, sizeof(outfilename) - 1, "%s.dec", infilename);
    else
        strncpy(outfilename, argv[2], sizeof(outfilename) - 1);

    int outfd = open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (outfd == -1) {
        perror("Unable to open output file");
        return 1;
    }

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
