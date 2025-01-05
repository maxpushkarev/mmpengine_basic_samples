#include <Empty/App.hpp>

namespace Sample::Empty
{
	App::App(const std::shared_ptr<MMPEngine::Feature::BaseLogger>& logger) : UserApp(logger)
	{
	}

	void App::Initialize()
	{
		UserApp::Initialize();

		{
			const auto executor = GetDefaultStream()->CreateExecutor();
		}
	}

}