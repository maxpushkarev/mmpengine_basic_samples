#include <cassert>
#include <Compute/App.hpp>
#include <Frontend/Shader.hpp>
#include <Core/Material.hpp>
#include <Frontend/Compute.hpp>
#include <Frontend/Buffer.hpp>

namespace Sample::Compute
{
	App::App(const std::shared_ptr<MMPEngine::Feature::BaseLogger>& logger) : UserApp(logger)
	{
	}

	void App::Initialize()
	{
		UserApp::Initialize();

		Test_SetValue();
		//Test_Filter();
	}

	void App::Test_SetValue()
	{
		const auto uniform1 = std::make_shared<MMPEngine::Frontend::UniformBuffer<TestUniform>>(GetContext());
		const auto uniform2 = std::make_shared<MMPEngine::Frontend::UniformBuffer<TestUniform>>(GetContext());

		const auto computeShader = MMPEngine::Frontend::Shader::LoadFromFile<MMPEngine::Core::ComputeShader>(GetContext(), std::filesystem::path("Compute_SetValue.json"));
		const auto stream = GetDefaultStream();

		std::vector<std::uint32_t> expectedVec(_vecSize, 0);

		for (std::size_t i = 0; i < expectedVec.size(); ++i)
		{
			expectedVec[i] = static_cast<std::uint32_t>(i) * 100 + 10;
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


		MMPEngine::Core::BaseMaterial::Parameters params {
			std::vector {
				MMPEngine::Core::BaseMaterial::Parameters::Entry {
					"test_buffer",
						"Buffer",
						uaBuffer,
						MMPEngine::Core::BaseMaterial::Parameters::Buffer {
						MMPEngine::Core::BaseMaterial::Parameters::Buffer::Type::UnorderedAccess
					}
				},
				MMPEngine::Core::BaseMaterial::Parameters::Entry {
				"test_uniform1",
					"uniform",
						uniform1,
						MMPEngine::Core::BaseMaterial::Parameters::Buffer {
						MMPEngine::Core::BaseMaterial::Parameters::Buffer::Type::Uniform
					}
				},
				MMPEngine::Core::BaseMaterial::Parameters::Entry {
				"test_uniform2",
					"uniform",
						uniform2,
						MMPEngine::Core::BaseMaterial::Parameters::Buffer {
						MMPEngine::Core::BaseMaterial::Parameters::Buffer::Type::Uniform
					}
				}
			}
		};
		const auto material = std::make_shared<MMPEngine::Core::ComputeMaterial>(std::move(params), computeShader);
		/*const auto computeJob = std::make_shared<MMPEngine::Frontend::DirectComputeJob>(GetContext(), material);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(computeShader->CreateInitializationTask());
			stream->Schedule(uniform1->CreateInitializationTask());
			stream->Schedule(uniform2->CreateInitializationTask());
			stream->Schedule(uaBuffer->CreateInitializationTask());

			stream->Schedule(readBackBuffer->CreateInitializationTask());
			stream->Schedule(computeJob->CreateInitializationTask());
		}

		{
			const auto executor = stream->CreateExecutor();

			stream->Schedule(uniform1->CreateWriteAsyncTask({100}));
			stream->Schedule(uniform2->CreateWriteAsyncTask({10}));

			const auto computeExecutionTask = computeJob->CreateExecutionTask();
			computeExecutionTask->GetTaskContext()->dimensions = {1, 1, 1};

			stream->Schedule(computeExecutionTask);
			stream->Schedule(uaBuffer->CopyToBuffer(readBackBuffer));
			stream->Schedule(readBackBuffer->CreateReadTask(readVec.data(), byteLength, 0));
		}

		assert(std::equal(expectedVec.cbegin(), expectedVec.cend(), readVec.cbegin()));*/
	}

	void App::Test_Filter()
	{
		const auto globalContext = GetContext();
		const auto stream = GetDefaultStream();

		const auto computeShaderPositive = MMPEngine::Frontend::Shader::LoadFromFile<MMPEngine::Core::ComputeShader>(globalContext, std::filesystem::path("Compute_Filter_Positive.json"));
		const auto computeShaderNegative = MMPEngine::Frontend::Shader::LoadFromFile<MMPEngine::Core::ComputeShader>(globalContext, std::filesystem::path("Compute_Filter_Negative.json"));
		

		std::vector<std::int32_t> inputVec(_vecSize, 0);
		std::vector<std::int32_t> filteredPositive {};
		std::vector<std::int32_t> filteredNegative {};
		MMPEngine::Core::CounteredUnorderedAccessBuffer::CounterValueType counterValuePositive = 123;
		MMPEngine::Core::CounteredUnorderedAccessBuffer::CounterValueType counterValueNegative= 123;

		for (std::int32_t i = 0; i < static_cast<std::int32_t>(inputVec.size()); ++i)
		{
			inputVec[i] = (i % 3 == 0) ? (i + 1) : -(i + 1);
		}

		std::copy_if(inputVec.cbegin(), inputVec.cend(),
			std::back_inserter(filteredPositive),
			[](const auto v) { return v > 0; }
		);

		std::copy_if(inputVec.cbegin(), inputVec.cend(),
			std::back_inserter(filteredNegative),
			[](const auto v) { return v < 0; }
		);

		std::sort(filteredNegative.begin(), filteredNegative.end());
		std::sort(filteredPositive.begin(), filteredPositive.end());
		std::vector<std::int32_t> outputVecPositive(_vecSize, 0);
		std::vector<std::int32_t> outputVecNegative(_vecSize, 0);

		constexpr std::size_t byteLength = _vecSize * sizeof(decltype(outputVecPositive)::value_type);

		const auto uploadBufferPositive = std::make_shared<MMPEngine::Frontend::UploadBuffer>(
			globalContext,
			MMPEngine::Core::Buffer::Settings{
			byteLength,
				"test_upload_buffer_positive"
		}
		);

		const auto uaBufferPositive = std::make_shared<MMPEngine::Frontend::CounteredUnorderedAccessBuffer>(
			globalContext,
			MMPEngine::Core::BaseUnorderedAccessBuffer::Settings{
			sizeof(decltype(inputVec)::value_type),
				_vecSize,
				"test_ua_buffer_positive"
		}
		);


		const auto uploadBufferNegative = std::make_shared<MMPEngine::Frontend::UploadBuffer>(
			globalContext,
			MMPEngine::Core::Buffer::Settings{
			byteLength,
				"test_upload_buffer_negative"
		}
		);

		const auto uaBufferNegative = std::make_shared<MMPEngine::Frontend::CounteredUnorderedAccessBuffer>(
			globalContext,
			MMPEngine::Core::BaseUnorderedAccessBuffer::Settings{
			sizeof(decltype(inputVec)::value_type),
				_vecSize,
				"test_ua_buffer_negative"
		}
		);

		const auto readBackBufferPositive = std::make_shared<MMPEngine::Frontend::ReadBackBuffer>(
			globalContext,
			MMPEngine::Core::Buffer::Settings{
			byteLength,
				"test_read_back_buffer_positive"
		}
		);

		const auto counterReadBackPositive = std::make_shared<MMPEngine::Frontend::ReadBackBuffer>(globalContext,
			MMPEngine::Core::Buffer::Settings{
			sizeof(counterValuePositive),
				"test_counter_read_back_buffer_positive"
		});

		const auto readBackBufferNegative = std::make_shared<MMPEngine::Frontend::ReadBackBuffer>(
			globalContext,
			MMPEngine::Core::Buffer::Settings{
			byteLength,
				"test_read_back_buffer_negative"
		}
		);

		const auto counterReadBackNegative = std::make_shared<MMPEngine::Frontend::ReadBackBuffer>(globalContext,
			MMPEngine::Core::Buffer::Settings{
			sizeof(counterValuePositive),
				"test_counter_read_back_buffer_negative"
		});


		MMPEngine::Core::BaseMaterial::Parameters paramsPositive {
			std::vector {
				MMPEngine::Core::BaseMaterial::Parameters::Entry {
					"input",
					"set",
						uploadBufferPositive,
						MMPEngine::Core::BaseMaterial::Parameters::Buffer {
						MMPEngine::Core::BaseMaterial::Parameters::Buffer::Type::ReadonlyAccess
					}
				},
					MMPEngine::Core::BaseMaterial::Parameters::Entry {
					"output",
						"set",
						uaBufferPositive,
						MMPEngine::Core::BaseMaterial::Parameters::Buffer {
						MMPEngine::Core::BaseMaterial::Parameters::Buffer::Type::UnorderedAccess
					}
				}
			}
		};


		MMPEngine::Core::BaseMaterial::Parameters paramsNegative {
			std::vector {
				MMPEngine::Core::BaseMaterial::Parameters::Entry {
					"input",
					"set",
						uploadBufferNegative,
						MMPEngine::Core::BaseMaterial::Parameters::Buffer {
						MMPEngine::Core::BaseMaterial::Parameters::Buffer::Type::ReadonlyAccess
					}
				},
					MMPEngine::Core::BaseMaterial::Parameters::Entry {
					"output",
						"set",
						uaBufferNegative,
						MMPEngine::Core::BaseMaterial::Parameters::Buffer {
						MMPEngine::Core::BaseMaterial::Parameters::Buffer::Type::UnorderedAccess
					}
				}
			}
		};

		const auto materialPositive = std::make_shared<MMPEngine::Core::ComputeMaterial>(std::move(paramsPositive), computeShaderPositive);
		const auto computeJobPositive = std::make_shared<MMPEngine::Frontend::DirectComputeJob>(globalContext, materialPositive);

		const auto materialNegative = std::make_shared<MMPEngine::Core::ComputeMaterial>(std::move(paramsNegative), computeShaderNegative);
		const auto computeJobNegative = std::make_shared<MMPEngine::Frontend::DirectComputeJob>(globalContext, materialNegative);
		{
			const auto executor = stream->CreateExecutor();

			stream->Schedule(uploadBufferPositive->CreateInitializationTask());
			stream->Schedule(uaBufferPositive->CreateInitializationTask());
			stream->Schedule(uploadBufferNegative->CreateInitializationTask());
			stream->Schedule(uaBufferNegative->CreateInitializationTask());

			stream->Schedule(computeShaderPositive->CreateInitializationTask());
			stream->Schedule(computeJobPositive->CreateInitializationTask());

			stream->Schedule(computeShaderNegative->CreateInitializationTask());
			stream->Schedule(computeJobNegative->CreateInitializationTask());

			stream->Schedule(readBackBufferPositive->CreateInitializationTask());
			stream->Schedule(counterReadBackPositive->CreateInitializationTask());

			stream->Schedule(readBackBufferNegative->CreateInitializationTask());
			stream->Schedule(counterReadBackNegative->CreateInitializationTask());

		}


		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(uaBufferPositive->CreateCopyCounterTask(counterReadBackPositive, sizeof(counterValuePositive), 0));
			stream->Schedule(counterReadBackPositive->CreateReadTask(&counterValuePositive, sizeof(counterValuePositive), 0));

			stream->Schedule(uaBufferNegative->CreateCopyCounterTask(counterReadBackNegative, sizeof(counterValueNegative), 0));
			stream->Schedule(counterReadBackNegative->CreateReadTask(&counterValueNegative, sizeof(counterValueNegative), 0));
		}

		assert(counterValuePositive == 0);
		assert(counterValueNegative == 0);

		{
			const auto executor = stream->CreateExecutor();

			stream->Schedule(uploadBufferPositive->CreateWriteTask(inputVec.data(), byteLength, 0));
			stream->Schedule(uploadBufferNegative->CreateWriteTask(inputVec.data(), byteLength, 0));

			const auto computePositiveExecutionTask = computeJobPositive->CreateExecutionTask();
			computePositiveExecutionTask->GetTaskContext()->dimensions = { 1, 1, 1 };
			stream->Schedule(computePositiveExecutionTask);

			const auto computeNegativeExecutionTask = computeJobNegative->CreateExecutionTask();
			computeNegativeExecutionTask->GetTaskContext()->dimensions = { 1, 1, 1 };
			stream->Schedule(computeNegativeExecutionTask);


			stream->Schedule(uaBufferPositive->CopyToBuffer(readBackBufferPositive));
			stream->Schedule(readBackBufferPositive->CreateReadTask(outputVecPositive.data(), byteLength, 0));

			stream->Schedule(uaBufferPositive->CreateCopyCounterTask(counterReadBackPositive, sizeof(counterValuePositive), 0));
			stream->Schedule(counterReadBackPositive->CreateReadTask(&counterValuePositive, sizeof(counterValuePositive), 0));


			stream->Schedule(uaBufferNegative->CopyToBuffer(readBackBufferNegative));
			stream->Schedule(readBackBufferNegative->CreateReadTask(outputVecNegative.data(), byteLength, 0));

			stream->Schedule(uaBufferNegative->CreateCopyCounterTask(counterReadBackNegative, sizeof(counterValueNegative), 0));
			stream->Schedule(counterReadBackNegative->CreateReadTask(&counterValueNegative, sizeof(counterValueNegative), 0));
		}


		assert(counterValuePositive == static_cast<decltype(counterValuePositive)>(filteredPositive.size()));

		auto tail = outputVecPositive.begin();
		std::advance(tail, counterValuePositive);
		outputVecPositive.erase(tail, outputVecPositive.end());

		std::sort(outputVecPositive.begin(), outputVecPositive.end());
		assert(std::equal(filteredPositive.cbegin(), filteredPositive.cend(), outputVecPositive.cbegin()));


		assert(counterValueNegative == static_cast<decltype(counterValueNegative)>(filteredNegative.size()));

		tail = outputVecNegative.begin();
		std::advance(tail, counterValueNegative);
		outputVecNegative.erase(tail, outputVecNegative.end());

		std::sort(outputVecNegative.begin(), outputVecNegative.end());
		assert(std::equal(filteredNegative.cbegin(), filteredNegative.cend(), outputVecNegative.cbegin()));


		{
			const auto executor = stream->CreateExecutor();

			stream->Schedule(uaBufferNegative->CreateResetCounterTask());

			stream->Schedule(uaBufferNegative->CopyToBuffer(readBackBufferNegative));
			stream->Schedule(uaBufferNegative->CreateCopyCounterTask(counterReadBackNegative, sizeof(counterValueNegative), 0));
			stream->Schedule(counterReadBackNegative->CreateReadTask(&counterValueNegative, sizeof(counterValueNegative), 0));
			stream->Schedule(readBackBufferNegative->CreateReadTask(outputVecNegative.data(), sizeof(decltype(outputVecNegative)::value_type) * outputVecNegative.size(), 0));

			stream->Schedule(uaBufferPositive->CopyToBuffer(readBackBufferPositive));
			stream->Schedule(uaBufferPositive->CreateCopyCounterTask(counterReadBackPositive, sizeof(counterValuePositive), 0));
			stream->Schedule(counterReadBackPositive->CreateReadTask(&counterValuePositive, sizeof(counterValuePositive), 0));
			stream->Schedule(readBackBufferPositive->CreateReadTask(outputVecPositive.data(), sizeof(decltype(outputVecPositive)::value_type)* outputVecPositive.size(), 0));
		}

		assert(counterValuePositive == static_cast<decltype(counterValuePositive)>(filteredPositive.size()));
		std::sort(outputVecPositive.begin(), outputVecPositive.end());
		assert(std::equal(filteredPositive.cbegin(), filteredPositive.cend(), outputVecPositive.cbegin()));


		assert(counterValueNegative == 0);
		std::sort(outputVecNegative.begin(), outputVecNegative.end());
		assert(std::equal(filteredNegative.cbegin(), filteredNegative.cend(), outputVecNegative.cbegin()));
	}

}
