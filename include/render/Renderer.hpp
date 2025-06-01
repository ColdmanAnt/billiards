//
// Created by Anton on 01.06.2025.
//

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SFML/Graphics.hpp>
#include "physics/Ball.hpp"

namespace render {

    class Renderer {
    public:
        void drawBall(sf::RenderWindow& win, const physics::Ball& ball) {
            float r = ball.radius();
            sf::CircleShape shape(r);
            shape.setOrigin({r, r});
            b2Vec2 p = ball.body()->GetPosition();
            shape.setPosition({p.x, p.y});
            shape.setFillColor(sf::Color::White);
            win.draw(shape);
        }
    };

}

#endif //RENDERER_HPP
