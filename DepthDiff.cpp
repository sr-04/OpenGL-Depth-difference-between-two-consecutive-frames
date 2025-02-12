#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>

// Window dimensions
const int WIDTH = 800, HEIGHT = 600;

// Depth buffers for storing previous and current frame depth values
std::vector<float> prevDepthBuffer(WIDTH * HEIGHT, 1.0f);
std::vector<float> currDepthBuffer(WIDTH * HEIGHT, 1.0f);

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void calculateDepthDifference();
void renderCube();

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set OpenGL version to 4.1 (macOS max support)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required for macOS

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Depth Difference OpenGL", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Process user input
        processInput(window);

        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // Render scene (rotating cube)
        renderCube();

        // Read depth buffer of the current frame
        glReadPixels(0, 0, WIDTH, HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT, currDepthBuffer.data());

        // Compute depth difference
        calculateDepthDifference();

        // Swap depth buffers for next frame comparison
        prevDepthBuffer = currDepthBuffer;

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup and exit
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// Handle window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Process keyboard input
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Function to compute depth difference
void calculateDepthDifference() {
    float totalDepthDiff = 0.0f;
    int changedPixels = 0;

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        float diff = std::abs(currDepthBuffer[i] - prevDepthBuffer[i]);
        if (diff > 0.001f) { // Ignore small variations
            totalDepthDiff += diff;
            changedPixels++;
        }
    }

    // Print depth change info
    if (changedPixels > 0) {
        std::cout << "Depth change detected in " << changedPixels << " pixels, Average change: "
                  << (totalDepthDiff / changedPixels) << std::endl;
    }
}

// Function to render a simple cube
void renderCube() {
    static float angle = 0.0f;
    angle += 0.01f; // Rotate cube slightly each frame

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -3.0f); // Move cube back
    glRotatef(angle, 1.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);

    // Define cube faces
    glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glColor3f(1.0f, 1.0f, 0.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

    // Other cube faces...
    glEnd();
    glPopMatrix();
}