#include "Scene.h"

#include <iostream>
#include <fstream>
#include <sstream>


#include <vector>

#include "RotateObject.h"
#include "PlayerObject.h"

Scene::Scene(int winWidth, int winHeight)
	: width{ winWidth }, height{ winHeight }
{
}

Scene::~Scene()
{
	release();
}

void Scene::initialize()
{
	shader = makeShader("./Shader/vertex.glsl", "./Shader/fragment.glsl");
	plainShader = makeShader("./Shader/plainVert.glsl", "./Shader/plainFrag.glsl");

	// initBuffer(&sphereVAO, &sphereVertexCount, "./model/OBJ/sphere.obj");
	// initBuffer(&teapotVAO, &teapotVertexCount, "./model/OBJ/teapot.obj");

	player = new PlayerObject;

	player->rotateY(180.f);

	player->setPosition(0.f, 0.f, 10.f);

	srand(clock());

	objects[0] = new RotateObject;		// 업캐스팅
	objects[0]->setShader(shader);
	objects[0]->setVAO(sphereVAO, sphereVertexCount);
	objects[0]->setPosition(0.f, 0.5f, 0.f);

	objectCount = 1;


	// 모델 로드
	scene = loadModel("./model/fbx/link.fbx");
	if (!scene) exit(0);

	processNode(scene->mRootNode, scene, meshes);

}

void Scene::release()
{
	for (int i = 0; i < objectCount; ++i)
		delete objects[i];

	delete player;
}

void Scene::update(float elapsedTime)
{
	player->update(elapsedTime);

	for (int i = 0; i < objectCount; ++i)
		objects[i]->update(elapsedTime);	// 업캐스팅 시에도 RotateObject의 update가 호출된다! -> virtual
}

void Scene::draw() const
{
	glm::vec3 cameraPos = player->getPosition();		// 플레이어 위치에서
	cameraPos.y = 1.f;
	glm::vec3 targetPos = cameraPos + player->getLook();// 플레이어 앞을 바라본다

	glm::mat4 viewMatrix = glm::lookAt(cameraPos, targetPos, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 projMatrix = glm::perspective(glm::radians(45.f), float(width) / float(height), 0.1f, 100.f);

	{	// 바닥을 깔아준다
		// 카메라, 투영은 씬 전체에 적용..
		glUseProgram(plainShader);

		GLint viewLoc = glGetUniformLocation(plainShader, "viewTransform");
		if (viewLoc < 0)
			std::cout << "viewLoc 찾지 못함\n";
		GLint projLoc = glGetUniformLocation(plainShader, "projTransform");
		if (projLoc < 0)
			std::cout << "projLoc 찾지 못함\n";

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glUseProgram(shader);

	// 카메라, 투영은 씬 전체에 적용..
	GLint viewLoc = glGetUniformLocation(shader, "viewTransform");
	if (viewLoc < 0)
		std::cout << "viewLoc 찾지 못함\n";
	GLint projLoc = glGetUniformLocation(shader, "projTransform");
	if (projLoc < 0)
		std::cout << "projLoc 찾지 못함\n";
	GLint cameraPosLoc = glGetUniformLocation(shader, "cameraPos");
	if (cameraPosLoc < 0)
		std::cout << "cameraPosLoc 찾지 못함\n";

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
	glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

	// 메쉬 렌더링
	for (const auto& mesh : meshes) {
		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
	}


	// 오브젝트 그리기
	player->draw();		// 안그리긴 해도... 나중에 그릴 수 있으니 호출해준다

	//for (int i = 0; i < objectCount; ++i)
	//	objects[i]->draw();		// 업캐스팅 시에도 RotateObject의 draw가 호출된다! -> virtual

}

void Scene::keyboard(unsigned char key, bool isPressed)
{
	player->keyboard(key, isPressed);

	if (isPressed) {			// 눌러졌을 때
		switch (key) {
		default:
			break;
		}
	}
	else {						// 떼졌을 때
		switch (key) {
		default:
			break;
		}
	}
}

void Scene::specialKeyboard(int key, bool isPressed)
{
}

void Scene::mouse(int button, int state, int x, int y)
{
	player->mouse(button, state, x, y);

	// 화면 업데이트가 된다....

	constexpr int WHEEL_UP = 3;
	constexpr int WHEEL_DOWN = 4;

	switch (state) {
	case GLUT_DOWN:
		switch (button) {
		case GLUT_LEFT_BUTTON: {
			float xPos = (static_cast<float>(x) / width * 2.f - 1.f);
			float yPos = -(static_cast<float>(y) / height * 2.f - 1.f);

			//std::cout << "좌클릭 : " << x << ", " << y << std::endl;
			//std::cout << "OpenGL x 좌표는 " << xPos << std::endl;
			//std::cout << "OpenGL y 좌표는 " << yPos << std::endl;

			break;
		}
		case GLUT_MIDDLE_BUTTON:
			std::cout << "휠클릭 : " << x << ", " << y << std::endl;
			break;
		case GLUT_RIGHT_BUTTON:
			std::cout << "우클릭 : " << x << ", " << y << std::endl;
			break;
		case WHEEL_UP:
			std::cout << "휠  업 : " << x << ", " << y << std::endl;
			break;
		case WHEEL_DOWN:
			std::cout << "휠다운 : " << x << ", " << y << std::endl;
			break;
		}
		break;
	case GLUT_UP:
		switch (button) {
		case GLUT_LEFT_BUTTON:
			break;
		case GLUT_RIGHT_BUTTON:
			break;
		case GLUT_MIDDLE_BUTTON:
			break;
		}
		break;
	}
}

void Scene::mouseMove(int x, int y)
{
	player->mouseMove(x, y);
}

void Scene::setWindowSize(int winWidth, int winHeight)
{
	width = winWidth;
	height = winHeight;
}

void Scene::initBuffer(GLuint* VAO, GLsizei* vertexCount, std::string objFilename)
{
	glGenVertexArrays(1, VAO);		// 동적 할당....
	glBindVertexArray(*VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	std::vector<glm::vec3> verticies = readOBJ(objFilename);

	*vertexCount = verticies.size() / 3;

	// GPU는 하나의 컴퓨터와 같다... 내부에 gpu전용 cpu, 램, 등등 존재...

	// CPU 메모리에 있는 데이터를, GPU 메모리에 복사
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies[0]) * verticies.size(), verticies.data(), GL_STATIC_DRAW);

	// 이 데이터가 어떤 데이터인지, 우리가 정의를 했기 때문에, openGL에 알려줘야 한다!
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, 0);
	// location, 갯수, 타입, nomalized?, 간격(바이트), 시작오프셋
	glEnableVertexAttribArray(0);

	//// 이 데이터가 어떤 데이터인지, 우리가 정의를 했기 때문에, openGL에 알려줘야 한다!
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, reinterpret_cast<void*>(sizeof(float) * 3));
	// location, 갯수, 타입, nomalized?, 간격(바이트), 시작오프셋
	glEnableVertexAttribArray(1);

	//// 이 데이터가 어떤 데이터인지, 우리가 정의를 했기 때문에, openGL에 알려줘야 한다!
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, reinterpret_cast<void*>(sizeof(float) * 6));
	// location, 갯수, 타입, nomalized?, 간격(바이트), 시작오프셋
	glEnableVertexAttribArray(2);

}

