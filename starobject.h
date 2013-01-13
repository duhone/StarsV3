#include "display.h"




class forestar
{
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
protected:
	void createnewstar(bool first);
public:
	void project();
	forestar();
	~forestar();
	inline void movestar(float time) {z -= time;};
	inline void drawstar() {sprite->Render();};
	void setres(int w,int h);
};


void loadstarimages();
void destroystarimages();