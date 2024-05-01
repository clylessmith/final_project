#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <math.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "camera.h"
#include "texture.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;
using std::vector;
using glm::perspective;

GLint maxTessLevel;

GLuint heightMapProgram, waterProgram, cubeProgram;

int lastFrame;
int deltaTime;

int lastTime;
int nFrames;
float progTimeFact1 = 0.0, progTimeFact2 = 0.5;

int width, height, nrChannels,
    dudvWidth, dudvHeight, dudvNrChannels,
    flowMapWidth, flowMapHeight, flowMapChannels;
const unsigned int NUM_PATCH_PTS = 4;
unsigned rez = 100;
float lastX;
float lastY;
bool firstMouse = true;

GLuint terrainVAO, terrainVBO, 
            waterVAO, waterVBO, waterIBO, 
            reflectionFBO, reflectionDBO,
            refractionFBO,
            quadVAO, quadVBO, quadIBO;

GLuint terrainTexture, waterTexture, reflectionTex, refractionTex, refractionDepthTex, duDvMap, flowMap;

Camera camera(glm::vec3(67.0f, 100.0f, 169.9f),
              glm::vec3(0.0f, 1.0f, 0.0f),
              -128.1f, -42.4f);

// ---------------------
// Shader Initialization
// ---------------------
void readShader(const char* fname, char *source)
{
	FILE *fp;
	fp = fopen(fname,"r");
	if (fp==NULL)
	{
		fprintf(stderr,"The shader file %s cannot be opened!\n",fname);
		glfwTerminate();
		exit(1);
	}
	char tmp[300];
	while (fgets(tmp,300,fp)!=NULL)
	{
		strcat(source,tmp);
		strcat(source,"\n");
	}
}

unsigned int loadShader(const char *source, unsigned int mode)
{
	GLuint id;
	id = glCreateShader(mode);

	glShaderSource(id,1,&source,NULL);

	glCompileShader(id);

	char error[1000];

	glGetShaderInfoLog(id,1000,NULL,error);
	printf("Compile status: \n %s\n",error);

	return id;
}

