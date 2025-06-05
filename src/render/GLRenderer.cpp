#include "render/GLRenderer.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using namespace render;

namespace {
    // Вспомогательная функция чтения текстового файла в std::string
    static std::string readFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << path << "\n";
            return {};
        }
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }
}

bool GLRenderer::init(const std::string& shaderDir)
{
    // 1) Читаем и компилируем вершинный шейдер
    std::string vertCode = readFile(shaderDir + "/phong.vert");
    if (vertCode.empty()) return false;
    unsigned int vs = compileShader(vertCode.c_str(), GL_VERTEX_SHADER);
    if (vs == 0) {
        std::cerr << "Vertex shader compilation failed\n";
        return false;
    }

    // 2) Читаем и компилируем фрагментный шейдер
    std::string fragCode = readFile(shaderDir + "/phong.frag");
    if (fragCode.empty()) return false;
    unsigned int fs = compileShader(fragCode.c_str(), GL_FRAGMENT_SHADER);
    if (fs == 0) {
        std::cerr << "Fragment shader compilation failed\n";
        glDeleteShader(vs);
        return false;
    }

    // 3) Линкуем программу
    m_program = linkProgram(vs, fs);
    // После линковки удаляем шейдеры
    glDeleteShader(vs);
    glDeleteShader(fs);
    if (m_program == 0) {
        std::cerr << "Shader program linking failed\n";
        return false;
    }

    // 4) Создаём меши: сфера и стол
    // радиус шара: 10 px -> 10/100 = 0.1 m, но вы можете скорректировать
    createSphereMesh(/*radius*/ 0.1f, /*rings*/16, /*sectors*/32);
    createTableMesh(/*wPx*/ 1280.f, /*hPx*/720.f, /*hBorderPx*/ 20.f);

    // 5) Включаем тест глубины
    glEnable(GL_DEPTH_TEST);

    return true;
}

void GLRenderer::resize(int w, int h)
{
    if (h == 0) h = 1;
    m_aspect = static_cast<float>(w) / static_cast<float>(h);
}

