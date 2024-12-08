#include <Frontend/App.hpp>
#include <Compute/App.hpp>
#include <Frontend/Shader.hpp>
#include <Frontend/Material.hpp>

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

		const auto stream = GetDefaultStream();

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(computeShader->CreateInitializationTask());
			stream->Schedule(material->CreateInitializationTask());

			MMPEngine::Core::BaseMaterial::Parameters params { std::vector<MMPEngine::Core::BaseMaterial::Parameters::Entry> {} };
			stream->Schedule(material->CreateTaskForUpdateParameters(std::move(params)));
		}
	}


}