GLuint Scene::makeShader(std::string vertexFilename, std::string fragmentFilename)
{
	// 쉐이더 만들자...

	GLint result;
	GLchar errorLog[512];

	GLuint vertexShader, fragmentShader;

	std::string vertexStr = readFile(vertexFilename);	// 이 변수는 지역변수라서
	const char* vertexSource{ vertexStr.c_str() };		// 이 변수에 저장되려면 위 변수가 소멸되면 안된다..!

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << vertexFilename << " ERROR: vertex shader error\n" << errorLog << std::endl;
	}
	else {
		std::cout << vertexFilename << " 컴파일 성공!" << std::endl;
	}

	std::string fragmentStr = readFile(fragmentFilename);
	const char* fragmentSource{ fragmentStr.c_str() };

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << fragmentFilename << " ERROR: fragment shader error\n" << errorLog << std::endl;
	}
	else {
		std::cout << fragmentFilename << " 컴파일 성공!" << std::endl;
	}

	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShader);		// 만든 vertex Shader를 쉐이더 프로그램에 추가해 준다
	glAttachShader(shaderID, fragmentShader);	// 만든 fragment Shader를 쉐이더 프로그램에 추가해 준다
	glLinkProgram(shaderID);					// 추가한 쉐이더들을 하나의 프로그램으로 생성한다!

	glDeleteShader(vertexShader);				// 이제 더이상 vertex Shaer는 필요없다...
	glDeleteShader(fragmentShader);				// 이제 더이상 fragment Shaer는 필요없다...

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << vertexFilename << ", " << fragmentFilename << " ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		//return;
	}
	else {
		std::cout << vertexFilename << ", " << fragmentFilename << " shader program 생성 성공!" << std::endl;
	}

	return shaderID;
}

