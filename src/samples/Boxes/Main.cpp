#include <MainTemplate.hpp>
#include <Boxes/App.hpp>

#ifdef MMPENGINE_WIN
std::int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE,
	PSTR, std::int32_t)
{
	return MainTemplate::WinMain<Sample::Boxes::App>(hInstance, MMPEngine::Core::BackendType::Dx12, "Boxes");
	//return MainTemplate::SharedMain<Sample::Boxes::App>(MMPEngine::Core::BackendType::Dx12, "Boxes");
}
#endif