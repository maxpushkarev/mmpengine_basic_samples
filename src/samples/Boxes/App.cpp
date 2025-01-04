#include <Boxes/App.hpp>
#include <Frontend/Shader.hpp>
#include <Frontend/Material.hpp>
#include <Frontend/Geometry.hpp>
#include <Core/Node.hpp>

namespace Sample::Boxes
{
	App::App(const std::shared_ptr<MMPEngine::Feature::BaseLogger>& logger) : UserApp(logger)
	{
	}

	void App::Initialize()
	{
		MMPEngine::Feature::UserApp::Initialize();

		_viewportIndependentData = std::make_unique<ViewportIndependentData>();

		const auto globalContext = GetContext();
		const auto stream = GetDefaultStream();


		const auto vs = MMPEngine::Frontend::Shader::LoadFromFile<MMPEngine::Core::VertexShader>(
			globalContext, std::filesystem::path("Vertex_Test.json")
		);

		const auto ps = MMPEngine::Frontend::Shader::LoadFromFile<MMPEngine::Core::PixelShader>(
			globalContext, std::filesystem::path("Pixel_Test.json")
		);

		auto matSettings = MMPEngine::Core::RenderingMaterial::Settings {};
		matSettings.fillMode = MMPEngine::Core::RenderingMaterial::Settings::FillMode::WireFrame;

		_viewportIndependentData->material = std::make_shared<MMPEngine::Frontend::MeshMaterial>(globalContext, matSettings, vs, ps);

		auto boxProto = MMPEngine::Frontend::Geometry::Generate<MMPEngine::Frontend::Geometry::PrimitiveType::Box>();
		_viewportIndependentData->mesh = std::make_shared<MMPEngine::Frontend::Mesh>(globalContext, std::move(boxProto));

		_viewportIndependentData->renderer = std::make_shared<MMPEngine::Frontend::Mesh::Renderer>(
			globalContext, 
			MMPEngine::Core::Mesh::Renderer::Settings {
				{true},
				{1}
			}, 
			_viewportIndependentData->mesh, 
			std::make_shared<MMPEngine::Core::Node>()
		);

		_viewportIndependentData->cameraNode = std::make_shared<MMPEngine::Core::Node>();
		_viewportIndependentData->cameraNode->localTransform.position = { 0.0f, 0.0f, -5.0f };

		_viewportIndependentData->camera = std::make_shared<MMPEngine::Frontend::PerspectiveCamera>(
			globalContext,
			MMPEngine::Core::PerspectiveCamera::Settings {{}, {}},
			_viewportIndependentData->cameraNode
		);

		{
			const auto executor = stream->CreateExecutor();

			stream->Schedule(vs->CreateInitializationTask());
			stream->Schedule(ps->CreateInitializationTask());

			stream->Schedule(_viewportIndependentData->material->CreateInitializationTask());
			stream->Schedule(_viewportIndependentData->mesh->CreateInitializationTask());
			stream->Schedule(_viewportIndependentData->renderer->CreateInitializationTask());
			stream->Schedule(_viewportIndependentData->camera->CreateInitializationTask());
		}

		_viewportIndependentData->updateRendererTask = _viewportIndependentData->renderer->CreateTaskToUpdateAndWriteUniformData();
		_viewportIndependentData->updateCameraTask = _viewportIndependentData->camera->CreateTaskToUpdateUniformData();

	}

	void App::OnNativeWindowUpdated()
	{
		UserApp::OnNativeWindowUpdated();

		_viewportDependentData = nullptr;
		_viewportDependentData = std::make_unique<ViewportDependentData>();

		const auto stream = GetDefaultStream();
		const auto globalContext = GetContext();

		_viewportDependentData->screen = std::make_shared<MMPEngine::Frontend::Screen>(globalContext, MMPEngine::Core::Screen::Settings {
			1, 2, true, MMPEngine::Core::Vector4Float {1.0f, 1.0f, 1.0f, 1.0f }
		});

		_viewportDependentData->depthStencilTexture = std::make_shared<MMPEngine::Frontend::DepthStencilTargetTexture>(
			globalContext, 
			MMPEngine::Core::DepthStencilTargetTexture::Settings {
				MMPEngine::Core::DepthStencilTargetTexture::Settings::Format::Depth24_Stencil8,
					std::make_tuple(1.0f, 0),
				{MMPEngine::Core::TargetTexture::Settings::Antialiasing::MSAA_0, globalContext->windowSize, "depth/stencil"}
			});


		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(_viewportDependentData->screen->CreateInitializationTask());
			stream->Schedule(_viewportDependentData->depthStencilTexture->CreateInitializationTask());
		}

		_viewportDependentData->screenSwapTask = _viewportDependentData->screen->CreateTaskToSwapBuffer();
	}

	void App::OnUpdate(std::float_t dt)
	{
		UserApp::OnUpdate(dt);
	}

	void App::OnRender()
	{
		UserApp::OnRender();

		const auto stream = GetDefaultStream();

		{
			const auto executor = stream->CreateExecutor();

			stream->Schedule(_viewportIndependentData->updateRendererTask);
			stream->Schedule(_viewportIndependentData->updateCameraTask);


			stream->Schedule(_viewportDependentData->screenSwapTask);
		}
	}
}
