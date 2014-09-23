#include "Vendors/GL/freeglut.h"

#include "Display.hpp"
#include "Globals.hpp"
#include "ImguiConfig.hpp"
#include "GridBuffer.hpp"
#include "CancerBehaviour.hpp"

void initThreads()
{
	for (auto &e : TCC::cancerBehaviours)
		e->quit();
	TCC::cancerBehaviours.clear();
	TCC::cancerBehaviours.resize(TCC::threadNumber);
	for (auto i = 0; i < TCC::cancerBehaviours.size(); ++i)
	{
		TCC::cancerBehaviours[i] = std::make_unique<TCC::CancerBehaviour>();
		TCC::cancerBehaviours[i]->launch();
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static bool pause = false;
	static bool step = false;

	std::vector<std::future<std::array<unsigned int, 3>>> res;
	res.resize(TCC::cancerBehaviours.size());

	if (!pause || step)
	{
		auto range = (unsigned int)std::ceil((float)(TCC::windowWidth * TCC::windowHeight) / (float)TCC::cancerBehaviours.size());
		for (auto i = 0; i < TCC::cancerBehaviours.size(); ++i)
		{
			auto from = range * i;
			auto to = (from + range);
			if (to > TCC::windowWidth * TCC::windowHeight)
				to = TCC::windowWidth * TCC::windowHeight;
			res[i] = TCC::cancerBehaviours[i]->getCommandQueue()
				.priorityFutureEmplace<TCC::CancerBehaviour::Compute, std::array<unsigned int, 3>>(from, to);
			//		TCC::cancerBehaviours[i]->getCommandQueue().releaseReadability();
		}


		for (auto i = 0; i < res.size(); ++i)
		{
			auto t = res[i].get();
			//counter[0] += t[0];
			//counter[1] += t[1];
			//counter[2] += t[2];
		}
	}
	step = false;
	TCC::buffer->fillDisplay(*TCC::displayBuffer);

	for (auto i = 0; i < TCC::injectionThickness; ++i)
	{
		if (TCC::injectionRadius == 1 && TCC::injectionThickness == 1)
			TCC::displayBuffer->drawPixel(TCC::Position(TCC::mouse_x / TCC::zoom, (TCC::windowHeight - TCC::mouse_y) / TCC::zoom), TCC::Color(30));
		else
			TCC::displayBuffer->drawCircle(TCC::Position(TCC::mouse_x / TCC::zoom, (TCC::windowHeight - TCC::mouse_y) / TCC::zoom), TCC::injectionRadius + i, TCC::Color(30));
	}

	if (TCC::rMouse)
	{
		TCC::buffer->inject(TCC::mouse_x / TCC::zoom, (TCC::windowHeight - TCC::mouse_y) / TCC::zoom);
	}

	TCC::displayBuffer->render();

	ImguiConf::UpdateImGui();

	ImGui::Text("Right click to inject medecine ! \nCancer cells are red, healthy one are green, and medecine is yellow ! \n");

	static float ms_per_frame[120] = { 0 };
	static int ms_per_frame_idx = 0;
	static float ms_per_frame_accum = 0.0f;
	ms_per_frame_accum -= ms_per_frame[ms_per_frame_idx];
	ms_per_frame[ms_per_frame_idx] = ImGui::GetIO().DeltaTime * 1000.0f;
	ms_per_frame_accum += ms_per_frame[ms_per_frame_idx];
	ms_per_frame_idx = (ms_per_frame_idx + 1) % 120;
	const float ms_per_frame_avg = ms_per_frame_accum / 120;
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", ms_per_frame_avg, 1000.0f / ms_per_frame_avg);

	ImGui::Text("Injection Radius");
	ImGui::SliderInt("Injection Radius", &TCC::injectionRadius, 1, 200);
	ImGui::SliderInt("Injection Thickness", &TCC::injectionThickness, 1, 100);

	if (ImGui::SliderInt("Cancer %", &TCC::cancerPercent, 0, 100 - TCC::healthyPercent))
	{
		TCC::buffer->randomFill();
	}
	if (ImGui::SliderInt("Healthy %", &TCC::healthyPercent, 0, 100 - TCC::cancerPercent))
	{
		TCC::buffer->randomFill();
	}

	if (ImGui::SliderInt("Threads number", &TCC::threadNumber, 1, 48))
	{
		initThreads();
	}

	if (ImGui::SliderFloat("Zoom ", &TCC::zoom, 1, 15));
	{
	}

	//ImGui::Text("Healthy cells : %i", TCC::Counter[TCC::Healthy]);
	//ImGui::Text("Cancer cells : %i", TCC::Counter[TCC::Cancer]);
	//ImGui::Text("Medecine cells : %i", TCC::Counter[TCC::Medecine]);
	//ImGui::Text("Empty cells : %i", TCC::Counter[TCC::None]);

	if (ImGui::Button("Reset"))
	{
		TCC::buffer->randomFill();
	}

	static std::string playpausestr = "Pause";

	if (ImGui::Button(playpausestr.c_str()))
	{
		pause = !pause;
		if (pause)
			playpausestr = "Play";
		else
			playpausestr = "Pause";
	}

	if (pause)
	{
		if (ImGui::Button("Step forward"))
		{
			step = true;
		}
	}
	ImGui::Render();
	TCC::buffer->swap();
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
	TCC::buffer = new TCC::GridBuffer(TCC::windowWidth, TCC::windowHeight);
	initThreads();
	ImguiConf::InitImGui();
	TCC::Counter[0] = 0;
	TCC::Counter[1] = 0;
	TCC::Counter[2] = 0;
	TCC::Counter[3] = 0;
	TCC::buffer->randomFill();
}

void keyboard ( unsigned char key, int mousePositionX, int mousePositionY )
{
	switch ( key )
	{
	case 27:
		glutLeaveMainLoop();
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
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutMainLoop();
	TCC::running = true;
	delete TCC::displayBuffer;
	ImGui::Shutdown();
	for (auto &e : TCC::cancerBehaviours)
		e->quit();
	return 0;
}