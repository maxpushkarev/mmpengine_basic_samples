#pragma once

#ifdef MMPENGINE_WIN
#include <Windows.h>
#endif

#include <Core/Context.hpp>
#include <Feature/App.hpp>
#include <Feature/Logger.hpp>
#include <Feature/AppContainer.hpp>

class MainTemplate final
{
public:

#ifdef MMPENGINE_WIN
	template<typename TUserApp>
	static std::int32_t WinMain(HINSTANCE hInstance, MMPEngine::Core::BackendType backendType, const std::string& userAppName, std::optional<bool> isDebug = std::nullopt);
#endif

};

#ifdef MMPENGINE_WIN

template<typename TUserApp>
inline std::int32_t MainTemplate::WinMain(HINSTANCE hInstance, MMPEngine::Core::BackendType backendType, const std::string& userAppName, std::optional<bool> isDebug)
{
	MMPEngine::Core::GlobalContext::Settings appContextSettings {};

	if(isDebug.has_value())
	{
		appContextSettings.isDebug = isDebug.value();
	}
	else
	{
#if (defined(DEBUG) || defined(_DEBUG)) 
		appContextSettings.isDebug = true;
#else
		appContextSettings.isDebug = false;
#endif
	}

	appContextSettings.backend = backendType;

	const auto logger = std::make_shared<MMPEngine::Feature::Win::OutputLogger>(userAppName + " logger");
	const auto userApp = std::make_shared<TUserApp>(logger);
	const auto rootApp = MMPEngine::Feature::App::BuildRootApp(appContextSettings, userApp, nullptr, logger);

	MMPEngine::Feature::Win::AppContainer::Settings appContainerSettings {};
	appContainerSettings.base = { userAppName };
	appContainerSettings.platform = { hInstance };

	const auto appContainer = std::make_unique<MMPEngine::Feature::Win::AppContainer>(std::move(appContainerSettings), rootApp);
	return appContainer->Run();
}

#endif