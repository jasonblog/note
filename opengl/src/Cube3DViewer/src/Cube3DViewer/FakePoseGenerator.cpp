#include "FakePoseGenerator.h"

#include <stdio.h>
#include <unistd.h>

FakePoseGenerator::FakePoseGenerator() 
{
	mThread = std::thread(&FakePoseGenerator::run, this);
}

FakePoseGenerator::~FakePoseGenerator() 
{
	if (mThread.joinable())
		mThread.join();
}

inline void opencv2opengl(float pos[3], float euler[3])
{
	pos[0] = -pos[0];
	pos[1] = -pos[1];
	//pos[2] = pos[2];
	euler[0] = -euler[0];
	euler[1] = -euler[1];
	euler[2] = -euler[2];
}

void FakePoseGenerator::run()
{
	poseFromGenerator();
	//poseFromFile("pose1.log");
}

void FakePoseGenerator::poseFromGenerator()
{
	mPos[0] = 0;
	mPos[1] = 0;
	mPos[2] = 0;
	mRot[0] = 0;
	mRot[1] = 0;
	mRot[2] = 0;

	for (int i = 0; i < 50; i++) {
		if (mPos[2] <= 10.0f) // z axis
			mPos[2] += 0.3;
		usleep(1000*100);
	}

	for (int i = 0; i < 100; i++) {
		if (mRot[1] <= 175.0f) // y axis
			mRot[1] += 2.5;
		usleep(1000*100);
	}
}

void FakePoseGenerator::poseFromFile(const char *filename)
{
	char buf[512];
	FILE *fp = NULL;
	int ret;
	float pos[3];
	float rot[3];
	float scale = 100.0;

	fp = fopen(filename, "r");
	while (1) {
		ret = fscanf(fp, "%f, %f, %f, %f, %f, %f", &pos[0], &pos[1], &pos[2], &rot[0], &rot[1], &rot[2]);
		if (ret == EOF)
			break;
		usleep(1000*50);

		opencv2opengl(pos, rot);
		mPos[0] = pos[0] * scale;
		mPos[1] = pos[1] * scale;
		mPos[2] = pos[2] * scale;
		mRot[0] = rot[0];
		mRot[1] = rot[1];
		mRot[2] = rot[2];
	}
	if (fp != NULL)
		fclose(fp);
}

void FakePoseGenerator::getPose(float pos[3], float rot[3])
{
	pos[0] = mPos[0];
	pos[1] = mPos[1];
	pos[2] = mPos[2];
	rot[0] = mRot[0];
	rot[1] = mRot[1];
	rot[2] = mRot[2];
}
