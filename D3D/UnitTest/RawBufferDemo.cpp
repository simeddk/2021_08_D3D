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
		float RetValue;
	};

	float* data = new float[count];
	for (int i = 0 ; i < count; i++)
		data[i] = Math::Random(0.0f, 1000.0f);

	RawBuffer* rawBuffer = new RawBuffer(data, sizeof(float) * count, sizeof(Output) * count);

	shader->AsSRV("Input")->SetResource(rawBuffer->SRV());
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
			"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f\n",
			temp.GroupId[0], temp.GroupId[1], temp.GroupId[2], //GroupID
			temp.GroupThreadId[0], temp.GroupThreadId[1], temp.GroupThreadId[2], //GroupThreadID
			temp.DispatchThreadId[0], temp.DispatchThreadId[1], temp.DispatchThreadId[2], //DispatchThreadId
			temp.GroupIndex, //GroupIndex
			temp.RetValue
		);
	}
	fclose(file);

}
