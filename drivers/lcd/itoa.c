#include <stdio.h>
#include <stdlib.h>

/*Maximum size of string returned*/
#define MAX 11

/*Function Prototype*/
char *num_to_str(int i);

/*Converts and integer into a string*/
char *num_to_str(int i)
{
	char *buff = malloc(sizeof(char)*MAX);
	char tmp;
	unsigned char digit = 0;
	unsigned int j = 0, y = 0;
	
	//Generate string
	while(i >= 1)
	{
		digit = i % 10;
		buff[j] = '0' + digit;
		i = i / 10;
		j++;
	}
	
	//Reverse string
	for(unsigned int k = j-1; k > y; k--)
	{
		tmp = buff[k];
		buff[k] = buff[y];
		buff[y] = tmp;
		y++;
	}
	
	//Set null terminating character
	buff[j] = '\0';
	
	return buff;
}