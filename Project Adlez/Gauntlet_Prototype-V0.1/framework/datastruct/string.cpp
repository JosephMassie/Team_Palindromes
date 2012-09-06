#include "string.h"
#include <stdio.h>

#define __TOKENIZER_MINUS_SIGN		'-'
#define __TOKENIZER_DECIMAL_POINT	'.'

/** case-insensitive strcmp */
int STRCMP(const char * const & a, const char * const & b){
	char ca, cb;
#define TOLOWER(CHAR)	if(CHAR >= 'A' && CHAR <= 'Z')CHAR += ('a'-'A');
	for(int i = 0; a[i] || b[i]; ++i){
		ca = a[i];	cb = b[i];
		TOLOWER(ca)
		TOLOWER(cb)
		if(ca != cb){return ca-cb;}
	}
	return 0;
#undef TOLOWER
}

///** @return 0 if equal, > 0 if a comes first */
//int stringCompare(char * const & a, char * const & b)
//{
//	for(int i = 0; a[i] || b[i]; ++i)
//	{
//		if(a[i] != b[i])
//			return a[i] - b[i];
//	}
//	return 0;
//}

///**
// * @param a
// * @param b null terminated string
// * @param len max length of a
// */
//int stringCompare(char * const & a, char * const & b, int const & len)
//{
//	for(int i = 0; (len > 0 && i < len) || b[i]; ++i)
//	{
//		if(a[i] != b[i])
//			return a[i] - b[i];
//	}
//	return 0;
//}

void String::print()
{
	char * end = data+size;
	char * c = data;
	while(c < end){putchar(*c++);}
}
void String::print(int a_size)
{
	char * end = data+size;
	int i = 0;
	while(i < size && i < a_size){if(data[i] == '\n')break;putchar(data[i++]);}
}


/** @return new char[stringLength+1] */
char * stringCopy(const char * const & a_string)
{
	int len = String::length(a_string);
	char * c = NEWMEM_ARR(char, (len+1));//new char[len+1];
	for(int i = 0; i < len; ++i)
		c[i] = a_string[i];
	c[len] = '\0';
	return c;
}


void String::printNoWhitespace()
{
	for(int i = 0; i < size; ++i){
		switch(data[i]){
		case '\0':	case '\a':	case '\b':
		case '\t':	case '\n':	case '\r':
		case ' ':	case -1:	break;
		default:	putchar(data[i]);	break;
		}
	}
}

/** @return true if the given character is a digit */
static inline bool isADigit(const char & c)
{
	return c >= '0' && c <= '9';
}

