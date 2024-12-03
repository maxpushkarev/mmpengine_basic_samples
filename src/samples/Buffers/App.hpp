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

		struct TestStruct final
		{
			MMPEngine::Core::Vector4Uint v1;
			MMPEngine::Core::Vector4Uint v2;
		};

		void Test_Upload_To_Resident_To_ReadBack();
		void Test_Structured_Buffers();
		void TestConstantBufferTo_ReadBack();
		static constexpr std::size_t _vecSize = 128;
	};
}
