#include <iostream>
#include <chrono>

#include "Scene.h"

// 각종 콜백 함수 미리 선언
void DisplayFunc(void);
void ReshapeFunc(int width, int height);
void KeyboardFunc(unsigned char, int, int);
void KeyboardUpFunc(unsigned char, int, int);
void SpecialFunc(int, int, int);
void SpecialUpFunc(int, int, int);
void MouseFunc(int, int, int, int);
void MotionFunc(int, int);
//void TimerFunc(int);
void IdleFunc();

// 전역 변수
constexpr int winWidth = 1600, winHeight = 900;

const std::string windowTitle = "MyGame";

// Scene 클래스... 나중에는 게임에 카메라...플레이어... -> GameFramework
Scene g_scene{ winWidth, winHeight };		// initialize_list --> 추후 들어볼 일 있을것...

// *********************************************************************************************************************

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Tecture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Model {
public:
	Model(const std::string& path) {
		loadModel(path);
	}

private:
	void loadModel(const std::string& path) {
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(
			path,
			aiProcess_Triangulate |		// 삼각형으로 변환
			aiProcess_FlipUVs |		// UV 뒤집기
			aiProcess_GenNormals |	// 정점 노멀 생성
			aiProcess_JoinIdenticalVertices	// 중복 정점 병합
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cerr << "Error::Assimp::" << importer.GetErrorString() << std::endl;
		}

		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode* node, const aiScene* scene) {
		// 노드에 포함된 모든 메쉬를 처리
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(mesh, scene);
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}

	void processMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		// 정점 데이터 처리
		for (unsigned int i = 0; i < mesh->mNumAnimMeshes; i++) {
			Vertex vertex;
			vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

			if (mesh->HasNormals()) {
				vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			}

			if (mesh->mTextureCoords[0]) {	// 텍스처 좌표가 있을 경우
				vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			}
			else {
				vertex.texCoords = glm::vec2(0.0f, 0.0f);
			}

			vertices.push_back(vertex);
		}

		// 인덱스 데이터 처리
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		// 여기에 텍스처 처리 (생략 가능)
	}
};

// 애니메이션 데이터 처리
void processAnimation(const aiScene* scene) {
	if (scene->HasAnimations()) {
		for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
			aiAnimation* animation = scene->mAnimations[i];
			std::cout << "Animation Name : " << animation->mName.C_Str() << std::endl;
			std::cout << "Duration : " << animation->mDuration << std::endl;
			std::cout << "Ticks Per Second : " << animation->mTicksPerSecond << std::endl;

			for (unsigned int j = 0; j < animation->mNumChannels; j++) {
				aiNodeAnim* channel = animation->mChannels[j];
				std::cout << "Bone Name : " << channel->mNodeName.C_Str() << std::endl;

				// 포지션 키프레임 출력
				for (unsigned int k = 0; k < channel->mNumPositionKeys; k++) {
					aiVectorKey  key = channel->mPositionKeys[k];
					std::cout << "Time : " << key.mTime << ", Position : ("
						<< key.mValue.x << ", "
						<< key.mValue.y << ", "
						<< key.mValue.z << ") " << std::endl;
				}
			}

		}
	}
	else {
		std::cout << "No animatinons found in the scene." << std::endl;
	}
}

// *********************************************************************************************************************

