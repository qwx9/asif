/* Daniel J. Bernstein hashing algorithm, retrieved from:
 * http://www.partow.net/programming/hashfunctions/index.html */

unsigned int djbhash(const char* str, unsigned int length)
{
	unsigned int hash	= 5381;
	unsigned int i		= 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash = ((hash << 5) + hash) + (*str);
	}

	return hash;
}
