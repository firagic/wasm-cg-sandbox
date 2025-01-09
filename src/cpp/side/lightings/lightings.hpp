#define __MODULE_INTERFACE__
#include "../../../hpp/globals.hpp"

#include <fstream>

#include "lodepng/lodepng.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }


// class Shader
// struct Shader
// {
// public:
    // unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    unsigned int generate_shader(const char* vertexPath, const char* fragmentPath)
    {
        unsigned int ID;
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try 
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();		
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();		
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char * vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        // std::cout << "Vertex code " << vertexCode << std::endl;	

        return ID;

    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use_shader(unsigned int shader_id)// const
    { 
        glUseProgram(shader_id); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void set_bool(unsigned int shader_id, const std::string &name, bool value)// const
    {         
        glUniform1i(glGetUniformLocation(shader_id, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void set_int(unsigned int shader_id, const std::string &name, int value)// const
    { 
        glUniform1i(glGetUniformLocation(shader_id, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void set_float(unsigned int shader_id, const std::string &name, float value)// const
    { 
        glUniform1f(glGetUniformLocation(shader_id, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void set_vec2(unsigned int shader_id, const std::string &name, const glm::vec2 &value)// const
    { 
        glUniform2fv(glGetUniformLocation(shader_id, name.c_str()), 1, &value[0]); 
    }
    void set_vec2(unsigned int shader_id, const std::string &name, float x, float y)// const
    { 
        glUniform2f(glGetUniformLocation(shader_id, name.c_str()), x, y); 
    }
    // ------------------------------------------------------------------------
    void set_vec3(unsigned int shader_id, const std::string &name, const glm::vec3 &value)// const
    { 
        glUniform3fv(glGetUniformLocation(shader_id, name.c_str()), 1, &value[0]); 
    }
    void set_vec3(unsigned int shader_id, const std::string &name, float x, float y, float z)// const
    { 
        glUniform3f(glGetUniformLocation(shader_id, name.c_str()), x, y, z); 
    }
    // ------------------------------------------------------------------------
    void set_vec4(unsigned int shader_id, const std::string &name, const glm::vec4 &value)// const
    { 
        glUniform4fv(glGetUniformLocation(shader_id, name.c_str()), 1, &value[0]); 
    }
    void set_vec4(unsigned int shader_id, const std::string &name, float x, float y, float z, float w)// const
    { 
        glUniform4f(glGetUniformLocation(shader_id, name.c_str()), x, y, z, w); 
    }
    // ------------------------------------------------------------------------
    void set_mat2(unsigned int shader_id, const std::string &name, const glm::mat2 &mat)// const
    {
        glUniformMatrix2fv(glGetUniformLocation(shader_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void set_mat3(unsigned int shader_id, const std::string &name, const glm::mat3 &mat)// const
    {
        glUniformMatrix3fv(glGetUniformLocation(shader_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void set_mat4(unsigned int shader_id, const std::string &name, const glm::mat4 &mat)// const
    {
        glUniformMatrix4fv(glGetUniformLocation(shader_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

// private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
// };


// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(void * arg);
unsigned int loadTexture(const char *path);
unsigned char* load_png(const char* filename, int* width, int* height, int* nrComponents);

// settings
// const unsigned int SCR_WIDTH = 800;
// const unsigned int SCR_HEIGHT = 600;

// camera
// Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
// float lastX = SCR_WIDTH / 2.0f;
// float lastY = SCR_HEIGHT / 2.0f;
// bool firstMouse = true;

// timing
// float deltaTime = 0.0f;
// float lastFrame = 0.0f;

// lighting
// glm::vec3 lightPos(1.2f, 1.0f, 2.0f);



typedef struct   
{
    unsigned int lightingShaderID;
    unsigned int lightCubeShaderID;
    unsigned int cubeVAO;
    unsigned int lightCubeVAO;
    unsigned int VBO;  

    unsigned int diffuseMap;
    unsigned int specularMap;

    // lighting
    // glm::vec3 * pointLightPositions;
    // glm::vec3 * cubePositions;
    void * pointLightPositions;
    void * cubePositions;
    void * lightPos;

    // camera
    Camera * camera;
    float lastX;
    float lastY;
    bool firstMouse;

    // timing
    float deltaTime;
    float lastFrame;
    
    GLFWwindow * window;



} ModuleData;