//
// Created by Anton on 01.06.2025.
//

#ifndef INPUTCONTROLLER_HPP
#define INPUTCONTROLLER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "physics/Ball.hpp"

namespace core {

    class InputController {
    public:
        InputController(float maxDragMeters = 2.f,   // 2 м = 200 px
                        float maxImpulse    = 2.f)   // 2 Н·с
            : m_maxDrag(maxDragMeters)
            , m_maxImpulse(maxImpulse)
        {}

        bool handleEvent(const sf::Event&,
                         sf::RenderWindow&,
                         std::vector<physics::Ball>&);

        void drawAim(sf::RenderWindow&) const;

    private:
        float m_maxDrag;      // метры
        float m_maxImpulse;   // Н·с
        bool  m_dragging = false;

        sf::Vector2f m_startPx{},   // ← точка начала drag-а (пиксели)
                     m_currPx{};    // ← текущий курсор      (пиксели)

        physics::Ball* m_selected = nullptr;

        physics::Ball* findBallUnder(const sf::Vector2f& ptMeters,
                                            std::vector<physics::Ball>&);

        b2Vec2 computeImpulse(const sf::Vector2f& dragMeters) const;
    };

} // namespace core

#endif //INPUTCONTROLLER_HPP
