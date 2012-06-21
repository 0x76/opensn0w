/*-
 * Copyright (c) 2005 Pascal Gloor <pascal.gloor@spale.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "sn0w.h"

void *our_memmem(l, l_len, s, s_len)
const void *l;
size_t l_len;
const void *s;
size_t s_len;
{
	register char *cur, *last;
	const char *cl = (const char *)l;
	const char *cs = (const char *)s;

	/* we need something to compare */
	if (l_len == 0 || s_len == 0)
		return NULL;

	/* "s" must be smaller or equal to "l" */
	if (l_len < s_len)
		return NULL;

	/* special case where s_len == 1 */
	if (s_len == 1)
		return memchr(l, (int)*cs, l_len);

	/* the last position where its possible to find "s" in "l" */
	last = (char *)cl + l_len - s_len;

	for (cur = (char *)cl; cur <= last; cur++)
		if (cur[0] == cs[0] && memcmp(cur, cs, s_len) == 0)
			return cur;

	return NULL;
}

/* from http://sws.dett.de/mini/hexdump-c/ */
void hex_dump(void *data, int size)
{
	/* dumps size bytes of *data to stdout. Looks like:
	 * [0000] 75 6E 6B 6E 6F 77 6E 20
	 *                  30 FF 00 00 00 00 39 00 unknown 0.....9.
	 * (in a single line of course)
	 */

	unsigned char *p = data;
	unsigned char c;
	int n;
	char bytestr[4] = { 0 };
	char addrstr[10] = { 0 };
	char hexstr[16 * 3 + 5] = { 0 };
	char charstr[16 * 1 + 5] = { 0 };
	for (n = 1; n <= size; n++) {
		if (n % 16 == 1) {
			/* store address for this line */
			snprintf(addrstr, sizeof(addrstr), "%08x",
				 ((unsigned int)p - (unsigned int)data));
		}

		c = *p;
		if (isalnum(c) == 0) {
			c = '.';
		}

		/* store hex str (for left side) */
		snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
		strncat(hexstr, bytestr, sizeof(hexstr) - strlen(hexstr) - 1);

		/* store char str (for right side) */
		snprintf(bytestr, sizeof(bytestr), "%c", c);
		strncat(charstr, bytestr,
			sizeof(charstr) - strlen(charstr) - 1);

		if (n % 16 == 0) {
			/* line completed */
			printf("%s   %-50.50s  %s\n", addrstr, hexstr, charstr);
			hexstr[0] = 0;
			charstr[0] = 0;
		} else if (n % 8 == 0) {
			/* half line: add whitespaces */
			//  strncat(hexstr, "  ", sizeof(hexstr)-strlen(hexstr)-1);
			//  strncat(charstr, " ", sizeof(charstr)-strlen(charstr)-1);
		}
		p++;		/* next byte */
	}

	if (strlen(hexstr) > 0) {
		/* print rest of buffer if not empty */
		printf("%s   %-50.50s  %s\n", addrstr, hexstr, charstr);
	}
}

int endian() {
	int i = 1;
	char *p = (char *)&i;

	if (p[0] == 1)
        	return ENDIAN_LITTLE;
	else
        	return ENDIAN_BIG;
}

char* endian_to_string(int endian) {
	switch(endian) {
		case ENDIAN_LITTLE:
			return "Little Endian";
		case ENDIAN_BIG:
			return "Big Endian";
		default:
			return "Bad Endian";
	}
	return NULL;
}

#define BUFSIZE		2048

void debug_printf(int dbglevel, char *fmt, ...) {
	char buf[BUFSIZE];		/* i know,  too large */
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, BUFSIZE, fmt, ap);
	va_end(ap);

	if(dbglevel <= opensn0w_debug_level)
		printf("%s", buf);

	return;
}

void dos_cursor() {
	static int position = 0;
	char cursor[4] = {'/', '-', '\\', '|'};
			// / - \ |
	printf("%c\b", cursor[position]);
	fflush(stdout);
	position = (position + 1) % 4;
}


#ifndef HAVE_STRCASESTR
char *strcasestr (char *haystack, char *needle)
{
	char *p, *startn = 0, *np = 0;

	for (p = haystack; *p; p++) {
		if (np) {
			if (toupper(*p) == toupper(*np)) {
				if (!*++np)
					return startn;
			} else
				np = 0;
		} else if (toupper(*p) == toupper(*needle)) {
			np = needle + 1;
			startn = p;
		}
	}

	return 0;
}

#endif

#ifndef HAVE_STRNDUP
char *strndup (const char *s, size_t n) {
	char *result;
	size_t len = strlen (s);

	if (n < len)
		len = n;

	result = (char *) malloc (len + 1);
	if (!result)
		return 0;

	result[len] = '\0';
	return (char *) memcpy (result, s, len);
}

#endif

#ifndef HAVE_WCSCASECMP

int
wcscasecmp(const wchar_t *s1, const wchar_t *s2)
{
	wchar_t l1, l2;

	while ((l1 = towlower(*s1++)) == (l2 = towlower(*s2++))) {
		if (l1 == 0)
			return (0);
	}

	return ((wchar_t)l1 - (wchar_t)l2);
}

#endif

#ifndef HAVE_STRCASECMP

int strcasecmp(const char *s1, const char *s2)
{
  while (*s1 != '\0' && tolower(*s1) == tolower(*s2))
    {
      s1++;
      s2++;
    }

  return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
}

#endif

#ifdef MSVC_VER

char *optarg;
int optind = 1;
int opterr, optopt;
static const char *__optptr;

int getopt(int argc, char * const *argv, const char *optstring)
{
  const char *carg = argv[optind];
  const char *osptr;
  int opt;

  /* We don't actually need argc */
  (void)argc;

  /* First, eliminate all non-option cases */

  if ( !carg || carg[0] != '-' || !carg[1] ) {
    return -1;
  }

  if ( carg[1] == '-' && !carg[2] ) {
    optind++;
    return -1;
  }

  if ( (uintptr_t)(__optptr-carg) > (uintptr_t)strlen(carg) )
    __optptr = carg+1;	/* Someone frobbed optind, change to new opt. */

  opt = *__optptr++;

  if ( opt != ':' && (osptr = strchr(optstring, opt)) ) {
    if ( osptr[1] == ':' ) {
      if ( *__optptr ) {
	/* Argument-taking option with attached argument */
	optarg = (char *)__optptr;
	optind++;
      } else {
	/* Argument-taking option with non-attached argument */
	if ( argv[optind+1] ) {
	  optarg = (char *)argv[optind+1];
	  optind += 2;
	} else {
	  /* Missing argument */
	  return (optstring[0] == ':') ? ':' : '?';
	}
      }
      return opt;
    } else {
      /* Non-argument-taking option */
      /* __optptr will remember the exact position to resume at */
      if ( ! *__optptr )
	optind++;
      return opt;
    }
  } else {
    /* Unknown option */
    optopt = opt;
    if ( ! *__optptr )
      optind++;
    return '?';
  }
}

#endif