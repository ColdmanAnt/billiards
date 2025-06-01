//
// Created by Anton on 01.06.2025.
//

#ifndef INPUTCONTROLLER_HPP
#define INPUTCONTROLLER_HPP

#include <SFML/Graphics.hpp>
#include "physics/Ball.hpp"

namespace core {

    class InputController {
    public:
        explicit InputController(float impulseScale = 5'000.f)
            : m_impulseScale(impulseScale) {}

        /// true → удар выполнен (можно проиграть звук)
        bool handleEvent(const sf::Event& ev,
                         sf::RenderWindow& win,
                         physics::Ball& ball);

        void drawAim(sf::RenderWindow& win) const;

    private:
        float        m_impulseScale;
        bool         m_dragging = false;
        sf::Vector2f m_start;   // точка нажатия
        sf::Vector2f m_curr;    // текущий курсор (для прицела)
    };

} // namespace core

#endif //INPUTCONTROLLER_HPP
