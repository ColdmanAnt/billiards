#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "glad/glad.h"
#include <iostream>
#include <cmath>

#include "physics/World.hpp"
#include "physics/Table.hpp"
#include "physics/Ball.hpp"
#include "physics/Pockets.hpp"

#include "render/GLRenderer.hpp"
#include "core/InputController.hpp"
#include "core/ScoreBoard.hpp"
#include "Utils/Scale.hpp"            // px2m, m2px

using physics::inPocket;

int main()
{
    // Создаём окно SFML с контекстом OpenGL 3.3
    sf::RenderWindow win(
        sf::VideoMode{ sf::Vector2u{1280, 720} },  // теперь передаём Vector2u
        "Billiards 3-D",
        sf::State::Windowed,
        sf::ContextSettings{24, 8, 4, 3, 3}
    );
    win.setFramerateLimit(60);

    // Инициализируем OpenGL через glad
    win.setActive(true);
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    win.setActive(false);

    // ────────────────────────────────────────────────────────────────────
    // 1. Инициализируем 3D-рендерер
    render::GLRenderer glRenderer;
    if (!glRenderer.init("shaders")) {
        std::cerr << "Failed to initialize GLRenderer (shaders)\n";
        return -1;
    }
    glRenderer.resize(1280, 720);
    // ────────────────────────────────────────────────────────────────────

    // ── шрифт + 2D-счёток поверх 3D ──────────────────────────────────
    sf::Font font;
    if (!font.openFromFile("assets/fonts/OpenSans-Regular.ttf")) {
        std::cerr << "Font not found!\n";
        return 1;
    }
    core::ScoreBoard scoreboard(font);

    // ── физика: Box2D-мир и «стол» ────────────────────────────────────
    physics::World world;
    physics::Table table(world.raw(), 1280.f, 720.f, /*offsetPx=*/20.f);

    // ── создаём все шары ─────────────────────────────────────────────
    std::vector<physics::Ball> balls;

    // 1) биток
    const sf::Vector2f cueStartPx{300.f, 360.f};
    balls.emplace_back(world.raw(), 10.f, cueStartPx);

    // 2) пирамида из 15 шаров (5 рядов)
    const float R   = 10.f;            // радиус в пикселях
    const float GAP = 0.5f;
    const float STEPX = (2.f * R + GAP) * 0.8660254f; // √3/2
    const float BASEY = 360.f;
    const float BASEx = 900.f;

    for (int row = 0; row < 5; ++row) {
        float x  = BASEx + STEPX * row;
        float y0 = BASEY - row * (R + GAP/2.f);
        for (int col = 0; col <= row; ++col) {
            float y = y0 + col * (2.f * R + GAP);
            balls.emplace_back(world.raw(), R, sf::Vector2f{ x, y });
        }
    }

    // ── 6 карманов (перевод в метры внутри defaultPockets) ─────────────
    auto pockets = physics::defaultPockets(1280.f, 720.f, /*rPx=*/18.f);

    // ── контроллер ввода (удар) и 2D-рендерер прицела ─────────────────
    core::InputController input(/*maxDrag_m=*/2.0f,   // 2 м = 200 px
                                /*maxImpulse=*/0.2f);  // 0.2 Н·с

    const float dt = 1.f / 120.f;
    sf::Clock clk;

    while (win.isOpen()) {
        // --- события ---------------------------------------------------
        while (auto e = win.pollEvent()) {
            if (e->is<sf::Event::Closed>())
                win.close();

            input.handleEvent(*e, win, balls);
        }

        // --- физика (с sub-steps для повышения предела скорости) ------------
        for (int i = 0; i < 4; ++i)
            world.step(dt / 4.f);

        // затухание почти-нулевых скоростей и «усыпление»
        const float vEps2 = px2m(3.f) * px2m(3.f);  // 3 px/с → м²/с²
        const float wEps  = 0.01f;                  // 0.01 рад/с
        for (auto& b : balls) {
            b2Body* body = b.body();
            if (!body) continue;

            bool slowLin = body->GetLinearVelocity().LengthSquared() < vEps2;
            bool slowAng = std::abs(body->GetAngularVelocity()) < wEps;

            if (slowLin) body->SetLinearVelocity({0,0});
            if (slowAng) body->SetAngularVelocity(0);

            if (slowLin && slowAng)
                body->SetAwake(false);
        }

        // --- проверка карманов (все в метрах) -----------------------------
        for (auto it = balls.begin(); it != balls.end(); ) {
            bool potted = false;
            for (auto& pocket : pockets) {
                if (inPocket(pocket, *it)) {
                    potted = true;
                    break;
                }
            }

            if (potted) {
                // если биток (первый элемент) → вернуть на старт
                if (&(*it) == &balls.front()) {
                    b2Body* b = it->body();
                    b->SetTransform(
                        { px2m(cueStartPx.x), px2m(cueStartPx.y) },
                        0.f
                    );
                    b->SetLinearVelocity({0,0});
                    ++it;
                }
                else {
                    world.raw().DestroyBody(it->body());
                    it = balls.erase(it);
                    scoreboard.increase();
                }
            }
            else ++it;
        }

        // --- рендер 3D сцены через OpenGL -------------------------------
        win.setActive(true);
        glViewport(0, 0, 1280, 720);
        glRenderer.drawScene(balls, pockets, 1280.f, 720.f);
        win.setActive(false);

        // --- HUD 2D (SFML) поверх 3D ------------------------------------
        win.pushGLStates();
        scoreboard.draw(win);       // рисуем счёт
        input.drawAim(win);         // рисуем прицел-линию (2D)
        win.popGLStates();

        win.display();

        sf::sleep(sf::seconds(dt) - clk.restart());
    }

    return 0;
}
