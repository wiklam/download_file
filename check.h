//Wiktor Pilarczyk 308533

void get_entrance(int argc, char **argv, char **ip, int *port, char **file, int *size);
int min(int a, int b);
int Socket(int domain, int type, int protocol);
int Open(const char *path, int flags, int mode);
int Close(int filefd);
int Write(int fd, const char *buf, size_t count);
ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags, 
                struct sockaddr *src_addr, socklen_t *addrlen);