void initShaders()
{
    // --------------------------
    // Water shader program setup
    // --------------------------
    char *waterVsSource, *waterFsSource;
    GLuint waterVs, waterFs;

    waterVsSource = (char *)malloc(sizeof(char)*20000);
    waterFsSource = (char *)malloc(sizeof(char)*20000);

    waterVsSource[0]='\0';
    waterFsSource[0]='\0';

    waterProgram = glCreateProgram();

    readShader("shader/water.vs",waterVsSource);
	readShader("shader/water.fs",waterFsSource);

    waterVs = loadShader(waterVsSource, GL_VERTEX_SHADER);
    waterFs = loadShader(waterFsSource, GL_FRAGMENT_SHADER);

    glAttachShader(waterProgram,waterVs);
	glAttachShader(waterProgram,waterFs);

    printf("Linking water shader\n");
	glLinkProgram(waterProgram);

    GLint isLinked = 0;
    glGetProgramiv(waterProgram, GL_LINK_STATUS, &isLinked); 
    if (isLinked == GL_FALSE) {
        printf("ERROR - Could not link water shader\n");
    } else {
        printf("Successfully linked\n");
    }
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        printf("OpenGL error in initShaders() after water: %u \n", err);
    }

    // ----------------------------
    // Terrain shader program setup
    // ----------------------------
	char *terrainVsSource, *terrainFsSource, *terrainTcsSource, *terrainTesSource;
	GLuint vs,fs,tcs,tes;

	terrainVsSource = (char *)malloc(sizeof(char)*20000);
	terrainFsSource = (char *)malloc(sizeof(char)*20000);
	terrainTcsSource = (char *)malloc(sizeof(char)*20000);
	terrainTesSource = (char *)malloc(sizeof(char)*20000);

	terrainVsSource[0]='\0';
	terrainFsSource[0]='\0';
	terrainTcsSource[0]='\0';
	terrainTesSource[0]='\0';

	heightMapProgram = glCreateProgram();

	readShader("shader/terrain_height.vs",terrainVsSource);
	readShader("shader/terrain_height.fs",terrainFsSource);
	readShader("shader/terrain_height.tcs",terrainTcsSource);
	readShader("shader/terrain_height.tes",terrainTesSource);

	vs = loadShader(terrainVsSource,GL_VERTEX_SHADER);
	fs = loadShader(terrainFsSource,GL_FRAGMENT_SHADER);
	tcs = loadShader(terrainTcsSource,GL_TESS_CONTROL_SHADER);
	tes = loadShader(terrainTesSource,GL_TESS_EVALUATION_SHADER);

	glAttachShader(heightMapProgram,vs);
	glAttachShader(heightMapProgram,fs);
	glAttachShader(heightMapProgram,tcs);
	glAttachShader(heightMapProgram,tes);

    printf("Linking terrain shader\n");
	glLinkProgram(heightMapProgram);

    isLinked = 0;
    glGetProgramiv(heightMapProgram, GL_LINK_STATUS, &isLinked); 
    if (isLinked == GL_FALSE) {
        printf("ERROR - Could not link terrain shader\n");
    } else {
        printf("Successfully linked\n");
    }

	//glUseProgram(heightMapProgram);
    err;
    while((err = glGetError()) != GL_NO_ERROR) {
        printf("OpenGL error in initShaders() after terrain: %u \n", err);
    }

    // -------------------
    // Cube Shader Program
    // -------------------
    char *cubeVsSource, *cubeFsSource;
	GLuint cubeVs,cubeFs;

	cubeVsSource = (char *)malloc(sizeof(char)*20000);
	cubeFsSource = (char *)malloc(sizeof(char)*20000);

	cubeVsSource[0]='\0';
	cubeFsSource[0]='\0';

	cubeProgram = glCreateProgram();

	readShader("shader/cube.vs",cubeVsSource);
	readShader("shader/cube.fs",cubeFsSource);

	cubeVs = loadShader(cubeVsSource,GL_VERTEX_SHADER);
	cubeFs = loadShader(cubeFsSource,GL_FRAGMENT_SHADER);

	glAttachShader(cubeProgram,cubeVs);
	glAttachShader(cubeProgram,cubeFs);

	glLinkProgram(cubeProgram);

    isLinked = 0;
    glGetProgramiv(cubeProgram, GL_LINK_STATUS, &isLinked); 
    if (isLinked == GL_FALSE) {
        printf("ERROR - Could not link quad shader\n");
    } else {
        printf("Successfully linked\n");
    }

    err;
    while((err = glGetError()) != GL_NO_ERROR) {
        printf("OpenGL error in initShaders() after quad: %u \n", err);
    }
	
}

// --------------------
// Framebuffer Creation
// --------------------
void createReflectionFB() {
    //glUseProgram(cubeProgram);

    // Generate and bind the framebuffer
    glGenFramebuffers(1, &reflectionFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    // Create the render texture object
    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &reflectionTex);
    glBindTexture(GL_TEXTURE_2D, reflectionTex);
    #ifdef __APPLE__
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    #else
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    #endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex, 0);

    // Create the depth buffer
    glGenRenderbuffers(1, &reflectionDBO);
    glBindRenderbuffer(GL_RENDERBUFFER, reflectionDBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflectionDBO);

    // // Set the targets for the fragment output variables
    // GLenum drawBuffers[] = {GL_NONE, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    // glDrawBuffers(3, drawBuffers);


    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if( result == GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer is complete.\n");
    } else {
        printf("Framebuffer is not complete.\n");
    }

    // The last call should be to revert to the default framebuffer
    // Unbind the framebuffer, and revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void createRefractionFB() {
    //glUseProgram(cubeProgram);

    // Generate and bind the framebuffer
    glGenFramebuffers(1, &refractionFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    // Create the render texture object
    glActiveTexture(GL_TEXTURE2);
    glGenTextures(1, &refractionTex);
    glBindTexture(GL_TEXTURE_2D, refractionTex);
    #ifdef __APPLE__
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    #else
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    #endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTex, 0);

    // Create the depth texture object
    glGenTextures(1, &refractionDepthTex);
    glBindTexture(GL_TEXTURE_2D, refractionDepthTex);
    #ifdef __APPLE__
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    #else
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    #endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, refractionDepthTex, 0);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if( result == GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer is complete.\n");
    } else {
        printf("Framebuffer is not complete.\n");
    }

    // The last call should be to revert to the default framebuffer
    // Unbind the framebuffer, and revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// ----------------
// Texture Creation
// ----------------
void loadTerrainTexture() {
    // load and create a texture
    // -------------------------
    glUseProgram(heightMapProgram);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &terrainTexture);
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    unsigned char *data = stbi_load("/Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/final_project/resources/river_heightmap_6.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glUniform1i(glGetUniformLocation(heightMapProgram, "heightMap"), 0);
        printf("Loaded heightmap of size %d x %d\n", height, width);
    }
    else
    {
        const char* failureReason = stbi_failure_reason();
        printf("Failed to load terrain texture: %s\n", failureReason);
    }
    stbi_image_free(data);
}

