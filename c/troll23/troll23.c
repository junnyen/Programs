#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <time.h>

//Catch rand() calls etc...
time_t time(time_t *tloc) { srand(23); return (time_t)0; }
int rand(void) {	srand(23); return 23; }
long int random(void) {	srand(23); return 23; }
int rand_r(unsigned int *seedp) { srand(23); return 23; }
int random_r(struct random_data *buf, int32_t *result) { srand(23); return 23; }

//Catch open() calls (while redirecting filename):
static const char *redirect_name(const char *name)
{
	if(
		(strcmp(name,"/dev/random") == 0) ||
		(strcmp(name,"/dev/urandom") == 0)
	) {
		printf("REDIRECT HIT: %s\n", name);
		return "/dev/zero";
	}
	return name;
}

int open(const char *filename, int flags, ...)
{
	//srand(23);
	static int (*open_orig)(const char *, int, mode_t);
	int ret;
	va_list ap;
	mode_t mode;

	if (!open_orig) {
		open_orig = dlsym(RTLD_NEXT, "open");
	}

	va_start(ap, flags);
	mode = va_arg(ap, mode_t);
	va_end(ap);

	ret = open_orig(redirect_name(filename), flags, mode);
	//ret = syscall(SYS_open,redirect_name(filename),flags,mode);

	printf("open(\"%s\", 0x%x, %o) -> %d\n", filename, flags, mode, ret);

	return ret;
}

int open64(const char *filename, int flags, ...)
{
	//srand(23);
	static int (*open64_orig)(const char *, int, mode_t);
	int ret;
	va_list ap;
	mode_t mode;

	if (!open64_orig) {
		open64_orig = dlsym(RTLD_NEXT, "open64");
	}

	va_start(ap, flags);
	mode = va_arg(ap, mode_t);
	va_end(ap);

	ret = open64_orig(redirect_name(filename), flags, mode);
	//ret = syscall(SYS_open64,redirect_name(filename),flags,mode);

	printf("open64(\"%s\", 0x%x, %o) -> %d\n", filename, flags, mode, ret);

	return ret;
}
