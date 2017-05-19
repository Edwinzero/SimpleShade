#pragma once
#ifndef _RENDER_PIPELINE_H
#define _RENDER_PIPELINE_H
#include <iostream>

#include <GL\glew.h>
#include <GL\freeglut.h>
#include "imgui.h"
#include "imgui_impl_glut.h"
#include <camera.h>
#include <Model.h>
#include <RenderUtils.h>
#include <ModelLoader.h>
#include <plyloader.h>

using namespace std;
unsigned int screenWidth = 1280;
unsigned int screenHeight = 780;
unsigned int pre_screenWidth = 0; 
unsigned int pre_screenHeight = 0;
bool show_debug_window = true;
bool show_test_window = false;
bool show_cameraInfo_window = false;
bool show_instruction_window = false;

Camera camera;

Model model;

// CL programs Key: V
bool reComplieKernel = false;
// GL programs Key: C
bool reComplieShader = false;
GLuint GLPointRenderProgram;
GLuint GLMocaPointRenderProgram;
GLuint GLGradientProgram;

//================================
// default draw event
//================================
void DrawCoord() {
	int num = 2 * 7;
	int grad = 50;

	glBegin(GL_LINES);

	for (int z = -num; z <= num; z++) {
		if (z == 0) {
			//x axis
			glColor3f(1, 0, 0);
		}
		else {
			glColor3f(0.2, 0.2, 0.3);
		}
		glVertex3f(-num * grad, 0, z * grad);
		glVertex3f(+num * grad, 0, z * grad);
	}

	for (int x = -num; x <= num; x++) {
		if (x == 0) {
			//z axis
			glColor3f(0, 0, 1);
		}
		else {
			glColor3f(0.2, 0.2, 0.3);
		}
		glVertex3f(x * grad, 0, -num * grad);
		glVertex3f(x * grad, 0, +num * grad);
	}

	glEnd();
}
//=========================================================================
//		Draw methods
//=========================================================================
void DrawScene2D() {
	{
		//glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
		glDisable(GL_DEPTH_TEST);
	//	glEnableClientState(GL_VERTEX_ARRAY);
	//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//	glEnableClientState(GL_COLOR_ARRAY);
		// Setup viewport, orthographic projection matrix
		int offsetx = screenWidth * 0.5;
		int offsety = screenHeight * 0.5;
		glViewport(offsetx, offsety, (GLsizei)screenWidth/2, (GLsizei)screenHeight/2);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0f, screenWidth,  0.0f, screenHeight, -1.0f, +1.0f);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		// Draw a triangle at 0.5 z
		//glBegin(GL_TRIANGLES);
		//glColor3f(1.0f, 0.0f, 0.0f);
		//glVertex3f(50.5, 50.5, 0.5);
		//glVertex3f(550.5, 50.5, 0.5);
		//glVertex3f(550.0, 150.5, 0.5);
		//glEnd();

	//	glDisableClientState(GL_COLOR_ARRAY);
	//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//	glDisableClientState(GL_VERTEX_ARRAY);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		//glPopAttrib();	
	}
}

void DrawScene3D() {
	{
		//glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
		glEnable(GL_DEPTH_TEST);
		// Setup viewport, orthographic projection matrix
		glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluPerspective(45.0, (GLdouble)screenWidth / (GLdouble)screenHeight, 1.0, 10000.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMultMatrixf(&camera.Mat()[0][0]);

		// default coord grids
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		{
			glLineWidth(2.0);
			DrawCoord();
			glColor3f(0, 1, 0);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 500, 0);
			glEnd();
		}
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		{
			model.Draw();
		}
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		//glPopAttrib();
	}
}

