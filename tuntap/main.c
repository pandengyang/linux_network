#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/if_tun.h>

static int tun_open_common(char *dev, int istun);

int main(int argc, char **argv)
{
	int fd_tun;
	char name_dev[16] = "tun0";

	int count_read;
	char buf[2048];
	char tmp[4];

	unsigned short port;
	unsigned char *pport;

	pport = (unsigned char *) &port;
	if ((fd_tun = tun_open_common(name_dev, 1)) < 0) {
		return fd_tun;
	}
	printf("%s\n\n", name_dev);

	while (1) {
		if ((count_read = read(fd_tun, buf, sizeof buf)) < 0) {
			printf("count_read < 0");
			break;
		}

		printf("READ: %d bytes\n", count_read);

		/* 发送方 IP */
		for (int i = 12; i < 16; i++) {
			if (15 == i) {
				printf("%d:", (unsigned char) buf[i]);
			} else {
				printf("%d.", (unsigned char) buf[i]);
			}
		}

		/* 发送方端口号 */
		pport[0] = buf[21];
		pport[1] = buf[20];
		printf("%d", port);

		printf(" -> ");

		/* 接收方 IP */
		for (int i = 16; i < 20; i++) {
			if (19 == i) {
				printf("%d:", (unsigned char) buf[i]);
			} else {
				printf("%d.", (unsigned char) buf[i]);
			}
		}

		/* 接收方端口号 */
		pport[0] = buf[23];
		pport[1] = buf[22];
		printf("%d", port);

		/* 内容 */
		buf[count_read] = '\x00';
		printf(" %s\n", buf + 28);

		/* 对调 IP */
		memcpy(tmp, buf + 12, 4);
		memcpy(buf + 12, buf + 16, 4);
		memcpy(buf + 16, tmp, 4);

		/* 对调端口号 */
		memcpy(tmp, buf + 20, 2);
		memcpy(buf + 20, buf + 22, 2);
		memcpy(buf + 22, tmp, 2);

		write(fd_tun, buf, count_read);
	}
}

static int tun_open_common(char *dev, int istun)
{
	struct ifreq ifr;
	int fd;

	if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
		perror("open /dev/net/tun error");
		return fd;
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = (istun ? IFF_TUN : IFF_TAP) | IFF_NO_PI;

	if (*dev) {
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	}

	if (ioctl(fd, TUNSETIFF, (void *) &ifr) < 0) {
		perror("ioctl TUNSETIFF error");
		goto failed;
	}

	strcpy(dev, ifr.ifr_name);
	return fd;

      failed:
	close(fd);
	return -1;
}
