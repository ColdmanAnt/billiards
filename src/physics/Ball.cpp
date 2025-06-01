#include "physics/Ball.hpp"

namespace physics {

    Ball::Ball(b2World& world, float r, const sf::Vector2f& pos)
        : m_world(world)
        , m_radius(r)
    {
        // 1) Создаём тело
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(pos.x, pos.y);
        m_body = m_world.CreateBody(&bodyDef);

        // 2) Описываем форму круга
        b2CircleShape circle;
        circle.m_radius = r;

        // 3) Фикстура: связываем форму с телом
        b2FixtureDef fixDef;
        fixDef.shape       = &circle;
        fixDef.density     = 1.0f;
        fixDef.restitution = 0.95f; // почти упругое
        fixDef.friction    = 0.2f;  // небольшое трение

        m_body->CreateFixture(&fixDef);
    }

} // namespace physics
