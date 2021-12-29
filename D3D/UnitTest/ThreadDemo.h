#pragma once
#include "Systems/IExecute.h"

class ThreadDemo : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override {};
	virtual void Render() override;
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};


private:
	void Loop();

	void Loop1();
	void Loop2();
	void MultiThread();

	void ThreadOneParam(int a);
	void OneParam();

	void Sum(vector<int>::iterator start, vector<int>::iterator end, int* result);
	void Thread_Sum();

	void Lamda();

	void Thread_RaceCondition(int& count);
	void RaceCondition();

	void Thread_Deadlock1(mutex& m1, mutex& m2);
	void Thread_Deadlock2(mutex& m1, mutex& m2);
	void Deadlock();

private:
	float progress = 0.f;
	mutex m;

	mutex m1;
	mutex m2;
};