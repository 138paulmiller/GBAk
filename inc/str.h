//Mini version of itoa and strcat

#ifndef STR_H
#define STR_H

//Atoi fo
char* itoa(unsigned int num, char* str, int base=10)
{
	//assume base ten
	int i =0, r = 0;
	while(num > 0)
	{
		r =  num%base; //get lower num 0-9 
		str[i++] = (r > 9 )? 'a'+r:'0'+r; //convert to ascii if >9 usse hex digits 
		num/=num;
	}
	return str;
} 


char *strcat(char* left, char* right)
{
	
}

#endif //STR_H 