//
// Created by Anton on 01.06.2025.
//

#ifndef TABLE_HPP
#define TABLE_HPP

#include <box2d/box2d.h>

namespace physics {

    /// Четыре статичные «стенки» в Box2D 2.4.1 (EdgeShape).
    class Table {
    public:
        /// \param world  — ссылка на b2World
        /// \param width  — полная ширина (например 1280)
        /// \param height — полная высота (например 720)
        /// \param offset — отступ от края окна до «стенки»
        Table(b2World& world, float width, float height, float offset) {
            // Вычисляем внутренний прямоугольник
            float L = offset;
            float R = width  - offset;
            float T = offset;
            float B = height - offset;

            // Создаём одно статичное тело, к нему прикрепим все 4 грани
            b2BodyDef bd;
            bd.type = b2_staticBody;
            bd.position.Set(0.0f, 0.0f);
            b2Body* walls = world.CreateBody(&bd);

            auto makeEdge = [&](float x1, float y1, float x2, float y2) {
                b2EdgeShape edge;
                edge.m_vertex1.Set(x1, y1);
                edge.m_vertex2.Set(x2, y2);
                walls->CreateFixture(&edge, 0.0f);
            };

            makeEdge(L, T, R, T); // верх
            makeEdge(L, B, R, B); // низ
            makeEdge(L, T, L, B); // слева
            makeEdge(R, T, R, B); // справа
        }
    };

} // namespace physics


#endif //TABLE_HPP
