#include "physics/Ball.hpp"

namespace physics {

    // Конструктор: получаем р-р и позицию в пикселях, переводим в метры
    Ball::Ball(b2World& world, float rPix, const sf::Vector2f& posPix)
        : m_world(world)
        , m_radiusPx(rPix)
    {
        // 1) Переводим радиус и позицию в метры
        float r  = px2m(rPix);
        float px = px2m(posPix.x);
        float py = px2m(posPix.y);

        // 2) Описываем тело
        b2BodyDef bd;
        bd.type          = b2_dynamicBody;
        bd.position.Set(px, py);
        bd.linearDamping  = 0.9f;  // сильное демпфирование
        bd.angularDamping = 0.9f;
        m_body = m_world.CreateBody(&bd);

        // 3) Форма — круг
        b2CircleShape circle;
        circle.m_radius = r;

        // 4) Фикстура: плотность, трение, упругость
        b2FixtureDef fd;
        fd.shape       = &circle;
        fd.density     = 160.f;    // ~0.16 кг/πr², шар ~0.17 кг
        fd.friction    = 0.05f;    // мало трения с бортом
        fd.restitution = 0.93f;    // заметный отскок

        m_body->CreateFixture(&fd);
    }

    // Move-конструктор
    Ball::Ball(Ball&& other) noexcept
        : m_world(other.m_world)
        , m_body(other.m_body)
        , m_radiusPx(other.m_radiusPx)
    {
        other.m_body = nullptr;
    }

    // Move-assignment
    Ball& Ball::operator=(Ball&& other) noexcept
    {
        if (this == &other) return *this;

        // Удаляем своё тело, если есть
        if (m_body) {
            m_world.DestroyBody(m_body);
        }

        m_body     = other.m_body;
        m_radiusPx = other.m_radiusPx;

        other.m_body = nullptr;
        return *this;
    }

    // Деструктор: уничтожаем тело в мире
    Ball::~Ball()
    {
        if (m_body) {
            m_world.DestroyBody(m_body);
        }
    }

} // namespace physics