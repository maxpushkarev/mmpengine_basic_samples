#include <Buffers/App.hpp>
#include <Frontend/Buffer.hpp>

namespace Sample::Buffers
{
	App::App() = default;

	void App::Initialize()
	{
		UserApp::Initialize();

		Test_Upload_To_ReadBack();
		TestConstantBufferTo_ReadBack();
	}

	void App::Test_Upload_To_ReadBack()
	{
		const auto stream = GetDefaultStream();


		std::vector<std::size_t> uploadVec(_vecSize, 0);
		for (std::size_t i = 0; i < uploadVec.size(); ++i)
		{
			uploadVec[i] = i;
		}

		constexpr std::size_t byteLength = _vecSize * sizeof(decltype(uploadVec)::value_type);

		const auto uploadBuffer = std::make_shared<MMPEngine::Frontend::UploadBuffer>(
			GetContext(),
			MMPEngine::Core::Buffer::Settings{
			byteLength,
				"test_upload_buffer"
		}
		);

		const auto readBack = std::make_shared<MMPEngine::Frontend::ReadBackBuffer>(
			GetContext(),
			MMPEngine::Core::Buffer::Settings{
			byteLength,
				"test_read_back_buffer"
		}
		);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(uploadBuffer->CreateInitializationTask());
			stream->Schedule(readBack->CreateInitializationTask());
		}

		uploadBuffer->Write(uploadVec.data(), byteLength);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(uploadBuffer->CopyToBuffer(readBack));
		}

		std::vector<std::size_t> readBackVec(_vecSize, 0);
		readBack->Read(readBackVec.data(), byteLength, 0);

		assert(std::equal(uploadVec.cbegin(), uploadVec.cend(), readBackVec.cbegin()));
	}

	void App::TestConstantBufferTo_ReadBack()
	{
		const auto stream = GetDefaultStream();

		const auto constantBuffer = std::make_shared<MMPEngine::Frontend::ConstantBuffer<ConstantBufferStruct>>(GetContext(), "test_constant_buffer");
		const auto readBackBuffer = std::make_shared<MMPEngine::Frontend::ReadBackBuffer>(
			GetContext(),
			MMPEngine::Core::Buffer::Settings{
			sizeof(ConstantBufferStruct),
				"test_read_back_buffer"
		}
		);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(constantBuffer->CreateInitializationTask());
			stream->Schedule(readBackBuffer->CreateInitializationTask());
		}

		const ConstantBufferStruct expected {
			{1,2,3,4},
			{5,6,7,8}
		};

		constantBuffer->Write(expected);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(constantBuffer->CopyToBuffer(readBackBuffer));
		}

		ConstantBufferStruct actual {};
		readBackBuffer->Read(&actual, sizeof(actual), 0);

		assert(expected.v1.x == actual.v1.x);
		assert(expected.v1.y == actual.v1.y);
		assert(expected.v1.z == actual.v1.z);
		assert(expected.v1.w == actual.v1.w);

		assert(expected.v2.x == actual.v2.x);
		assert(expected.v2.y == actual.v2.y);
		assert(expected.v2.z == actual.v2.z);
		assert(expected.v2.w == actual.v2.w);
	}

}