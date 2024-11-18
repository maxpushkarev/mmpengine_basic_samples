#include <cstdint>
#include <Core/Context.hpp>
#include <Frontend/App.hpp>
#include <Frontend/Logger.hpp>
#include <Cube/App.hpp>
#ifdef MMPENGINE_WIN
#include <Windows.h>
#endif

#ifdef MMPENGINE_WIN
std::int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE,
	PSTR, std::int32_t)
{
	MMPEngine::Core::AppContextSettings appContextSettings {};
#if (defined(DEBUG) || defined(_DEBUG)) 
	appContextSettings.isDebug = true;
#else
	appContextSettings.isDebug = false;
#endif

	appContextSettings.backend = MMPEngine::Core::BackendType::Dx12;

	const auto userApp = std::make_shared<Cube::App>();
	const auto rootApp = MMPEngine::Frontend::App::BuildRootApp(appContextSettings, userApp);
	rootApp->GetContext()->logger = std::make_unique<MMPEngine::Frontend::Win::OutputLogger>("Cube sample logger");

	return 0;
}
#endif