#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int
gethexchar(char *buffer, int offset)
{
    if ((buffer[offset] >= '0') && (buffer[offset] <= '9')) {
	return (int) (buffer[offset] - '0');
    } else if ((buffer[offset] >= 'a') && (buffer[offset] <= 'f')) {
	return (int) (buffer[offset] - 'a' + 10);
    } else if ((buffer[offset] >= 'A') && (buffer[offset] <= 'F')) {
	return (int) (buffer[offset] - 'A' + 10);
    }
    return -1;
}

int
gethexbyte(char *buffer, int offset)
{
    int             hi, lo;

    if (((hi = gethexchar(buffer, offset)) == -1) || ((lo = gethexchar(buffer, offset + 1)) == -1)) {
	return -1;
    }
    return 16 * hi + lo;
}

int
gethexword(char *buffer, int offset)
{
    int             hi, lo;
    if (((hi = gethexbyte(buffer, offset)) == -1) || ((lo = gethexbyte(buffer, offset + 2)) == -1)) {
	return -1;
    }
    return 256 * hi + lo;
}

int
main(int argc, char **argv)
{

    unsigned int    lineno;
    unsigned int    bufpos;
    char            linebuf[1024];
    unsigned int    bytecount;
    unsigned int    byteaddr;
    unsigned int    linetype;
    unsigned int    databyte[256];
    unsigned int    checkbyte;
    unsigned int    i;
    unsigned char   sum;

    lineno = 0;
    while (fgets(linebuf, sizeof(linebuf), stdin)) {
	lineno++;

	if (strlen(linebuf) == 0) {
	    continue;
	}
	if (linebuf[strlen(linebuf) - 1] == '\n') {
	    linebuf[strlen(linebuf) - 1] = '\0';
	}
	bufpos = 0;
	if (linebuf[bufpos] != ':') {
	    fprintf(stderr, "%s: Bad input, line #%d doesn't begin with a ':'\n",
		    argv[0], lineno);
	    exit(2);
	}
	bufpos++;

	if ((strlen(linebuf) - bufpos) < 2) {
	    fprintf(stderr, "%s: Bad input, line #%d doesn't have enough digits for byte count\n",
		    argv[0], lineno);
	    exit(2);
	}
	if ((bytecount = gethexbyte(linebuf, bufpos)) == -1) {
	    fprintf(stderr, "%s: Bad input, line #%d unable to determine length byte\n",
		    argv[0], lineno);
	    exit(2);
	}
	bufpos += 2;

	if ((strlen(linebuf) - bufpos) < 4) {
	    fprintf(stderr, "%s: Bad input, line #%d doesn't have enough digits for addr\n",
		    argv[0], lineno);
	    exit(2);
	}
	if ((byteaddr = gethexword(linebuf, bufpos)) == -1) {
	    fprintf(stderr, "%s: Bad input, line #%d unable to determine address\n",
		    argv[0], lineno);
	    exit(2);
	}
	bufpos += 4;

	if ((strlen(linebuf) - bufpos) < 2) {
	    fprintf(stderr, "%s: Bad input, line #%d doesn't have enough digits for line type byte\n",
		    argv[0], lineno);
	    exit(2);
	}
	if ((linetype = gethexbyte(linebuf, bufpos)) == -1) {
	    fprintf(stderr, "%s: Bad input, line #%d unable to determine length byte\n",
		    argv[0], lineno);
	    exit(2);
	}
	bufpos += 2;

	for (i = 0; i < bytecount; i++) {
	    if ((strlen(linebuf) - bufpos) < 2) {
		fprintf(stderr, "%s: Bad input, line #%d doesn't have enough digits for databyte\n",
			argv[0], lineno);
		exit(2);
	    }
	    if ((databyte[i] = gethexbyte(linebuf, bufpos)) == -1) {
		fprintf(stderr, "%s: Bad input, line #%d unable to determine databyte\n",
			argv[0], lineno);
		exit(2);
	    }
	    bufpos += 2;
	}

	if ((strlen(linebuf) - bufpos) < 2) {
	    fprintf(stderr, "%s: Bad input, line #%d doesn't have enough digits for check byte\n",
		    argv[0], lineno);
	    exit(2);
	}
	if ((checkbyte = gethexbyte(linebuf, bufpos)) == -1) {
	    fprintf(stderr, "%s: Bad input, line #%d unable to determine check byte\n",
		    argv[0], lineno);
	    exit(2);
	}
	bufpos += 2;

	if (strlen(linebuf) > bufpos) {
	    fprintf(stderr, "%s: Bad input, line #%d has extra stuff after check byte\n",
		    argv[0], lineno);
	    exit(2);
	}
	sum = bytecount + (byteaddr / 256) + (byteaddr % 256) + linetype;
	for (i = 0; i < bytecount; i++) {
	    sum += databyte[i];
	}
	sum = 256 - sum;

	if (checkbyte != sum) {
	    fprintf(stderr, "%s: Bad input, line #%d checksum is 0x%02x but checkbyte is 0x%02x\n",
		    argv[0], lineno, sum, checkbyte);
	    exit(2);
	}
/*
	printf("%4d - bytecount=0x%02x,byteaddr=0x%04x,linetype=0x%02x,bytes=",
	       lineno, bytecount, byteaddr, linetype);
	for (i = 0; i < bytecount; i++) {
	    printf("%02x ", databyte[i]);
	}
	printf("checkbyte=0x%02x, sum=0x%02x\n", checkbyte, sum);
*/

	if (bytecount > 0) {
	    printf("%c%c%c", byteaddr / 256, byteaddr % 256, bytecount);
	    for (i = 0; i < bytecount; i++) {
		printf("%c", databyte[i]);
	    }
	}
    }
}
