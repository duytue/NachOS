#include "syscall.h"

int
main()
{
    SpaceId newProc;
    OpenFileID input = ConsoleInput;
    OpenFileID output = ConsoleOutput;
    char prompt[2], ch, buffer[60];
    int i;

    prompt[0] = '-';
    prompt[1] = '-';

    while( 1 )
    {
	WriteFile(prompt, 2, output);

	i = 0;
	
	do {
	
	    ReadFile(&buffer[i], 1, input); 

	} while( buffer[i++] != '\n' );

	buffer[--i] = '\0';

	if( i > 0 ) {
		newProc = Exec(buffer);
		Join(newProc);
	}
    }
}

