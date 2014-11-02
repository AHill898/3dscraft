#include <3ds.h>

#include "camera.h"
#include "gs.h"

void initCamera(camera_s* c)
{
	if(!c)return;

	initProjectionMatrix((float*)c->projection, 1.3962634f, 240.0f/400.0f, 0.01f, 10.0f);
	loadIdentity44((float*)c->orientation);
	c->position=vect3Df(0.0f, 0.0f, 0.0f);
}

void updateCameraFrustum(camera_s* c)
{
	if(!c)return;

	float final[4*4];
	multMatrix44((float*)c->projection, (float*)c->modelview, final);

	const vect4Df_s rowX = getMatrixColumn4(final, 0);
	const vect4Df_s rowY = getMatrixColumn4(final, 1);
	const vect4Df_s rowZ = getMatrixColumn4(final, 2);
	const vect4Df_s rowW = getMatrixColumn4(final, 3);

	c->frustumPlane[0] = vnormf4(vaddf4(rowW, rowX));
	c->frustumPlane[1] = vnormf4(vsubf4(rowW, rowX));
	c->frustumPlane[2] = vnormf4(vaddf4(rowW, rowY));
	c->frustumPlane[3] = vnormf4(vsubf4(rowW, rowY));
	c->frustumPlane[4] = vnormf4(vaddf4(rowW, rowZ));
	c->frustumPlane[5] = vnormf4(vsubf4(rowW, rowZ));
}

void updateCamera(camera_s* c)
{
	if(!c)return;

	memcpy(c->modelview, c->orientation, sizeof(mtx44));
	translateMatrix((float*)c->modelview, -c->position.x, -c->position.y, -c->position.z);

	updateCameraFrustum(c);
}

void setCamera(camera_s* c)
{
	if(!c)return;

	gsMatrixMode(GS_PROJECTION);
	gsLoadIdentity();
	gsMultMatrix((float*)c->projection);

	gsMatrixMode(GS_MODELVIEW);
	gsMultMatrix((float*)c->modelview);
}

bool pointInCameraFrustum(camera_s* c, vect3Df_s pt)
{
	if(!c)return false;
	const vect4Df_s pt4=vect4Df(pt.x,pt.y,pt.z,1.0f);
    int i; for(i=0;i<6;i++)if(vdotf4(pt4,c->frustumPlane[i])<0.0f)return false;
    return true;
}