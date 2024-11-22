#include <cstdint>
#include <Core/Context.hpp>
#include <Frontend/App.hpp>
#include <Frontend/Logger.hpp>
#include <Cube/App.hpp>
#include <Frontend/AppContainer.hpp>
#ifdef MMPENGINE_WIN
#include <Windows.h>
#endif

#ifdef MMPENGINE_WIN
std::int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE,
	PSTR, std::int32_t)
{
	MMPEngine::Core::AppContext::Settings appContextSettings {};
#if (defined(DEBUG) || defined(_DEBUG)) 
	appContextSettings.isDebug = true;
#else
	appContextSettings.isDebug = false;
#endif

	appContextSettings.backend = MMPEngine::Core::BackendType::Dx12;

	const auto userApp = std::make_shared<Sample::Cube::App>();
	const auto rootApp = MMPEngine::Frontend::App::BuildRootApp(appContextSettings, userApp);
	rootApp->GetContext()->logger = std::make_unique<MMPEngine::Frontend::Win::OutputLogger>("Cube logger");

	MMPEngine::Frontend::Win::AppContainer::Settings appContainerSettings {};
	appContainerSettings.base = {"Cube"};
	appContainerSettings.platform = {hInstance};

	const auto appContainer = std::make_unique<MMPEngine::Frontend::Win::AppContainer>(std::move(appContainerSettings), rootApp);

	return appContainer->Run();
}
#endif