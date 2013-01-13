// planets.cpp: implementation of the planets class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "planets.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern DISPLAY_ENGINE *dengine;
extern vector<TEXTURE_OBJECT*> planet_textures;

planets::planets()
{
	sprite = dengine->CreateSpriteObject();
	trans = dengine->CreateTransform();
	for(int count = 0;count < 16;count++)
	{
		sprite->AttachTextureObject(planet_textures[count]);
	}
	sprite->AttachTransform(trans);
	sprite->SetFrame(0);
	createnewplanet(true);
	sprite->SetMultiTex(false);
	done = false;
}

planets::~planets()
{
	dengine->DestroyObject(sprite);
	dengine->DestroyObject(trans);
}



void planets::createnewplanet(bool first)
{
	done = false;
	x = ((rand() % 2000) - 1000);
	y = ((rand() % 1500) - 750);
		
	z = 364000;

	if(abs(x) <= 200)
	{
		if(x > -200) x = -200;
		else x = 200;
	}
	if(abs(y) <= 200) 
	{
		if(y > -200) y = -200;
		else y = 200;
	}
	type = rand() % 16;
	sprite->SetTexture(type);
	float color1,color2;
	color1 = 0.7f;
	color2 = color1 + (rand() % 3000) / (float)10000;
	size = ((rand() % 5000) + 5001);
	size *= 0.10f;
	type = rand() % 4;
	switch(type)
	{
	case 0:
		r = color2;
		g = color1;
		b = color1;
		break;
	case 1:
		r = color2;
		g = color2;
		b = color1;
		break;
	case 2:
		r = color1;
		g = color1;
		b = color2;
		break;
	case 3:
		r = color2;
		g = color2;
		b = color2;
		break;
	}
}


void planets::setres(int w,int h)
{
	minsize = 350.0f/(h*70);
}


void planets::project()
{
	if(tempz != 0.0f) tempz = 1 / z;
	else tempz = 9999999.0f;
	xs = (x *tempz);
	ys = (y *tempz);
	temps = size*tempz;
	if(temps < minsize) temps = minsize;
	tempz = 200000.0f*tempz + 0.2f;

	if(tempz > 1.1f) tempz = 1.1f;
	rs = r * tempz;
	gs = g * tempz;
	bs = b * tempz;

	if(!(ys > -2 && ys <= 2 && xs > -3 && xs <= 3 && z >= 1))
	{
		done = true;
	}
	trans->Translate(xs,ys,z*0.0001);
	trans->Scale(temps,temps);
	sprite->SetColor(rs,gs,bs,1);

}
