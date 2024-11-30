#include <Buffers/App.hpp>
#include <Frontend/Buffers.hpp>

namespace Sample::Buffers
{
	App::App() = default;

	void App::Initialize()
	{
		UserApp::Initialize();

		Test_Upload_To_ReadBack();
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

}