#include <Buffers/App.hpp>
#include <Frontend/Buffer.hpp>

namespace Sample::Buffers
{
	App::App() = default;

	void App::Initialize()
	{
		UserApp::Initialize();

		Test_Upload_To_Resident_To_ReadBack();
		TestConstantBufferTo_ReadBack();
		Test_Structured_Buffers();
	}

	void App::Test_Upload_To_Resident_To_ReadBack()
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

		const auto resident = std::make_shared<MMPEngine::Frontend::ResidentBuffer>(
			GetContext(),
			MMPEngine::Core::Buffer::Settings{
			byteLength,
				"test_resident_buffer"
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
			stream->Schedule(resident->CreateInitializationTask());
		}

		uploadBuffer->Write(uploadVec.data(), byteLength);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(uploadBuffer->CopyToBuffer(resident));
			stream->Schedule(resident->CopyToBuffer(readBack));
		}

		std::vector<std::size_t> readBackVec(_vecSize, 0);
		readBack->Read(readBackVec.data(), byteLength, 0);

		assert(std::equal(uploadVec.cbegin(), uploadVec.cend(), readBackVec.cbegin()));
	}

    void App::Test_Structured_Buffers()
    {
		const auto stream = GetDefaultStream();


		std::vector<MMPEngine::Core::Vector2Uint> uploadVec(_vecSize, MMPEngine::Core::Vector2Uint{});

		for (std::size_t i = 0; i < uploadVec.size(); ++i)
		{
			uploadVec[i] = {static_cast<std::uint32_t>(i), static_cast<std::uint32_t>(i) };
		}

		const auto uploadBuffer = std::make_shared<MMPEngine::Frontend::StructuredUploadBuffer<MMPEngine::Core::Vector2Uint>>(
			GetContext(),
			MMPEngine::Frontend::BaseStructuredBuffer::Settings{
			uploadVec.size(),
				"test_upload_buffer"
		}
		);

		const auto resident = std::make_shared<MMPEngine::Frontend::StructuredResidentBuffer<MMPEngine::Core::Vector2Uint>>(
			GetContext(),
			MMPEngine::Frontend::BaseStructuredBuffer::Settings{
			uploadVec.size(),
				"test_resident_buffer"
		}
		);

		const auto readBack = std::make_shared<MMPEngine::Frontend::StructuredReadBackBuffer<MMPEngine::Core::Vector2Uint>>(
			GetContext(),
			MMPEngine::Frontend::BaseStructuredBuffer::Settings{
			uploadVec.size(),
				"test_read_back_buffer"
		}
		);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(uploadBuffer->CreateInitializationTask());
			stream->Schedule(readBack->CreateInitializationTask());
			stream->Schedule(resident->CreateInitializationTask());
		}

		for(std::size_t i = 0; i < uploadVec.size(); ++i)
		{
			uploadBuffer->WriteStruct(uploadVec[i], i);
		}

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(uploadBuffer->CopyToBuffer(resident));
			stream->Schedule(resident->CopyToBuffer(readBack));
		}

		std::vector<MMPEngine::Core::Vector2Uint> readBackVec(_vecSize, MMPEngine::Core::Vector2Uint{});

		for (std::size_t i = 0; i < uploadVec.size(); ++i)
		{
			readBack->ReadStruct(readBackVec[i], i);
		}

		assert(std::equal(uploadVec.cbegin(), uploadVec.cend(), readBackVec.cbegin()));
    }

	void App::TestConstantBufferTo_ReadBack()
	{
		const auto stream = GetDefaultStream();

		const auto constantBuffer = std::make_shared<MMPEngine::Frontend::ConstantBuffer<TestStruct>>(GetContext(), "test_constant_buffer");
		const auto readBackBuffer = std::make_shared<MMPEngine::Frontend::ReadBackBuffer>(
			GetContext(),
			MMPEngine::Core::Buffer::Settings{
			sizeof(TestStruct),
				"test_read_back_buffer"
		}
		);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(constantBuffer->CreateInitializationTask());
			stream->Schedule(readBackBuffer->CreateInitializationTask());
		}

		const TestStruct expected {
			{1,2,3,4},
			{5,6,7,8}
		};

		constantBuffer->Write(expected);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(constantBuffer->CopyToBuffer(readBackBuffer));
		}

		TestStruct actual {};
		readBackBuffer->Read(&actual, sizeof(actual), 0);

		assert(expected.v1 == actual.v1);
		assert(expected.v2 == actual.v2);
	}

}