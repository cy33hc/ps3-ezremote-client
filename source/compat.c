
// compatibility functions missing in PSL1GHT sdk

int getuid(void)
{
    return 1000;
}

int geteuid(void)
{
    return 1000;
}

int getgid(void)
{
    return 1000;
}

int getegid(void)
{
    return 1000;
}

/*
int lstat(const char *path, struct stat *buf)
{
    return stat(path, buf);
}

int symlink(const char *path1, const char *path2)
{
    return -1;
}

ssize_t readlink(const char *restrict pathname, char *restrict buf, size_t bufsiz)
{
    return -1;
}

int clock_gettime(clockid_t clockid, struct timespec *tp)
{
    return sysGetCurrentTime(&tp->tv_sec, &tp->tv_nsec);
}
*/