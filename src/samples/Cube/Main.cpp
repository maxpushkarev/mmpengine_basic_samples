#include <MainTemplate.hpp>
#include <Cube/App.hpp>

#ifdef MMPENGINE_WIN
std::int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE,
	PSTR, std::int32_t)
{
	return MainTemplate::WinMain<Sample::Cube::App>(hInstance, MMPEngine::Core::BackendType::Dx12, "Cube");
}
#endif