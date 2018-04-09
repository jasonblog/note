#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include "SocketPoseClient.h"

using namespace std;

void sendPoseFromFakePoseGenerator()
{
	SocketPoseClient spc;
	float pos[3] = {0.0, 0.0, 2.0};
	float rot[3] = {0.0, 0.0, 0.0};

	for (int i = 0; i < 20; i++) {
		pos[2] += 0.3;
		spc.sendPose(pos, rot);
		usleep(1000*100);
	}
}

void sendPoseFromFile(const char *filename)
{
	SocketPoseClient spc;

	char buf[128];
	FILE *fp = NULL;
	int ret;
	float pos[3];
	float rot[3];

	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Error: can not read %s file!\n", filename);
		return;
	}

	while (1) {
		ret = fscanf(fp, "%f, %f, %f, %f, %f, %f", &pos[0], &pos[1], &pos[2], &rot[0], &rot[1], &rot[2]);
		if (ret == EOF)
			break;

		spc.sendPose(pos, rot, true);
		usleep(1000*50);
	}

	if (fp != NULL)
		fclose(fp);
}

int main(int argc, char* argv[])
{
    /*if (argc < 3) {
        fprintf(stderr, "Usage: %s ip_address port_number\n", argv[0]);
        exit(1);
    }*/

	//sendPoseFromFakePoseGenerator();
	sendPoseFromFile(argv[1]);

	printf("end of process.\n");

    return 0;
}
