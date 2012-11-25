//-------------------------------------------------------------------------------------
//
// This is part of MarioDemo, a platformer demo for JGE++
// 
// Copyright (C) 2006 James Hui (a.k.a. Dr.Watson)
// 
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option) any
// later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA 02111-1307 USA
// 
// Bugs and comments can be forwarded to jhkhui@yahoo.com. 
// 
//-------------------------------------------------------------------------------------

#ifndef _MESH_EFFECT_H_
#define _MESH_EFFECT_H_


#include "JDistortionMesh.h"


#define MESH_EFFECT1			0
#define MESH_EFFECT2			1


class MeshEffect
{
private:
	float mMax;
	JDistortionMesh* mDistortionMesh;

	float mAlpha;
	float mTime;
	int mMode;

	int mCols;
	int mRows;

	float mX;
	float mY;


public:
	MeshEffect(JTexture* tex, float x, float y, float width, float height, int cols=16, int rows=16) 
	{
 		mDistortionMesh = new JDistortionMesh(tex, x, y, width, height, cols, rows);

		mMax = 32.0f;
		mTime = 0.0f;
		mMode = MESH_EFFECT1;

		mCols = cols;
		mRows = rows;
	}


	~MeshEffect() 
	{
		delete mDistortionMesh;
	}


	void SetMode(int mode)
	{
		mMode = mode;
	}


	void SetMax(float max) 
	{
		mMax = max;
	}


	void SetAlpha(float alpha)
	{
		mAlpha = alpha;
	}

	
	void SetPosition(float x, float y)
	{
		mX = x;
		mY = y;
	}


	void Update(float dt)
	{
	
		int i, j, col;

		mTime += (dt/1000.0f);
	
		int alpha = (int)mAlpha;
		float xDis, yDis;

	
		switch(mMode)
		{
			case MESH_EFFECT1:	
				for(i=1;i<mRows-1;i++)
					for(j=1;j<mCols-1;j++)
					{
						xDis = cosf(mTime*10+(i+j)/2)*5;
						yDis = sinf(mTime*10+(i+j)/2)*5;
						if (xDis >= 0.0f)
						{
							if (xDis > mMax)
								xDis = mMax;
						}
						else
						{
							if (xDis < -mMax)
								xDis = -mMax;
						}

						if (yDis >= 0.0f)
						{
						
							if (yDis > mMax)
								yDis = mMax;
						}
						else
						{
							if (yDis < -mMax)
								yDis = -mMax;
						}
						mDistortionMesh->SetDisplacement(j,i,xDis,yDis);
						mDistortionMesh->SetColor(j,i,ARGB(alpha,0,0,0));
					}
				break;

			case MESH_EFFECT2:	
				for(i=0;i<mRows;i++)
					for(j=1;j<mCols-1;j++)
					{
						xDis = cosf(mTime*5+j/2)*15;
						if (xDis >= 0.0f)
						{
							if (xDis > mMax)
								xDis = mMax;
						}
						else
						{
							if (xDis < -mMax)
								xDis = -mMax;
						}
						
						mDistortionMesh->SetDisplacement(j,i,xDis,0);
						col=int((cosf(mTime*5+(i+j)/2)+1)*35);
						mDistortionMesh->SetColor(j,i,ARGB(alpha,col,col,col));
					}
				break;
		}

	}


	void Render() 
	{
		mDistortionMesh->Render(mX, mY);

	}

};


#endif

