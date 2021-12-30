#include "stdafx.h"
#include "ThreadDemo.h"
#include <iostream>
using namespace std;

string Macaroon::tasts[5] =
{
	"���Ǹ�",
	"�Ҵ߸�",
	"�ع���",
	"���ʸ�",
	"��������"
};

void ThreadDemo::Initialize()
{
	//Loop();
	//MultiThread();
	//OneParam();
	//Thread_Sum();
	//Lamda();
	//RaceCondition();
	//Deadlock();
	//Provide_Consumer();
	//ExecuteTimer();
	PerformanceMeasurement();
	
}

void ThreadDemo::Destroy()
{

}

void ThreadDemo::Update()
{
	progress += 0.1f; //Write
	ImGui::ProgressBar(progress / 1000.0f);
}

void ThreadDemo::Render()
{
	
}

void ThreadDemo::Loop()
{
	for (UINT i = 0; i < 100; i++)
		printf("Loop1 : %d\n", i);
	printf("Loop1 End\n");

	for (UINT i = 0; i < 100; i++)
		printf("Loop2 : %d\n", i);
	printf("Loop2 End\n");
}

void ThreadDemo::Loop1()
{
	for (UINT i = 0; i < 100; i++)
		printf("Loop1 : %d\n", i);
	printf("Loop1 End\n");
}

void ThreadDemo::Loop2()
{
	for (UINT i = 0; i < 100; i++)
		printf("Loop2 : %d\n", i);
	printf("Loop2 End\n");
}

void ThreadDemo::MultiThread()
{
	thread t1(bind(&ThreadDemo::Loop1, this));
	thread t2(bind(&ThreadDemo::Loop2, this));

	//join -> �������� ���� ������ ����
	//t2.join();
	//printf("t2 join\n");
	//t1.join();
	//printf("t1 join\n");

	//detach -> �������� ���� ������ ���������� �ʰ� ��ƾ�� ����Ǹ� �ڵ� ����
	t2.detach();
	printf("t2 detach\n");
	t1.detach();
	printf("t1 detach\n");

}

void ThreadDemo::ThreadOneParam(int a)
{
	thread::id id = this_thread::get_id();

	for (int i = 0; i < a; i++)
		printf("%d : %d\n", id, i);;
}

void ThreadDemo::OneParam()
{
	function<void(int)> f = bind(&ThreadDemo::ThreadOneParam, this, placeholders::_1);

	thread t1(f, 10);
	thread t2(&ThreadDemo::ThreadOneParam, this, 5);
	t1.detach();
	t2.detach();
}

void ThreadDemo::Sum(vector<int>::iterator start, vector<int>::iterator end, int* result)
{
	int sum = 0;
	for (auto iter = start; iter < end; iter++)
		sum += *iter;

	*result = sum;

	thread::id this_id = this_thread::get_id();
	printf("������(%x)���� %d ~ %d���� �ջ��� ��� %d\n", this_id, *start, *end - 1, sum);
	//cout << "������(" << this_id << ")���� " << *start << " ~ " << *end - 1 << "���� �ջ��� ��� " << sum << endl;
}

void ThreadDemo::Thread_Sum()
{
	//Data
	vector<int> datas;
	for (int i = 0; i <= 100; i++)
		datas.push_back(i);

	//�� ������ �׷��� ó�� ����� ������� ����
	vector<int> thread_groups_each_sums(4);

	//���� ������ ����
	vector<thread> threads;
	for (int i = 0; i < 4; i++)
		threads.push_back(thread
		(
			&ThreadDemo::Sum,
			this,
			datas.begin() + i * 25,
			datas.begin() + ((i + 1) * 25),
			&thread_groups_each_sums[i])
		);

	//������ ����
	for (int i = 0; i < 4; i++)
		threads[i].join();

	//������ �׷��� ó�� ����� �ջ�
	int sum = 0;
	for (int i = 0; i < 4; i++)
		sum += thread_groups_each_sums[i];

	printf("Sum : %d\n", sum);
	
}

void ThreadDemo::Lamda()
{
	thread t1([&]()
	{ 
		while (true)
		{
			Sleep(100);

			printf("Progress : %f\n", progress); //Read

			if (progress >= 1000)
			{
				printf("Done!\n");
				break;
			}
			
		}
	});

	
	t1.detach();

}

