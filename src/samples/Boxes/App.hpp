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
		std::shared_ptr<MMPEngine::Core::MeshMaterial> _material;
		std::shared_ptr<MMPEngine::Frontend::Mesh> _mesh;
	};
}
