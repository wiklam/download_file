//Wiktor Pilarczyk 308533

void get_entrance(int argc, char **argv, char **ip, int *port, char **file, int *size);
int min(int a, int b);
int Socket(int domain, int type, int protocol);
int Open(const char *path, int flags);
int Close(int filefd);
int Write(int fd, const char *buf, size_t count);