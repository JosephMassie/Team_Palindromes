#pragma once

#include "../mem.h"

class String;

/** case-insensitive strcmp */
int STRCMP(const char * const & a, const char * const & b);

/**
 * @param c a character to find in a string
 * @param a_charField a string to find a character in
 * @param a_numChars the length of the string
 */
bool isIn(char const & c, const char * const & charField, int const & a_numchars);

// TODO move string methods into String class
// TODO use String in place of char *

///** @return 0 if equal, > 0 if a comes first */
//int stringCompare(char * const & a, char * const & b);
//
///**
// * @param a
// * @param b null terminated string
// * @param len max length of a
// */
//int stringCompare(char * const & a, char * const & b, int const & len);

/** @return new char[stringLength+1] */
char * stringCopy(const char * const & a_string);

/** @return true if string begings with prefix */
bool isStartingWith(const char * const & string, const char * const & prefix, int const & prefixLength);
/**
 * @param a_token which token is being searched for
 * @param a_possibleTokens null-terminated list of tokens
 * @return -1 if the given token was not found in the list of possible tokens
 */
int isMulticharToken(const String * const & a_token, const char ** const & a_possibleTokens);

/**
 <code>
 char * FLAG_NAMES[] = {
	"managed", 
	"int", 
	"float",
	"str",
	"in_literal",
	"//",
	"ignore_//",
	"again",
};
	int flag = (1<<0)|(1<<2)|(1<<7);	// = 1|4|128 = 133
	// output: "(managed|float|again)"
	printOnFlags(flag, FLAG_NAMES);</code><br>
 */
void printOnFlags(int const & a_flag, char ** flagNames);

/**
 * @return true if the given string is a representation of an integer
 * @param a_length how long the string is
 */
bool isInteger(const char * source, const int & length);

/**
 * @return true if the given string is a representation of an integer
 */
bool isInteger(const char * source);

/**
 * @return true if the given string is a representation of a hexadecimal value
 * @param a_length how long the string is
 */
bool isHex(const char * source, const int & length);

/**
 * @return true if the given string is a representation of an hexadecimal value
 */
bool isHex(const char * source);

/**
 * @return true if the given string is a representation of a float
 * @param a_length how long the string is
 */
bool isFloat(const char * source, const int & length);

/** @return true if the given string is a representation of a float */
bool isFloat(const char * a_source);

/**
 * @param a_source read an integer variable out of the given string
 * @param a_length how long the string is
 */
int parseInt(const char * source, const int & length);

/**
 * @param a_source read a hex variable out of the given string
 */
int parseHex(const char * a_source);

/**
 * @param a_source read a hex variable out of the given string
 * @param a_length how long the string is
 */
int parseHex(const char * source, const int & length);

/**
 * @param a_source read an integer variable out of the given string
 */
int parseInt(const char * a_source);

/**
 * @param a_source read a floating point variable out of the given string
 * @param a_length how long the string is
 */
float parseFloat(const char * a_source, int const & a_length);

/**
 * @param a_source read a floating point variable out of the given string
 */
float parseFloat(const char * a_source);

/** String class */
class String
{
private:
	/** ASCII data of the string (NOT null-terminated) */
	char * data;
	/** number of ASCII elements in the string */
	int size;
	/**
	 * ALLOCATED : 
	 */
	int flags;
	/** this bit is set if data was allocated using new and must be released during the destructor */
	static const int ALLOCATED = (1<<0);
public:
	static void copy(char * const & a_dest, int const & a_length, const char * const & a_source){
		for(int i = 0; i < a_length; ++i){
			a_dest[i] = a_source[i];
		}
	}

