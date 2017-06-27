#ifndef STRING_H_
#define STRING_H_

size_t strlen(string str)
{
	size_t size = 0;
	while(str[size] != '\0')
		size++;
	return size;
}

#endif