#pragma once
#include <Feature/App.hpp>
#include <Core/Material.hpp>

namespace Sample::Boxes
{
	class App : public MMPEngine::Feature::UserApp
	{
	public:
		App(const std::shared_ptr<MMPEngine::Feature::BaseLogger>& logger);
		void Initialize() override;
	private:
		std::shared_ptr<MMPEngine::Core::MeshMaterial> _boxMaterial;
	};
}
