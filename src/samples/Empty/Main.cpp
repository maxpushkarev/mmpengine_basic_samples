#include <MainTemplate.hpp>
#include <Empty/App.hpp>

#ifdef MMPENGINE_WIN
std::int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE,
	PSTR, std::int32_t)
{
	return MainTemplate::WinMain<Sample::Empty::App>(hInstance, MMPEngine::Core::BackendType::Dx12, "Empty");
}
#endif