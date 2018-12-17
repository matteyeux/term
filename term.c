#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define INFO(...) printf("[INFO] " __VA_ARGS__)
#define ERROR(...) printf("[ERROR] " __VA_ARGS__)

int fd = -1;

void set_speed (struct termios *config, speed_t speed)
{
	cfsetispeed(config, speed);
	cfsetospeed(config, speed);
}

void INThandler(int sig)
{
	char yn;
	signal(sig, SIG_IGN);
	printf("quit term ? [Y/n]\n");
	yn = getchar();

	if (yn == 'y' || yn == 'Y')
	{
		close(fd);
		exit(EXIT_SUCCESS);
	}

	exit(0);
}

int term(const char *port)
{
	struct termios conf;
	struct termios save;

	char buffer[1024];
	int reads;


	signal(SIGINT, INThandler);

	/*No block opening to switch to non-local mode*/
	fd = open(port, O_RDWR | O_NONBLOCK);
	if (fd < 0) {
		ERROR("%s : %s\n",port, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (tcgetattr(fd, &conf) != 0) {
		ERROR("%s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	conf.c_cflag   &= ~ CLOCAL;
	tcsetattr(fd, TCSANOW, &conf);

	// We manipulate fd to block it
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) &  ~O_NONBLOCK);

	INFO("%s : opened\n", port);
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

	INFO("%s : configured\n", port);

	INFO("Data reception started\n");

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

	INFO("[INFO] Reception ended \n");

	fd = open(port, O_RDWR | O_NONBLOCK);
	save.c_cflag |= CLOCAL;
	tcsetattr(fd, TCSANOW, &save);
	close(fd);
	return EXIT_SUCCESS;
}
