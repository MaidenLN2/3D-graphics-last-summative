//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2022 Media Design School
//
// File Name      : main.cpp
// Description    : main code containing graphical features and matrices for positioning
// Author         : Lera Blokhina
// Mail           : valeriia.blokhina@mds.ac.nz
//

// library includes
#pragma once
#include <iostream>
#include <stdlib.h>
#include <map>
#include <ctime>
#include <glew.h>
#include <glfw3.h>
#include "ShaderLoader.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "camera.h"
#include "Utilities.h"
#include <fmod.hpp>
#include "Skybox.h"
#include "Terrain.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h> // check properties for release version

// adding variables for programs, textures and window
GLFWwindow* Window = nullptr;
GLuint Program_Light;
GLuint Program_DirLight;
GLuint Program_PointLight;
GLuint Program_Reflection;
GLuint Program_Color;
float CurrentTime;
GLuint Texture_Terrain;
GLuint Program_Terrain;

// bools
bool scissor = false;
bool stencil = false;
bool wireframe = false;
bool facecull = false;

// variables for delta time and objects
float PreviousTimeStep; // delta time
camera ortho;
Skybox* environment = nullptr;
Terrain* terrainMap = nullptr;

// variables for matrices
glm::mat4 ObjModelMat;
glm::mat4 PVMMat;


// load the image data 
void ImageLoad(const char* FilePath, GLuint& TextureID)
{
	int ImageWidth;
	int ImageHeight;
	int ImageComponents;
	unsigned char* ImageData = stbi_load(FilePath, &ImageWidth, &ImageHeight, &ImageComponents, 0);


	// create and bind a new texture template
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	// setting the filtering and mipmap parameters for this texture ID
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//check how many components the loaded image has (rgb or rgba)
	GLint LoadedComponents = (ImageComponents == 4) ? GL_RGBA : GL_RGB;

	//populate the texture with the image data
	glTexImage2D(GL_TEXTURE_2D, 0, LoadedComponents, ImageWidth, ImageHeight, 0,
		LoadedComponents, GL_UNSIGNED_BYTE, ImageData);
	


	// generate the mipmaps, free the memory and unbind the texture
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(ImageData);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// calllback function called in response to keyboard input, processed during glfwPollEvents()
void KeyInput(GLFWwindow* InputWindow, int Key, int ScanCode, int Action, int Mods)
{
	if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(Window, true);
	}
	if (Key == GLFW_KEY_Z && Action == GLFW_PRESS)
	{
		scissor = !scissor;
	}
	if (Key == GLFW_KEY_X && Action == GLFW_PRESS)
	{
		stencil = !stencil;
	}
	if (Key == GLFW_KEY_C && Action == GLFW_PRESS)
	{
		wireframe = !wireframe;
	}
	if (Key == GLFW_KEY_V && Action == GLFW_PRESS)
	{
		facecull = !facecull;
	}
	if (Key == GLFW_KEY_R && Action == GLFW_PRESS)
	{
		//reset the scene
		scissor = false;
		stencil = false;
		wireframe = false;
		facecull = false;
	}
}

//setup the initial elements of the program
void InitialSetup()
{
	//set the colour of the window for when the buffer is cleared
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f); // red, green, blue, alpha

	// random seed
	srand((unsigned int)time(NULL));

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);

	// maps the range of the window size to NDC (-1 -> 1
	glViewport(0, 0, 800, 800);

	// create mapping
	std::map<std::string, GLuint> ShaderMap; //can be initialized globally or in main based where map access is needed

	// calling skybox
	environment = new Skybox(ShaderMap, &ortho);

	environment->ImageLoad();

	// create the program
	Program_Color = ShaderLoader::CreateProgram("Resources/Shaders/3D_Normals.vs",
		"Resources/Shaders/FixedColor.fs",
		ShaderMap);
	Program_Terrain = ShaderLoader::CreateProgram("Resources/Shaders/HeightMap.vs",
		"Resources/Shaders/Texture.fs",
		ShaderMap);
	
	// inverting vertical image
	stbi_set_flip_vertically_on_load(true);

	//calling terrain
	ImageLoad("Resources/Textures/Terrain.png", Texture_Terrain);
	terrainMap = new Terrain(Texture_Terrain, Program_Terrain);
	terrainMap->SetPosition(glm::vec3(0.0f, -15.0f, 1.0f));

	// callback for the key input (needed for ESC button)
	glfwSetKeyCallback(Window, KeyInput);
	
}

