#pragma once

#include "Framework.h"
#pragma comment(lib, "../Debug/Framework.lib")

//디자이너가 만든 모델 파일(*.fbx, *.obj)을 읽어오는 역할
#include "Assimp/Importer.hpp"

//모델 파일을 읽어올 때 옵션을 줄 수 있는 역할
#include "Assimp/postprocess.h"

//Bone(Node), Mesh, Material 정보가 배열 데이터로 들어오는 곳
#include "Assimp/scene.h"

#pragma comment(lib, "Assimp/assimp-vc140-mt.lib")