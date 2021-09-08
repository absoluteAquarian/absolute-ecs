#include "core.h"
#include "utility.h"

/* Windows builds just redirect to <conio.h> for some of the utilities */
#ifdef OS_UNIX
#include <termios.h>
#include <sys/select.h>
#elif defined(OS_WIN)
#include <conio.h>
#else
#error "Unknown OS specified.  Please build this project via the makefile present."
#endif

size_t g_totalalloc;

DLL_SYMBOL void *malloc_debug(size_t size){
	void *ptr = malloc(size);
	g_totalalloc += size;
	return ptr;
}

DLL_SYMBOL void free_debug(void *ptr, size_t size){
	free(ptr);
	g_totalalloc -= size;
}

DLL_SYMBOL int AECS_kbhit(){
#ifdef OS_UNIX
	struct timeval tv;
	fd_set fds;
	
	/* Set the time interval to zero */
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
	/* Initialize the file descriptor set, "fds", to zero for all descriptors */
	FD_ZERO(&fds);
	
	/* Set the STDIN_FILENO descriptor to 1 */
	FD_SET(STDIN_FILENO, &fds);
	
	/* select(nfds, readfds, writefds, exceptfds, timeout)
		see: https://man7.org/linux/man-pages/man2/select.2.html
	
		nfds
			Highest-numbered descriptor + 1
		readfds
			Descriptor set for reading
		writefds
			Descriptor set for writing
		exceptfds
			Descriptor set for "exceptional conditions"
		timeout
			(timeval) instance for blocking until either the descriptor becomes ready,
			the call is interrupted by a signal handler or the (timeout) expires
			- if all members are zero, select() returns immediately
			- if (timeout) is NULL, select() will block until a descriptor is ready
	   */
	select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
	
	/* STDIN_FILENO is usually 0 */
	return FD_ISSET(STDIN_FILENO, &fds);
#else
	return _kbhit();
#endif
}

#ifdef OS_UNIX
/* Helper methods for AECS_getch() on Unix systems */
/* From: https://stackoverflow.com/a/7469410/8420233 */
static struct termios old, current;

/* Initialize new terminal I/O settings */
void UNIX_initTermios(int echo){
	/* Grab old terminal I/O settings */
	tcgetattr(0, &old);
	/* Make new settings the same as old settings */
	current = old;
	/* Disable buffered I/O */
	current.c_lflag &= ~ICANON;
	
	if(echo)
		current.c_lflag |= ECHO; /* Set to "echo" mode */
	else
		current.c_lflag &= ~ECHO; /* Set to "no echo" mode */
	
	/* Use these new terminal I/O settings now */
	tcsetattr(0, TCSANOW, &current);
}

/* Restore old terminal I/O settings */
void UNIX_resetTermios(void){
	tcsetattr(0, TCSANOW, &old);
}

char getch_(int echo){
	char ch;
	UNIX_initTermios(echo);

	ch = getchar();

	UNIX_resetTermios();

	return ch;
}

char _getch(){
	return getch_(0);
}
#endif

DLL_SYMBOL char AECS_getch(){
	return _getch();
}