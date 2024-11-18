#pragma once
#include <Core/App.hpp>

namespace Cube
{
	class App : public MMPEngine::Core::UserApp
	{
	public:
		App(const std::shared_ptr<MMPEngine::Core::AppContext>& context);
	};
}
