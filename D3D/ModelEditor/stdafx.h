#pragma once

#include "Framework.h"
#pragma comment(lib, "../Debug/Framework.lib")

//�����̳ʰ� ���� �� ����(*.fbx, *.obj)�� �о���� ����
#include "Assimp/Importer.hpp"

//�� ������ �о�� �� �ɼ��� �� �� �ִ� ����
#include "Assimp/postprocess.h"

//Bone(Node), Mesh, Material ������ �迭 �����ͷ� ������ ��
#include "Assimp/scene.h"

#pragma comment(lib, "Assimp/assimp-vc140-mt.lib")