void drawGUI()
{
	ImGui_ImplGLUT_NewFrame(screenWidth, screenHeight);

	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	if(show_debug_window)
	{	
		ImGui::SetWindowSize(ImVec2(200, 150), ImGuiSetCond_FirstUseEver);
		ImGui::SetWindowPos(ImVec2(0, screenHeight-250));
		static float f = 0.0f;
		ImGui::Text("Debug information");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		if (ImGui::Button("Test Window")) show_test_window ^= 1;
		if (ImGui::Button("Camera Info")) show_cameraInfo_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	// 2. Show another simple window, this time using an explicit Begin/End pair
	if (show_cameraInfo_window)
	{
		ImGui::SetNextWindowSize(ImVec2(200, 150), ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(0, screenHeight-350), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Another Window", &show_cameraInfo_window);
		ImGui::Text("Camera information");
		glm::vec3 campos = camera.GetPos();
		ImGui::Text("Camera position: (%.3f, %.3f, %.3f)", campos.x, campos.y, campos.z);
		glm::vec3 camdir = camera.GetDir();
		ImGui::Text("Camera direction: (%.3f, %.3f, %.3f)", camdir.x, camdir.y, camdir.z);
		glm::vec3 camright = glm::cross(camdir, glm::vec3(0.0f, 1.0f, 0.0f));
		ImGui::Text("Camera right: (%.3f, %.3f, %.3f)", camright.x, camright.y, camright.z);
		ImGui::End();
	}

	// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	//if (show_test_window)
	//{
	//	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
	//	ImGui::ShowTestWindow(&show_test_window);
	//}

	if (show_instruction_window) {
		ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(150, 20), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("User Instruction", &show_instruction_window, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Key 9: turn on/off display upper camera data");
		ImGui::Text("Key 0: turn on/off display upper camera data");
		ImGui::Text("Key f: do registration on currently visiable data");
		ImGui::Text("Key t: revert transformation");
		ImGui::Text("Key 1: load board dataset");
		ImGui::Text("Key 2: load chair dataset");
		ImGui::Text("Key 3: load human dataset");
		ImGui::Text("Key Esc: exit program");
		ImGui::End();
	}
	ImGui::Render();
}

//=========================================================================
//		Render
//=========================================================================
void Render(void)
{
	// Get Back to the Modelview
	glClearColor(0.15f, 0.15f, 0.18f, 1.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);

	if (1) {
		// draw 3D scene
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		DrawScene3D();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	} 
	if (0) {
		// draw 2D scene
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		DrawScene2D();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	if(0){
		// draw gui, seems unnecessary to push/pop matrix and attributes
		//glPushAttrib(GL_ALL_ATTRIB_BITS);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		drawGUI();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		//glPopAttrib();
	}

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}
//=========================================================================
//		Reshape
//=========================================================================
void Reshape(int w, int h)
{
	// update screen width and height for imgui new frames
	screenWidth = w;
	screenHeight = h;

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;
	float ratio = 1.0f* w / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
	glutPostRedisplay();
}

//=========================================================================
//		Update
//=========================================================================
void Update(void) {

	if (reComplieShader) {
		GLPointRenderProgram = CompileGLShader("PointRender", "Shaders/PointRender.vs", "Shaders/PointRender.fs");
		GLMocaPointRenderProgram = CompileGLShader("MOCA_PointRender", "Shaders/PointCloudRender.vs", "Shaders/PointCloudRender.fs");
		reComplieShader = false;
		printf("[SHADER] :: Compile done !!!\n");
	}
}

//=========================================================================
//		keyboard & mouse callback
//=========================================================================
bool keyboardEvent(unsigned char nChar, int nX, int nY)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(nChar);

	if (nChar == 27) { //Esc-key
		glutLeaveMainLoop();
	}
	if (nChar == '+') {
		camera.ChangeAperture(0.02f);
	}
	if (nChar == '-') {
		camera.ChangeAperture(-0.02f);
	}
	if (nChar == ']') {
		camera.ChangeFocalDistance(10.0f);
	}
	if (nChar == '[') {
		camera.ChangeFocalDistance(-10.0f);
	}
	if (nChar == 'D' || nChar == 'd') {
		show_debug_window = !show_debug_window;
	}
	if (nChar == 'I' || nChar == 'i') {
		show_instruction_window = !show_instruction_window;
	}
	if (nChar == 'c') {
		reComplieShader = !reComplieShader;
	}
	if (nChar == 'v') {
		reComplieKernel = !reComplieKernel;
	}
	
	return true;
}

void KeyboardSpecial(int key, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(key);
}

void keyboardCallback(unsigned char nChar, int x, int y)
{
	if (keyboardEvent(nChar, x, y))
	{
		glutPostRedisplay();
	}
}

bool mouseEvent(int button, int state, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);

	if (state == GLUT_DOWN && (button == GLUT_LEFT_BUTTON))
		io.MouseDown[0] = true;
	else
		io.MouseDown[0] = false;

	if (state == GLUT_DOWN && (button == GLUT_RIGHT_BUTTON))
		io.MouseDown[1] = true;
	else
		io.MouseDown[1] = false;

	if (state == GLUT_DOWN && (button == GLUT_MIDDLE_BUTTON))
		io.MouseDown[2] = true;
	else
		io.MouseDown[2] = false;

	return true;
}

void MouseWheel(int button, int dir, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);
	if (dir > 0)
	{
		// Zoom in
		io.MouseWheel = 1.0;
		camera.Zoom(dir * 2.0f);
	}
	else if (dir < 0)
	{
		// Zoom out
		io.MouseWheel = -1.0;
		camera.Zoom(dir * 2.0f);
	}
}

void MouseEventCallback(int button, int state, int x, int y)
{
	if (mouseEvent(button, state, x, y))
	{

	}
}

void MouseDragCallback(int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);
	float dy = (io.MousePos.y - io.MousePosPrev.y);
	float dx = (io.MousePos.x - io.MousePosPrev.x);
	if (io.MouseDown[0]) {
		camera.Rot(-dy * 0.20f, -dx * 0.20f);
	}
	if (io.MouseDown[2]) {
		camera.Move(-dx * 1.0f, dy * 1.0f);
	}
	glutPostRedisplay();
}

