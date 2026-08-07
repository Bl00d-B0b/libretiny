/* Copyright (c) Bl00d-B0b 2026-08-06. */

// Newlib syscall stubs the SDK does not provide on AmebaD.

#include <sys/types.h>

// Normally from crtbegin, which -nostartfiles drops; referenced by
// __cxa_atexit() for C++ static objects. Nothing runs destructors on a
// firmware image, so the address only has to be unique.
void *__dso_handle = 0;

void _exit(int status) {
	(void)status;
	while (1) {}
}

int _kill(pid_t pid, int sig) {
	(void)pid;
	(void)sig;
	return -1;
}

pid_t _getpid(void) {
	return 1;
}
