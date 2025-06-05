//
// Created by Anton on 01.06.2025.
//

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Utils/Scale.hpp"
#include <SFML/Graphics.hpp>
#include "physics/Ball.hpp"

namespace render {

    class Renderer {
    public:
        void drawBall(sf::RenderWindow& win, const physics::Ball& ball) {
            // Получаем радиус шара в пикселях (хранится в m_radius в px)
            float rPx = ball.radius();
            sf::CircleShape shape(rPx);

            // Центрируем форму по середине круга
            shape.setOrigin({ rPx, rPx });

            // Получаем позицию тела (в метрах) и конвертируем в пиксели
            b2Vec2 posM = ball.body()->GetPosition();
            sf::Vector2f posPx = m2px(sf::Vector2f{ posM.x, posM.y });
            shape.setPosition(posPx);

            // Задаём цвет шара (можно расширить, чтобы шары разных цветов)
            shape.setFillColor(sf::Color::White);

            win.draw(shape);
        }
        void drawBalls(sf::RenderWindow& w, const std::vector<physics::Ball>& balls) {
            for (auto& b : balls) drawBall(w, b);
        }
    };

}

#endif //RENDERER_HPP
