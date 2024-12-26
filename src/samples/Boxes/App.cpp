#include <Boxes/App.hpp>
#include <Frontend/Shader.hpp>
#include <Frontend/Material.hpp>

namespace Sample::Boxes
{
	App::App(const std::shared_ptr<MMPEngine::Feature::BaseLogger>& logger) : UserApp(logger)
	{
	}

	void App::Initialize()
	{
		MMPEngine::Feature::UserApp::Initialize();

		const auto globalContext = GetContext();
		const auto stream = GetDefaultStream();

		const auto vs = MMPEngine::Frontend::Shader::LoadFromFile<MMPEngine::Core::VertexShader>(
			globalContext, std::filesystem::path("Vertex_Test.json")
		);

		const auto ps = MMPEngine::Frontend::Shader::LoadFromFile<MMPEngine::Core::PixelShader>(
			globalContext, std::filesystem::path("Pixel_Test.json")
		);

		const auto matSettings = MMPEngine::Core::RenderingMaterial::Settings {};
		_boxMaterial = std::make_shared<MMPEngine::Frontend::MeshMaterial>(globalContext, matSettings, vs, ps);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(vs->CreateInitializationTask());
			stream->Schedule(ps->CreateInitializationTask());
			stream->Schedule(_boxMaterial->CreateInitializationTask());
		}
	}

}
