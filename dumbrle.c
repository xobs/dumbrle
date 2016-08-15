#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    int infd = open("hdmi_phy_test1_wrapper.bin", O_RDONLY);
    int outfd = open("hdmi_phy_test1_wrapper.bin.rle", O_WRONLY | O_CREAT | O_TRUNC, 0777);

    return 0;
}
