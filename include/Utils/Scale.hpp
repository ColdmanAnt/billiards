//
// Created by Anton on 02.06.2025.
//

#ifndef SCALE_HPP
#define SCALE_HPP

#include <SFML/Graphics.hpp>

constexpr float PPM = 100.f;    // 100 px = 1 m

inline float px2m(float px)  { return px / PPM; }
inline float m2px(float m)   { return m  * PPM; }

inline sf::Vector2f px2m(const sf::Vector2f& v) { return { v.x/PPM, v.y/PPM }; }
inline sf::Vector2f m2px(const sf::Vector2f& v) { return { v.x*PPM, v.y*PPM }; }

#endif //SCALE_HPP
