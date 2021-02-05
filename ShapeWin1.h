#pragma once
#include "resource.h"
#include <Windows.h>
#include <string>
#include <vector>
#include <math.h>

struct KLocation;

class KPLocation
{
	KLocation** p;
	int size;
public:
	KPLocation(int pSize) { p = new KLocation * [size = pSize]; }
	~KPLocation() { delete[] p; }
	KLocation*& operator [] (int i) { return p[i]; }
};
struct KLocation
{
protected:
	double x;
	double y;

public:
	KLocation(double ax = 0, double ay = 0) :x(ax), y(ay) {};
	double GetX() const { return x; }
	double GetY() const { return y; }
	virtual void SetX(double ax) { x = ax; }
	virtual void SetY(double ay) { y = ay; }
	virtual ~KLocation() {};
};
//-----------------------------------------------------------------
class KPoint :
	public KLocation
{
protected:
	bool is_visible;
	bool is_active = false;
	COLORREF outline;
public:
	KPoint(COLORREF acolor = RGB(0, 0, 0), double ax = 0, double ay = 0) : outline(acolor) { x = ax; y = ay; 
	 is_visible = true; is_active = false; };

	bool GetVisibility() const { return is_visible; };
	bool GetState() const { return is_active; };
	void SetOutline(COLORREF color) { outline = color; };
	virtual void SetColor(COLORREF color) { };

	virtual void Shift(HDC&, double, double);
	virtual void MoveTo(HDC&, KPoint&);
	virtual void Show(HDC&);
	virtual void Hide(HDC&);
	virtual void Draw(HDC&, bool);
	virtual void Rotate(HDC&, double) {};
	virtual void RotateBack(HDC&, double) {};
	virtual void Expand(HDC&, double) {};
	virtual ~KPoint() {};
};
//-----------------------------------------------------------------
class KShape : public KPoint
{
protected:
	double sidelen;
	double angle;
	double rotangle;
	KPoint center;
	double koef;
	double SetAngle(double);
	void SetSide(double);
public:
	KShape() : KPoint(), koef(1), angle(0), rotangle(0), center(0, 0), sidelen(0) {};
	KShape(int side, double degrees, COLORREF outline, double rotangle = 0.0, double ax = 0.0, double ay = 0.0, double koef = 1.0) :
		KPoint(outline, ax, ay), sidelen(side), angle(degrees), rotangle(rotangle), koef(koef) {
		is_visible = true;
	};
	virtual void IsAbstract() const = 0 {};

	void FSetTransform(HDC& hdc, int OrgX, int OrgY, double rotangle);

	KPoint GetCenter() const { return center; }
	double GetAngle() const { return angle; }
	double GetRotangle() const { return rotangle; }
	double GetSide() const { return sidelen; }

	virtual void Show(HDC&) override;
	virtual void Hide(HDC&) override;
	virtual void Draw(HDC&, bool) override;
	virtual void Rotate(HDC&, double) override;
	virtual void RotateBack(HDC&, double) override;
	virtual void Expand(HDC&, double) override;
	virtual ~KShape() {};
};
//-----------------------------------------------------------------
class KRhombus :
	public KShape
{
protected:
	void CalcPeaks(double);
	struct figure
	{
		KPoint left_up;
		KPoint left_down;
		KPoint right_up;
		KPoint right_down;
	} figure;
public:
	KRhombus() : KShape() { figure = {}; };
	KRhombus(int side, double alpha, double rotation, COLORREF acolor, double ax, double ay) :
		KShape(side, alpha, acolor, rotation, ax, ay) {
	};
	virtual void IsAbstract() const {};

	virtual void Draw(HDC&, bool) override;
	virtual ~KRhombus() {};
};
//-----------------------------------------------------------------
class KTriangle :
	public KShape
{
protected:
	void CalcPeaks(double, double);
	void Construct(int, int, double, double, COLORREF, double, double);
	std::pair<double, double> CalcAngle(int, int, int);
	int SetSide(int, int, double);
	struct figure2
	{
		double first_side;
		double second_side;
		int first_angle;
		int second_angle;
		KPoint peak;
		KPoint peak_2;
		KPoint peak_3;
	} figure2;
public:
	virtual void IsAbstract() const {};

	KTriangle() : KShape() { figure2 = {}; };
	KTriangle(int a, int b, double alpha, double rotation, COLORREF acolor, double ax, double ay);   //1 angle & 2 sides
	KTriangle(int a, int b, int c, double rotation, COLORREF acolor, double ax, double ay, bool bo) : 
		KShape(a, CalcAngle(a, b, c).first, acolor, rotation, ax, ay) {
		if (bo == true)	Construct(a, b, CalcAngle(a, b, c).first, acolor, rotation, ax, ay); //3 sides
		else Construct(a, (int)(a * sin(SetAngle(c)) / sin(SetAngle(b))), b, acolor, rotation, ax, ay);  //1 side & 2 angles
	}
	KTriangle(int a, double alpha, double beta, double rotation, COLORREF acolor, double ax, double ay, bool bo) : 
		KShape(a, alpha, acolor, rotation, ax, ay) {
		if (bo == true)	Construct(a, alpha, CalcAngle(a, alpha, beta).first, acolor, rotation, ax, ay); //3 sides
		else Construct(a, (int)(a * sin(SetAngle(beta)) / sin(SetAngle(alpha))), alpha, acolor, rotation, ax, ay);  //1 side & 2 angles
	}
	virtual void Draw(HDC&, bool) override;
	virtual ~KTriangle() {};
};
//-----------------------------------------------------------------
class KPaintTriangle :
	public KTriangle
{
protected:
	COLORREF color;
public:
	KPaintTriangle() : color(RGB(255, 255, 255)) {};
	KPaintTriangle(int a, int b, double alpha, double rotation, COLORREF outline, COLORREF acolor, double ax, double ay) :
		KTriangle(a, b, alpha, rotation, outline, ax, ay), color(acolor) {
	};   //1 angle & 2 sides
	KPaintTriangle(int a, int b, int c, double rotation, COLORREF outline, COLORREF acolor, double ax, double ay, bool bo) :
		KTriangle(a, b, c, rotation, outline, ax, ay, bo), color(acolor) {
	};   //3 sides
	KPaintTriangle(int a, double alpha, double beta, double rotation, COLORREF outline, COLORREF acolor, double ax, double ay, bool bo) :
		KTriangle(a, alpha, beta, rotation, outline, ax, ay, bo), color(acolor) {
	};  //1 side & 2 angles

	virtual void SetColor(COLORREF acolor) override { color = acolor; };
	virtual void Draw(HDC&, bool) override;
	virtual ~KPaintTriangle() {};
};