void loadDuDvMap() {
    // load and create a texture
    // -------------------------
    glUseProgram(waterProgram);
    glActiveTexture(GL_TEXTURE3);
    glGenTextures(1, &duDvMap);
    glBindTexture(GL_TEXTURE_2D, duDvMap); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    unsigned char *data = stbi_load("/Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/final_project/resources/wdudv.png", &dudvWidth, &dudvHeight, &dudvNrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dudvWidth, dudvHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glUniform1i(glGetUniformLocation(waterProgram, "duDvMap"), 0);
        printf("Loaded dudv map of size %d x %d\n", dudvHeight, dudvWidth);
    }
    else
    {
        const char* failureReason = stbi_failure_reason();
        printf("Failed to load dudv map: %s\n", failureReason);
    }
    stbi_image_free(data);
}

void loadFlowMap() {
    // load and create a texture
    // -------------------------
    glUseProgram(waterProgram);
    glActiveTexture(GL_TEXTURE4);
    glGenTextures(1, &flowMap);
    glBindTexture(GL_TEXTURE_2D, flowMap); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    unsigned char *data = stbi_load("/Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/final_project/resources/flowmap.png", &flowMapWidth, &flowMapHeight, &flowMapChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, flowMapWidth, flowMapHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glUniform1i(glGetUniformLocation(waterProgram, "flowMap"), 0);
        printf("Loaded flow map of size %d x %d\n", flowMapHeight, flowMapWidth);
    }
    else
    {
        const char* failureReason = stbi_failure_reason();
        printf("Failed to load flow map: %s\n", failureReason);
    }
    stbi_image_free(data);
}

void genWaterTexture() {
    createReflectionFB();
    createRefractionFB();
    loadDuDvMap();
    loadFlowMap();
}

// -----------------------
// Buffer/ Object Creation
// -----------------------
void setupTerrainBuffers() {
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::vector<float> vertices;

    
    for(unsigned i = 0; i <= rez-1; i++)
    {
        for(unsigned j = 0; j <= rez-1; j++)
        {
            vertices.push_back(-width/2.0f + width*i/(float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height/2.0f + height*j/(float)rez); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back(j / (float)rez); // v

            vertices.push_back(-width/2.0f + width*(i+1)/(float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height/2.0f + height*j/(float)rez); // v.z
            vertices.push_back((i+1) / (float)rez); // u
            vertices.push_back(j / (float)rez); // v

            vertices.push_back(-width/2.0f + width*i/(float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height/2.0f + height*(j+1)/(float)rez); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back((j+1) / (float)rez); // v

            vertices.push_back(-width/2.0f + width*(i+1)/(float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height/2.0f + height*(j+1)/(float)rez); // v.z
            vertices.push_back((i+1) / (float)rez); // u
            vertices.push_back((j+1) / (float)rez); // v
        }
    }
    printf("Loaded %d patches of 4 control points each\n", rez*rez);
    printf("Processing %d vertices in vertex shader\n", rez*rez*4);

    // first, configure the cube's VAO (and terrainVBO)
    
    glUseProgram(heightMapProgram);
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
}

void setupWaterBuffers() {
    GLfloat positionData[] = {
        -width/2.0f, -height/2.0f,
        width/2.0f, -height/2.0f,
        width/2.0f, height/2.0f,
        -width/2.0f, height/2.0f
    };

    GLfloat texCoordData[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    GLuint indexData[] = {
        0, 1, 2,  // First triangle
        0, 3, 2   // Second triangle
    };

    glUseProgram(waterProgram);
    glGenVertexArrays(1, &waterVAO);
    glBindVertexArray(waterVAO);

    glGenBuffers(1, &waterVBO);
    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positionData) + sizeof(texCoordData), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positionData), positionData);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(positionData), sizeof(texCoordData), texCoordData);

    glGenBuffers(1, &waterIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1); // texture coordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(positionData));

    glBindVertexArray(0); // Unbind VAO
}

void genQuadBuffers() {
    float quadVertices[] = {
        // positions  
        -100, -100,
        100, -100,
        100, 100,
        -100, 100
    };

   GLfloat texCoordData[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    GLuint indexData[] = {
        0, 1, 2,  // First triangle
        0, 3, 2   // Second triangle
    };

    glUseProgram(cubeProgram);
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices) + sizeof(texCoordData), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVertices), quadVertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(quadVertices), sizeof(texCoordData), texCoordData);

    glGenBuffers(1, &quadIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1); // texture coordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(quadVertices));

    glBindVertexArray(0); // Unbind VAO
}

