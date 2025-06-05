#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <glad/glad.h>
#include <iostream>
#include <filesystem>

#include "render/GLRenderer.hpp"
#include "physics/World.hpp"
#include "physics/Table.hpp"
#include "physics/Ball.hpp"
#include "physics/Pockets.hpp"
#include "core/InputController.hpp"
#include "core/ScoreBoard.hpp"
#include "Utils/Scale.hpp"

int main()
{
    // 1) Создаём окно SFML с контекстом OpenGL 3.3
    sf::RenderWindow win(
        sf::VideoMode{ sf::Vector2u{1280, 720}, 32u },
        "Billiards 3-D",
        sf::State::Windowed,
        sf::ContextSettings{24, 8, 4, 3, 3}  // depth=24, stencil=8, AA=4, OpenGL 3.3
    );
    win.setFramerateLimit(60);

    // 2) Активируем окно, чтобы Glad увидел контекст
    if (!win.setActive(true)) {
        std::cerr << "Failed to set SFML window active (for GL)\n";
        return -1;
    }

    // 3) Инициализируем Glad
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // 4) Пока контекст активен, выводим реальную версию OpenGL/GLSL
    std::cout << "OpenGL Vendor:   "
              << (const char*)glGetString(GL_VENDOR)   << "\n";
    std::cout << "OpenGL Renderer: "
              << (const char*)glGetString(GL_RENDERER) << "\n";
    std::cout << "OpenGL Version:  "
              << (const char*)glGetString(GL_VERSION)  << "\n";
    std::cout << "GLSL Version:    "
              << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)
              << "\n";

    // После того, как мы убедились, какая версия, можно деактивировать контекст
    win.setActive(false);

    // 5) Проверим, что папка shaders/ лежит рядом с .exe
    std::cout << "Current working directory: "
              << std::filesystem::current_path() << "\n";
    if (!std::filesystem::exists("shaders/phong.vert")) {
        std::cerr << "Error: shaders/phong.vert not found in working directory\n";
        std::cerr << "Make sure you used file(COPY \"${CMAKE_SOURCE_DIR}/shaders\" DESTINATION \"${CMAKE_BINARY_DIR}\") in CMakeLists.txt\n";
        return -1;
    }

    // 6) Создаём рендерер и пытаемся инициализировать 3D-шейдеры
    render::GLRenderer glRenderer;
    if (!glRenderer.init("shaders")) {
        std::cerr << "Failed to initialize GLRenderer (shaders)\n";
        return -1;
    }
    glRenderer.resize(1280, 720);

    // 7) Загружаем шрифт и настраиваем 2D-счёт
    sf::Font font;
    if (!font.openFromFile("assets/fonts/OpenSans-Regular.ttf")) {
        std::cerr << "Font not found!\n";
        return 1;
    }
    core::ScoreBoard scoreboard(font);

    // 8) Физический мир, стол, шары
    physics::World world;
    physics::Table table(world.raw(), 1280.f, 720.f, /*offsetPx=*/20.f);

    std::vector<physics::Ball> balls;
    const sf::Vector2f cueStartPx{300.f, 360.f};
    balls.emplace_back(world.raw(), 10.f, cueStartPx);

    // Пирамида из 15 шаров
    const float R   = 10.f;
    const float GAP = 0.5f;
    const float STEPX = (2.f * R + GAP) * 0.8660254f;
    const float BASEY = 360.f;
    const float BASEx = 900.f;
    for (int row = 0; row < 5; ++row) {
        float x  = BASEx + STEPX * row;
        float y0 = BASEY - row * (R + GAP / 2.f);
        for (int col = 0; col <= row; ++col) {
            float y = y0 + col * (2.f * R + GAP);
            balls.emplace_back(world.raw(), R, sf::Vector2f{x, y});
        }
    }

    // 9) Карманы
    auto pockets = physics::defaultPockets(1280.f, 720.f, /*rPx=*/18.f);

    // 10) Контроллер ввода
    core::InputController input(/*maxDrag_m=*/2.0f, /*maxImpulse=*/0.2f);

    const float dt = 1.0f / 120.0f;
    sf::Clock clk;

    // 11) Главный цикл
    while (win.isOpen()) {
        // 11.1  События
        while (auto e = win.pollEvent()) {
            if (e->is<sf::Event::Closed>()) {
                win.close();
            }
            input.handleEvent(*e, win, balls);
        }

        // 11.2  Физика
        for (int i = 0; i < 4; ++i)
            world.step(dt / 4.0f);

        const float vEps2 = px2m(3.f) * px2m(3.f);
        const float wEps  = 0.01f;
        for (auto& b : balls) {
            b2Body* body = b.body();
            if (!body) continue;
            bool slowLin = body->GetLinearVelocity().LengthSquared() < vEps2;
            bool slowAng = std::abs(body->GetAngularVelocity()) < wEps;
            if (slowLin) body->SetLinearVelocity({0, 0});
            if (slowAng) body->SetAngularVelocity(0);
            if (slowLin && slowAng) body->SetAwake(false);
        }

        // 11.3  Проверка карманов
        for (auto it = balls.begin(); it != balls.end();) {
            bool potted = false;
            for (auto& pocket : pockets) {
                if (physics::inPocket(pocket, *it)) {
                    potted = true;
                    break;
                }
            }
            if (potted) {
                if (&(*it) == &balls.front()) {
                    b2Body* b = it->body();
                    b->SetTransform(
                        { px2m(cueStartPx.x), px2m(cueStartPx.y) }, 0.f
                    );
                    b->SetLinearVelocity({0, 0});
                    ++it;
                } else {
                    world.raw().DestroyBody(it->body());
                    it = balls.erase(it);
                    scoreboard.increase();
                }
            } else ++it;
        }

        // 11.4  Рендер 3D
        win.setActive(true);
        glViewport(0, 0, 1280, 720);
        glRenderer.drawScene(balls, pockets, 1280.f, 720.f);
        win.setActive(false);

        // 11.5  HUD 2D
        win.pushGLStates();
        scoreboard.draw(win);
        input.drawAim(win);
        win.popGLStates();

        win.display();
        sf::sleep(sf::seconds(dt) - clk.restart());
    }

    return 0;
}
