#include <Boxes/App.hpp>
#include <Frontend/Shader.hpp>
#include <Core/Material.hpp>
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

		auto matSettings = MMPEngine::Core::RenderingMaterial::Settings{};
		matSettings.fillMode = MMPEngine::Core::RenderingMaterial::Settings::FillMode::WireFrame;
		matSettings.cullMode = MMPEngine::Core::RenderingMaterial::Settings::CullMode::Back;

		_viewportIndependentData->materialData = std::make_tuple(matSettings, vs, ps);

		auto boxProto = MMPEngine::Frontend::Geometry::Generate<MMPEngine::Frontend::Geometry::PrimitiveType::Box>();
		_viewportIndependentData->mesh = std::make_shared<MMPEngine::Frontend::Mesh>(globalContext, std::move(boxProto));

		const auto meshRendererNode = std::make_shared<MMPEngine::Core::Node>();

		meshRendererNode->localTransform.position.x += 2.5f;
		meshRendererNode->localTransform.position.y += 1.5f;
		meshRendererNode->localTransform.position.z += 1.0f;

		_viewportIndependentData->meshRenderer = std::make_shared<MMPEngine::Frontend::Mesh::Renderer>(
			globalContext, 
			MMPEngine::Core::Mesh::Renderer::Settings {
				{true},
				{1}
			}, 
			_viewportIndependentData->mesh, 
			meshRendererNode
		);

		_viewportIndependentData->cameraNode = std::make_shared<MMPEngine::Core::Node>();
		_viewportIndependentData->cameraNode->localTransform.position = { 0.0f, 0.0f, -5.0f };

		_viewportIndependentData->cameraMovement = std::make_unique<MMPEngine::Feature::Movement::FreeController>(
			MMPEngine::Feature::Movement::FreeController::Settings {},
			globalContext,
			GetInput().get(),
			_viewportIndependentData->cameraNode
		);

		{
			const auto executor = stream->CreateExecutor();

			stream->Schedule(vs->CreateInitializationTask());
			stream->Schedule(ps->CreateInitializationTask());

			stream->Schedule(_viewportIndependentData->mesh->CreateInitializationTask());
			stream->Schedule(_viewportIndependentData->meshRenderer->CreateInitializationTask());
		}

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(_viewportIndependentData->meshRenderer->CreateTaskToUpdateAndWriteUniformData());
		}
	}

	void App::OnNativeWindowUpdated()
	{
		UserApp::OnNativeWindowUpdated();

		_viewportDependentData = nullptr;
		_viewportDependentData = std::make_unique<ViewportDependentData>();

		const auto stream = GetDefaultStream();
		const auto globalContext = GetContext();

		_viewportDependentData->screen = std::make_shared<MMPEngine::Frontend::Screen>(globalContext, MMPEngine::Core::Screen::Settings {
			1, 2, true, MMPEngine::Core::Vector4Float {0.0f, 0.0f, 0.0125f, 0.0f }
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

		_viewportDependentData->screenStartFrameTask = _viewportDependentData->screen->CreateStartFrameTask();
		_viewportDependentData->screenPresentTask = _viewportDependentData->screen->CreatePresentationTask();

		_viewportDependentData->camera = std::make_shared<MMPEngine::Frontend::PerspectiveCamera>(
			globalContext,
			MMPEngine::Core::PerspectiveCamera::Settings {{}, {}},
			_viewportIndependentData->cameraNode,
			MMPEngine::Core::Camera::Target {
				{ {_viewportDependentData->screen->GetBackBuffer(), true} },
				{_viewportDependentData->depthStencilTexture, true }
			}
		);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(_viewportDependentData->camera->CreateInitializationTask());
		}

		auto materialParams = MMPEngine::Core::BaseMaterial::Parameters{
			std::vector {
				MMPEngine::Core::BaseMaterial::Parameters::Entry {
					"camera_data",
						"scene",
						_viewportDependentData->camera->GetUniformDataEntity(),
						MMPEngine::Core::BaseMaterial::Parameters::Buffer {
						MMPEngine::Core::BaseMaterial::Parameters::Buffer::Type::Uniform
					}
				},
				MMPEngine::Core::BaseMaterial::Parameters::Entry {
				"mesh_data",
					"object",
						_viewportIndependentData->meshRenderer->GetUniformDataEntity(),
						MMPEngine::Core::BaseMaterial::Parameters::Buffer {
						MMPEngine::Core::BaseMaterial::Parameters::Buffer::Type::Uniform
					}
				}
			}
		};

		const auto material = std::make_shared<MMPEngine::Core::MeshMaterial>(
			std::get<0>(_viewportIndependentData->materialData),
			std::move(materialParams),
			std::get<1>(_viewportIndependentData->materialData),
			std::get<2>(_viewportIndependentData->materialData)
		);


		_viewportDependentData->renderJob = std::make_shared<MMPEngine::Frontend::Camera::DrawCallsJob>(
			globalContext,
			_viewportDependentData->camera,
			std::vector<MMPEngine::Core::Camera::DrawCallsJob::Item>{
				MMPEngine::Core::Camera::DrawCallsJob::Item{
					_viewportIndependentData->meshRenderer,
					material
				}
			}
		);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(_viewportDependentData->renderJob->CreateInitializationTask());
		}

		_viewportDependentData->updateCameraTask = _viewportDependentData->camera->CreateTaskToUpdateUniformData();
		_viewportDependentData->renderJobExecutionTask = _viewportDependentData->renderJob->CreateExecutionTask();

	}

	void App::OnUpdate(std::float_t dt)
	{
		UserApp::OnUpdate(dt);
		_viewportIndependentData->cameraMovement->Update(dt);
	}

	void App::OnRender()
	{
		UserApp::OnRender();

		const auto stream = GetDefaultStream();

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(_viewportDependentData->screenStartFrameTask);
			stream->Schedule(_viewportDependentData->updateCameraTask);
			stream->Schedule(_viewportDependentData->renderJobExecutionTask);
			stream->Schedule(_viewportDependentData->screenPresentTask);
		}
	}
}
