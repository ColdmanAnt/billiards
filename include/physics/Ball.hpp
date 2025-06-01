//
// Created by Anton on 01.06.2025.
//

#ifndef BALL_HPP
#define BALL_HPP
#include <SFML/System/Vector2.hpp>
#include <box2d/box2d.h>

namespace physics {

    /// Класс Ball для Box2D 2.4.1.
    class Ball {
    public:
        /// \param world — ссылка на b2World
        /// \param r     — радиус шара (в тех же «единицах», что и мир)
        /// \param pos   — стартовая позиция (x,y)
        Ball(b2World& world, float r, const sf::Vector2f& pos);

        /// Вернёт указатель на тело (для получения позиции и пр.)
        [[nodiscard]] b2Body* body() const { return m_body; }

        /// Радиус (для отрисовки)
        [[nodiscard]] float radius() const { return m_radius; }

    private:
        b2World& m_world;   ///< «сырое» b2World (не владеем)
        b2Body*  m_body{};  ///< указатель на физическое тело
        float    m_radius{};///< радиус
    };

}


#endif //BALL_HPP
