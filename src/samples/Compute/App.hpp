#pragma once
#include <Core/App.hpp>

namespace Sample::Compute
{
	class App : public MMPEngine::Core::UserApp
	{
	public:
		App();
		void Initialize() override;
	private:
		void Test_SetThreadId();
	};
}
