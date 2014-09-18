#include <windows.h>
#include <GL/glut.h>

#include "Display.hpp"
#include "Globals.hpp"
#include "ImguiConfig.hpp"
#include "GridBuffer.hpp"
#include "CancerBehaviour.hpp"

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	TCC::CancerBehaviour(0, TCC::windowWidth * TCC::windowHeight);
	TCC::readBuf->fillDisplay(*TCC::displayBuffer);
	for (auto i = 0; i < TCC::injectionThickness; ++i)
	{
		TCC::displayBuffer->drawCircle(TCC::Position(TCC::mouse_x, TCC::windowHeight - TCC::mouse_y), TCC::injectionRadius + i, TCC::Color(255));
	}
	TCC::displayBuffer->render();
	std::swap(TCC::readBuf, TCC::writeBuf);

	ImguiConf::UpdateImGui();
	ImGui::Text("Injection Radius");
	ImGui::SliderInt("Injection Radius", &TCC::injectionRadius, 1, 200);
	ImGui::SliderInt("Injection Thickness", &TCC::injectionThickness, 1, 40);
	ImGui::SliderInt("Cancer %", &TCC::cancerPercent, 1, 99);

	bool resetSimulation = false;
	resetSimulation = ImGui::Button("Reset");
	if (resetSimulation)
	{
		TCC::readBuf->randomFill(TCC::Cancer, TCC::cancerPercent, TCC::Healthy);
	}
	ImGui::Render();

	glutSwapBuffers();
}

void initialize ()
{
	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, TCC::windowWidth, TCC::windowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glClearColor(1, 1, 1, 1);
	TCC::displayBuffer = new TCC::Display(TCC::windowWidth, TCC::windowHeight);
	TCC::buffer1 = new TCC::GridBuffer(TCC::windowWidth, TCC::windowHeight);
	TCC::buffer2 = new TCC::GridBuffer(TCC::windowWidth, TCC::windowHeight);
	TCC::readBuf = TCC::buffer1;
	TCC::writeBuf = TCC::buffer2;
	TCC::readBuf->randomFill(TCC::Cancer, TCC::cancerPercent, TCC::Healthy);
	ImguiConf::InitImGui();
}

void keyboard ( unsigned char key, int mousePositionX, int mousePositionY )
{
	switch ( key )
	{
	case 27:
		exit ( 0 );
		break;

	default:
		break;
	}

	ImguiConf::key_callback(key, glutGetModifiers());

	TCC::mouse_x = mousePositionX;
	TCC::mouse_y = mousePositionY;
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
		TCC::lMouse = state == GLUT_DOWN;
	else if (button == GLUT_RIGHT_BUTTON)
		TCC::rMouse = state == GLUT_DOWN;
	if ((button == 3) || (button == 4)) // It's a wheel event
	{
		ImguiConf::scroll_callback(x, y);
	}
	TCC::mouse_x = x;
	TCC::mouse_y = y;
}

void passiveMouse(int x, int y)
{
	TCC::mouse_x = x;
	TCC::mouse_y = y;
}

int main(int argc, char **argv)
{
	// initialize
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize(TCC::windowWidth, TCC::windowHeight);
	glutCreateWindow("<3 Threads <3");
	glutDisplayFunc(display);
	glutIdleFunc( display );
	glutKeyboardFunc( keyboard );
	glutMouseFunc( mouse );
	glutPassiveMotionFunc(passiveMouse);
	glutMotionFunc(passiveMouse);
	initialize();
	glutMainLoop();
	delete TCC::displayBuffer;
	ImGui::Shutdown();
	return 0;
}