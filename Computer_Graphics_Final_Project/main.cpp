#include <iostream>
#include <chrono>

#include "Scene.h"

// ���� �ݹ� �Լ� �̸� ����
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

// ���� ����
constexpr int winWidth = 1600, winHeight = 900;

const std::string windowTitle = "MyGame";

// Scene Ŭ����... ���߿��� ���ӿ� ī�޶�...�÷��̾�... -> GameFramework
Scene g_scene{ winWidth, winHeight };		// initialize_list --> ���� �� �� ������...

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
			aiProcess_Triangulate |		// �ﰢ������ ��ȯ
			aiProcess_FlipUVs |		// UV ������
			aiProcess_GenNormals |	// ���� ��� ����
			aiProcess_JoinIdenticalVertices	// �ߺ� ���� ����
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cerr << "Error::Assimp::" << importer.GetErrorString() << std::endl;
		}

		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode* node, const aiScene* scene) {
		// ��忡 ���Ե� ��� �޽��� ó��
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

		// ���� ������ ó��
		for (unsigned int i = 0; i < mesh->mNumAnimMeshes; i++) {
			Vertex vertex;
			vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

			if (mesh->HasNormals()) {
				vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			}

			if (mesh->mTextureCoords[0]) {	// �ؽ�ó ��ǥ�� ���� ���
				vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			}
			else {
				vertex.texCoords = glm::vec2(0.0f, 0.0f);
			}

			vertices.push_back(vertex);
		}

		// �ε��� ������ ó��
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		// ���⿡ �ؽ�ó ó�� (���� ����)
	}
};

// �ִϸ��̼� ������ ó��
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

				// ������ Ű������ ���
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
	//--- ������ �����ϱ� (freeglut)
	glutInit(&argc, argv);							// glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);	// ���÷��� ��� ����
	glutInitWindowPosition(100, 100);				// �������� ��ġ ����
	glutInitWindowSize(winWidth, winHeight);			// �������� ũ�� ����
	glutCreateWindow(windowTitle.c_str());				// ������ ���� (������ �̸�)

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {					// glew �ʱ�ȭ
		std::cerr << "Unable to initialize GLEW\n";
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "GLEW Initialized\n";
	}

	glClearColor(0.f, 0.f, 0.f, 1.f);		// Ŭ���� ���� (�ƹ��͵� ������ ��)

	// �ʱ� ������
	g_scene.initialize();

	glEnable(GL_CULL_FACE);			// �ø�(�޸� ����) Ȱ��ȭ
	//glCullFace(GL_FRONT);			// �ո� ������� �޸� �������
	//glFrontFace(GL_CCW);			// ��� �ո�����?? GL_CW -> �ð�, GL_CCW -> �ݽð�(�⺻��)

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// ���� �˻� Ȱ��ȭ --> z���ۿ��ٰ� ����� �ϱ� �����ϰ�, �׸��� ���ƾ� �� ���� �׸��� �ʴ´�....
	glEnable(GL_DEPTH_TEST);

	// �ݹ� �Լ���
	glutKeyboardFunc(KeyboardFunc);		// �Ϲ� Ű���� (abcde..)
	glutKeyboardUpFunc(KeyboardUpFunc);	// �Ϲ� Ű���尡 ������ ��
	glutSpecialFunc(SpecialFunc);		// Ư�� Ű���� (F1 ~ F12, HOME, CTRL, ALT ���..)
	glutSpecialUpFunc(SpecialUpFunc);	// Ư�� Ű���尡 ������ ��

	glutMouseFunc(MouseFunc);			// ���콺 �Է� (��Ŭ��, ��Ŭ��, ��Ŭ��... 
	glutMotionFunc(MotionFunc);			// ȭ�� ������ �巡��

	glutDisplayFunc(DisplayFunc);		// ��� �Լ��� ����
	glutReshapeFunc(ReshapeFunc);		// ȭ�� ũ�Ⱑ ����Ǿ��� ��....
	//glutTimerFunc(16, TimerFunc, 0);		// 16ms --> �ʴ� 60��
	glutIdleFunc(IdleFunc);

	glutMainLoop(); // �̺�Ʈ ó�� ����
}

void DisplayFunc(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// �ĸ���� �����

	// ���� �׸���
	g_scene.draw();

	Model myModel("link.fbx");

	// �ĸ���۸� ���� ���ۿ� �ٲ��ش�!
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
//	case 0:			// ������Ʈ
//		g_scene.update();
//		glutPostRedisplay();	// ���� ������Ʈ ������, ȭ�鿡 �׷���� �Ѵ�..
//
//		glutTimerFunc(16, TimerFunc, 0);
//		break;
//	}
//}

void IdleFunc()	// ���� ����ȭ�� �ǰ��ִ�.. --> ��ǻ�͸��� �ٸ��� ������ �� ����
{
	// static�� ���� ����ó�� Ȱ��
	static int fps = 0;
	// ���� �ð�
	static std::chrono::high_resolution_clock::time_point last_time = std::chrono::high_resolution_clock::now();
	static std::chrono::high_resolution_clock::duration accm_time;	// ���� �ð�

	// ms->10-3, us->10-6, ns->10-9

	++fps;
	auto now_time = std::chrono::high_resolution_clock::now();
	auto dur_time = now_time - last_time;
	accm_time += dur_time;
	last_time = now_time;

	if (accm_time >= std::chrono::seconds(1)) {
		std::string str = windowTitle + " | (FPS : " + std::to_string(fps) + ")";
		glutSetWindowTitle(str.c_str());		// â�� FPS ǥ��

		fps = 0;
		accm_time = std::chrono::seconds(0);
	}

	g_scene.update(dur_time.count() / static_cast<float>(1E09));
	glutPostRedisplay();	// ���� ������Ʈ ������, ȭ�鿡 �׷���� �Ѵ�..
}
