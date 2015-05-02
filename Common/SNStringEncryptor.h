#ifndef SNStringEncryptor_h__
#define SNStringEncryptor_h__
//----------------------------------------------------------
#include <stdint.h>
#include <string>
//----------------------------------------------------------
class SNStringEncryptor
{
public:
	SNStringEncryptor();
	~SNStringEncryptor();
	void Crypt(char st[]);
	void Crypt(char* st, int32_t size);

	void DecryptToString(const char* st, int32_t size, std::string& result);
private:
	void Seed(unsigned long seed);
	uint8_t GetNext();
private:
	unsigned long Next;
};
//----------------------------------------------------------
#endif // SNStringEncryptor_h__
