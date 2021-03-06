﻿#include "System.h"

#include "../Entity.h"
#include "../EntityFactory.h"
#include "../../components/Components.h"
#include "../../level/tile/TileCollision.h"

#include "../../maths/maths.h"
#include "../../util/Log.h"
#include "../../util/Timestep.h"
#include "../../level/tile/TileFlooding.h"
#include "../../app/Application.h"

namespace Entity
{
	using namespace Components;

	void MoveSystem::update(const Timestep& ts, Entity* entity)
	{
		PhysicsComponent* c_physics = entity->getComponent<PhysicsComponent>();

		if (c_physics)
		{
			auto colliding = Level::tileCollision(c_physics->pos, c_physics->velocity, c_physics->bounds, ts);
					
			if (!colliding.first)
				c_physics->pos.x += c_physics->velocity.x * ts.asSeconds();
			if (!colliding.second)
				c_physics->pos.y += c_physics->velocity.y * ts.asSeconds();
				
			c_physics->velocity.x = 0;
			c_physics->velocity.y = 0;
		}
	}

	void ScriptSystem::update(const Timestep& ts, Entity* entity)
	{
		ScriptComponent* c_script = entity->getComponent<ScriptComponent>();
		if (c_script)
		{
			luabridge::LuaRef update = getGlobal(c_script->LuaState, "update");
			try 
			{
				LuaEntityHandle&& ll = LuaEntityHandle(entity);
				update(ll, &Application::get());
			}
			catch (luabridge::LuaException const& e) 
			{
				LOG_ERROR("LuaException: ", e.what());
			}
		}
	}

	void StatsSystem::update(const Timestep& ts, Entity* entity)
	{
		StatsComponent* c_stats = entity->getComponent<StatsComponent>();
		if (c_stats)
		{
			for (auto effect : c_stats->active_buffs)
			{
				effect->manageDuration();
				effect->effect(c_stats->stats);
			}
		}
	}

	void AnimatorSystem::update(const Timestep& ts, Entity* entity)
	{
		SpriteComponent* c_sprite = entity->getComponent<SpriteComponent>();

		if (c_sprite && c_sprite->animated)
			c_sprite->animator.update(ts, c_sprite->sprite);
	}

	void LightingSystem::update(const Timestep& ts, Entity* entity)
	{
		PhysicsComponent* c_physics = entity->getComponent<PhysicsComponent>();
		LightComponent* c_light = entity->getComponent<LightComponent>();

		if (c_physics && c_light)
		{
			Vec2i old = { c_light->light.x, c_light->light.y };
			c_light->light.x = static_cast<int32>(c_physics->pos.x / TILE_SIZE);
			c_light->light.y = static_cast<int32>(c_physics->pos.y / TILE_SIZE);
			if (old.x != c_light->light.x || old.y != c_light->light.y)
				State::Playing::get().getLevel().getTiles().requestRebuild(0);

			if (!c_light->added)
			{
				State::Playing::get().getLevel().getTiles().addStaticLight(0, &c_light->light);
				c_light->added = true;
			}
		}
	}

	void RenderSystem::update(const Timestep& ts, Entity* entity)
	{
		PhysicsComponent* c_physics = entity->getComponent<PhysicsComponent>();
		SpriteComponent* c_sprite = entity->getComponent<SpriteComponent>();

		if (c_physics && c_sprite)
		{
			c_sprite->sprite.setScale(c_sprite->flipX ? -1.0f : 1.0f, 1.0f);
			
			sf::RenderStates states;
			states.transform.translate(Vec2(c_physics->pos));
			
			/* 
			    Draws rectangle outline (for debugging purposes)
				*/
#define SHOW_BOUNDS 0
#if SHOW_BOUNDS
			auto rs = sf::RectangleShape(c_physics->bounds.size);
			rs.setPosition(c_physics->bounds.position);
			rs.setFillColor({ 0, 0, 0, 0 });
			rs.setOutlineColor({ 255, 0, 0, 255 });
			rs.setOutlineThickness(1);
			Application::get().getWindow().draw(rs, states);
#endif
			Application::get().getWindow().draw(c_sprite->sprite, states);
		}
	}

	void LifeSystem::update(const Timestep& ts, Entity* entity)
	{
		LifeComponent* c_life = entity->getComponent<LifeComponent>();

		if (c_life)
		{
			if (c_life->done)
				State::Playing::get().getLevel().removeEntity(entity);
			c_life->life -= ts.asSeconds();
			if (c_life->life <= 0)
				c_life->done = true;
		}
	}
}
