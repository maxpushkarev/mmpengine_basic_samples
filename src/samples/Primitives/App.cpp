#include <Primitives/App.hpp>
#include <Frontend/Shader.hpp>
#include <Frontend/Material.hpp>
#include <Frontend/Geometry.hpp>
#include <Core/Node.hpp>

namespace Sample::Primitives
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

		_viewportIndependentData->shaderPack = std::make_shared<MMPEngine::Frontend::ShaderPack>(globalContext, std::filesystem::path("Primitives.json"));

		const auto vs = _viewportIndependentData->shaderPack->Unpack("VertexTest");
		const auto ps = _viewportIndependentData->shaderPack->Unpack("PixelTest");

		auto matSettings = MMPEngine::Core::RenderingMaterial::Settings{};
		matSettings.fillMode = MMPEngine::Core::RenderingMaterial::Settings::FillMode::WireFrame;
		matSettings.cullMode = MMPEngine::Core::RenderingMaterial::Settings::CullMode::Back;

		_viewportIndependentData->materialData = std::make_tuple(matSettings, vs, ps);

		auto boxProto = MMPEngine::Frontend::Geometry::Generate<MMPEngine::Frontend::Geometry::PrimitiveType::Box>();
		auto quadProto = MMPEngine::Frontend::Geometry::Generate<MMPEngine::Frontend::Geometry::PrimitiveType::Quad>();

		const auto boxMesh = std::make_shared<MMPEngine::Frontend::Mesh>(globalContext, std::move(boxProto));
		const auto quadMesh = std::make_shared<MMPEngine::Frontend::Mesh>(globalContext, std::move(quadProto));

		const auto boxMeshRendererNode = std::make_shared<MMPEngine::Core::Node>();
		const auto quadMeshRendererNode = std::make_shared<MMPEngine::Core::Node>();

		boxMeshRendererNode->localTransform.position.x += 2.5f;
		boxMeshRendererNode->localTransform.position.y += 1.5f;
		boxMeshRendererNode->localTransform.position.z += 1.0f;

		const auto boxRenderer = std::make_shared<MMPEngine::Frontend::Mesh::Renderer>(
			globalContext, 
			MMPEngine::Core::Mesh::Renderer::Settings {
				{true},
				{1}
			}, 
			boxMesh, 
			boxMeshRendererNode
		);

		const auto quadRenderer = std::make_shared<MMPEngine::Frontend::Mesh::Renderer>(
			globalContext,
			MMPEngine::Core::Mesh::Renderer::Settings{
				{
					true,
					std::vector<MMPEngine::Core::GeometryPrototype::VertexAttribute> {
						{MMPEngine::Core::VertexBufferPrototype::Semantics::Position, 0 }
					}
				},
				{1}
			},
			quadMesh,
			quadMeshRendererNode
		);

		_viewportIndependentData->meshRenderers.push_back(boxRenderer);
		_viewportIndependentData->meshRenderers.push_back(quadRenderer);

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

			stream->Schedule(_viewportIndependentData->shaderPack->CreateInitializationTask());
			stream->Schedule(vs->CreateInitializationTask());
			stream->Schedule(ps->CreateInitializationTask());

			for (const auto& mr : _viewportIndependentData->meshRenderers)
			{
				stream->Schedule(mr->GetMesh()->CreateInitializationTask());
			}

			for (const auto& mr : _viewportIndependentData->meshRenderers)
			{
				stream->Schedule(mr->CreateInitializationTask());
			}
		}

		{
			const auto executor = stream->CreateExecutor();
			for (const auto& mr : _viewportIndependentData->meshRenderers)
			{
				stream->Schedule(mr->CreateTaskToUpdateAndWriteUniformData());
			}
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

		auto drawItems = std::vector<MMPEngine::Core::Camera::DrawCallsJob::Item> {};

		for (const auto& mr : _viewportIndependentData->meshRenderers)
		{
			auto materialParams = MMPEngine::Core::BaseMaterial::Parameters{
			std::vector {
				MMPEngine::Core::BaseMaterial::Parameters::Entry {
					"camera_data",
						_viewportDependentData->camera->GetUniformDataEntity(),
						MMPEngine::Core::BaseMaterial::Parameters::Buffer {
						MMPEngine::Core::BaseMaterial::Parameters::Buffer::Type::Uniform
					}
				},
				MMPEngine::Core::BaseMaterial::Parameters::Entry {
				"mesh_data",
						mr->GetUniformDataEntity(),
						MMPEngine::Core::BaseMaterial::Parameters::Buffer {
						MMPEngine::Core::BaseMaterial::Parameters::Buffer::Type::Uniform
					}
				}
			}
			};

			const auto material = std::make_shared<MMPEngine::Frontend::MeshMaterial>(
                globalContext,
				std::get<0>(_viewportIndependentData->materialData),
				std::move(materialParams),
				std::get<1>(_viewportIndependentData->materialData),
				std::get<2>(_viewportIndependentData->materialData)
			);

            {
                const auto executor = stream->CreateExecutor();
                stream->Schedule(material->CreateInitializationTask());
            }
            
			drawItems.push_back(MMPEngine::Core::Camera::DrawCallsJob::Item{mr, material});
		}
        
		_viewportDependentData->renderJob = std::make_shared<MMPEngine::Frontend::Camera::DrawCallsJob>(
			globalContext,
			_viewportDependentData->camera,
			std::move(drawItems)
		);

		{
			const auto executor = stream->CreateExecutor();
			stream->Schedule(_viewportDependentData->renderJob->CreateInitializationTask());
		}

		const auto updateCameraTask = _viewportDependentData->camera->CreateTaskToUpdateUniformData();
		const auto renderJobExecutionTask = _viewportDependentData->renderJob->CreateExecutionTask();


		_viewportDependentData->screenFrameTask = _viewportDependentData->screen->CreateFrameTask();
		_viewportDependentData->screenFrameTask->GetTaskContext()->internalTask = std::make_shared<MMPEngine::Core::StaticBatchTask>(std::initializer_list<std::shared_ptr<MMPEngine::Core::BaseTask>>
		{
			updateCameraTask,
			renderJobExecutionTask
		});


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
			stream->Schedule(_viewportDependentData->screenFrameTask);
		}
	}
}
