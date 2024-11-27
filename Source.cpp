#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/glm/glm.hpp>
#include <vector>

using namespace std;
using namespace glm;

struct Vertex
{
    vec3 Position;
    vec3 Color;
};

GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);

const GLint WIDTH = 600, HEIGHT = 600;
GLuint VBO, VAO, ProgramID;
std::vector<std::vector<Vertex>> AllLineVertices;  // List of lines, each with its own vertices

bool isDrawing = false; // Flag to track if the user is currently drawing
std::vector<Vertex> CurrentLineVertices;  // Current line vertices, being drawn

// Function to compile shaders and set up the program
void CompileShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name, GLuint& programId)
{
    programId = InitShader(vertex_shader_file_name, fragment_shader_file_name);
    glUseProgram(programId);
}

int Init()
{
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        cout << "Error initializing GLEW\n";
        getchar();
        return 1;
    }

    cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << endl;
    cout << "\tVendor: " << glGetString(GL_VENDOR) << endl;
    cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
    cout << "\tVersion: " << glGetString(GL_VERSION) << endl;
    cout << "\tGLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    // Compile shaders and create program
    CompileShader("VS.glsl", "FS.glsl", ProgramID);

    // Generate VAO and VBO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec3)));
    glEnableVertexAttribArray(1);

    glClearColor(0, 0.5, 0.5, 1);

    return 0;
}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);

    // Loop over all lines and render them separately
    for (const auto& line : AllLineVertices) {
        glBufferData(GL_ARRAY_BUFFER, line.size() * sizeof(Vertex), line.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINE_STRIP, 0, line.size());
    }

    // Also render the current line being drawn
    if (isDrawing) {
        glBufferData(GL_ARRAY_BUFFER, CurrentLineVertices.size() * sizeof(Vertex), CurrentLineVertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINE_STRIP, 0, CurrentLineVertices.size());
    }
}

void StartNewLine()
{
    // When the user starts drawing, create a new line's vertices
    CurrentLineVertices.clear();
}

void AddLineVertex(float x, float y)
{
    // Convert mouse coordinates to OpenGL normalized device coordinates
    float nx = (x / WIDTH) * 2.0f - 1.0f;
    float ny = -((y / HEIGHT) * 2.0f - 1.0f);
    std::cout << "hh" << std::endl;

    // Add the vertex to the current line's list with a default color (e.g., red)
    CurrentLineVertices.push_back({ vec3(nx, ny, 0.0f), vec3(1.0f, 0.0f, 0.0f) });
}

int main()
{
    sf::ContextSettings context;
    context.depthBits = 24;
    sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "Draw Lines by Dragging", sf::Style::Close, context);

    if (Init()) return 1;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                isDrawing = true;

                // Start a new line and begin adding vertices
                StartNewLine();
                AddLineVertex(event.mouseButton.x, event.mouseButton.y);
            }
            else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
            {
                isDrawing = false;

                // After finishing a line, add it to the list of all lines
                AllLineVertices.push_back(CurrentLineVertices);
            }
            else if (event.type == sf::Event::MouseMoved && isDrawing)
            {
                AddLineVertex(event.mouseMove.x, event.mouseMove.y);
            }
        }

        // Render all lines and the current line in real-time
        Render();
        window.display();
    }
    return 0;
}
