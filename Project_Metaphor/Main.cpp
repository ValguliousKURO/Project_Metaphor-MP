#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model3D.h"
#include "Shader.h"
#include "Light.h"
#include "DirectionLight.h"
#include "PointLight.h"
#include "ColorLight.hpp"
#include "Camera.h"
#include "OrthoCamera.h"
#include "PerspectiveCamera.h"
#include "Stopwatch.h"
#include "Player.h"
#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define FINISH_LINE 231

//general movement
// 

bool skyState = false;
bool stateGhost = false;


float x = 0.f, y = 0.f, z = -20.0f;
//false = main, true = sphere
bool stateControl = false;
bool stateCam = false;
bool finishState = false;
bool finishState2 = false;
bool finishState3 = false;
bool stateSkybox = false; //false = morning, true = night Rafael Ira R. Villanueva

float x_scale = 0.5f, y_scale = 0.5f, z_scale = 0.5f;

float thetaX = 1.f, thetaY = 1.f;

float x_axisX = 0.f, y_axisX = 1.f, z_axisX = 0.f;

float x_axisY = 1.f, y_axisY = 0.f, z_axisY = 0.f;



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
Player player_car({ 0,0,0 });
Model3D road({ 0,0,0 });
Model3D landmark_1({ 0,0,0 });
Model3D landmark_2({ 0,0,0 });
Model3D ghost_car1({ 0,0,0 });
Model3D ghost_car2({ 0,0,0 });
Model3D plane_road({ 0,0,0 });
//Model3D light_ball({ 0,1,0 });
OrthoCamera orca({ 0,2,0 });    
PerspectiveCamera perca({ 0,0,0 }, height, width);
Stopwatch stopwatch;
Stopwatch stopwatch2;
Stopwatch stopwatch3;


//Point light variables
float brightness = 0.0f;
float dl_brightness = 2.0f;

glm::vec3 sphere_color = { 1.f,1.f,1.f };
float carSpeed = 0.0f;
float ghostSpeed1 = 0.0f;
float ghostSpeed2 = 0.0f;

std::string facesSkyboxMorning[]{
   "Skybox/morning_rt.png",
   "Skybox/morning_lf.png",
   "Skybox/morning_up.png", 
   "Skybox/morning_dn.png",
   "Skybox/morning_ft.png",
   "Skybox/morning_bk.png"
};

std::string facesSkyboxNight[]{
   "Skybox/night_rt.png",
   "Skybox/night_lf.png",
   "Skybox/night_up.png", 
   "Skybox/night_dn.png",
   "Skybox/night_ft.png",
   "Skybox/night_bk.png"
};

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
    float yoffset = ypos -lastY; //reversed
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
    /*if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;*/

    //setting front and the change
   // if (stateCam == false) {
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * 2;
        direction.y = sin(glm::radians(pitch)) * 2;
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * 2;
        perca.setCameraPos(direction);
        //std::cout << "Mouse" << direction.x << "" << direction.y << "" << direction.z << std::endl;
        //perca.updateCameraPosition(player_car.getPosition());
        perca.setFront(glm::vec3(-perca.getCameraPos().x, -perca.getCameraPos().y, -perca.getCameraPos().z));


        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
        lastX = windowWidth / 2;
        lastY = windowHeight / 2;
    //}
    /*else if (stateCam == true){
       

    }*/
}

//perca.updateCameraPosition(player_car.getPosition());



void Key_Callback(GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods) {

    float turnSpeed = 0.3f;
	



    //float turnSpeed = 0.05f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (finishState == false) {
            if (carSpeed < 0)
            {
                carSpeed = 0.005;

            }

            else if (carSpeed < 1)
            {
                carSpeed += 0.005;
            }
            std::cout << "Speed: " << carSpeed << std::endl;
            player_car.translate(glm::vec3{ 0,0,1 } *carSpeed);
		}
		else if (finishState == true) {
			carSpeed = 0;
			std::cout << "Speed: " << carSpeed << std::endl;
			player_car.translate(glm::vec3{ 0,0,0 } *carSpeed);
		}

    }




    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) //Will fix this later
    {
        if (carSpeed > 0)               //Rafael Ira R. Villanueva
        {
            carSpeed = -0.005;           //Rafael Ira R. Villanueva
        }                                //Rafael Ira R. Villanueva

        else if (carSpeed > -1) 
        {
            carSpeed -= 0.005;
        }

        std::cout << "Speed: " << carSpeed << std::endl; //Rafael Ira R. Villanueva
        player_car.translate(glm::vec3{ 0,0,1 } * carSpeed); //-perca original Rafael Ira R. Villanueva

		//player_car.translate(player_car.getPosition() * -carSpeed);
        //perca.updateCameraPosition(player_car.getPosition());
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
    {
        player_car.translate(glm::vec3{ 1,0,0 } * turnSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        player_car.translate(-glm::vec3{ 1,0,0 } * turnSpeed);
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        
        if (stateGhost == true) {
            std::cout << "Ghost Stop" << std::endl;
            stateGhost = false;
            
		}
		else if (stateGhost == false) {
            std::cout << "Ghost Start" << std::endl;
            stateGhost = true;
            
		}
		
       
    }




    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        
        if (stateCam == true) {
            stateCam = false;
            perca.setFront(glm::vec3(-perca.getCameraPos().x, -perca.getCameraPos().y, -perca.getCameraPos().z));
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 
            glfwSetCursorPosCallback(window, nullptr) ;
			
        }
		else if(stateCam == false){
			stateCam = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetCursorPosCallback(window, mouse_callback);

		}
    }


    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
		dl_brightness = 2.0f;
		brightness = 0.0f;
        stateSkybox = false;
        std::cout << "Morning" << std::endl;

        for (unsigned int i = 0; i < 6; i++)
        {
            int w, h, skyCChannel;

            stbi_set_flip_vertically_on_load(false);
            unsigned char* data = stbi_load(facesSkyboxMorning[i].c_str(), &w, &h, &skyCChannel, 0);

            if (data) {
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    GL_RGBA, //Rafael Ira R. Villanueva Orig RGB
                    w,
                    h,
                    0,
                    GL_RGBA, //Rafael Ira R. Villanueva Orig RGB
                    GL_UNSIGNED_BYTE,
                    data
                );

                stbi_image_free(data);
            }
        }
    }

    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        dl_brightness = 0.5f;
		brightness = 1.0f;
        stateSkybox = true;
        std::cout << "Night" << std::endl;

        for (unsigned int i = 0; i < 6; i++)
        {
            int w, h, skyCChannel;

            stbi_set_flip_vertically_on_load(false);
            unsigned char* data = stbi_load(facesSkyboxNight[i].c_str(), &w, &h, &skyCChannel, 0);

            if (data) {
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    GL_RGBA, //Rafael Ira R. Villanueva Orig RGB
                    w,
                    h,
                    0,
                    GL_RGBA, //Rafael Ira R. Villanueva Orig RGB
                    GL_UNSIGNED_BYTE,
                    data
                );

                stbi_image_free(data);
            }
        }
    }
   
  
};

