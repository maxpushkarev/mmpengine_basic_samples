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

		struct ConstantBufferStruct final
		{
			MMPEngine::Core::Vector4Uint v1;
			MMPEngine::Core::Vector4Uint v2;
		};

		void Test_Upload_To_ReadBack();
		void TestConstantBufferTo_ReadBack();
		static constexpr std::size_t _vecSize = 128;
	};
}