	/** makes a 'new' char array that is a copy of the given char array */
	static char * copyCStr(const char * const & a_string, int len)
	{
		char * c = NEWMEM_ARR(char, len+1);//new char[len+1];
//		for(int i = 0; i < len; ++i)
//			c[i] = a_string[i];
		copy(c, len, a_string);
		c[len] = '\0';
		return c;
	}
private:
	/**
	 * @param a_str actual character array text to copy
	 * @param a_len how much of the given character array to use as this string
	 * @param a_makeCopy if true, will allocate separate memory for the string
	 */
	void init(const char * const & a_str, int const & a_len, bool const & a_makeCopy)
	{
		flags = 0;
		size = a_len;
		data = !a_makeCopy?const_cast<char*>(a_str):String::copyCStr(a_str, a_len);
		if(a_makeCopy)
			flags = ALLOCATED;
	}
public:
	/** @param data BY REFERENCE, not a copy */
	String(char * data, int size)
		:data(data),size(size),flags(0)
	{}
	String():data(0),size(0),flags(0)
	{}
	void print();
	void print(int a_size);
	void printNoWhitespace();
	/** @return a copy of the given c-string (null-terminated) */
	static char * copyCStr(const char * const & a_string)
	{
		int len = String::length(a_string);
		return copyCStr(a_string, len);
	}
	/** @return length of the null terminated string */
	static int length(const char * const & str)
	{
		int len = 0;
		while(str[len++]);
		return len-1;
	}
	/** make this string a copy (it's own memory if a_makeCopy is true) of the given string segment */
	String(char * const & a_str, int const & a_len, bool const & a_makeCopy)
	{
		init(a_str, a_len, a_makeCopy);
	}
	/** make this string a copy (it's own memory if a_makeCopy is true) of the given null-terminated string */
	String(char * const & a_str, bool const & a_makeCopy)
	{
		init(a_str, String::length(a_str), a_makeCopy);
	}
	/** make this string a reference to the given null-terminated string */
	String(const char * const & a_str)
	{
		init(a_str, String::length(a_str), false);
	}
	/** make this string a reference to the given string */
	String(String const & str):flags(0)
	{
		size = str.size;
		data = str.data;
	}
	/** make this string a copy (it's own memory if a_makeCopy is true) to the given string */
	String(String & str, bool const & a_makeCopy):flags(0)
	{
		init(str.cstr(), str.length(), a_makeCopy);
	}
	/** @note dangerous! if string allocated memory, it will be released here */
	void setData(char * a_data)
	{
		if(flags & ALLOCATED)
			this->~String();
		data = a_data;
	}
	/** @return the index of a_char in this string, starting the linear search from index a_start */
	int indexOf(char const & a_char, int const & a_start)
	{
		int found = -1;
		for(int i = a_start; found < 0 && i < size; ++i)
		{
			if(data[i] == a_char)
				found = i;
		}
		return found;
	}
	/** @return the index of a_char in this string, starting the linear search from index a_start */
	inline int indexOf(char const & a_char){
		return indexOf(a_char, 0);
	}
	~String(){
		if((flags & ALLOCATED) != 0 && data){
			DELMEM_ARR(data);//delete [] data;
			data = 0;
			size = 0;
			flags &= ~ALLOCATED;
		}
	}
	/** @return the character at the given location */
	inline char & get(int const & a_index){
		return data[a_index];
	}
	/** @param a_value what value to put in the given index */
	inline void set(int const & a_index, char const & a_value){
		data[a_index] = a_value;
	}
	/** @return the size of this string */
	inline int length(){
		return size;
	}
	/** @return {@link length()} */
	inline int getLength(){
		return length();
	}
	/** @return a string that is a sub-string (allocates no memory for the substring) */
	inline String subString(int const & a_startIndex, int const & length){
		return String(&(data[a_startIndex]), length);
	}
	/** @return a string that is a sub-string (allocates no memory for the substring) */
	inline String subString(int const & a_startIndex){
		return subString(a_startIndex, size-a_startIndex);
	}
	/** 
	 * @param a_length sets the string length to this value
	 * @note use with caution. increasing the size should not be allowed... 
	 */
	void setLength(int const & a_length){
		size = a_length;
	}

	/**
	 * @warning may not be safe in future implementations... avoid if possible 
	 * TODO remove from use...
	 */
	char * cstr(){
		return data;
	}

	/** copy this string into the given standard character array */
	inline void writeIntoCharBuffer(char * const & a_buffer, int const & bufferSize){
		int end = (size<(bufferSize-1))?size:(bufferSize-1);
		for(int i = 0; i < end; ++i){a_buffer[i]=data[i];}
		a_buffer[end]='\0';
	}

	/** @return the index where this String and the given c-string literal stop matching exactly */
	int sameUntil(const char * const & b) const{
		int i;
		for(i=0;i<size && data[i]==b[i];++i);
		return i;
	}

	static int compare(char * a, char * b){
		while(*a && *a == *b){++a;++b;}
		return *a - *b;
	}

	static int compare(const char * const & a, int const & size, const char * const & b){
		for(int i = 0; (size > 0 && i < size) || b[i]; ++i)
		{
			if(a[i] != b[i])
				return a[i] - b[i];
		}
		return 0;
	}

	int compare(const char * const & b){
		return compare(data, size, b);
	}

	bool isInteger(){	return ::isInteger(data, size);	}
	bool isFloat(){		return ::isFloat(data, size);	}
	bool isHex(){		return ::isHex(data, size);	}

	int parseInteger(){	return ::parseInt(data, size);	}
	float parseFloat(){	return ::parseFloat(data, size);	}
	int parseHex(){		return ::parseHex(data, size);	}
};
