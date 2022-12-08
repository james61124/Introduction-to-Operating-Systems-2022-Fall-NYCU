#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int read_line(int fd, char *word) {
	*word = '\0';
	char buf[2] = "";
	int not_eof;

	while ( (not_eof = read(fd, buf, 1)) && *buf != '\n' ) {
		strcat(word, buf);
	}

	return not_eof;
}

int main() {
	
	char *ptr[1000] = { NULL };
	int fd = open("test.txt", O_RDONLY, 0777);
	char line[128];
	int id, size, not_eof;
	
	do {
		not_eof = read_line(fd, line);

		if ( sscanf(line, "A\t%d\t%d", &id, &size) ) {
			//write(1,"haha",4);
			ptr[id] = malloc(size);
			for (int i = 0; i < size; ++i) {
				//write(1,"haha",4);
				ptr[id][i] = rand();
				// char pstr[sizeof(ptr[id]) * 2 + 2 + 1]; // Each byte of the pointer is two hexadecimal character, plus a potential 0x, plus terminator
				// snprintf(pstr, sizeof(ptr[id]), "%p", (void *) ptr[id]);
				// write(filedescriptor, pstr, strlen(pstr));
				// char int_str[20];
				// sprintf(int_str, "%d\n", ptr[id][i]);
				// write(1, int_str ,32);
			}
		}
		else if ( sscanf(line, "D\t%d", &id) ) {
			//write(1,"haha",4);
			free(ptr[id]);
		}

	} while (not_eof);
	//write(1,"first\n",6);
	malloc(0);
	return 0;
}