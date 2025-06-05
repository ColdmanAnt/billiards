//
// Created by Anton on 02.06.2025.
//

#ifndef POCKETS_HPP
#define POCKETS_HPP

#include <SFML/System/Vector2.hpp>
#include <vector>

namespace physics {

    struct Pocket {
        sf::Vector2f center;
        float        radius;
    };

    inline std::vector<Pocket> defaultPockets(float wPix, float hPix, float rPix) {
        // Переводим все параметры в метры
        float w   = px2m(wPix);
        float h   = px2m(hPix);
        float r   = px2m(rPix);
        float off = r;  // отступ от края (в метрах)

        return {
                    {{ off,    off    }, r},  // левый верхний угол
                    {{ w/2.f,  off    }, r},  // верхний центр
                    {{ w-off,  off    }, r},  // правый верхний угол
                    {{ off,    h-off  }, r},  // левый нижний угол
                    {{ w/2.f,  h-off  }, r},  // нижний центр
                    {{ w-off,  h-off  }, r}   // правый нижний угол
        };
    }
        inline bool inPocket(const physics::Pocket& p, const physics::Ball& b)
        {
            // Центр шара в метрах
            b2Vec2 c = b.body()->GetPosition();

            // Δx и Δy между центром шара и центром кармана
            float dx = c.x - p.center.x;
            float dy = c.y - p.center.y;

            // Квадрат расстояния ≤ квадрат радиуса кармана?
            return (dx * dx + dy * dy) <= (p.radius * p.radius);
        }
};


#endif //POCKETS_HPP
