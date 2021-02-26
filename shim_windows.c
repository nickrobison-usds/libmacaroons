
/* c */
#include <windows.h>
#include <bcrypt.h>

void arc4random_buf(void* const buf, const size_t size)
{
	BCryptGenRandom(NULL, buf, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
}