#include <Buffers/App.hpp>
#include <Frontend/Buffer.hpp>
#include <cassert>

namespace Sample::Buffers
{
	App::App(const std::shared_ptr<MMPEngine::Feature::BaseLogger>& logger) : UserApp(logger)
	{
	}

	void App::Initialize()
	{
		UserApp::Initialize();

		Test_Upload_To_Resident_To_ReadBack();
		/*Test_UniformBufferTo_ReadBack();
		Test_Structured_Buffers();
		Test_IndexBuffer();
		Test_VertexBuffer();*/
	}

	void App::Test_Upload_To_Resident_To_ReadBack()
	{
		const auto stream = GetDefaultStream();


		std::vector<std::size_t> uploadVec(_vecSize, 0);
		std::vector<std::size_t> readBackVec(_vecSize, 0);

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

			//stream->Schedule(uploadBuffer->CreateWriteTask(uploadVec.data(), byteLength));
			//stream->Schedule(uploadBuffer->CopyToBuffer(resident));
			//stream->Schedule(resident->CopyToBuffer(readBack));
			//stream->Schedule(readBack->CreateReadTask(readBackVec.data(), byteLength, 0));
		}


		//assert(std::equal(uploadVec.cbegin(), uploadVec.cend(), readBackVec.cbegin()));
	}

	void App::Test_IndexBuffer()
	{
		const auto stream = GetDefaultStream();

		std::vector<std::int16_t> indexVec(_vecSize, 0);
		std::vector<std::int16_t> readBackVec(_vecSize, 0);

		for (std::size_t i = 0; i < indexVec.size(); ++i)
		{
			indexVec[i] = static_cast<std::int16_t>(i);
		}

		const auto indexBuffer = std::make_shared<MMPEngine::Frontend::IndexBuffer>(
			GetContext(),
			MMPEngine::Core::InputAssemblerBuffer::Settings{
				{
					indexVec.data()
				},
				{
					_vecSize * sizeof(decltype(indexVec)::value_type),
					"test_index_buffer"
				}
			}
		);

		const auto readBack = std::make_shared<MMPEngine::Frontend::ReadBackBuffer>(
			GetContext(),
			MMPEngine::Core::Buffer::Settings{
			indexBuffer->GetSettings().byteLength,
				"test_read_back_buffer"
		}
		);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(indexBuffer->CreateInitializationTask());
			stream->Schedule(readBack->CreateInitializationTask());
			stream->Schedule(indexBuffer->CopyToBuffer(readBack));
			stream->Schedule(readBack->CreateReadTask(readBackVec.data(), readBack->GetSettings().byteLength, 0));
		}

		assert(std::equal(indexVec.cbegin(), indexVec.cend(), readBackVec.cbegin()));
	}


	void App::Test_VertexBuffer()
	{
		const auto stream = GetDefaultStream();

		std::vector<MMPEngine::Core::Vector3Uint> vertexVec(_vecSize, MMPEngine::Core::Vector3Uint{});
		std::vector<MMPEngine::Core::Vector3Uint> readBackVec(_vecSize, MMPEngine::Core::Vector3Uint{});

		for (std::size_t i = 0; i < vertexVec.size(); ++i)
		{
			vertexVec[i] = MMPEngine::Core::Vector3Uint {
				static_cast<std::uint32_t>(i),
				static_cast<std::uint32_t>(i),
				static_cast<std::uint32_t>(i)
			};
		}

		const auto vertexBuffer = std::make_shared<MMPEngine::Frontend::VertexBuffer>(
			GetContext(),
			MMPEngine::Core::InputAssemblerBuffer::Settings{
				{
					vertexVec.data()
				},
				{
					_vecSize * sizeof(decltype(vertexVec)::value_type),
					"test_vertex_buffer"
				}
		}
		);

		const auto readBack = std::make_shared<MMPEngine::Frontend::ReadBackBuffer>(
			GetContext(),
			MMPEngine::Core::Buffer::Settings{
			vertexBuffer->GetSettings().byteLength,
				"test_read_back_buffer"
		}
		);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(vertexBuffer->CreateInitializationTask());
			stream->Schedule(readBack->CreateInitializationTask());
			stream->Schedule(vertexBuffer->CopyToBuffer(readBack));
			stream->Schedule(readBack->CreateReadTask(readBackVec.data(), readBack->GetSettings().byteLength, 0));
		}

		assert(std::equal(vertexVec.cbegin(), vertexVec.cend(), readBackVec.cbegin()));
	}

    void App::Test_Structured_Buffers()
    {
		const auto stream = GetDefaultStream();

		std::vector<MMPEngine::Core::Vector2Uint> uploadVec(_vecSize, MMPEngine::Core::Vector2Uint{});
		std::vector<MMPEngine::Core::Vector2Uint> readBackVec(_vecSize, MMPEngine::Core::Vector2Uint{});


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

			for (std::size_t i = 0; i < uploadVec.size(); ++i)
			{
				stream->Schedule(uploadBuffer->CreateWriteStructTask(uploadVec[i], i));
			}

			stream->Schedule(uploadBuffer->CopyToBuffer(resident));
			stream->Schedule(resident->CopyToBuffer(readBack));

			for (std::size_t i = 0; i < uploadVec.size(); ++i)
			{
				stream->Schedule(readBack->CreateReadStructTask(readBackVec[i], i));
			}
		}

		assert(std::equal(uploadVec.cbegin(), uploadVec.cend(), readBackVec.cbegin()));
    }

	void App::Test_UniformBufferTo_ReadBack()
	{
		const auto stream = GetDefaultStream();

		const auto uniformBuffer1 = std::make_shared<MMPEngine::Frontend::UniformBuffer<TestStruct>>(GetContext(), "test_uniform_buffer1");
		const auto uniformBuffer2 = std::make_shared<MMPEngine::Frontend::UniformBuffer<TestStruct>>(GetContext(), "test_uniform_buffer2");

		const auto readBackBuffer = std::make_shared<MMPEngine::Frontend::ReadBackBuffer>(
			GetContext(),
			MMPEngine::Core::Buffer::Settings{
			sizeof(TestStruct),
				"test_read_back_buffer"
		}
		);

		constexpr TestStruct expected1 {
	{1,2,3,4},
	{5,6,7,8}
		};

		constexpr TestStruct expected2{
	{10,20,30,40},
	{50,60,70,80}
		};

		TestStruct actual{};

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(uniformBuffer1->CreateInitializationTask());
			stream->Schedule(uniformBuffer2->CreateInitializationTask());
			stream->Schedule(readBackBuffer->CreateInitializationTask());
		}

		{
			const auto executor = stream->CreateExecutor();

			const auto uniformWriteTask1 = uniformBuffer1->CreateWriteAsyncTask({});
			const auto uniformWriteTask2 = uniformBuffer2->CreateWriteAsyncTask({});

			uniformWriteTask1->GetTaskContext()->data = expected1;
			uniformWriteTask2->GetTaskContext()->data = expected2;

			stream->Schedule(uniformWriteTask1);
			stream->Schedule(uniformWriteTask2);

		}

		{
			const auto executor = stream->CreateExecutor();

			stream->Schedule(uniformBuffer1->CopyToBuffer(readBackBuffer));
			stream->Schedule(readBackBuffer->CreateReadTask(&actual, sizeof(actual), 0));
		}

		assert(std::memcmp(&expected1, &actual, sizeof(expected1)) == 0);

		{
			const auto executor = stream->CreateExecutor();

			stream->Schedule(uniformBuffer2->CopyToBuffer(readBackBuffer));
			stream->Schedule(readBackBuffer->CreateReadTask(&actual, sizeof(actual), 0));
		}

		assert(std::memcmp(&expected2, &actual, sizeof(expected2)) == 0);
	}

}