void Update()
{
	glfwPollEvents();

	// get the current time
	CurrentTime = (float)glfwGetTime();

	// calculate current DeltaTime and update the PreviousTimeStep for the next frame
	float CurrentTimeStep = (float)glfwGetTime();
	float DeltaTime = CurrentTimeStep - PreviousTimeStep;
	PreviousTimeStep = CurrentTimeStep;

	// calling freecam
	ortho.Update(Window, DeltaTime);

	terrainMap->Update(DeltaTime, ortho.GetMatrixPV());

	// skybox update
	environment->Update(DeltaTime);

}
//render all the objects
void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//bind vertex array for sphere
	glUseProgram(Program_Reflection);

	// send variables to shaders with uniform
	GLint CurrentTimeLoc = glGetUniformLocation(Program_DirLight, "CurrentTime");
	glUniform1f(CurrentTimeLoc, CurrentTime);

	//send variables to shaders via uniform (reflection)
	GLint ModelMatLoc = glGetUniformLocation(Program_Reflection, "Model");
	glUniformMatrix4fv(ModelMatLoc, 1, GL_FALSE, glm::value_ptr(ObjModelMat));
	GLint PVMMatLoc = glGetUniformLocation(Program_Reflection, "PVM");
	glUniformMatrix4fv(PVMMatLoc, 1, GL_FALSE, glm::value_ptr(PVMMat));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environment->GetTextureID());
	glUniform1i(glGetUniformLocation(Program_Reflection, "Texture0"), 0);

	GLint CameraPosLoc = glGetUniformLocation(Program_Reflection, "CameraPos");
	glUniform3fv(CameraPosLoc, 1, glm::value_ptr(ortho.GetPosition()));
	
	// terrain render
	terrainMap->Render();

	// environment render
	environment->Render();

	//scissor test
	if (scissor == true)
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(200, 200, 400, 400);
	}

	// wireframe/faceculling toggle
	if (wireframe == true)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


	for (size_t i = 0; i < 10; i++)
	{

		// stencil test enabled
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		// stencil test, set value, 1st pass
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF); // enable writing to stencil buffer

		if (stencil == true)
		{
			// stencil test, 2nd pass
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //write to areas where value is not equal to 1
			glStencilMask(0x00); // disable writing to stencil buffer

			glStencilMask(0x00); // disable writing to stencil mask
			glDisable(GL_STENCIL_TEST); //disable test

			glStencilMask(0xFF); // enable stencil test for the next time
		}			
	}

	//disable scissor test
	glDisable(GL_SCISSOR_TEST);

	// unbind assets (because if not they might cause a lot of errors)
	glBindVertexArray(0);
	glUseProgram(0);

	glfwSwapBuffers(Window);
}

int main()
{
	// initializing GLFW and setting the version to 4.6 with only Core functionality available
	glfwInit();
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	// enabling anti-aliasing
	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);
	
	//create an GLFW controlled context window
	Window = glfwCreateWindow(800, 800, "First OpenGL Window", NULL, NULL);

	if (Window == NULL)
	{
		std::cout << "GLFW failed to initialize properly. Terminating program." << std::endl;
		system("pause");

		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(Window);

	// initialising GLEW to populate OpenGL function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW failed to initialize properly. Terminating program." << std::endl;
		system("pause");

		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(Window);

	//setup the initial elements of the program
	InitialSetup();

	////main loop
	while (glfwWindowShouldClose(Window) == false)
	{
		//update all objects and run the processes
		Update();

		//render all the objects
		Render();

	}

	// ensuring correct shutdown of GLFW
	glfwTerminate();
	return 0;
}
