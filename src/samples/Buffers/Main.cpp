#include <MainTemplate.hpp>
#include <Buffers/App.hpp>

#ifdef MMPENGINE_WIN
std::int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE,
	PSTR, std::int32_t)
{
	//return MainTemplate::WinMain<Sample::Buffers::App>(hInstance, MMPEngine::Core::BackendType::Dx12, "Buffers");
	return MainTemplate::SharedMain<Sample::Buffers::App>(MMPEngine::Core::BackendType::Vulkan, "Buffers");
}
#endif