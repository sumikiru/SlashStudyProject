#pragma once
#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(Location) if(GetWorld())DrawDebugSphere(GetWorld(),Location,25.f,12,FColor::Red,true);
#define DRAW_SPHERE_COLOR(Location,Color) if(GetWorld())DrawDebugSphere(GetWorld(),Location,8.f,12,Color,false,5.f);
#define DRAW_SPHERE_SingleFrame(Location) if(GetWorld())DrawDebugSphere(GetWorld(),Location,25.f,12,FColor::Red,false,-1.f);
#define DRAW_LINE(LineStart,LineEnd) if(GetWorld())DrawDebugLine(GetWorld(),LineStart,LineEnd,FColor::Red,true,-1.f,0,1.f);
#define DRAW_LINE_SingleFrame(LineStart,LineEnd) if(GetWorld())DrawDebugLine(GetWorld(),LineStart,LineEnd,FColor::Red,false,-1.f,0,1.f);
#define DRAW_POINT(Location) if(GetWorld())DrawDebugPoint(GetWorld(),Location,15.f,FColor::Red,true);
#define DRAW_POINT_SingleFrame(Location) if(GetWorld())DrawDebugPoint(GetWorld(),Location,15.f,FColor::Red,false,-1.f);

//进行多行上的定义
#define DRAW_VECTOR(StartLocation, EndLocation) if(GetWorld()) \
	{ \
		DrawDebugLine(GetWorld(),StartLocation,EndLocation,FColor::Red,true,-1.f,0,1.f); \
		DrawDebugPoint(GetWorld(),EndLocation,15.f,FColor::Red,true); \
	}
#define DRAW_VECTOR_SingleFrame(StartLocation, EndLocation) if(GetWorld()) \
	{ \
		DrawDebugLine(GetWorld(),StartLocation,EndLocation,FColor::Red,false,-1.f,0,1.f); \
		DrawDebugPoint(GetWorld(),EndLocation,15.f,FColor::Red,false,-1.f); \
	}