#pragma once
#include <Feature/App.hpp>

namespace Sample::Compute
{
	class App : public MMPEngine::Feature::UserApp
	{
	public:
		App(const std::shared_ptr<MMPEngine::Feature::BaseLogger>& logger);
		void Initialize() override;
	private:
		static constexpr std::size_t _vecSize = 128;
		void Test_SetThreadId();
		void Test_Filter();
	};
}
