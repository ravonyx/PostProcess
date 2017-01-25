#ifndef CAMERA_H
#define CAMERA_H

#define MOVE_SPEED 1.0f
#define SENSIBILITY 0.001f

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include "Quaternion.hpp"

class Camera
{
public:
	Camera(int posX, int posY, int posZ, int dirZ);

	char locked;

	float posx;
	float posy;
	float posz;

	float dirx;
	float diry;
	float dirz;

	char deltaForward;
	char deltaStrafe;

	glm::mat4 _matrix;

	glm::vec3& getUpVector();
	void updatePos(void);
	void releaseCam(void);
	void grabCam(int x, int y);
	void orienterCam(int x, int y);
	glm::mat4 GetOrientation();
	void reset();

protected:
private:
	float angleh;
	float anglev;

	int xOrigin;
	int yOrigin;
	float deltaAnglex;
	float deltaAngley;

};

#endif // CAMERA_H
