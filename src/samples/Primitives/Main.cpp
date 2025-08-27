#include <MainTemplate.hpp>
#include <Primitives/App.hpp>

#ifdef MMPENGINE_WIN
std::int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE,
	PSTR, std::int32_t)
{
	//return MainTemplate::WinMain<Sample::Primitives::App>(hInstance, MMPEngine::Core::BackendType::Dx12, "Primitives");
	return MainTemplate::SharedMain<Sample::Primitives::App>(MMPEngine::Core::BackendType::Vulkan, "Primitives");
}
#endif

#ifdef MMPENGINE_MAC
std::int32_t main()
{
    return MainTemplate::SharedMain<Sample::Primitives::App>(MMPEngine::Core::BackendType::Metal, "Primitives");
}
#endif
