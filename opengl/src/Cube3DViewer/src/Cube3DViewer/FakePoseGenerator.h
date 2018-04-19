#ifndef FAKEPOSEGENERATOR_H
#define FAKEPOSEGENERATOR_H

#include <thread>

class FakePoseGenerator
{
public:
	FakePoseGenerator();
	~FakePoseGenerator();

	void getPose(float pos[3], float rot[3]);

private:
	std::thread mThread;
	float mPos[3] = {0.0f, 0.0f, 0.0f};
	float mRot[3] = {0.0f, 0.0f, 0.0f};

	void run();
	void poseFromGenerator();
	void poseFromFile(const char *filename);
};

#endif // FAKEPOSEGENERATOR_H
