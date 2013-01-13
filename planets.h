// planets.h: interface for the planets class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLANETS_H__AC16E466_59DB_4366_8A99_C8B588BDCBDC__INCLUDED_)
#define AFX_PLANETS_H__AC16E466_59DB_4366_8A99_C8B588BDCBDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "display.h"

class planets  
{
public:
	planets();
	virtual ~planets();
	void project();
	inline void movestar(float time) {z -= time;};
	inline void drawstar() {if(z > 0) sprite->Render();};
	void setres(int w,int h);
	inline bool IsDone() {return done;};
	void createnewplanet(bool first);

private:
	float minsize;
	float x,y,z;
	float xs,ys;
	SPRITE_OBJECT *sprite;
	TRANSFORM *trans;
	float rs,gs,bs;
	float r,g,b;
	int type;
	float size;
	float temps,tempz;
	bool done;
};

#endif // !defined(AFX_PLANETS_H__AC16E466_59DB_4366_8A99_C8B588BDCBDC__INCLUDED_)
