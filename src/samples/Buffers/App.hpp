#pragma once
#include <Core/App.hpp>

namespace Sample::Buffers
{
	class App : public MMPEngine::Core::UserApp
	{
	public:
		App();
		void Initialize() override;
	private:
		void Test_Upload_To_ReadBack();
		static constexpr std::size_t _vecSize = 128;
	};
}
