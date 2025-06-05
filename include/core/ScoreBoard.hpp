//
// Created by Anton on 02.06.2025.
//

#ifndef SCOREBOARD_HPP
#define SCOREBOARD_HPP

#include <SFML/Graphics.hpp>

namespace core {

    class ScoreBoard {
    public:
        explicit ScoreBoard(const sf::Font& font)
        : m_text(font, "Score: 0", 28)          // ←- новый конструктор
        {
            m_text.setFillColor(sf::Color::Yellow);
        }


        void increase() { ++m_score; updateText(); }

        void draw(sf::RenderWindow& win) const { win.draw(m_text); }

    private:
        int         m_score = 0;
        sf::Text    m_text;
        void updateText() { m_text.setString("Score: " + std::to_string(m_score)); }
    };

} // namespace core

#endif //SCOREBOARD_HPP
