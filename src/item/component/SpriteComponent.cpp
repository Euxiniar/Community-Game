#include "SpriteComponent.h"
#include "../../app/Application.h"

namespace Item {
	SpriteComponent::SpriteComponent(nlohmann::json json)
	: sprite(sf::Sprite(Application::instance->getResources().textures.get(json["src"]), sf::IntRect(json["rect"]["left"], json["rect"]["top"], json["rect"]["width"], json["rect"]["height"]))){

	}
	ComponentType* SpriteComponent::getStaticType()
	{
		static ComponentType type({ "Sprite" });
		return &type;
	}

	ComponentType* SpriteComponent::getType() const
	{
		return getStaticType();
	}
}
