
#include "stdafx.h"
#include "starobject.h"
#include <stdlib.h>
#include <math.h>
#include "display.h"

#define red 3
#define yellow 1
#define blue 2
#define white 0




extern TEXTURE_OBJECT *starpic;	
extern DISPLAY_ENGINE *dengine;
extern vector<TEXTURE_OBJECT*> planet_textures;

void loadstarimages()
{
	starpic = dengine->CreateTextureObject();
	starpic->LoadTexture("white star.png",1,1);
	starpic->SetNumFrames(1);
	char temp[100];
	TEXTURE_OBJECT *tempt;
	for(int count = 0;count < 16;count++)
	{
		sprintf(temp,"planet%d.png",count);
		tempt = dengine->CreateTextureObject();
		tempt->LoadTexture(temp,1,1);
		tempt->SetNumFrames(1);
		planet_textures.push_back(tempt);
	}
}

void destroystarimages()
{
	dengine->DestroyObject(starpic);
	TEXTURE_OBJECT *tempt;
	for(int count = 0;count < 16;count++)
	{
		tempt = planet_textures[count];
		dengine->DestroyObject(tempt);
	}
	planet_textures.clear();

}









forestar::forestar()
{
	sprite = dengine->CreateSpriteObject();
	trans = dengine->CreateTransform();
	sprite->AttachTextureObject(starpic);
	sprite->AttachTransform(trans);
	createnewstar(true);
}

forestar::~forestar()
{
	dengine->DestroyObject(sprite);
	dengine->DestroyObject(trans);
}

void forestar::createnewstar(bool first)
{

	if(first) 
	{
		x = ((rand() % 4000)*1000 - 2000000);
		y = ((rand() % 3000)*1000 - 1500000);

		z = ((rand() % 1000)*1000 + 1);
	}
	else
	{
		x = ((rand() % 4000)*1000 - 2000000);
		y = ((rand() % 3000)*1000 - 1500000);

		z = 1000000;

	}
	if(abs(x) <= 4000)
	{
		if(x > -4000) x = -4000;
		else x = 4000;
	}
	if(abs(y) <= 4000) 
	{
		if(y > -4000) y = -4000;
		else y = 4000;
	}

	type = rand() % 7;
	if(type == 0 || type == 1 || type == 2) type = white;
	if(type == 3 || type == 4) type = yellow;
	if(type == 5) type = blue;
	if(type == 6) type = red;
	float color1,color2;
	color1 = 0.7f;
	color2 = color1 + (rand() % 3000) / (float)10000;
	size = ((rand() % 7500) + 2001);
	size *= 0.8f;
	switch(type)
	{
	case red:
		r = color2;
		g = color1;
		b = color1;
		break;
	case yellow:
		r = color2;
		g = color2;
		b = color1;
		break;
	case blue:
		r = color1;
		g = color1;
		b = color2;
		break;
	case white:
		r = color2;
		g = color2;
		b = color2;
		break;
	}
}


void forestar::setres(int w,int h)
{
	minsize = 350.0f/(h*70);
}



void forestar::project()
{
	if(z != 0.0f) tempz = 1 / z;
	else tempz = 999999999.0f;
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
		createnewstar(false);
		project();
	}
	trans->Translate(xs,ys,z*0.0001);
	trans->Scale(temps,temps);
	sprite->SetColor(rs,gs,bs,1);

}
