#pragma once
#include <Feature/App.hpp>
#include <Feature/Movement.hpp>
#include <Core/Material.hpp>
#include <Frontend/Mesh.hpp>
#include <Frontend/Screen.hpp>
#include <Frontend/Texture.hpp>
#include <Frontend/Camera.hpp>
#include <Frontend/DrawCall.hpp>
#include <Frontend/Shader.hpp>

namespace Sample::Boxes
{
	class App : public MMPEngine::Feature::UserApp
	{
	public:
		App(const std::shared_ptr<MMPEngine::Feature::BaseLogger>& logger);
		void Initialize() override;
		void OnNativeWindowUpdated() override;
		void OnUpdate(std::float_t dt) override;
		void OnRender() override;
	private:

		class ViewportIndependentData final
		{
		public:
			std::shared_ptr<MMPEngine::Frontend::ShaderPack> shaderPack;
			std::tuple<
				MMPEngine::Core::MeshMaterial::Settings,
				std::shared_ptr<MMPEngine::Core::Shader>,
				std::shared_ptr<MMPEngine::Core::Shader>
			> materialData;
			std::shared_ptr<MMPEngine::Core::Mesh> mesh;
			std::shared_ptr<MMPEngine::Core::Mesh::Renderer> meshRenderer;
			std::shared_ptr<MMPEngine::Core::Node> cameraNode;
			std::unique_ptr<MMPEngine::Feature::Movement::Controller> cameraMovement;
		};

		class ViewportDependentData final
		{
		public:
			std::shared_ptr<MMPEngine::Core::Camera> camera;
			std::shared_ptr<MMPEngine::Core::Screen> screen;
			std::shared_ptr<MMPEngine::Core::ContextualTask<MMPEngine::Core::Screen::FrameTaskContext>> screenFrameTask;
			std::shared_ptr<MMPEngine::Core::DepthStencilTargetTexture> depthStencilTexture;
			std::shared_ptr<MMPEngine::Core::Camera::DrawCallsJob> renderJob;
		};

		std::unique_ptr<ViewportIndependentData> _viewportIndependentData;
		std::unique_ptr<ViewportDependentData> _viewportDependentData;
	};
}