unsigned int GLRenderer::compileShader(const char* src, unsigned int type)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // 1) Проверяем status
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (!isCompiled) {
        // 2) Узнаём длину лога
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // 3) Если лог есть — выделяем вектор нужного размера
        if (maxLength > 1) {
            std::vector<char> errorLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog.data());
            const char* shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
            std::cerr << "Error: " << shaderType << " shader compilation failed:\n"
                      << std::string(errorLog.data(), maxLength) << "\n";
        } else {
            // Если даже INFO_LOG пустой (maxLength ≤ 1)
            const char* shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
            std::cerr << "Error: " << shaderType
                      << " shader failed to compile, but INFO_LOG is empty\n";
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

unsigned int GLRenderer::linkProgram(unsigned int vs, unsigned int fs)
{
    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Проверяем статус линковки
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Error: Shader program linking failed:\n" << infoLog << "\n";
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

/// Генерация UV-сферы: создаём вершины и индексы, записываем в VBO/IBO.
void GLRenderer::createSphereMesh(float radius, int rings, int sectors)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    const float PI = 3.14159265358979323846f;

    for (int r = 0; r <= rings; ++r) {
        float theta = PI * r / rings;
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);

        for (int s = 0; s <= sectors; ++s) {
            float phi = 2.0f * PI * s / sectors;
            float sinPhi = std::sin(phi);
            float cosPhi = std::cos(phi);

            // Позиция
            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            vertices.push_back(x * radius);
            vertices.push_back(y * radius);
            vertices.push_back(z * radius);

            // Нормаль (просто нормализованная)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    for (int r = 0; r < rings; ++r) {
        for (int s = 0; s < sectors; ++s) {
            int first  = (r * (sectors + 1)) + s;
            int second = first + sectors + 1;
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    m_indexCount = static_cast<int>(indices.size());

    // Создаём VAO
    glGenVertexArrays(1, &m_vaoSphere);
    glBindVertexArray(m_vaoSphere);

    // VBO
    glGenBuffers(1, &m_vboSphere);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSphere);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);

    // IBO
    glGenBuffers(1, &m_iboSphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboSphere);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    // Позиция: layout(location = 0) в шейдере
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), (void*)0);

    // Нормаль: layout(location = 1) в шейдере
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), (void*)(3 * sizeof(float)));

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/// Создаём VBO для плоскости стола (две треугольных плоскости) и борта (четыре прямоугольника).
void GLRenderer::createTableMesh(float wPx, float hPx, float hBorderPx)
{
    // Конвертируем размеры из пикселей в метры (100 px = 1 m)
    const float PPM = 100.f;
    auto toM = [&](float px) { return px / PPM; };

    float halfW = toM(wPx) / 2.0f;
    float halfH = toM(hPx) / 2.0f;
    float borderHeight = toM(hBorderPx);

    // Сначала строим «плоскость» стола (четыре вершины)
    //  (x,y,z) → здесь z=0, y=0 (плоскость XZ)
    std::vector<float> data;

    // 6 координат (x, y, z) на каждый из шести треугольников (2 треугольника для плоскости, 
    // 4 треугольника для каждого борта). В данном примере мы упрощаем:
    // 1) Стол: две треугольных плоскости (по XZ, y=0):
    data.insert(data.end(), {
        -halfW, 0.0f, -halfH,
         halfW, 0.0f, -halfH,
        -halfW, 0.0f,  halfH,

         halfW, 0.0f, -halfH,
         halfW, 0.0f,  halfH,
        -halfW, 0.0f,  halfH
    });

    // 2) Борта (высота) — четыре боковых прямоугольника:
    // Левый борт (x = -halfW):
    data.insert(data.end(), {
        -halfW, 0.0f, -halfH,
        -halfW, borderHeight, -halfH,
        -halfW, 0.0f,  halfH,

        -halfW, borderHeight, -halfH,
        -halfW, borderHeight,  halfH,
        -halfW, 0.0f,  halfH
    });

    // Правый борт (x = +halfW)
    data.insert(data.end(), {
         halfW, 0.0f, -halfH,
         halfW, 0.0f,  halfH,
         halfW, borderHeight, -halfH,

         halfW, borderHeight, -halfH,
         halfW, 0.0f,  halfH,
         halfW, borderHeight,  halfH
    });

    // Передний борт (z = +halfH)
    data.insert(data.end(), {
        -halfW, 0.0f,  halfH,
         halfW, 0.0f,  halfH,
        -halfW, borderHeight,  halfH,

         halfW,  borderHeight,  halfH,
         halfW,  0.0f,  halfH,
        -halfW,  borderHeight,  halfH
    });

    // Задний борт (z = -halfH)
    data.insert(data.end(), {
        -halfW, 0.0f, -halfH,
        -halfW, borderHeight, -halfH,
         halfW, 0.0f, -halfH,

        -halfW, borderHeight, -halfH,
         halfW,  borderHeight, -halfH,
         halfW,  0.0f, -halfH
    });

    // Создаём VAO для стола
    glGenVertexArrays(1, &m_vaoTable);
    glBindVertexArray(m_vaoTable);

    // VBO
    glGenBuffers(1, &m_vboTable);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboTable);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float),
                 data.data(), GL_STATIC_DRAW);

    // Позиция: layout(location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(float), (void*)0);

    // В данном примере нормали и текстуры мы не задаём (в простейшем случае цвет задаётся шейдером).

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GLRenderer::drawScene(const std::vector<physics::Ball>&    balls,
                           const std::vector<physics::Pocket>& pockets,
                           float tableWpx,
                           float tableHpx)
{
    // 1) Очищаем цвет и глубину
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2) Активируем шейдерную программу
    glUseProgram(m_program);

    // 3) Вычисляем матрицы view и proj
    glm::mat4 proj = glm::perspective(
        glm::radians(45.0f),
        m_aspect,
        0.1f,
        50.0f
    );

    glm::vec3 eye = glm::vec3(
        m_dist * std::cos(glm::radians(m_pitch)) * std::sin(glm::radians(m_yaw)),
        m_dist * std::sin(glm::radians(m_pitch)),
        m_dist * std::cos(glm::radians(m_pitch)) * std::cos(glm::radians(m_yaw))
    );
    glm::mat4 view = glm::lookAt(eye, glm::vec3(0,0,0), glm::vec3(0,1,0));

    // 4) Передаём матрицы в шейдер
    int locView = glGetUniformLocation(m_program, "uView");
    glUniformMatrix4fv(locView, 1, GL_FALSE, &view[0][0]);

    int locProj = glGetUniformLocation(m_program, "uProj");
    glUniformMatrix4fv(locProj, 1, GL_FALSE, &proj[0][0]);

    int locViewPos = glGetUniformLocation(m_program, "uView");
    glUniform3f(locViewPos, eye.x, eye.y, eye.z);

    // 5) Рисуем стол + борта
    glBindVertexArray(m_vaoTable);
    glm::mat4 identity = glm::mat4(1.0f);

    // Цвет стола (тёмно-зелёный)
    int locColor = glGetUniformLocation(m_program, "uColor");
    glUniform3f(locColor, 0.05f, 0.35f, 0.05f);

    // uModel = identity
    int locModel = glGetUniformLocation(m_program, "uModel");
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &identity[0][0]);

    // Всего вершин: 6 для плоскости + 6*4 для четырёх бортов = 30
    glDrawArrays(GL_TRIANGLES, 0, 30);

    // 6) Рисуем шары
    glBindVertexArray(m_vaoSphere);
    glUniform3f(locColor, 0.9f, 0.9f, 0.9f);  // белые шары

    for (const auto& b : balls) {
        b2Vec2 posM = b.body()->GetPosition();
        float   rM  = (b.radius() / 100.f);      // px → m (PPM = 100)

        // Модельная матрица: сначала сдвигаем (x, r, z), потом масштабируем
        glm::mat4 model = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(posM.x, rM, posM.y)
        ) * glm::scale(
            glm::mat4(1.0f),
            glm::vec3(rM, rM, rM)
        );

        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    }

    // 7) Привязывать обратно не обязательно, 
    // но во избежание «залипания»:
    glBindVertexArray(0);
    glUseProgram(0);
}

