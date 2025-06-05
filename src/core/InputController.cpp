#include "core/InputController.hpp"
#include "Utils/Scale.hpp"   // Для px2m()
#include <cmath>

using physics::Ball;

namespace core {

/// Проверяет, что все шары полностью остановились (линейная и угловая скорости ниже порога).
static bool allBallsStopped(const std::vector<Ball>& balls)
{
    const float vEps2 = px2m(3.f) * px2m(3.f);  // ≈ (3 px)² в м²/с²
    const float wEps  = 0.01f;                  // 0.01 рад/с

    for (const auto& b : balls) {
        b2Body* body = b.body();
        if (!body) continue;

        if (body->GetLinearVelocity().LengthSquared() > vEps2)   return false;
        if (std::abs(body->GetAngularVelocity()) > wEps)         return false;
    }
    return true;
}

/// Находит шар, центр которого ближе всего к точке ptM (в метрах).
physics::Ball* InputController::findBallUnder(const sf::Vector2f& ptM,
                                                  std::vector<Ball>& balls)
{
    Ball*  hit   = nullptr;
    float  best2 = std::numeric_limits<float>::max();

    for (auto& b : balls) {
        b2Vec2 cM = b.body()->GetPosition();       // в метрах
        float  rM = px2m(b.radius());              // радиус в метрах

        float dx = cM.x - ptM.x;
        float dy = cM.y - ptM.y;
        float d2 = dx*dx + dy*dy;

        if (d2 <= rM*rM && d2 < best2) {
            best2 = d2;
            hit   = &b;
        }
    }
    return hit;
}

bool InputController::handleEvent(const sf::Event& ev,
                                  sf::RenderWindow& win,
                                  std::vector<Ball>& balls)
{
    // Проверяем, все ли шары остановлены; если нет, новые нажатия ЛКМ игнорируются.
    bool ready = allBallsStopped(balls);

    // ───── Нажатие ЛКМ ─────
    if (auto mb = ev.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mb->button == sf::Mouse::Button::Left && ready) {
            // Получаем курсор в пикселях и переводим в пиксели координаты вида
            sf::Vector2f mousePx = win.mapPixelToCoords(mb->position);
            // Конвертируем пиксели → метры для поиска шара
            sf::Vector2f mouseM  = px2m(mousePx);

            m_selected = findBallUnder(mouseM, balls);
            if (m_selected) {
                m_dragging = true;
                m_startPx = mousePx;   // сохраняем «точку начала» в пикселях
                m_currPx  = mousePx;   // и текущую тоже
            }
        }
    }
    // ───── Движение мыши ─────
    else if (auto mm = ev.getIf<sf::Event::MouseMoved>(); mm && m_dragging) {
        m_currPx = win.mapPixelToCoords(mm->position);
    }
    // ───── Отпускание ЛКМ ─────
    else if (auto mr = ev.getIf<sf::Event::MouseButtonReleased>(); mr && m_dragging)
    {
        if (mr->button == sf::Mouse::Button::Left) {
            m_dragging = false;

            // 1) Координаты отпускания в пикселях
            sf::Vector2f endPx = win.mapPixelToCoords(mr->position);

            // 2) Расчёт drag-вектора в пикселях
            sf::Vector2f dragPx = m_startPx - endPx;

            // 3) Конвертация пиксели → метры
            sf::Vector2f dragM = px2m(dragPx);

            // 4) Получаем импульс (в Н·с)
            b2Vec2 impulse = computeImpulse(dragM);

            // 5) Применяем к выбранному шару
            m_selected->body()->ApplyLinearImpulseToCenter(impulse, true);
            m_selected = nullptr;
            return true;
        }
    }

    return false;
}

b2Vec2 InputController::computeImpulse(const sf::Vector2f& dragM) const
{
    // dragM — в метрах
    float len = std::hypot(dragM.x, dragM.y);
    if (len < 1e-4f) return {0.f, 0.f};

    float clamped = std::min(len, m_maxDrag);   // m_maxDrag уже в метрах
    float scale   = (clamped / m_maxDrag) * m_maxImpulse; // m_maxImpulse в Н·с

    sf::Vector2f dir = { dragM.x / len, dragM.y / len };
    return { dir.x * scale, dir.y * scale };
}

void InputController::drawAim(sf::RenderWindow& win) const
{
    if (!m_dragging) return;

    // Рисуем линию из m_startPx в m_currPx (оба в пикселях)
    sf::Vertex line[] = {
        sf::Vertex(m_startPx, sf::Color::Red),
        sf::Vertex(m_currPx,  sf::Color::Red)
    };
    win.draw(line, 2, sf::PrimitiveType::Lines);
}

} // namespace core