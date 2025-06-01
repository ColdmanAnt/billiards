#include "core/InputController.hpp"

namespace {

bool pointInBall(const sf::Vector2f& p, const physics::Ball& ball)
{
    b2Vec2 c = ball.body()->GetPosition();
    float  r = ball.radius();
    sf::Vector2f center{c.x, c.y};
    sf::Vector2f d = center - p;
    return (d.x*d.x + d.y*d.y) <= r*r;
}

} // anonymous

namespace core {

bool InputController::handleEvent(const sf::Event& ev,
                                  sf::RenderWindow& win,
                                  physics::Ball& ball)
{
    // ── нажата ЛКМ ──────────────────────────────────────────────
    if (auto mb = ev.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mb->button == sf::Mouse::Button::Left) {
            sf::Vector2f m = win.mapPixelToCoords(mb->position);
            if (pointInBall(m, ball)) {
                m_dragging = true;
                m_start = m_curr = m;
            }
        }
    }
    // ── движение мыши ───────────────────────────────────────────
    else if (auto mm = ev.getIf<sf::Event::MouseMoved>(); mm && m_dragging)
    {
        m_curr = win.mapPixelToCoords(mm->position);
    }
    // ── отпустили ЛКМ ───────────────────────────────────────────
    else if (auto mr = ev.getIf<sf::Event::MouseButtonReleased>(); mr && m_dragging)
    {
        if (mr->button == sf::Mouse::Button::Left) {
            m_dragging = false;
            sf::Vector2f end = win.mapPixelToCoords(mr->position);
            sf::Vector2f drag = m_start - end;

            b2Vec2 impulse(drag.x * m_impulseScale,
                           drag.y * m_impulseScale);
            ball.body()->ApplyLinearImpulseToCenter(impulse, true);
            return true;        // удар случился
        }
    }
    return false;
}

void InputController::drawAim(sf::RenderWindow& w) const
{
    if (!m_dragging) return;

    sf::Vertex line[] = {
        sf::Vertex(m_start, sf::Color::Red),
        sf::Vertex(m_curr,  sf::Color::Red)
    };
    w.draw(line, 2, sf::PrimitiveType::Lines);
}

} // namespace core
