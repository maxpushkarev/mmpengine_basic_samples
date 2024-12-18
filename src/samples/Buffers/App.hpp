#pragma once
#include <Feature/App.hpp>

namespace Sample::Buffers
{
	class App : public MMPEngine::Feature::UserApp
	{
	public:
		App(const std::shared_ptr<MMPEngine::Feature::BaseLogger>& logger);
		void Initialize() override;
	private:

		struct TestStruct final
		{
			MMPEngine::Core::Vector4Uint v1;
			MMPEngine::Core::Vector4Uint v2;
		};

		void Test_Upload_To_Resident_To_ReadBack();
		void Test_Structured_Buffers();
		void Test_ConstantBufferTo_ReadBack();
		void Test_IndexBuffer();
		void Test_VertexBuffer();
		static constexpr std::size_t _vecSize = 128;
	};
}
