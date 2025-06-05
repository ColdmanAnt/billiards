//
// Created by Anton on 01.06.2025.
//

#ifndef BALL_HPP
#define BALL_HPP

#include <SFML/System/Vector2.hpp>
#include <box2d/box2d.h>
#include "Utils/Scale.hpp"

namespace physics {

    /// Класс Ball для Box2D, где 100 px = 1 m.
    class Ball {
    public:
        ///
        /// \param world   — ссылка на b2World
        /// \param rPix    — радиус шара в пикселях
        /// \param posPix  — стартовая позиция центра шара в пикселях
        ///
        Ball(b2World& world, float rPix, const sf::Vector2f& posPix);

        // Move semantics: разрешаем перемещение
        Ball(Ball&& other) noexcept;
        Ball& operator=(Ball&& other) noexcept;

        // Копирование запрещено
        Ball(const Ball&)            = delete;
        Ball& operator=(const Ball&) = delete;

        ~Ball();  // Удаляем тело из мира

        [[nodiscard]] b2Body* body()   const { return m_body; }
        [[nodiscard]] float   radius() const { return m_radiusPx; }

    private:
        b2World&   m_world;     ///< «сырое» поле физики (метры)
        b2Body*    m_body{};    ///< указатель на тело Box2D
        float      m_radiusPx;  ///< радиус шара в пикселях
    };

} // namespace physics



#endif //BALL_HPP
