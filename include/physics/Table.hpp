//
// Created by Anton on 01.06.2025.
//

#ifndef TABLE_HPP
#define TABLE_HPP

#include <box2d/box2d.h>
#include "Utils/Scale.hpp"


namespace physics {

    /// Четыре статичные «стенки» в Box2D 2.4.1 (EdgeShape).
    class Table {
    public:
        /// \param world  — ссылка на b2World
        /// \param width  — полная ширина (например 1280)
        /// \param height — полная высота (например 720)
        /// \param offset — отступ от края окна до «стенки»
        Table(b2World& world, float widthPix, float heightPix, float offsetPix)
        {
            // Переводим всё из пикселов в метры:
            float left   = px2m(offsetPix);
            float right  = px2m(widthPix  - offsetPix);
            float top    = px2m(offsetPix);
            float bottom = px2m(heightPix - offsetPix);

            // Создаём одно статическое тело, к которому прикрепим все четыре EdgeShape.
            b2BodyDef bd;
            bd.type = b2_staticBody;
            bd.position.Set(0.0f, 0.0f);
            b2Body* walls = world.CreateBody(&bd);

            // Лямбда для упрощения: создаёт EdgeShape от (x1,y1) до (x2,y2) в метрах.
            auto makeEdge = [&](float x1, float y1, float x2, float y2) {
                b2EdgeShape edge;
                edge.m_vertex1.Set(x1, y1);
                edge.m_vertex2.Set(x2, y2);
                walls->CreateFixture(&edge, 0.0f);
            };

            // 1) Верхняя грань
            makeEdge(left,   top,    right,  top);
            // 2) Нижняя грань
            makeEdge(left,   bottom, right,  bottom);
            // 3) Левая грань
            makeEdge(left,   top,    left,   bottom);
            // 4) Правая грань
            makeEdge(right,  top,    right,  bottom);
        }
    };

} // namespace physics


#endif //TABLE_HPP
