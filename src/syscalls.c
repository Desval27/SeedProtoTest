#include <sys/stat.h>
#include <errno.h>

/* Standard headers might define errno as a macro; undefine if necessary */
#undef errno
extern int errno;

/**
 * @brief Stub for closing a file descriptor.
 * Overrides the weak symbol in libnosys to prevent linker warnings.
 * 
 * @param file The file descriptor to close.
 * @return Returns -1 to indicate failure, setting errno to EBADF.
 */
int _close(int file) {
    /* (void)file; is used to prevent "unused parameter" compiler warnings */
    (void)file;

    /* EBADF (Bad File Descriptor) is the standard error for a failed close */
    errno = EBADF;
    return -1;
}

/* _fstat: Status of an open file. Often used by libraries to check if a file is a terminal. */
int _fstat(int file, struct stat *st) {
    (void)file;
    st->st_mode = S_IFCHR; // Mark as character device (like a terminal)
    return 0;
}

/* _getpid: Return current process ID. Minimal implementation returns 1. */
int _getpid(void) {
    return 1;
}

/* _isatty: Query whether output stream is a terminal. */
int _isatty(int file) {
    (void)file;
    return 1;
}

/* _kill: Send a signal to a process. Bare-metal doesn't support signals. */
int _kill(int pid, int sig) {
    (void)pid; (void)sig;
    errno = EINVAL; // Invalid argument
    return -1;
}

/* _lseek: Set position in a file. Returns 0 as these devices usually aren't seekable. */
int _lseek(int file, int ptr, int dir) {
    (void)file; (void)ptr; (void)dir;
    return 0;
}

/* _read: Read from a file/device. Returns 0 (EOF) unless mapped to hardware input. */
int _read(int file, char *ptr, int len) {
    (void)file; (void)ptr; (void)len;
    return 0;
}

/* _write: Write to a file/device. Redirect ptr to your UART/Log function here. */
int _write(int file, char *ptr, int len) {
    (void)file;
    /* Example: for(int i=0; i<len; i++) { UART_Send(ptr[i]); } */
    return len; // Pretend all bytes were written
}