std::string Scene::readFile(std::string filename)
{
	std::ifstream in{ filename };
	if (!in) {
		std::cout << filename << "파일 존재하지 않음!" << std::endl;
		exit(1);
	}

	std::string str;
	std::string temp;
	while (std::getline(in, temp)) {
		str += temp;
		str.append(1, '\n');
	}

	return str;
}

std::vector<glm::vec3> Scene::readOBJ(std::string filename)
{
	std::ifstream in{ filename };
	if (!in) {
		std::cout << filename << " file read failed\n";
		exit(1);
	}

	// c++ stream --> input output을 해주는 흐름?

	srand(static_cast<unsigned int>(time(nullptr)));

	std::vector<glm::vec3> vertex;
	std::vector<glm::vec3> color;
	std::vector<glm::vec3> normal;
	std::vector<glm::ivec3> vindex;
	std::vector<glm::ivec3> nindex;
	while (in) {
		std::string line;
		std::getline(in, line);
		std::stringstream ss{ line };
		std::string str;
		ss >> str;
		if (str == "v") {
			glm::vec3 v;
			for (int i = 0; i < 3; ++i) {
				std::string subStr;
				ss >> subStr;
				v[i] = std::stof(subStr);
			}
			vertex.push_back(v);
			color.push_back(glm::vec3(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));
		}
		else if (str == "vn") {
			glm::vec3 n;
			for (int i = 0; i < 3; ++i) {
				std::string subStr;
				ss >> subStr;
				n[i] = std::stof(subStr);
			}
			normal.push_back(n);
		}
		else if (str == "f") {
			glm::ivec3 fv;
			glm::ivec3 fn;
			for (int i = 0; i < 3; ++i) {
				std::string substr;
				ss >> substr;
				std::stringstream subss{ substr };
				std::string vIdx;
				std::getline(subss, vIdx, '/');
				fv[i] = std::stoi(vIdx) - 1;
				std::getline(subss, vIdx, '/');
				// 텍스처 건너뛰기
				std::getline(subss, vIdx, '/');
				fn[i] = std::stoi(vIdx) - 1;
			}
			vindex.push_back(fv);
			nindex.push_back(fn);
		}
	}

	std::vector<glm::vec3> data;
	for (int i = 0; i < vindex.size(); ++i) {
		data.push_back(vertex[vindex[i][0]]);
		data.push_back(color[vindex[i][0]]);
		data.push_back(normal[nindex[i][0]]);
		data.push_back(vertex[vindex[i][1]]);
		data.push_back(color[vindex[i][1]]);
		data.push_back(normal[nindex[i][1]]);
		data.push_back(vertex[vindex[i][2]]);
		data.push_back(color[vindex[i][2]]);
		data.push_back(normal[nindex[i][2]]);
	}

	std::cout << filename << " File Read, " << data.size() / 3 << " Vertices Exists." << std::endl;
	return data;
}

const aiScene* Scene::loadModel(const std::string& filePath)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(
		filePath,
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
		return nullptr;
	}

	std::cout << "Model loaded successfully: " << filePath << std::endl;
	return scene;
}

MeshData Scene::processMesh(const aiMesh* mesh)
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	// 정점 데이터 로드
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		vertices.push_back(mesh->mVertices[i].x);
		vertices.push_back(mesh->mVertices[i].y);
		vertices.push_back(mesh->mVertices[i].z);

		if (mesh->HasNormals()) {
			vertices.push_back(mesh->mNormals[i].x);
			vertices.push_back(mesh->mNormals[i].y);
			vertices.push_back(mesh->mNormals[i].z);
		}

		if (mesh->mTextureCoords[0]) {
			vertices.push_back(mesh->mTextureCoords[0][i].x);
			vertices.push_back(mesh->mTextureCoords[0][i].y);
		}
		else {
			vertices.push_back(0.0f);
			vertices.push_back(0.0f);
		}
	}

	// 인덱스 데이터 로드
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		const aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// OpenGL VAO/VBO/EBO 생성
	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// 정점 속성 설정
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	MeshData meshData = { VAO, VBO, EBO, static_cast<unsigned int>(indices.size()) };
	return meshData;
}

void Scene::processNode(aiNode* node, const aiScene* scene, std::vector<MeshData>& meshes)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene, meshes);
	}
}

glm::mat4 Scene::calculateBoneTransform(float animationTime, const aiNodeAnim* channel)
{
	// 위치, 회전, 스케일 보간 및 본 변환 계산
	aiVector3D position(0.0f, 0.0f, 0.0f);
	aiQuaternion rotation(1.0f, 0.0f, 0.0f, 0.0f);
	// ... 보간 로직 추가 ...
	return glm::mat4(1.0f);
}