// -------------------
// Rendering Functions
// -------------------
void renderTerrain(mat4 projection, mat4 model, mat4 view, vec4 plane) {
    glUseProgram(heightMapProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainTexture);
    glUniformMatrix4fv(glGetUniformLocation(heightMapProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(heightMapProgram, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(heightMapProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform4fv(glGetUniformLocation(heightMapProgram, "plane"), 1, &plane[0]);
    glBindVertexArray(terrainVAO);
    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS*rez*rez);
     GLenum err;
        while((err = glGetError()) != GL_NO_ERROR) {
            printf("OpenGL error in renderTerrain: %u\n", err);
        }
}

void renderWater(mat4 projection, mat4 model, mat4 view, vec3 camPos) {
    glUseProgram(waterProgram);

    glUniformMatrix4fv(glGetUniformLocation(waterProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(waterProgram, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(waterProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3fv(glGetUniformLocation(waterProgram, "cam"), 1, &camPos[0]);
    glUniform1fv(glGetUniformLocation(waterProgram, "time1"), 1, &progTimeFact1);
    glUniform1fv(glGetUniformLocation(waterProgram, "time2"), 1, &progTimeFact2);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, reflectionTex); 
    glUniform1i(glGetUniformLocation(waterProgram, "reflectTex"), 1);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, refractionTex); 
    glUniform1i(glGetUniformLocation(waterProgram, "refractTex"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, duDvMap); 
    glUniform1i(glGetUniformLocation(waterProgram, "duDvMap"), 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, flowMap); 
    glUniform1i(glGetUniformLocation(waterProgram, "flowMap"), 4);        
    
    glBindVertexArray(waterVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        printf("OpenGL error in renderWater: %u\n", err);
    }
}

void renderQuad(mat4 projection, mat4 model, mat4 view, bool isReflect) {
    mat4 MVP = projection * model * view;

    glUseProgram(cubeProgram);
    if (isReflect) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, reflectionTex); 
        glUniform1i(glGetUniformLocation(cubeProgram, "cubeTex"), 1); 
        }
    else {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, refractionTex);
        glUniform1i(glGetUniformLocation(cubeProgram, "cubeTex"), 2);  
    }     

    
    glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        printf("OpenGL error in renderQuad: %u\n", err);
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, lastTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, lastTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, lastTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, lastTime);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever a key event occurs, this callback is called
// ---------------------------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers)
{
    if(action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        } else {
            switch(key)
                {
                    default:
                        break;
                }
        }
        
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void showFPS(GLFWwindow* window) {
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
	    char ss[500] = {};
		std::string wTitle = "Final Project";
        nFrames++;
        if ( delta >= 1.0 ){ // If last update was more than 1 sec ago
            double fps = ((double)(nFrames)) / delta;
            sprintf(ss,"%s running at %lf FPS.",wTitle.c_str(),fps);
            glfwSetWindowTitle(window, ss);
            nFrames = 0;
            lastTime = currentTime;
        }
}

std::string getErrorString(GLenum error) {
    switch (error) {
        case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW:                return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW:               return "GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_CONTEXT_LOST:                  return "GL_CONTEXT_LOST";
        default:                               return "Unknown Error";
    }
}

int main(void)
{
	// ---------------------
    // GLFW setup
    // ---------------------
    GLFWwindow* window;

    int i,j,c;
    GLint mvp_location,time_location;

	lastTime = 0;
	lastFrame = 0;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(800, 600, "Final Project", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

	gladLoadGL();

	const GLubyte *renderer = glGetString( GL_RENDERER );
	const GLubyte *vendor = glGetString( GL_VENDOR );
	const GLubyte *version = glGetString( GL_VERSION );
	const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	printf("GL Vendor            : %s\n", vendor);
	printf("GL Renderer          : %s\n", renderer);
	printf("GL Version (string)  : %s\n", version);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version         : %s\n", glslVersion);
	
    // ----------------------
    // Shader setup
    // ----------------------
    //glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
    //printf("Max available tess level: %d\n", maxTessLevel);

    initShaders();
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        printf("OpenGL error out of draw loop: %u - %s\n", err, getErrorString(err).c_str());
    }

    // ----------------------
    // Texture/Buffer setup
    // ----------------------
    loadTerrainTexture();    
    setupTerrainBuffers();

    setupWaterBuffers();
    genWaterTexture();

    genQuadBuffers();

    glEnable(GL_DEPTH_TEST);
	glClearColor(0.53,0.81,0.92,1.0);
    // --------------
    // Draw loop
    // --------------
    while (!glfwWindowShouldClose(window))
    {
        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR) {
            printf("OpenGL error: %u - %s\n", err, getErrorString(err).c_str());
        }

        float ratio;
        int win_width, win_height;
        glfwGetFramebufferSize(window, &win_width, &win_height);
        glViewport(0, 0, width, height);

        float w2 = width / 2.0f;
        float h2 = height / 2.0f;
        mat4 viewport = mat4( vec4(w2,0.0f,0.0f,0.0f),
                     vec4(0.0f,h2,0.0f,0.0f),
                     vec4(0.0f,0.0f,1.0f,0.0f),
                     vec4(w2+0, h2+0, 0.0f, 1.0f));

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        progTimeFact1 += 0.01;
        if (progTimeFact1 >= 1.0) {
            progTimeFact1 = 0.0;
        }
        progTimeFact2 += 0.01;
        if (progTimeFact2 >= 1.0) {
            progTimeFact2 = 0.0;
        }

        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)win_width / (float)win_height, 0.1f, 100000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        
        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec4 plane = glm::vec4(0, 1, 0, -8);
        
        // // FIRST PASS - REFLECTION
        glEnable(GL_CLIP_DISTANCE0);
        glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
        glViewport(0, 0, width, height); // set viewport to the framebuffer size
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the framebuffer

        float moveDistance = 2 * (camera.Position.y - 10);
        glm::vec3 camPos = camera.Position;
        camera.SetPosition(camPos - glm::vec3(0, moveDistance, 0));
        camera.SetPitch(camera.Pitch * -1);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)win_width / (float)win_height, 0.1f, 100000.0f);
        view = camera.GetViewMatrix();

        renderTerrain(projection, model, view, plane);

        camera.SetPosition(camPos);
        camera.SetPitch(camera.Pitch * -1);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)win_width / (float)win_height, 0.1f, 100000.0f);
        view = camera.GetViewMatrix();
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // SECOND PASS - REFRACTION
        glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
        glViewport(0, 0, width, height); // set viewport to the framebuffer size
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the framebuffer
        
        plane = glm::vec4(0, -1, 0, 10);
        // projection = glm::perspective(glm::radians(camera.Zoom), (float)win_width / (float)win_height, 0.1f, 100000.0f);
        // view = camera.GetViewMatrix();

        renderTerrain(projection, model, view, plane);

        // THIRD PASS - WORLD
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_CLIP_DISTANCE0);
        glViewport(0, 0, win_width, win_height); // set viewport to the framebuffer size
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the framebuffer
        plane = glm::vec4(0, -1, 0, 10000);
        // projection = glm::perspective(glm::radians(camera.Zoom), (float)win_width / (float)win_height, 0.1f, 100000.0f);
        // view = camera.GetViewMatrix();

        renderTerrain(projection, model, view, plane);
        renderWater(projection, model, view, camPos);

        // // render reflection test
        // model = glm::translate(model, vec3(400, 40, 0));
        // projection = glm::perspective(glm::radians(camera.Zoom), (float)win_width / (float)win_height, 0.1f, 100000.0f);
        // view = camera.GetViewMatrix();
        // renderQuad(projection, model, camera.GetViewMatrix(), true);

        // // render refraction test
        // model = glm::translate(model, vec3(200, 200, 0));
        // renderQuad(projection, model, view, false);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);

        showFPS(window);
        glfwPollEvents();

        // printf("CURRENT CAMERA POS: %f, %f, %f\n", camera.Position.x,camera.Position.y,camera.Position.z);
    }
	
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}