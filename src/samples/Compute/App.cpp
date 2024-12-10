#include <Frontend/App.hpp>
#include <Compute/App.hpp>
#include <Frontend/Shader.hpp>
#include <Frontend/Material.hpp>
#include <Frontend/Compute.hpp>
#include <Frontend/Buffer.hpp>

namespace Sample::Compute
{
	App::App() = default;

	void App::Initialize()
	{
		UserApp::Initialize();

		Test_SetThreadId();
	}

	void App::Test_SetThreadId()
	{
		const auto computeShader = MMPEngine::Frontend::Shader::LoadFromFile<MMPEngine::Core::ComputeShader>(GetContext(), std::filesystem::path("Compute_SetThreadId.json"));
		const auto material = std::make_shared<MMPEngine::Frontend::ComputeMaterial>(GetContext(), computeShader);
		const auto computeJob = std::make_shared<MMPEngine::Frontend::DirectComputeJob>(GetContext(), material);

		const auto stream = GetDefaultStream();


		std::vector<std::size_t> expectedVec(_vecSize, 0);

		for (std::size_t i = 0; i < expectedVec.size(); ++i)
		{
			expectedVec[i] = i;
		}

		std::vector<std::uint32_t> readVec(_vecSize, 0);
		constexpr std::size_t byteLength = _vecSize * sizeof(decltype(readVec)::value_type);

		const auto uaBuffer = std::make_shared<MMPEngine::Frontend::UnorderedAccessBuffer>(
			GetContext(),
			MMPEngine::Core::BaseUnorderedAccessBuffer::Settings{
			sizeof(decltype(readVec)::value_type),
				_vecSize,
				"test_ua_buffer"
		}
		);

		const auto readBackBuffer = std::make_shared<MMPEngine::Frontend::ReadBackBuffer>(
			GetContext(),
			MMPEngine::Core::Buffer::Settings{
			byteLength,
				"test_read_back_buffer"
		}
		);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(computeShader->CreateInitializationTask());
			stream->Schedule(material->CreateInitializationTask());

			stream->Schedule(uaBuffer->CreateInitializationTask());
			stream->Schedule(readBackBuffer->CreateInitializationTask());

			MMPEngine::Core::BaseMaterial::Parameters params {
				std::vector {
					MMPEngine::Core::BaseMaterial::Parameters::Entry {
						"test_buffer",
							uaBuffer,
							MMPEngine::Core::BaseMaterial::Parameters::Buffer {
								MMPEngine::Core::BaseMaterial::Parameters::Buffer::Type::UnorderedAccess
							}
					}
				}
			};
			stream->Schedule(material->CreateTaskForUpdateParameters(std::move(params)));
			stream->Schedule(computeJob->CreateInitializationTask());

		}

		{
			const auto executor = stream->CreateExecutor();

			const auto computeExecutionTask = computeJob->CreateExecutionTask();
			computeExecutionTask->GetContext()->dimensions = {_vecSize, 1, 1};

			stream->Schedule(computeExecutionTask);
			stream->Schedule(uaBuffer->CopyToBuffer(readBackBuffer));
			stream->Schedule(readBackBuffer->CreateReadTask(readVec.data(), byteLength, 0));
		}

		//assert(std::equal(expectedVec.cbegin(), expectedVec.cend(), readVec.cbegin()));
	}


}
