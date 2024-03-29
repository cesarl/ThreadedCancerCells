#include "Vendors/GL/freeglut.h"

#include "Display.hpp"
#include "Globals.hpp"
#include "ImguiConfig.hpp"
#include "GridBuffer.hpp"
#include "WorkerThread.hpp"

void initThreads()
{
	if ((unsigned int)TCC::threadNumber > TCC::workerThreads.size())
	{
		auto d = TCC::workerThreads.size();
		TCC::workerThreads.resize(TCC::threadNumber);
		for (int i = d; i < TCC::threadNumber; ++i)
		{
			TCC::workerThreads[i] = std::make_unique<TCC::WorkerThread>();
			TCC::workerThreads[i]->launch();
		}
	}
	else if ((unsigned int)TCC::threadNumber < TCC::workerThreads.size())
	{
		for (auto i = TCC::workerThreads.size() - 1; i >= (unsigned int)TCC::threadNumber; --i)
		{
			TCC::workerThreads[i]->quit();
		}
		TCC::workerThreads.resize(TCC::threadNumber);
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static bool pause = false;
	static bool step = false;

	std::vector<std::future<std::array<unsigned int, 4>>> res;
	res.resize(TCC::workerThreads.size());

	if (!pause || step)
	{
		auto range = (unsigned int)std::ceil((float)(TCC::windowWidth * TCC::windowHeight) / (float)TCC::workerThreads.size());
		for (std::size_t i = 0; i < TCC::workerThreads.size(); ++i)
		{
			auto from = range * i;
			auto to = (from + range);
			if (to > TCC::windowWidth * TCC::windowHeight)
				to = TCC::windowWidth * TCC::windowHeight;
			res[i] = TCC::workerThreads[i]->getCommandQueue()
				.priorityFutureEmplace<TCC::WorkerThread::Compute, std::array<unsigned int, 4>>(from, to);
		}


		TCC::Counter[TCC::Medecine] = 0;
		TCC::Counter[TCC::Cancer] = 0;
		TCC::Counter[TCC::None] = 0;
		TCC::Counter[TCC::Healthy] = 0;

		for (std::size_t i = 0; i < res.size(); ++i)
		{
			auto t = res[i].get();
			TCC::Counter[0] += t[0];
			TCC::Counter[1] += t[1];
			TCC::Counter[2] += t[2];
			TCC::Counter[3] += t[3];
		}

		TCC::buffer->swap();
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
		TCC::buffer->inject((unsigned int)(TCC::mouse_x / TCC::zoom), (unsigned int)((TCC::windowHeight - TCC::mouse_y) / TCC::zoom));
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

	if (ImGui::SliderInt("Threads number", &TCC::threadNumber, 1, 16))
	{
		initThreads();
	}

	ImGui::SliderFloat("Zoom ", &TCC::zoom, 1, 15);

	ImGui::Text("Healthy cells : %i", TCC::Counter[TCC::Healthy]);
	ImGui::Text("Cancer cells : %i", TCC::Counter[TCC::Cancer]);
	ImGui::Text("Medecine cells : %i", TCC::Counter[TCC::Medecine]);
	ImGui::Text("Empty cells : %i", TCC::Counter[TCC::None]);

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
	for (auto &e : TCC::workerThreads)
		e->quit();
	return 0;
}