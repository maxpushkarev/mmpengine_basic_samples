#pragma once
#include <Feature/App.hpp>
#include <Core/Material.hpp>
#include <Frontend/Mesh.hpp>

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
			std::tuple<
				MMPEngine::Core::RenderingMaterial::Settings,
				std::shared_ptr<MMPEngine::Core::VertexShader>,
				std::shared_ptr<MMPEngine::Core::PixelShader>
			> materialData;
			std::shared_ptr<MMPEngine::Frontend::Mesh> mesh;
			std::shared_ptr<MMPEngine::Frontend::Mesh::Renderer> renderer;
		};

		class ViewportDependentData final
		{
		public:
			std::shared_ptr<MMPEngine::Core::MeshMaterial> material;
		};

		std::unique_ptr<ViewportIndependentData> _viewportIndependentData;
		std::unique_ptr<ViewportDependentData> _viewportDependentData;
	};
}
