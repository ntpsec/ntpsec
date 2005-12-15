/*
 * Define malloc and friends.
 */

static char *
strdup( const char *s )
{
    char *cp;

    if (s == NULL)
	return NULL;

    cp = (char *) malloc((unsigned) (strlen(s)+1));

    if (cp != NULL)
	(void) strcpy(cp, s);

    return cp;
}
