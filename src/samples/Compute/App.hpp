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
		static constexpr std::size_t _vecSize = 128;
		void Test_SetThreadId();
	};
}
