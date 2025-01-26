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

	template<typename TUserApp>
	static std::int32_t SharedMain(MMPEngine::Core::BackendType backendType, const std::string& userAppName, std::optional<bool> isDebug = std::nullopt);
};

template<typename TUserApp>
inline std::int32_t MainTemplate::SharedMain(MMPEngine::Core::BackendType backendType, const std::string& userAppName, std::optional<bool> isDebug)
{
	MMPEngine::Core::GlobalContext::Settings appContextSettings {};

	if (isDebug.has_value())
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

#ifdef MMPENGINE_WIN
	const auto logger = std::make_shared<MMPEngine::Feature::Win::OutputLogger>(userAppName + " logger");
#endif
	auto userApp = std::make_unique<TUserApp>(logger);
	auto rootApp = MMPEngine::Feature::App::BuildRootApp(appContextSettings, std::move(userApp), nullptr, logger);

	MMPEngine::Feature::Shared::AppContainer::Settings appContainerSettings {};
	appContainerSettings.base.targetFps = 360;
	appContainerSettings.base = { userAppName };

	const auto appContainer = std::make_unique<MMPEngine::Feature::Shared::AppContainer>(std::move(appContainerSettings), std::move(rootApp));
	return appContainer->Run();
	return 0;
}

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
	auto userApp = std::make_unique<TUserApp>(logger);
	auto rootApp = MMPEngine::Feature::App::BuildRootApp(appContextSettings, std::move(userApp), nullptr, logger);

	MMPEngine::Feature::Win::AppContainer::Settings appContainerSettings {};
	appContainerSettings.base.targetFps = 360;
	appContainerSettings.base = { userAppName };
	appContainerSettings.platform = { hInstance };

	const auto appContainer = std::make_unique<MMPEngine::Feature::Win::AppContainer>(std::move(appContainerSettings), std::move(rootApp));
	return appContainer->Run();
}

#endif