int main(int argc, char** argv)
{
	//--- 윈도우 생성하기 (freeglut)
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);	// 디스플레이 모드 설정
	glutInitWindowPosition(100, 100);				// 윈도우의 위치 지정
	glutInitWindowSize(winWidth, winHeight);			// 윈도우의 크기 지정
	glutCreateWindow(windowTitle.c_str());				// 윈도우 생성 (윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {					// glew 초기화
		std::cerr << "Unable to initialize GLEW\n";
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "GLEW Initialized\n";
	}

	glClearColor(0.f, 0.f, 0.f, 1.f);		// 클리어 색상 (아무것도 안했을 때)

	// 초기 설정들
	g_scene.initialize();

	glEnable(GL_CULL_FACE);			// 컬링(뒷면 제거) 활성화
	//glCullFace(GL_FRONT);			// 앞면 지울건지 뒷면 지울건지
	//glFrontFace(GL_CCW);			// 어떤게 앞면인지?? GL_CW -> 시계, GL_CCW -> 반시계(기본값)

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 깊이 검사 활성화 --> z버퍼에다가 기록을 하기 시작하고, 그리지 말아야 할 것은 그리지 않는다....
	glEnable(GL_DEPTH_TEST);

	// 콜백 함수들
	glutKeyboardFunc(KeyboardFunc);		// 일반 키보드 (abcde..)
	glutKeyboardUpFunc(KeyboardUpFunc);	// 일반 키보드가 떼졌을 때
	glutSpecialFunc(SpecialFunc);		// 특수 키보드 (F1 ~ F12, HOME, CTRL, ALT 등등..)
	glutSpecialUpFunc(SpecialUpFunc);	// 특수 키보드가 떼졌을 때

	glutMouseFunc(MouseFunc);			// 마우스 입력 (좌클릭, 우클릭, 휠클릭... 
	glutMotionFunc(MotionFunc);			// 화면 누르고 드래그

	glutDisplayFunc(DisplayFunc);		// 출력 함수의 지정
	glutReshapeFunc(ReshapeFunc);		// 화면 크기가 변경되었을 때....
	//glutTimerFunc(16, TimerFunc, 0);		// 16ms --> 초당 60번
	glutIdleFunc(IdleFunc);

	glutMainLoop(); // 이벤트 처리 시작
}

void DisplayFunc(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 후면버퍼 지우기

	// 씬을 그린다
	g_scene.draw();

	Model myModel("link.fbx");

	// 후면버퍼를 전면 버퍼와 바꿔준다!
	glutSwapBuffers();
}

void ReshapeFunc(int width, int height)
{
	g_scene.setWindowSize(width, height);

	glutPostRedisplay();

	glViewport(0, 0, width, height);
}

void KeyboardFunc(unsigned char key, int x, int y)
{
	if (27 == key)		// ESC Key
		glutLeaveMainLoop();
	g_scene.keyboard(key, true);
}

void KeyboardUpFunc(unsigned char key, int, int)
{
	g_scene.keyboard(key, false);
}

void SpecialFunc(int key, int x, int y)
{
	g_scene.specialKeyboard(key, true);
}

void SpecialUpFunc(int key, int, int)
{
	g_scene.specialKeyboard(key, false);
}

void MouseFunc(int button, int state, int x, int y)
{
	g_scene.mouse(button, state, x, y);
}

void MotionFunc(int x, int y)
{
	g_scene.mouseMove(x, y);
}

//void TimerFunc(int id)
//{
//	switch (id) {
//	case 0:			// 업데이트
//		g_scene.update();
//		glutPostRedisplay();	// 씬을 업데이트 했으니, 화면에 그려줘야 한다..
//
//		glutTimerFunc(16, TimerFunc, 0);
//		break;
//	}
//}

void IdleFunc()	// 수직 동기화가 되고있다.. --> 컴퓨터마다 다르게 동작할 수 있음
{
	// static을 전역 변수처럼 활용
	static int fps = 0;
	// 지난 시각
	static std::chrono::high_resolution_clock::time_point last_time = std::chrono::high_resolution_clock::now();
	static std::chrono::high_resolution_clock::duration accm_time;	// 누적 시간

	// ms->10-3, us->10-6, ns->10-9

	++fps;
	auto now_time = std::chrono::high_resolution_clock::now();
	auto dur_time = now_time - last_time;
	accm_time += dur_time;
	last_time = now_time;

	if (accm_time >= std::chrono::seconds(1)) {
		std::string str = windowTitle + " | (FPS : " + std::to_string(fps) + ")";
		glutSetWindowTitle(str.c_str());		// 창에 FPS 표시

		fps = 0;
		accm_time = std::chrono::seconds(0);
	}

	g_scene.update(dur_time.count() / static_cast<float>(1E09));
	glutPostRedisplay();	// 씬을 업데이트 했으니, 화면에 그려줘야 한다..
}
