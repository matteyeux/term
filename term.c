#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

void usage(int argc, char *argv []){
    printf("usage : %s [PORT]\n", argv[0]);
}

void set_speed (struct termios * config, speed_t speed)
{
    cfsetispeed(config, speed);
    cfsetospeed(config, speed);
}


int main(int argc, char *argv [])
{
    //const char *port = "/dev/ttyUSB0";
    struct termios conf;
    struct termios save;

    char buffer[1024];
    int fd = -1;
    int reads;
    
    if (argc != 2) {
        usage(argc, argv);
        return 0;
    }

    if (strcmp(argv[1], "-h")==0 || strcmp(argv[1], "--help")==0)
    {
        usage(argc, argv);
    }
    const char *port = argv[1];
    
    /*No block opening to switch to non-local mode*/
    fd = open(port, O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        printf("[ERROR] %s : %s\n",port, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (tcgetattr(fd, &conf) != 0) {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    conf.c_cflag   &= ~ CLOCAL;
    tcsetattr(fd, TCSANOW, &conf);

    // We manipulate fd to block it
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) &  ~O_NONBLOCK);
    
    printf("[INFO] %s : opened\n", port);
    tcgetattr(fd, &conf);
    memcpy(&save, &conf, sizeof(struct termios));
    cfmakeraw(&conf);
    set_speed(&conf, B115200);
    
    conf.c_cflag &= ~PARENB;
    conf.c_cflag &= ~CSIZE;
    conf.c_cflag |= CS8;
    conf.c_cflag &= ~CSTOPB;
    conf.c_cflag |= CRTSCTS;
    conf.c_cc[VMIN] = 1;
    conf.c_cc[VTIME] = 0;
    conf.c_cflag &= ~CLOCAL;
    conf.c_cflag |= HUPCL;

    if (tcsetattr(fd, TCSANOW, &conf) < 0) {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("[INFO] %s : configured\n", port);

    printf("[INFO] Data reception started\n");
    while (1) {
        reads = read(fd, buffer, 1024);
        if (reads < 0)  {
            printf("%s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (reads == 0)
            break;
        write(STDOUT_FILENO, buffer, reads);
    }
    printf("[INFO] Reception ended \n");
    close(fd); // Not sure if I ctrl^c  
    // back to the past
    fd = open(port, O_RDWR | O_NONBLOCK);
    save.c_cflag |= CLOCAL;
    tcsetattr(fd, TCSANOW, &save);
    close(fd);
    return EXIT_SUCCESS;
}