int main(void)
{
    GLFWwindow* window;
    
    player_car.setScale(glm::vec3{ 0.35f,0.35f,0.35f });
	
	landmark_1.setPosition(glm::vec3{ -3.5, 0,227.0f });
	landmark_1.setRotation(0, 90, 0);
    
    landmark_2.setPosition(glm::vec3{ 3.5f, 0, 229.5f });
    landmark_2.setRotation(0, 180, 0);
    
    ghost_car1.setPosition(glm::vec3{ 2,0,0 });
    ghost_car1.setScale(glm::vec3{ 0.35f,0.35f,0.35f });
	
    ghost_car2.setPosition(glm::vec3{ -2,0,0 });
	ghost_car2.setScale(glm::vec3{ 0.35f,0.35f,0.35f });

	plane_road.setPosition(glm::vec3{ 0,0,110 });
	plane_road.setScale(glm::vec3{ 5.5f,2.5f, 120.5f });
	plane_road.setRotation(270, 0, 0);

	
	

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Dacanay & Villanueva GDGRAP MP", NULL, NULL);
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
        "3D/Car8_purple.png",
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
        GL_RGB,
        img_width,
        img_height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        tex_bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);

    //LANDMARK 1
    int lm1_img_width, lm1_img_height, lm1_colorChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* lm1_tex_bytes = stbi_load(
        "3D/Sandy.png", // Path to your texture image
        &lm1_img_width,
        &lm1_img_height,
        &lm1_colorChannels,
        0
    );

    GLuint lm1_texture;
    glGenTextures(1, &lm1_texture);
    glActiveTexture(GL_TEXTURE1); // Use a different texture unit if needed
    glBindTexture(GL_TEXTURE_2D, lm1_texture);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGBA,
        lm1_img_width,
        lm1_img_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        lm1_tex_bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(lm1_tex_bytes);

   //LANDMARK 2
    
    int lm2_img_width, lm2_img_height, lm2_colorChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* lm2_tex_bytes = stbi_load(
        "3D/Billy.png", // Path to your texture image
        &lm2_img_width,
        &lm2_img_height,
        &lm2_colorChannels,
        0
    );

    GLuint lm2_texture;
    glGenTextures(1, &lm2_texture);
    glActiveTexture(GL_TEXTURE2); // Use a different texture unit if needed
    glBindTexture(GL_TEXTURE_2D, lm2_texture);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGB,
        lm2_img_width,
        lm2_img_height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        lm2_tex_bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(lm2_tex_bytes);
  


    int ghost1_img_width, ghost1_img_height, ghost1_colorChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* ghost1_tex_bytes = stbi_load(
        "3D/police.png", // Path to your texture image
        &ghost1_img_width,
        &ghost1_img_height,
        &ghost1_colorChannels,
        0
    );

    GLuint ghost1_texture;
    glGenTextures(1, &ghost1_texture);
    glActiveTexture(GL_TEXTURE3); // Use a different texture unit if needed
    glBindTexture(GL_TEXTURE_2D, ghost1_texture);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGBA,
        ghost1_img_width,
        ghost1_img_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        ghost1_tex_bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(ghost1_tex_bytes);



    int plane_img_width, plane_img_height, plane_colorChannels; 
    stbi_set_flip_vertically_on_load(true); 
    unsigned char* plane_tex_bytes = stbi_load( 
        "3D/grey.jpg", // Path to your texture image
        &plane_img_width, 
        &plane_img_height,  
        &plane_colorChannels, 
        0 
    );

    GLuint plane_texture; 
    glGenTextures(1, &plane_texture); 
    glActiveTexture(GL_TEXTURE4);  // Use a different texture unit if needed
    glBindTexture(GL_TEXTURE_2D, plane_texture); 

    glTexImage2D(GL_TEXTURE_2D, 
        0,
        GL_RGB,
        plane_img_width, 
        plane_img_height, 
        0,
        GL_RGB, 
        GL_UNSIGNED_BYTE, 
        plane_tex_bytes); 
    glGenerateMipmap(GL_TEXTURE_2D); 
    stbi_image_free(plane_tex_bytes); 



    int ghost2_img_width, ghost2_img_height, ghost2_colorChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* ghost2_tex_bytes = stbi_load(
        "3D/car4_lightorange.png", // Path to your texture image
        &ghost2_img_width,
        &ghost2_img_height,
        &ghost2_colorChannels,
        0
    );

    GLuint ghost2_texture;
    glGenTextures(1, &ghost2_texture);
    glActiveTexture(GL_TEXTURE5); // Use a different texture unit if needed
    glBindTexture(GL_TEXTURE_2D, ghost2_texture);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGBA,
        ghost2_img_width,
        ghost2_img_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        ghost2_tex_bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(ghost2_tex_bytes);


    
    //std::string facesSkybox[6];
    
        

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
        unsigned char* data = stbi_load(facesSkyboxMorning[i].c_str(), &w, &h, &skyCChannel, 0);
 


        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGBA, //Rafael Ira R. Villanueva Orig RGB
                w,
                h,
                0,
                GL_RGBA, //Rafael Ira R. Villanueva Orig RGB
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
    Shader mainObjShader("Shaders/mainObj.vert", "Shaders/mainObj.frag");
    Shader lm1Shader("Shaders/lm1.vert", "Shaders/lm1.frag");
    Shader lm2Shader("Shaders/lm2.vert", "Shaders/lm2.frag");
	Shader ghost1Shader("Shaders/ghost1.vert", "Shaders/ghost1.frag");
	Shader ghost2Shader("Shaders/ghost2.vert", "Shaders/ghost2.frag");
	Shader planeShader("Shaders/plane.vert", "Shaders/plane.frag");
  

    //compile shader vertex
    std::fstream vertSrc("Shaders/directionLight.vert");
    std::stringstream vertBuff;

    vertBuff << vertSrc.rdbuf();

    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    //compile shader fragment
    std::fstream fragSrc("Shaders/directionLight.frag");
    std::stringstream fragBuff;

    fragBuff << fragSrc.rdbuf();

    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    //SPHERE OBJECT SHADER ///////////////////////////////////////////////////////////////////////////
    //compile shader vertex
    std::fstream vertSrcSphere("Shaders/pointLight.vert");
    std::stringstream vertBuffSphere;

    vertBuffSphere << vertSrcSphere.rdbuf();

    std::string vertSSphere = vertBuffSphere.str();
    const char* vSphere = vertSSphere.c_str();

    //compile shader fragment
    std::fstream fragSrcSphere("Shaders/pointLight.frag");
    std::stringstream fragBuffSphere;

    fragBuffSphere << fragSrcSphere.rdbuf();

    std::string fragSSphere = fragBuffSphere.str();
    const char* fSphere = fragSSphere.c_str();

    //SKYBOX ////////////////////////////////////////////////////////////////////////////
    //compile skybox vertex
    std::fstream vertSkyboxSrc("Shaders/skybox.vert");
    std::stringstream vertSkyboxBuff;

    vertSkyboxBuff << vertSkyboxSrc.rdbuf();

    std::string vertSB = vertSkyboxBuff.str();
    const char* vsb = vertSB.c_str();

    //compile skybox fragment
    std::fstream fragSkyboxSrc("Shaders/skybox.frag");
    std::stringstream fragSkyboxBuff;

    fragSkyboxBuff << fragSkyboxSrc.rdbuf();

    std::string fragSB = fragSkyboxBuff.str();
    const char* fsb = fragSB.c_str();

    //MAIN OBJECT //////////////////////////////////////////////////////////////////////
    //create vertex shader(used for movements)
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &v, NULL);

    glCompileShader(vertexShader);

    //create frag shader (our objects are turned into pixels/fragments which we will use to color in an object)
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragShader, 1, &f, NULL);

    glCompileShader(fragShader);

    //create shader program that'll just run both frag and vert together as one.
    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragShader);

    glLinkProgram(shaderProg);//compile to make sure computer remembers

    //SPHERE OBJECT //////////////////////////////////////////////////////////////////////
    //create vertex shader(used for movements)
    GLuint vertexShaderSphere = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShaderSphere, 1, &vSphere, NULL);

    glCompileShader(vertexShaderSphere);

    //create frag shader (our objects are turned into pixels/fragments which we will use to color in an object)
    GLuint fragShaderSphere = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragShaderSphere, 1, &fSphere, NULL);

    glCompileShader(fragShaderSphere);

    //create shader program that'll just run both frag and vert together as one.
    GLuint shaderProgSphere = glCreateProgram();
    glAttachShader(shaderProgSphere, vertexShaderSphere);
    glAttachShader(shaderProgSphere, fragShaderSphere);

    glLinkProgram(shaderProgSphere);//compile to make sure computer remembers

    //SKYBOX //////////////////////////////////////////////////////////////////////////
    //create vertex shader
    GLuint vertexSkyboxShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexSkyboxShader, 1, &vsb, NULL);

    glCompileShader(vertexSkyboxShader);

    //create frag shader (our objects are turned into pixels/fragments which we will use to color in an object)
    GLuint fragSkyboxShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragSkyboxShader, 1, &fsb, NULL);

    glCompileShader(fragSkyboxShader);

    //create shader program that'll just run both frag and vert together as one.
    GLuint skyboxShaderProg = glCreateProgram();
    glAttachShader(skyboxShaderProg, vertexSkyboxShader);
    glAttachShader(skyboxShaderProg, fragSkyboxShader);

    glLinkProgram(skyboxShaderProg);//compile to make sure computer remembers

    /////////////////////////////////////////////////////////////////////////////////////

    //set cursor to NONEXISTANT and make mouse events be called
    
    

 
    //std::string path = "3D/RedBellPepper.obj";
    std::string path = "3D/Car8.obj";
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
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) 
    {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        // Validate vertex index
        if (vData.vertex_index * 3 + 2 >= attributes.vertices.size()) {
            std::cerr << "Error: Vertex index out of range!" << std::endl;
            continue;
        }

        // vertex
        fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3)]);
        fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 1]);
        fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 2]);

        // Validate normal index
        if (vData.normal_index * 3 + 2 >= attributes.normals.size()) {
            std::cerr << "Error: Normal index out of range!" << std::endl;
            continue;
        }

        // normal
        fullVertexData.push_back(attributes.normals[(vData.normal_index * 3)]);
        fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 1]);
        fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 2]);

        // Validate texcoord index
        if (vData.texcoord_index * 2 + 1 >= attributes.texcoords.size()) {
            std::cerr << "Error: Texcoord index out of range!" << std::endl;
            continue;
        }

        // texcoord
        fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2)]);
        fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2) + 1]);
    }


    std::cout << "Vertex data size: " << fullVertexData.size() << std::endl;
    if (fullVertexData.empty()) {
        std::cerr << "Error: No vertex data loaded!" << std::endl;
    }
    else {
        // Print some of the vertex data for inspection
        for (size_t i = 0; i < std::min(fullVertexData.size(), size_t(30)); i += 8) {
            std::cout << "Vertex " << i / 8 << ": "
                << fullVertexData[i] << ", "
                << fullVertexData[i + 1] << ", "
                << fullVertexData[i + 2] << ", "
                << fullVertexData[i + 3] << ", "
                << fullVertexData[i + 4] << ", "
                << fullVertexData[i + 5] << ", "
                << fullVertexData[i + 6] << ", "
                << fullVertexData[i + 7] << std::endl;
        }
    }

 
    std::string lm1_path = "3D/Sandy.obj";
    std::vector<tinyobj::shape_t> lm1_shapes;
    std::vector<tinyobj::material_t> lm1_material;

    tinyobj::attrib_t lm1_attributes;

    bool sphere_success = tinyobj::LoadObj(
        &lm1_attributes,
        &lm1_shapes,
        &lm1_material,
        &warning,
        &error,
        lm1_path.c_str()
    );

    //array of mesh for the light ball
    std::vector<GLfloat> lm1_fullVertexData;
    for (int i = 0; i < lm1_shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = lm1_shapes[0].mesh.indices[i];

        //vertex
        lm1_fullVertexData.push_back(
            lm1_attributes.vertices[(vData.vertex_index * 3)]
        );

        lm1_fullVertexData.push_back(
            lm1_attributes.vertices[(vData.vertex_index * 3) + 1]
        );

        lm1_fullVertexData.push_back(
            lm1_attributes.vertices[(vData.vertex_index * 3) + 2]
        );

        //normal
        lm1_fullVertexData.push_back(
            lm1_attributes.normals[(vData.normal_index * 3)]
        );

        lm1_fullVertexData.push_back(
            lm1_attributes.normals[(vData.normal_index * 3) + 1]
        );

        lm1_fullVertexData.push_back(
            lm1_attributes.normals[(vData.normal_index * 3) + 2]
        );

        //texcoord
        lm1_fullVertexData.push_back(
            lm1_attributes.texcoords[(vData.texcoord_index * 2)]
        );

        lm1_fullVertexData.push_back(
            lm1_attributes.texcoords[(vData.texcoord_index * 2) + 1]
        );

    }
    


    std::string lm2_path = "3D/Billy.obj";
    std::vector<tinyobj::shape_t> lm2_shapes;
    std::vector<tinyobj::material_t> lm2_material;

    tinyobj::attrib_t lm2_attributes;

    bool lm1_success = tinyobj::LoadObj(
        &lm2_attributes,
        &lm2_shapes,
        &lm2_material,
        &warning,
        &error,
        lm2_path.c_str()
    );

    //array of mesh for the light ball
    std::vector<GLfloat> lm2_fullVertexData;
    for (int i = 0; i < lm2_shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = lm2_shapes[0].mesh.indices[i];
        if (vData.vertex_index * 3 + 2 >= lm2_attributes.vertices.size()) {
            std::cerr << "Error: Vertex index out of range!" << std::endl;
            continue;
        }
        //vertex
        lm2_fullVertexData.push_back(
            lm2_attributes.vertices[(vData.vertex_index * 3)]
        );

        lm2_fullVertexData.push_back(
            lm2_attributes.vertices[(vData.vertex_index * 3) + 1]
        );

        lm2_fullVertexData.push_back(
            lm2_attributes.vertices[(vData.vertex_index * 3) + 2]
        );
        if (vData.normal_index * 3 + 2 >= lm2_attributes.normals.size()) {
            std::cerr << "Error: Normal index out of range!" << std::endl;
            continue;
        }

        //normal
        lm2_fullVertexData.push_back(
            lm2_attributes.normals[(vData.normal_index * 3)]
        );

        lm2_fullVertexData.push_back(
            lm2_attributes.normals[(vData.normal_index * 3) + 1]
        );

        lm2_fullVertexData.push_back(
            lm2_attributes.normals[(vData.normal_index * 3) + 2]
        );

        if (vData.texcoord_index * 2 + 1 >= lm2_attributes.texcoords.size()) {
            std::cerr << "Error: Texcoord index out of range!" << std::endl;
            continue;
        }

        //texcoord
        lm2_fullVertexData.push_back(
            lm1_attributes.texcoords[(vData.texcoord_index * 2)]
        );

        lm2_fullVertexData.push_back(
            lm1_attributes.texcoords[(vData.texcoord_index * 2) + 1]
        );

    }



    std::string ghost1_path = "3D/police.obj";
    std::vector<tinyobj::shape_t> ghost1_shapes;
    std::vector<tinyobj::material_t> ghost1_material;

    tinyobj::attrib_t ghost1_attributes;

    bool ghost1_success = tinyobj::LoadObj(
        &ghost1_attributes,
        &ghost1_shapes,
        &ghost1_material,
        &warning,
        &error,
        ghost1_path.c_str()
    );

    //array of mesh for the light ball
    std::vector<GLfloat> ghost1_fullVertexData;
    for (int i = 0; i < ghost1_shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t gvData = ghost1_shapes[0].mesh.indices[i];

        //vertex
        ghost1_fullVertexData.push_back(
            ghost1_attributes.vertices[(gvData.vertex_index * 3)]
        );

        ghost1_fullVertexData.push_back(
            ghost1_attributes.vertices[(gvData.vertex_index * 3) + 1]
        );

        ghost1_fullVertexData.push_back(
            ghost1_attributes.vertices[(gvData.vertex_index * 3) + 2]
        );

        //normal
        ghost1_fullVertexData.push_back(
            ghost1_attributes.normals[(gvData.normal_index * 3)]
        );

        ghost1_fullVertexData.push_back(
            ghost1_attributes.normals[(gvData.normal_index * 3) + 1]
        );

        ghost1_fullVertexData.push_back(
            ghost1_attributes.normals[(gvData.normal_index * 3) + 2]
        );

        //texcoord
        ghost1_fullVertexData.push_back(
            ghost1_attributes.texcoords[(gvData.texcoord_index * 2)]
        );

        ghost1_fullVertexData.push_back(
            ghost1_attributes.texcoords[(gvData.texcoord_index * 2) + 1]
        );

    }





    std::string plane_path = "3D/plane.obj";
    std::vector<tinyobj::shape_t> plane_shapes;
    std::vector<tinyobj::material_t> plane_material;

    tinyobj::attrib_t plane_attributes;

    bool plane_success = tinyobj::LoadObj(
        &plane_attributes,
        &plane_shapes,
        &plane_material,
        &warning,
        &error,
        plane_path.c_str()
    );

    //array of mesh for the light ball
    std::vector<GLfloat> plane_fullVertexData;
    for (int i = 0; i < plane_shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t gvData = plane_shapes[0].mesh.indices[i];

        //vertex
        plane_fullVertexData.push_back(
            plane_attributes.vertices[(gvData.vertex_index * 3)]
        );

        plane_fullVertexData.push_back(
            plane_attributes.vertices[(gvData.vertex_index * 3) + 1]
        );

        plane_fullVertexData.push_back(
            plane_attributes.vertices[(gvData.vertex_index * 3) + 2]
        );

        //normal
        plane_fullVertexData.push_back(
            plane_attributes.normals[(gvData.normal_index * 3)]
        );

        plane_fullVertexData.push_back(
            plane_attributes.normals[(gvData.normal_index * 3) + 1]
        );

        plane_fullVertexData.push_back(
            plane_attributes.normals[(gvData.normal_index * 3) + 2]
        );

        //texcoord
        plane_fullVertexData.push_back(
            plane_attributes.texcoords[(gvData.texcoord_index * 2)]
        );

        plane_fullVertexData.push_back(
            plane_attributes.texcoords[(gvData.texcoord_index * 2) + 1]
        );

    }



    std::string ghost2_path = "3D/Car4.obj";
    std::vector<tinyobj::shape_t> ghost2_shapes;
    std::vector<tinyobj::material_t> ghost2_material;

    tinyobj::attrib_t ghost2_attributes;

    bool ghost2_success = tinyobj::LoadObj(
        &ghost2_attributes,
        &ghost2_shapes,
        &ghost2_material,
        &warning,
        &error,
        ghost2_path.c_str()
    );

    //array of mesh for the light ball
    std::vector<GLfloat> ghost2_fullVertexData;
    for (int i = 0; i < ghost2_shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t gvData = ghost2_shapes[0].mesh.indices[i];

        //vertex
        ghost2_fullVertexData.push_back(
            ghost2_attributes.vertices[(gvData.vertex_index * 3)]
        );

        ghost2_fullVertexData.push_back(
            ghost2_attributes.vertices[(gvData.vertex_index * 3) + 1]
        );

        ghost2_fullVertexData.push_back(
            ghost2_attributes.vertices[(gvData.vertex_index * 3) + 2]
        );

        //normal
        ghost2_fullVertexData.push_back(
            ghost2_attributes.normals[(gvData.normal_index * 3)]
        );

        ghost2_fullVertexData.push_back(
            ghost2_attributes.normals[(gvData.normal_index * 3) + 1]
        );

        ghost2_fullVertexData.push_back(
            ghost2_attributes.normals[(gvData.normal_index * 3) + 2]
        );

        //texcoord
        ghost2_fullVertexData.push_back(
            ghost2_attributes.texcoords[(gvData.texcoord_index * 2)]
        );

        ghost2_fullVertexData.push_back(
            ghost2_attributes.texcoords[(gvData.texcoord_index * 2) + 1]
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
    GLuint VAO, VBO;// EBO;// VBO_UV;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat) * fullVertexData.size(),
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
    GLuint lm1_VAO, lm1_VBO;

    glGenVertexArrays(1, &lm1_VAO);
    glGenBuffers(1, &lm1_VBO);

    glBindVertexArray(lm1_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, lm1_VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat)* lm1_fullVertexData.size(),
        lm1_fullVertexData.data(),
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

    GLintptr lm1_litPtr = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,

        8 * sizeof(float),
        (void*)lm1_litPtr

    );

    glEnableVertexAttribArray(1);

    GLintptr lm1_uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,

        8 * sizeof(float),
        (void*)lm1_uvPtr

    );

    glEnableVertexAttribArray(2);









    GLuint lm2_VAO, lm2_VBO;

    glGenVertexArrays(1, &lm2_VAO);
    glGenBuffers(1, &lm2_VBO);

    glBindVertexArray(lm2_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, lm2_VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat)* lm2_fullVertexData.size(),
        lm2_fullVertexData.data(),
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

    GLintptr lm2_litPtr = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,

        8 * sizeof(float),
        (void*)lm2_litPtr

    );

    glEnableVertexAttribArray(1);

    GLintptr lm2_uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,

        8 * sizeof(float),
        (void*)lm2_uvPtr

    );

    glEnableVertexAttribArray(2);








    GLuint ghost1_VAO, ghost1_VBO;

    glGenVertexArrays(1, &ghost1_VAO);
    glGenBuffers(1, &ghost1_VBO);

    glBindVertexArray(ghost1_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, ghost1_VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat)* ghost1_fullVertexData.size(),
        ghost1_fullVertexData.data(),
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

    GLintptr ghost1_litPtr = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,

        8 * sizeof(float),
        (void*)ghost1_litPtr

    );

    glEnableVertexAttribArray(1);

    GLintptr ghost1_uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,

        8 * sizeof(float),
        (void*)ghost1_uvPtr

    );

    glEnableVertexAttribArray(2);




    GLuint plane_VAO, plane_VBO;

    glGenVertexArrays(1, &plane_VAO);
    glGenBuffers(1, &plane_VBO);

    glBindVertexArray(plane_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, plane_VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat)* plane_fullVertexData.size(),
        plane_fullVertexData.data(),
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

    GLintptr plane_litPtr = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,

        8 * sizeof(float),
        (void*)plane_litPtr

    );

    glEnableVertexAttribArray(1);

    GLintptr plane_uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,

        8 * sizeof(float),
        (void*)plane_uvPtr

    );

    glEnableVertexAttribArray(2);





    GLuint ghost2_VAO, ghost2_VBO;

    glGenVertexArrays(1, &ghost2_VAO);
    glGenBuffers(1, &ghost2_VBO);

    glBindVertexArray(ghost2_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, ghost2_VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat)* ghost2_fullVertexData.size(),
        ghost2_fullVertexData.data(),
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

    GLintptr ghost2_litPtr = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,

        8 * sizeof(float),
        (void*)ghost2_litPtr

    );

    glEnableVertexAttribArray(1);

    GLintptr ghost2_uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,

        8 * sizeof(float),
        (void*)ghost2_uvPtr

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

    glm::vec3 lightColor = glm::vec3(1,1,1);

    float ambientStr = 0.2f;

    glm::vec3 ambientColor = lightColor;

    float specStr = 0.5f;

    float specPhong = 8;

    /*      Light Classes       */
    glm::vec3 lightDirection = { 4,-5,0 };
    DirectionLight directionLight(lightPos, lightColor, ambientStr, ambientColor, specStr, specPhong, lightDirection, dl_brightness);
    PointLight pointLight(player_car.getPosition() + glm::vec3{0.f,0.f, 100.f}, lightColor, ambientStr, ambientColor, specStr, specPhong, brightness);
    PointLight pointLight2(ghost_car1.getPosition() + glm::vec3{ 0.f,0.f, 100.f }, lightColor, ambientStr, ambientColor, specStr, specPhong, brightness);
	/*std::cout << "x: " << light_ball.getPosition(true).x << std::endl;
	std::cout << "y: " << light_ball.getPosition(true).y << std::endl;
	std::cout << "z: " << light_ball.getPosition(true).z << std::endl;*/
    ColorLight colorLight;
    /*typedef std::chrono::steady_clock clock;
    typedef std::chrono::time_point<clock> time_point;*/

    
    
        //time_point start = clock::now()
    stopwatch.start();
	stopwatch2.start();
	stopwatch3.start();
    

    
    glEnable(GL_BLEND); //Rafael Ira R. Villanueva
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Rafael Ira R. Villanueva
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        

        
        perca.updateCameraPosition(player_car.getPosition(), stateCam);
		
        
        glm::mat4 viewMatrix;
       //perca.updateCameraPosition(player_car.getPosition());

        //set camera to be MOVEABLE i.e. can be influenced
       
        
        
            //set the camera to perspective
        viewMatrix = glm::lookAt(perca.getCameraPos(), perca.getCameraPos() + perca.getFront(), perca.getWorldUp());
        
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
        mainObjShader.use();
        player_car.setTexture(&mainObjShader, &texture, "tex0");

        //      DRAWING THE MAIN OBJ

        //std::cout << "x: " << light_ball.getPosition(true).x << std::endl;

        pointLight.setPosition(player_car.getPosition() + glm::vec3{ 0.f,0.4f,4.0 });
        pointLight.setBrightness(brightness);
		//pointLight2.setBrightness(brightness);
        directionLight.setBrightness(dl_brightness);

        //attaches the same values for direction and point light
        directionLight.attachFundamentals(&mainObjShader);
        pointLight.attachFundamentals(&mainObjShader);

        //attaches the specific values of each light
        directionLight.attachSpecifics(&mainObjShader);
        pointLight.attachSpecifics(&mainObjShader);

       

        

            //set the camera to perspective
            player_car.setCamera(perca.getProjection(), perca.getCameraPos(), perca.getFront(), perca.getViewMat());
            //perca.updateCameraPosition(player_car.getPosition());
        
        
        //draw
        player_car.mainDraw(&mainObjShader, &VAO, &fullVertexData);
        
 
        
        lm1Shader.use();
		landmark_1.setTexture(&lm1Shader, &lm1_texture, "texture1");
        // Bind the texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, lm1_texture);
        lm1Shader.setInt("tex0", 1); // Set the texture unit to 1

      
            //set the camera to perspective
        landmark_1.setCamera(perca.getProjection(), perca.getCameraPos(), perca.getFront(), perca.getViewMat());
        
        pointLight.setPosition(player_car.getPosition() + glm::vec3{ 0.f,0.4f,4.0 });
        directionLight.setBrightness(dl_brightness);

        //attaches the same values for direction and point light
        directionLight.attachFundamentals(&lm1Shader);
        pointLight.attachFundamentals(&lm1Shader);

        //attaches the specific values of each light
        directionLight.attachSpecifics(&lm1Shader);
        pointLight.attachSpecifics(&lm1Shader);
        
        
        landmark_1.mainDraw(&lm1Shader, &lm1_VAO, &lm1_fullVertexData);





		lm2Shader.use();
		landmark_2.setTexture(&lm2Shader, &lm2_texture, "texture2");
		// Bind the texture
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, lm2_texture);
		lm2Shader.setInt("tex0", 2); // Set the texture unit to 1

       
        
            //set the camera to perspective
            landmark_2.setCamera(perca.getProjection(), perca.getCameraPos(), perca.getFront(), perca.getViewMat());
      
        pointLight.setPosition(player_car.getPosition() + glm::vec3{ 0.f,0.4f,4.0 });
            //pointLight.setPosition(glm::vec3{ 0,1,0 }/*light_ball.getPosition(true)*/); //set the current position of the light sphere to the lightPos
       // pointLight.setBrightness(brightness);
        directionLight.setBrightness(dl_brightness);

        //attaches the same values for direction and point light
        directionLight.attachFundamentals(&lm2Shader);
        pointLight.attachFundamentals(&lm2Shader);

        //attaches the specific values of each light
        directionLight.attachSpecifics(&lm2Shader);
        pointLight.attachSpecifics(&lm2Shader);

        //colorLight.setColor(sphere_color); //change the color
        //colorLight.perform(&sphereShader); //attaches the values of light into the shader program of sphere
        landmark_2.mainDraw(&lm2Shader, &lm2_VAO, &lm2_fullVertexData);
        

		ghost1Shader.use();
		ghost_car1.setTexture(&ghost1Shader, &ghost1_texture, "texture3");
		// Bind the texture
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, ghost1_texture);
		ghost1Shader.setInt("tex0", 3); // Set the texture unit to 1
     
        ghost_car1.setCamera(perca.getProjection(), perca.getCameraPos(), perca.getFront(), perca.getViewMat());
     
        pointLight.setPosition(player_car.getPosition() + glm::vec3{ 0.f,0.4f,4.0 });

        directionLight.setBrightness(dl_brightness);

        //attaches the same values for direction and point light
        directionLight.attachFundamentals(&ghost1Shader);
        pointLight.attachFundamentals(&ghost1Shader);

         //attaches the specific values of each light
        directionLight.attachSpecifics(&ghost1Shader);
        pointLight.attachSpecifics(&ghost1Shader);

         //colorLight.setColor(sphere_color); //change the color
         colorLight.perform(&ghost1Shader); //attaches the values of light into the shader program of sphere
        ghost_car1.mainDraw(&ghost1Shader, &ghost1_VAO, &ghost1_fullVertexData);

		
        
        
        
        
        planeShader.use();
		plane_road.setTexture(&planeShader, &plane_texture, "texture4");
		// Bind the texture
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, plane_texture);
		planeShader.setInt("tex0", 4); // Set the texture unit to 1
        
       
            //set the camera to perspective
            plane_road.setCamera(perca.getProjection(), perca.getCameraPos(), perca.getFront(), perca.getViewMat());
        

            pointLight.setPosition(player_car.getPosition() + glm::vec3{ 0.f,0.4f,4.0 });
           //pointLight2.setPosition(ghost_car1.getPosition() + glm::vec3{ 0.f,1.f,5.0 });


        //attaches the same values for direction and point light
        directionLight.attachFundamentals(&planeShader);
        pointLight.attachFundamentals(&planeShader);
        //pointLight2.attachFundamentals(&planeShader);

         //attaches the specific values of each light
        directionLight.attachSpecifics(&planeShader);
        pointLight.attachSpecifics(&planeShader);
        //pointLight2.attachSpecifics(&planeShader);

         //colorLight.setColor(sphere_color); //change the color
        //colorLight.perform(&planeShader); //attaches the values of light into the shader program of sphere
        plane_road.mainDraw(&planeShader, &plane_VAO, &plane_fullVertexData);
           


        ghost2Shader.use();
        ghost_car2.setTexture(&ghost2Shader, &ghost2_texture, "texture5");
        // Bind the texture
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, ghost2_texture);
        ghost2Shader.setInt("tex0", 5); // Set the texture unit to 1
        //if (stateCam) {
        //    //set the camera to ortho
        //    ghost_car1.setCamera(orca.getProjection(), orca.getCameraPos(), orca.getFront(), orca.getViewMat());
        //}
        //else {
            //set the camera to perspective
        ghost_car2.setCamera(perca.getProjection(), perca.getCameraPos(), perca.getFront(), perca.getViewMat());
        //}
        pointLight.setPosition(player_car.getPosition() + glm::vec3{ 0.f,0.4f,4.0 });
   
        directionLight.setBrightness(dl_brightness);

        //attaches the same values for direction and point light
        directionLight.attachFundamentals(&ghost2Shader);
        pointLight.attachFundamentals(&ghost2Shader);

         //attaches the specific values of each light
        directionLight.attachSpecifics(&ghost2Shader);
        pointLight.attachSpecifics(&ghost2Shader);

         //colorLight.setColor(sphere_color); //change the color
         //colorLight.perform(&sphereShader); //attaches the values of light into the shader program of sphere
        ghost_car2.mainDraw(&ghost2Shader, &ghost2_VAO, &ghost2_fullVertexData);
        
        //stopping conditions for finish line

        if (stateGhost == true) {
            if (ghostSpeed1 < 1.5)
            {
                ghostSpeed1 += 0.000005;
            }
            ghost_car1.translate(glm::vec3{ 0,0,1 } *ghostSpeed1);


            if (ghostSpeed2 < 0.7)
            {
                ghostSpeed2 += 0.0000005;
            }
			ghost_car2.translate(glm::vec3{ 0,0,1 } *ghostSpeed2);
        }

         if (stateGhost == false) {
            ghostSpeed1 = 0.0f;
            ghost_car1.translate(glm::vec3{ 0,0,0 } *ghostSpeed1);
			ghostSpeed2 = 0.0f;
			ghost_car2.translate(glm::vec3{ 0,0,0 } *ghostSpeed2);
            

         }

         if (ghost_car1.getPosition().z >= FINISH_LINE) {
             ghostSpeed1 = 0.0f;
             ghost_car1.translate(glm::vec3{ 0,0,0 } *ghostSpeed1);
			 finishState3 = true;
             stopwatch2.stop();


         }
		 if (ghost_car2.getPosition().z >= FINISH_LINE) {
			 ghostSpeed2 = 0.0f;
			 ghost_car2.translate(glm::vec3{ 0,0,0 } *ghostSpeed2);
			 finishState2 = true;
             stopwatch3.stop();
		 }

		 if (player_car.getPosition().z >= FINISH_LINE) {
			 carSpeed = 0.0f;
			 player_car.translate(glm::vec3{ 0,0,0 } * carSpeed);
			 finishState = true;
             stopwatch.stop();
		 }

		 if (finishState == true && finishState2 == true && finishState3 == true) {
			 
             std::cout << "GAME OVER\n" << std::endl;
             glfwSetWindowShouldClose(window, true);
		 }



        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        

    }
    
    //stopwatch.stop();
    std::cout << "Player time: " << stopwatch.elapsedSeconds() << " seconds\n" << std::endl;
    std::cout << "Ghost 1 time: " << stopwatch2.elapsedSeconds() << " seconds\n" << std::endl;
    std::cout << "Ghost 2 time: " << stopwatch3.elapsedSeconds() << " seconds\n" << std::endl;
    //std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

   

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    
   /* glDeleteBuffers(1, &EBO);*/
    glfwTerminate();
    return 0;
}