void ThreadDemo::Thread_RaceCondition(int & count)
{
	lock_guard<mutex> lock(m);
	//m.lock();
	for (int i = 0; i < 1e+6; i++)
		count++;
	//m.unlock();
}

void ThreadDemo::RaceCondition()
{
	int count = 0;
	vector<thread> threads;

	for (int i = 0; i < 4; i++)
	{
		function<void(int&)> deleOneParam = bind(&ThreadDemo::Thread_RaceCondition, this, placeholders::_1);
		threads.push_back(thread(deleOneParam, ref(count)));
	}

	for (int i = 0; i < 4; i++)
		threads[i].join();

	printf("Count : %d\n", count);
}

void ThreadDemo::Thread_Deadlock1(mutex & m1, mutex & m2)
{
	for (int i = 0; i < 10000; i++)
	{
		lock_guard<mutex> lock1(m1);
		lock_guard<mutex> lock2(m2);
		printf("Thread1 : %d\n", i);
	}
}

void ThreadDemo::Thread_Deadlock2(mutex & m1, mutex & m2)
{
	for (int i = 0; i < 10000; i++)
	{
		while (true)
		{
			m2.lock();

			if (m1.try_lock() == false)
			{
				m2.unlock();
				continue;
			}

			printf("Thread2 : %d\n", i);
			m1.unlock();
			m2.unlock();
			break;
		}
	}
}

void ThreadDemo::Deadlock()
{
	thread t1(&ThreadDemo::Thread_Deadlock1, this, ref(m1), ref(m2));
	thread t2(&ThreadDemo::Thread_Deadlock2, this, ref(m1), ref(m2));
	t1.join();
	t2.join();

	printf("���η�ƾ ��\n");
}

void ThreadDemo::Provide(queue<string>* receives, mutex * m, int index)
{
	string* tasts = Macaroon::Tasts();

	for (int i = 0; i < 5; i++)
	{
		//������ ������
		this_thread::sleep_for(chrono::microseconds(index * 100));
		string content = "������(" + to_string(index) + "), �� : " + tasts[i];

		m->lock();
		receives->push(content);
		m->unlock();
	}
}

void ThreadDemo::Consumer(queue<string>* receives, mutex * m, int * count)
{
	while (*count < 25)
	{
		m->lock();

		if (receives->empty())
		{
			m->unlock();

			this_thread::sleep_for(chrono::microseconds(10));
			continue;
		}

		string poped = receives->front();
		receives->pop();
		(*count)++;
		m->unlock();

		printf("Dequeue : %2d, %s\n", *count, poped.c_str());
		this_thread::sleep_for(chrono::microseconds(80));
	}
}

void ThreadDemo::Provide_Consumer()
{
	//������ ������ * 5EA ����
	vector<thread> providers;
	for (int i = 0; i < 5; i++)
		providers.push_back(thread(&ThreadDemo::Provide, this, &receives, &m, i + 1));

	//�Һ� ������ ����
	int processCount = 0;
	thread consumer(bind(&ThreadDemo::Consumer, this, placeholders::_1, placeholders::_2, placeholders::_3), &receives, &m, &processCount);

	//������ ����
	for (int i = 0; i < 5; i++)
		providers[i].join();
	consumer.join();

	cout << "���� ���� �� ���ҳ� ? : " << ((processCount != 25) ? "����" : "����") << endl;
}

void ThreadDemo::ExecuteTimer()
{
	timer[0].Start([]()
	{
		printf("�� 2�ʸ��� 2�� �ݵ���\n"); 
	}, 2000, 2);

	timer[1].Start([]() 
	{
		printf("�� 1�ʸ��� ���� �ݵ���\n");
	}, 1000);
}

void ThreadDemo::PerformanceMeasurement()
{
	int a[10000];
	for (int i = 0; i < 10000; i++)
		a[i] = Math::Random(0, 10000);

	Performance p;
	p.Start();
	{
		sort(a, a + 10000);
	}
	printf("����ð� : %f\n", p.End());

	for (int i = 0; i < 10000; i++)
	{
		if (i % 5 == 0)
			printf("\n");
		
		printf("%4d\t", a[i]);
	}
}


