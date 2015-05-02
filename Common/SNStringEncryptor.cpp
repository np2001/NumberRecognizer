#include "stdafx.h"
#include "SNStringEncryptor.h"
//-----------------------------------------------------------
SNStringEncryptor::SNStringEncryptor()
{

}
//-----------------------------------------------------------

SNStringEncryptor::~SNStringEncryptor()
{

}
//-----------------------------------------------------------

void SNStringEncryptor::Crypt(char st[])
{
	Crypt(st, sizeof(st));
}
//-----------------------------------------------------------

void SNStringEncryptor::Crypt(char* st, int32_t size)
{
	Seed(1);

	for (int32_t i = 0; i < sizeof(st); ++i)
	{
		st[i] ^= GetNext();
	}
}
//-----------------------------------------------------------

void SNStringEncryptor::DecryptToString(const char* st, int32_t size, std::string& result)
{
	result.reserve(size);

	Seed(1);

	for (int32_t i = 0; i < size; ++i)
	{
		result += (st[i] ^ GetNext());
	}
}
//-----------------------------------------------------------

void SNStringEncryptor::Seed(unsigned long seed)
{
	Next = seed;
}
//-----------------------------------------------------------

uint8_t SNStringEncryptor::GetNext()
{
	Next = Next * 1664525 + 1013904223;
	return Next % 256;
}
//-----------------------------------------------------------