void MouseMoveCallback(int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);

	glutPostRedisplay();
}
//=========================================================================
//		Initialize platforms
//=========================================================================
void Init_GLshader(void) {
	//GLPointRenderProgram = CompileGLShader("PointRender", "Shaders/PointRender.vs", "Shaders/PointRender.fs");
	//GLGradientProgram = CompileGLShader("ImageGradient", "Shaders/2Dcanvas.vs", "Shaders/2Dcanvas.fs");
	//GLMocaPointRenderProgram = CompileGLShader("MOCA_PointRender", "Shaders/PointCloudRender.vs", "Shaders/PointCloudRender.fs");
}
// initialize ogl and imgui
void Init_OpenGL(int argc, char **argv, const char* title)
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 0);
	//glutInitContextFlags(GLUT_FORWARD_COMPATIBLE); // cause error
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(500, 300);
	glutCreateWindow(title);
	fprintf(stdout, "INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));

	// glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		getchar();
		return;
	}

	// shaders
	Init_GLshader();

	// callback
	glutDisplayFunc(Render);
	glutReshapeFunc(Reshape);
	glutIdleFunc(Update);
	glutKeyboardFunc(keyboardCallback);
	glutSpecialFunc(KeyboardSpecial);
	glutMouseFunc(MouseEventCallback);
	glutMouseWheelFunc(MouseWheel);
	glutMotionFunc(MouseDragCallback);
	glutPassiveMotionFunc(MouseMoveCallback);
}

void Init_RenderScene(void) {

	// virtual camera
	{
		camera.focus = glm::vec3(0.0f, 0.0f, 0.0);
		camera.angle = glm::vec2(-30.0f, 0.0f);
		camera.fov = glm::vec2(45.0f, 45.0f);
		camera.apertureRadius = 0.01f;
		camera.focalDistance = 100.0f;
		camera.radius = 100.0f;
	}

	// load obj model
	{
		ObjModelLoader objloader;
		objloader.LoadOBJtoModel("../Model/crytek-sponza/sponza.obj", model);
		
	}
}

void Init_2DContents() {
	// for gui display, use cv process then bind gui to display
	//InitColorImage(colorTex, "rgb_color.jpeg");
	//InitDepthImage(depthTex, "BGdepth.png");
	//CreateFBO(imageCanvas, colorTex.width, colorTex.height);
}


void Init_Imgui(void) {
	//glClearColor(0.447f, 0.565f, 0.604f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplGLUT_Init();
}


//=========================================================================
//		Run Mainloop
//=========================================================================
void Run_Render(int argc, char **argv, const char* title){ 
	Init_OpenGL(argc, argv, title);

	Init_RenderScene();
	Init_2DContents();

	Init_Imgui();

	glutMainLoop();
	ImGui_ImplGLUT_Shutdown();
}


#endif // !_RENDER_PIPELINE_H
