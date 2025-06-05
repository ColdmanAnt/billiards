#pragma once

#include <string>
#include <vector>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include "physics/Ball.hpp"
#include "physics/Pockets.hpp"

// Мы предполагаем, что glad уже подключён глобально в проекте
// (в main.cpp или CMakeLists.txt настроены include-директории).
#include <glad/glad.h>

namespace render {

class GLRenderer {
public:
    /// Компилирует шейдеры, создаёт VAO/VBO/IBO для сферы и стола.
    /// \param shaderDir путь до папки, где лежат файлы "phong.vert" и "phong.frag"
    bool init(const std::string& shaderDir);

    /// Обновляет соотношение сторон для матрицы проекции (вызывается при старте и ресайзе).
    void resize(int w, int h);

    /// Рисует всю 3D-сцену: стол, борта, шары, карманы.
    /// \param balls   — вектор шаров (Box2D), у каждого можно взять позицию (метры) и радиус (px).
    /// \param pockets — список карманов (их центр и радиус уже в метрах).
    /// \param tableWpx — ширина стола в пикселях (1280).
    /// \param tableHpx — высота стола в пикселях (720).
    void drawScene(const std::vector<physics::Ball>&    balls,
                   const std::vector<physics::Pocket>& pockets,
                   float tableWpx,
                   float tableHpx);

private:
    // Приватные вспомогательные методы:

    /// Компилирует шейдер из текста `src` типа `type` (GL_VERTEX_SHADER или GL_FRAGMENT_SHADER).
    /// Возвращает ноль, если ошибка, иначе ID шейдера.
    unsigned int compileShader(const char* src, unsigned int type);

    /// Линкует вершинный (vs) и фрагментный (fs) шейдеры в одну программу.
    /// Возвращает ID программы или 0 при ошибке.
    unsigned int linkProgram(unsigned int vs, unsigned int fs);

    /// Генерирует меш сферы (VBO/IBO). \p radius — радиус в метрах, \p rings и \p sectors
    /// определяют детализацию. Заполняет m_vaoSphere, m_vboSphere, m_iboSphere, m_indexCount.
    void createSphereMesh(float radius, int rings, int sectors);

    /// Генерирует меш стола и бортов. \p wPx и \p hPx — размеры стола в пикселях,
    /// \p hBorderPx — высота борта в пикселях. Заполняет m_vaoTable, m_vboTable.
    void createTableMesh(float wPx, float hPx, float hBorderPx);

private:
    // OpenGL-объекты:
    unsigned int m_program      = 0;
    unsigned int m_vaoSphere    = 0;
    unsigned int m_vboSphere    = 0;
    unsigned int m_iboSphere    = 0;
    int          m_indexCount   = 0;

    unsigned int m_vaoTable     = 0;
    unsigned int m_vboTable     = 0;

    // Параметры камеры/матриц:
    float m_aspect = 1.0f;
    float m_yaw    = 25.f;     // угол вокруг Y
    float m_pitch  = -45.f;    // угол вокруг X
    float m_dist   = 8.f;      // расстояние камеры до центра
};

} // namespace render
