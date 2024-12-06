#pragma once

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>

// 메쉬 데이터 저장용 구조체
struct MeshData {
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	unsigned int indexCount;
};


class GameObject;
class PlayerObject;
class Scene
{
private:
	GLuint shader;
	GLuint plainShader;

	GLuint sphereVAO;
	int sphereVertexCount;
	GLuint teapotVAO;
	int teapotVertexCount;

	PlayerObject* player;

	const aiScene* scene;
	std::vector<MeshData> meshes;

	GameObject* objects[10];
	int objectCount;

	int width;
	int height;

public:
	// 게임 로직에서, update, draw..., 키 입력
	Scene(int winWidth, int winHeight);
	~Scene();

	void initialize();
	void release();

	void update(float elapsedTime);

	void draw() const;

	void keyboard(unsigned char key, bool isPressed);
	void specialKeyboard(int key, bool isPressed);
	void mouse(int button, int state, int x, int y);
	void mouseMove(int x, int y);

	void setWindowSize(int winWidth, int winHeight);

private:
	static void initBuffer(GLuint* VAO, GLsizei* vertexCount, std::string objFilename);

public:
	static GLuint makeShader(std::string vertexFilename, std::string fragmentFilename);
	static std::string readFile(std::string filename);
	static std::vector<glm::vec3> readOBJ(std::string filename);


	/// 모델 로드 함수
	const aiScene* loadModel(const std::string& filePath);
	// 메쉬 데이터 처리 함수
	MeshData processMesh(const aiMesh* mesh);

	void processNode(aiNode* node, const aiScene* scene, std::vector<MeshData>& meshes);

	glm::mat4 calculateBoneTransform(float animationTime, const aiNodeAnim* channel);
};

