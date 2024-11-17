#include <cstdint>
#ifdef MMPENGINE_WIN
#include <Windows.h>
#endif

#ifdef MMPENGINE_WIN
std::int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE,
	PSTR, std::int32_t)
{
	return 0;
}
#endif