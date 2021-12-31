#include "stdafx.h"
#include "RawBufferDemo.h"

void RawBufferDemo::Initialize()
{
	Shader* shader = new Shader(L"17_ByteAddress.fxo");

	UINT count = 10 * 8 * 3 * 2;

	struct Output
	{
		UINT GroupId[3];
		UINT GroupThreadId[3];
		UINT DispatchThreadId[3];
		UINT GroupIndex;
	};

	RawBuffer* rawBuffer = new RawBuffer(nullptr, 0, sizeof(Output) * count);

	shader->AsUAV("Output")->SetUnorderedAccessView(rawBuffer->UAV());
	shader->Dispatch(0, 0, 2, 1, 1);

	Output* output = new Output[count];
	rawBuffer->CopyFromOutput(output);

	FILE* file = nullptr;
	fopen_s(&file, "../RawBufferResult.csv", "w");
	for (UINT i = 0; i < count; i++)
	{
		Output temp = output[i];

		fprintf
		(
			file,
			"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
			temp.GroupId[0], temp.GroupId[1], temp.GroupId[2], //GroupID
			temp.GroupThreadId[0], temp.GroupThreadId[1], temp.GroupThreadId[2], //GroupThreadID
			temp.DispatchThreadId[0], temp.DispatchThreadId[1], temp.DispatchThreadId[2], //DispatchThreadId
			temp.GroupIndex //GroupIndex
		);
	}
	fclose(file);

}
