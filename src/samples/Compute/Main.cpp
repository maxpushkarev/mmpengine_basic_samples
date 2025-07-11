#include <MainTemplate.hpp>
#include <Compute/App.hpp>

#ifdef MMPENGINE_WIN
std::int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE,
	PSTR, std::int32_t)
{
	//return MainTemplate::WinMain<Sample::Compute::App>(hInstance, MMPEngine::Core::BackendType::Dx12, "Compute");
	return MainTemplate::SharedMain<Sample::Compute::App>(MMPEngine::Core::BackendType::Vulkan, "Compute");
}
#endif

#ifdef MMPENGINE_MAC
std::int32_t main()
{
    return 0;
}
#endif
