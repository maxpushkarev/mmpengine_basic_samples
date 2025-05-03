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
		struct TestUniform final
		{
			std::uint32_t value;
		};
		void Test_SetValue();
		void Test_Filter();
	};
}
