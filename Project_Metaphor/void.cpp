#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model3D.h"
#include "Light.h"
#include "DirectionLight.h"
#include "PointLight.h"
#include "ColorLight.hpp"
#include "Camera.h"
#include "OrthoCamera.h"
#include "PerspectiveCamera.h"

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

//general movement
// 

float x = 0.f, y = 0.f, z = -20.0f;
//false = main, true = sphere
bool stateControl = false;
bool stateCam = false;

float x_scale = 0.5f, y_scale = 0.5f, z_scale = 0.5f;

float thetaX = 1.f, thetaY = 1.f;

float x_axisX = 0.f, y_axisX = 1.f, z_axisX = 0.f;

float x_axisY = 1.f, y_axisY = 0.f, z_axisY = 0.f;

//for all time related additions
float deltaTime = 0.0f;	
float lastFrame = 0.0f; 
float coolDown = 0.0f;

//initialize camera vars
glm::vec3 cameraPos = glm::vec3(0, 0, 2.f);
glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
glm::vec3 Front = glm::vec3(0, 0.0f, -1);
//initialize for mouse movement
bool firstMouse = true;
float pitch = 0.0f;
float yaw = -90.0f;

//for initial mouse movement
float lastX = 400, lastY = 400;

float height = 800.0f;
float width = 800.0f;

//Initializing the object classes to be rendered
Model3D main_object({ 0,0,0 });
Model3D light_ball({ 0,1,0 });
OrthoCamera orca({ 0,2,0 });
PerspectiveCamera perca({ 0,0,0 }, height, width);

//Point light variables
float brightness = 3.0f;
float dl_brightness = 1.0f;

glm::vec3 sphere_color = { 1.f,1.f,1.f };

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{   
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) // for first mouse move
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    // calculate offsets to add influence front
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; //reversed
    lastX = xpos;
    lastY = ypos;

    //set how STRONG the movement is
    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    //for front calculation
    yaw += xoffset;
    pitch += yoffset;

    //making sure that you can't 360 via neck breaking
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    //setting front and the change
    if (stateCam == false) {
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * 2;
        direction.y = sin(glm::radians(pitch)) * 2;
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * 2;
        perca.setCameraPos(direction);
        //std::cout << "Mouse" << direction.x << "" << direction.y << "" << direction.z << std::endl;
        perca.setFront(glm::vec3(-perca.getCameraPos().x, -perca.getCameraPos().y, -perca.getCameraPos().z));
    }
}

void Key_Callback(GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods) {

    float cameraSpeed = 400.0f * deltaTime;// set Strength of movement
    
    //ROTATES WHATEVER IS TOGGLED
    if (glfwGetKey(window, GLFW_KEY_D)) {
        if (stateControl) {
            light_ball.rotate('y', 0);
        }
        else {
            main_object.rotate('y', 0);
        }
        
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
        
        if (stateControl) {
            light_ball.rotate('y', 1);
        }
        else {
            main_object.rotate('y', 1);
        }

    }

    //xRot
    if (glfwGetKey(window, GLFW_KEY_S)) {
        if (stateControl) {
            light_ball.rotate('x', 0);
        }
        else {
            main_object.rotate('x', 0);
        }
        
    }
    if (glfwGetKey(window, GLFW_KEY_W)) {
        if (stateControl) {
            light_ball.rotate('x', 1);
        }
        else {
            main_object.rotate('x', 1);
        }
        
    }

    //zRot
    if (glfwGetKey(window, GLFW_KEY_Q)) {
        if (stateControl) {
            light_ball.rotate('z', 0);
        }
        else {
            main_object.rotate('z', 0);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_E)) {
        if (stateControl) {
            light_ball.rotate('z', 1);
        }
        else {
            main_object.rotate('z', 1);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE)) {
        if (stateControl == false) { //set it to be able to control the light sphere
            sphere_color = glm::vec3{ 0.f,1.f,0.f };
            stateControl = true;
        }
        else if (stateControl == true) { //set it to be able to control the main object
            sphere_color = glm::vec3{ 1.f,1.f,1.f };
            stateControl = false;
        } 
    }

    if (glfwGetKey(window, GLFW_KEY_1)) {
        stateCam = false; //changes the camera into perspective mode
    }
    if (glfwGetKey(window, GLFW_KEY_2)) {
        stateCam = true; //changes the camera into ortho mode
    }
    if (glfwGetKey(window, GLFW_KEY_UP) && stateControl) brightness += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) && stateControl) brightness -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) && stateControl) dl_brightness -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) && stateControl) dl_brightness += 1.0f;
};

GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    // Read vertex shader source
    std::ifstream vertexFile(vertexPath);
    std::stringstream vertexStream;
    vertexStream << vertexFile.rdbuf();
    std::string vertexCode = vertexStream.str();
    const char* vertexShaderSource = vertexCode.c_str();

    // Read fragment shader source
    std::ifstream fragmentFile(fragmentPath);
    std::stringstream fragmentStream;
    fragmentStream << fragmentFile.rdbuf();
    std::string fragmentCode = fragmentStream.str();
    const char* fragmentShaderSource = fragmentCode.c_str();

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for vertex shader compile errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for fragment shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Clean up shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Reblando und Jaropojop", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    /*      TEXTURE     */
    int img_width, img_height, colorChannels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* tex_bytes = stbi_load(
        "3D/ayaya.png",
        &img_width,
        &img_height,
        &colorChannels,
        0
    );

    GLuint texture;

    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGBA,
        img_width,
        img_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        tex_bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);

    //faces of the skybox
    std::string facesSkybox[]{
        "Skybox/image3x2.png", //RIGHT
        "Skybox/image1x2.png", //left
        "Skybox/image2x1.png", //up
        "Skybox/image2x3.png", //down
        "Skybox/image2x2.png", //front
        "Skybox/image4x2.png", //back
    };

    unsigned int skyboxTex;
    glGenTextures(1, &skyboxTex);

    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++)
    {
        int w, h, skyCChannel;

        stbi_set_flip_vertically_on_load(false);

        unsigned char* data = stbi_load(facesSkybox[i].c_str(), &w, &h, &skyCChannel, 0);

        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGBA,
                w,
                h,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                data
            );

            stbi_image_free(data);
        }
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, 800, 800);

    glfwSetKeyCallback(window, Key_Callback);

    //      SHADERS
    GLuint mainObjShader = createShaderProgram("Shaders/mainObj.vert", "Shaders/mainObj.frag");
    GLuint sphereShader = createShaderProgram("Shaders/sphere.vert", "Shaders/sphere.frag");
    GLuint skyboxShaderProg = createShaderProgram("Shaders/skybox.vert", "Shaders/skybox.frag");

    //set cursor to NONEXISTANT and make mouse events be called
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    //credits to original artist Kenshin Reblando for art.
    std::string path = "3D/Cross.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &material,
        &warning,
        &error,
        path.c_str()
    );  

    GLfloat UV[]{
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        0.f, 0.f
    };
   
    //array of Mesh for the main object
    std::vector<GLfloat> fullVertexData;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        //vertex
        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3)]
        );

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 1]
        );

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 2]
        );

        //normal
        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3)]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 1]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 2]
        );

        //texcoord
        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2)]
        );

        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2) + 1]
        );

    }

    //credits to original artist Kenshin Reblando for art.
    std::string sphere_path = "3D/sphere.obj";
    std::vector<tinyobj::shape_t> sphere_shapes;
    std::vector<tinyobj::material_t> sphere_material;

    tinyobj::attrib_t sphere_attributes;

    bool sphere_success = tinyobj::LoadObj(
        &sphere_attributes,
        &sphere_shapes,
        &sphere_material,
        &warning,
        &error,
        sphere_path.c_str()
    );

    //array of mesh for the light ball
    std::vector<GLfloat> ball_fullVertexData;
    for (int i = 0; i < sphere_shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = sphere_shapes[0].mesh.indices[i];

        //vertex
        ball_fullVertexData.push_back(
            sphere_attributes.vertices[(vData.vertex_index * 3)]
        );

        ball_fullVertexData.push_back(
            sphere_attributes.vertices[(vData.vertex_index * 3) + 1]
        );

        ball_fullVertexData.push_back(
            sphere_attributes.vertices[(vData.vertex_index * 3) + 2]
        );

        //normal
        ball_fullVertexData.push_back(
            sphere_attributes.normals[(vData.normal_index * 3)]
        );

        ball_fullVertexData.push_back(
            sphere_attributes.normals[(vData.normal_index * 3) + 1]
        );

        ball_fullVertexData.push_back(
            sphere_attributes.normals[(vData.normal_index * 3) + 2]
        );

        //texcoord
        ball_fullVertexData.push_back(
            sphere_attributes.texcoords[(vData.texcoord_index * 2)]
        );

        ball_fullVertexData.push_back(
            sphere_attributes.texcoords[(vData.texcoord_index * 2) + 1]
        );

    }
    
    //SKYBOX
    //Vertices for the cube
    float skyboxVertices[]{
        -1.f, -1.f, 1.f, //0
        1.f, -1.f, 1.f,  //1
        1.f, -1.f, -1.f, //2
        -1.f, -1.f, -1.f,//3
        -1.f, 1.f, 1.f,  //4
        1.f, 1.f, 1.f,   //5
        1.f, 1.f, -1.f,  //6
        -1.f, 1.f, -1.f  //7
    };

    //Skybox Indices
    unsigned int skyboxIndices[]{
        1,2,6,
        6,5,1,

        0,4,7,
        7,3,0,

        4,5,6,
        6,7,4,

        0,3,2,
        2,1,0,

        0,1,5,
        5,4,0,

        3,7,6,
        6,2,3
    };

    //main object
    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat)* fullVertexData.size(),
        fullVertexData.data(),
        GL_DYNAMIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    GLintptr litPtr = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)litPtr
    );

    glEnableVertexAttribArray(1);

    GLintptr uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)uvPtr
    );

    glEnableVertexAttribArray(2);

    //sphere object
    GLuint sphere_VAO, sphere_VBO;

    glGenVertexArrays(1, &sphere_VAO);
    glGenBuffers(1, &sphere_VBO);

    glBindVertexArray(sphere_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat)* ball_fullVertexData.size(),
        ball_fullVertexData.data(),
        GL_DYNAMIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    GLintptr sphere_litPtr = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)sphere_litPtr
    );

    glEnableVertexAttribArray(1);

    GLintptr sphere_uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)sphere_uvPtr
    );

    glEnableVertexAttribArray(2);

    //skybox
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);

    glBindVertexArray(skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skyboxIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);

    //SET BINDINGS TO NULL
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glm::mat4 identity_matrix = glm::mat4(1.0f);

    /*glm::mat4 projection = glm::ortho(
        -2.f, //left
        2.f, //right
        - 2.f, //bot
        2.f, //top
        -1.f, //z near
        1.f);  //z far*/

        /*      CAMERA VARIABLES*/
    glm::mat4 projection = glm::perspective(
        glm::radians(60.f),//FOV
        height / width, //aspect ratio
        0.1f, //znear > 0
        1000.f //zfar
    );

    /*      LIGHT VARIABLES     */
    glm::vec3 lightPos = glm::vec3(-10, 10, 0);

    glm::vec3 lightColor = glm::vec3(1, 1, 1);

    float ambientStr = 0.2f;

    glm::vec3 ambientColor = lightColor;

    float specStr = 0.5f;

    float specPhong = 8;

    /*      Light Classes       */
    glm::vec3 lightDirection = { 4,-5,0 };
    DirectionLight directionLight(lightPos, lightColor, ambientStr, ambientColor, specStr, specPhong, lightDirection, dl_brightness);
    PointLight pointLight(light_ball.getPosition(), lightColor, ambientStr, ambientColor, specStr, specPhong, brightness);
    ColorLight colorLight;

    main_object.setScale(glm::vec3{ 0.05f,0.05f,0.05f });

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        //set time so that movement is uniform for all devices if needed
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        coolDown -= 1 * deltaTime;

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 viewMatrix;

        //set camera to be MOVEABLE i.e. can be influenced
        if (stateCam) {
            //set the camera to ortho
            viewMatrix = glm::lookAt(orca.getCameraPos(), orca.getCameraPos() + orca.getFront(), orca.getWorldUp());
        }
        else {
            //set the camera to perspective
            viewMatrix = glm::lookAt(perca.getCameraPos(), perca.getCameraPos() + perca.getFront(), perca.getWorldUp());
        }

        //skybox
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glUseProgram(skyboxShaderProg);

        glm::mat4 sky_view = glm::mat4(1.f);
        sky_view = glm::mat4(
            //cast the same view matrix of the camera turn it into a mat3 to remove translations
            glm::mat3(viewMatrix)
            //then reconvert it to a mat4
        );

        unsigned int skyboxViewLoc = glGetUniformLocation(skyboxShaderProg, "view");
        glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(sky_view));

        unsigned int skyboxProjLoc = glGetUniformLocation(skyboxShaderProg, "projection");
        glUniformMatrix4fv(skyboxProjLoc,
            1,
            GL_FALSE,
            glm::value_ptr(projection));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        /*      MAIN OBJECT     */
        /*      TEXTURE OF MAIN OBJECT      */
        glUseProgram(mainObjShader);
        glUniform1i(glGetUniformLocation(mainObjShader, "tex0"), 0);
        glBindTexture(GL_TEXTURE_2D, texture);

        //      DRAWING THE MAIN OBJ

        pointLight.setPosition(light_ball.getPosition(true)); //set the current position of the light sphere to the lightPos
        pointLight.setBrightness(brightness);
        directionLight.setBrightness(dl_brightness);

        //attaches the same values for direction and point light
        directionLight.attachFundamentals(&mainObjShader);
        pointLight.attachFundamentals(&mainObjShader);

        //attaches the specific values of each light
        directionLight.attachSpecifics(&mainObjShader);
        pointLight.attachSpecifics(&mainObjShader);

        if (stateCam) {
            //set the camera to ortho
            main_object.setCamera(orca.getProjection(), orca.getCameraPos(), orca.getFront(), orca.getViewMat());
        }
        else {
            //set the camera to perspective
            main_object.setCamera(perca.getProjection(), perca.getCameraPos(), perca.getFront(), perca.getViewMat());
        }

        //draw
        main_object.mainDraw(&mainObjShader, &VAO, &fullVertexData);

        /*      SPHERE OBJECT       */
        glUseProgram(sphereShader);
        if (stateCam) {
            //set the camera to ortho
            light_ball.setCamera(orca.getProjection(), orca.getCameraPos(), orca.getFront(), orca.getViewMat());
        }
        else {
            //set the camera to perspective
            light_ball.setCamera(perca.getProjection(), perca.getCameraPos(), perca.getFront(), perca.getViewMat());
        }

        colorLight.setColor(sphere_color); //change the color
        colorLight.perform(&sphereShader); //attaches the values of light into the shader program of sphere
        light_ball.sphereDraw(&sphereShader, &sphere_VAO, &ball_fullVertexData);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &sphere_VAO);
    glDeleteBuffers(1, &sphere_VBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteBuffers(1, &skyboxEBO);

    glfwTerminate();
    return 0;
}