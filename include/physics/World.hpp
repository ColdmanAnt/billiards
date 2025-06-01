//
// Created by Anton on 01.06.2025.
//

#ifndef WORLD_HPP
#define WORLD_HPP

#include <box2d/box2d.h>

namespace physics {

    /// Обёртка вокруг b2World (API 2.4.1)
    class World {
    public:
        /// Конструктор: мир без гравитации (гравитация = (0,0))
        World()
            : m_world(b2Vec2(0.0f, 0.0f))
        {}

        /// Шаг симуляции (dt в секундах)
        void step(float dt) {
            // velocityIterations = 8, positionIterations = 3 (типичные значения)
            m_world.Step(dt, 8, 3);
        }

        /// Возвращает «сырое» b2World для прямого доступа (например, GetBodyList)
        b2World& raw() { return m_world; }

    private:
        b2World m_world;
    };

}

#endif //WORLD_HPP