/** @return true if the given character is a hexadecimal digit */
static inline bool ishexdigit(const char & c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

/**
 * @param c a character to find in a string
 * @param a_charField a string to find a character in
 * @param a_numChars the length of the string
 */
inline bool isIn(char const & c, const char * const & a_charField, int const & a_numChars)
{
	for(int i = 0; i < a_numChars; ++i)
	{
		if(c == a_charField[i])
		{
			return true;
		}
	}
	return false;
}

/** @return true if string begings with prefix */
bool isStartingWith(const char * const & string, const char * const & prefix, int const & prefixLength){
	int i;
	for(i=0;i<prefixLength && prefix[i]==string[i];++i);
	return i>=prefixLength;
}

/**
 * @param a_token which token is being searched for
 * @param a_possibleTokens null-terminated
 * @return -1 if the given token was not found in the list of possible tokens
 */
int isMulticharToken(const String * const & a_token, const char ** const & a_possibleTokens)
{
	int index;
	for(int i = 0; a_possibleTokens[i]; ++i)
	{
		index = a_token->sameUntil(a_possibleTokens[i]);
		if(a_possibleTokens[i][index] == '\0')
		//if(str->compare(possibleTokens[i]) == 0)
		//if(stringCompare(str, possibleTokens[i], strlen) == 0)
			return i;
	}
	return -1;
}

/**
 <code>char * FLAG_NAMES[] = {
	"managed", 
	"int", 
	"float",
	"str",
	"in_literal",
	"//",
	"ignore_//",
	"again",
};
	int flag = 1|4|128;
	// output: "(managed|float|again)"
	printOnFlags(flag, FLAG_NAMES);</code><br>
 */
void printOnFlags(int const & a_flag, char ** flagNames)
{
	int printed = 0;
	printf("(");
	for(int i = 0; i < 8; ++i)
	{
		if((a_flag & (1 << i)) != 0)
		{
			if(printed > 0)
			{
				printf("|");
			}
			printf("%s", flagNames[i]);
			printed++;
		}
	}
	printf(")");
}

/**
 * @return true if the given string is a representation of an integer
 * @param a_length how long the string is
 */
bool isInteger(const char * source, const int & length)
{
	char c;
	for(int i = ((source[0]!=__TOKENIZER_MINUS_SIGN)?0:1); i < length && source[i] != '\0'; ++i)
	{
		c = source[i];
		if(!isADigit(c))
		{
			return false;
		}
	}
	return true;
}

/**
 * @return true if the given string is a representation of an integer
 */
bool isInteger(const char * source)
{
	return isInteger(source, String::length(source));
}

/**
 * @return true if the given string is a representation of a hexadecimal value
 * @param a_length how long the string is
 */
bool isHex(const char * source, const int & length)
{
	if(length < 2 || source[0] != '0' || source[1] != 'x')
		return false;
	char c;
	for(int i = 2; i < length && source[i] != '\0'; ++i)
	{
		c = source[i];
		if(!ishexdigit(c))
		{
			return false;
		}
	}
	return true;
}

/**
 * @return true if the given string is a representation of a hexadecimal value
 */
bool isHex(const char * source)
{
	return isHex(source, String::length(source));
}

/**
 * @return true if the given string is a representation of a float
 * @param a_length how long the string is
 */
bool isFloat(const char * source, const int & length)
{
	char c;
	bool havReadDecimal = false;
	for(int i = ((source[0]!=__TOKENIZER_MINUS_SIGN)?0:1); i < length; ++i)
	{
		c = source[i];
		if(!isADigit(c)
		&& (havReadDecimal 
		|| !(havReadDecimal = (c == __TOKENIZER_DECIMAL_POINT))))
		{
			return false;
		}
	}
	return true;
}

/** @return true if the given string is a representation of a float */
bool isFloat(const char * a_source)
{
	return isFloat(a_source, String::length(a_source));
}

/**
 * @param a_source read an integer variable out of the given string
 * @param a_length how long the string is
 */
int parseInt(const char * source, const int & length)
{
	int value = 0;
	char c;
	int i = source[0] != __TOKENIZER_MINUS_SIGN ? 0 : 1;
	for(; i < length; ++i)
	{
		c = source[i];
		value *= 10;
		value += (c - '0');
	}
	if(source[0] == __TOKENIZER_MINUS_SIGN)
	{
		value *= -1;
	}
	return value;
}

/**
 * @param a_source read an integer variable out of the given string
 */
int parseInt(const char * a_source)
{
	return parseInt(a_source, String::length(a_source));
}

/**
 * @param a_source read a hexadecimal variable out of the given string
 * @param a_length how long the string is
 */
int parseHex(const char * source, const int & length)
{
	int value = 0;
	char c;
	int i = (source[0] == '0' && source[1] == 'x')?2:0;
	for(; i < length; ++i)
	{
//		for(int a = 0; a < i; ++a)putchar(source[a]);
//		printf(" %c ",source[i]);
//		for(int a = i+1; a < length; ++a)putchar(source[a]);
//		printf("   %d, [%x]\n", value, value);
		c = source[i];
		if(c >= 'a' && c <= 'f')	c='9'+1+(c-'a');
		if(c >= 'A' && c <= 'F')	c='9'+1+(c-'A');
		value *= 16;
		value += (c - '0');
	}
//	printf("[%x]\n", value);
	return value;
}

/**
 * @param a_source read a hexadecimal variable out of the given string
 */
int parseHex(const char * a_source)
{
	return parseHex(a_source, String::length(a_source));
}

/**
 * @param a_source read a floating point variable out of the given string
 * @param a_length how long the string is
 */
float parseFloat(const char * a_source, int const & a_length)
{
	float value = 0;
	int decimal = 0;
	char c;
	int i = a_source[0] != __TOKENIZER_MINUS_SIGN ? 0 : 1;
	for(; i < a_length-1; ++i)
	{
		c = a_source[i];
		if(c != __TOKENIZER_DECIMAL_POINT)
		{
			value *= 10;
			if(decimal > 0)
			{
				decimal *= 10;
			}
			value += (c - '0');
		}else{
			decimal = 1;
		}
	}
	if(a_source[0] == __TOKENIZER_MINUS_SIGN)
	{
		value *= -1;
	}
	if(decimal > 0)
	{
		value /= decimal;
	}
	return value;
}

/**
 * @param a_source read a floating point variable out of the given string
 */
float parseFloat(const char * a_source)
{
	return parseFloat(a_source, String::length(a_source));
}
