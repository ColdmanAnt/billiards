#include <SFML/Graphics.hpp>
#include "physics/World.hpp"
#include "physics/Table.hpp"
#include "physics/Ball.hpp"
#include "render/Renderer.hpp"
#include "core/InputController.hpp"

int main() {
    sf::RenderWindow win(sf::VideoMode({1280u,720u}),
                         "Billiards — hit demo");
    win.setFramerateLimit(60);

    physics::World  world;
    physics::Table  table(world.raw(), 1280.f, 720.f, 20.f);
    physics::Ball   cue(world.raw(), 10.f, {640.f, 360.f});

    render::Renderer   ren;
    core::InputController input;

    const float dt = 1.f/120.f;
    sf::Clock clk;

    while (win.isOpen()) {
        while (auto ev = win.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) win.close();
            input.handleEvent(*ev, win, cue);
        }

        world.step(dt);

        win.clear({30,120,30});
        ren.drawBall(win, cue);
        input.drawAim(win);
        win.display();

        sf::sleep(sf::seconds(dt) - clk.restart());
    }
}
