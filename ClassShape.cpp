#include "ShapeWin1.h"
#include <Windowsx.h>
#include <iostream>
#include <iomanip>
#define _USE_MATH_DEFINES
#include <math.h>
#include <exception>

void KPoint::Shift(HDC& a_hdc, double dx, double dy) {
	if (is_active) {
		if (is_visible) {
			Hide(a_hdc);
			this->SetX(x += dx);
			this->SetY(y += dy);
			Show(a_hdc);
		}
		else {
			this->SetX(x += dx);
			this->SetY(y += dy);
		}
	}
}
void KPoint::MoveTo(HDC& a_hdc, KPoint& to) {
	if (is_active) {
		if (is_visible) {
			Hide(a_hdc);
			x = to.x;
			y = to.y;
			Show(a_hdc);
		}
		else {
			x = to.x;
			y = to.y;
		}
	}
}
void KPoint::Show(HDC& a_hdc) {
	if (is_active) {
		HPEN Pen1 = CreatePen(PS_SOLID, 4, outline);
		SelectObject(a_hdc, Pen1);
		is_visible = true;
		Draw(a_hdc, is_active);
	}
}
void KPoint::Hide(HDC& a_hdc) {
	if (is_active) {
		is_visible = false;
		Draw(a_hdc, is_active);
	}
}
void KPoint::Draw(HDC& a_hdc, bool isactive) {
	if (is_visible) {
		SetPixel(a_hdc, x, y, outline);
	}
	else {
		SetPixel(a_hdc, KLocation::x, KLocation::y, RGB(255, 255, 255));
	}
	is_active = isactive;
}
//-----------------------------------------------------------------

double KShape::SetAngle(double deg)
{
	if (!deg) rotangle = 0;
	return (double)((deg * M_PI) / 180);
}
void KShape::SetSide(double side)
{
	sidelen = side;
}
void KShape::Show(HDC& a_hdc) {
	HBRUSH h = CreateSolidBrush(RGB(255, 255, 255));
	SelectObject(a_hdc, h);
	is_visible = true;
	Draw(a_hdc, is_active);
}
void KShape::Hide(HDC& a_hdc) {
	HBRUSH h = CreateSolidBrush(RGB(255, 255, 255));
	SelectObject(a_hdc, h);
	is_visible = false;
	Draw(a_hdc, is_active);
}
void KShape::Draw(HDC& a_hdc, bool is_active) {
	HBRUSH h = CreateSolidBrush(RGB(255, 255, 255));
	SelectObject(a_hdc, h);
}
void KShape::Rotate(HDC& a_hdc, double rotation) {
	if (is_active) {
		if (is_visible && is_active) {
			Hide(a_hdc);
			rotangle += rotation;
			Show(a_hdc);
		}
		else rotangle += rotation;
	}
}
void KShape::RotateBack(HDC& a_hdc, double rotation) {
	if (is_active) {
		if (is_visible) {
			Hide(a_hdc);
			rotangle -= rotation;
			Show(a_hdc);
		}
		else rotangle -= rotation;
	}
}
void KShape::Expand(HDC& a_hdc, double coeff) {
	if (is_active) {
		if (is_visible) {
			Hide(a_hdc);
			koef *= coeff;
			Show(a_hdc);
		}
		else koef *= coeff;
	}
}
void KShape::FSetTransform(HDC& hdc, int OrgX, int OrgY, double angle)
{
	XFORM m_xm = { 1,0,0,1,0,0 };

	SetMapMode(hdc, MM_ANISOTROPIC);
	SetWindowExtEx(hdc, 1, 1, NULL);
	SetWindowOrgEx(hdc, 0, 0, NULL);

	SetViewportExtEx(hdc, 1, -1, NULL);
	SetViewportOrgEx(hdc, OrgX, OrgY, NULL);

	SetGraphicsMode(hdc, GM_ADVANCED);		

	int anglex = angle;
	anglex = anglex % 30000;
	double rad = SetAngle(anglex);
	m_xm.eM11 = (FLOAT)cos(rad);
	m_xm.eM12 = (FLOAT)sin(rad);
	m_xm.eM21 = -m_xm.eM12;
	m_xm.eM22 = m_xm.eM11;
	m_xm.eDx = 0;
	m_xm.eDy = 0;

	SetWorldTransform(hdc, &m_xm);
}
//-----------------------------------------------------------------
void KRhombus::CalcPeaks(double a)
{
	figure.left_down.SetX(center.GetX() - a * cos((double)SetAngle(angle) / 2) * cos((double)SetAngle(angle) / 2 + SetAngle(rotangle)));
	figure.left_down.SetY(center.GetY() - a * cos((double)SetAngle(angle) / 2) * sin((double)SetAngle(angle) / 2 + SetAngle(rotangle)));
	figure.left_up.SetX(figure.left_down.GetX() + a * cos(SetAngle(angle) + SetAngle(rotangle)));
	figure.left_up.SetY(figure.left_down.GetY() + a * sin(SetAngle(angle) + SetAngle(rotangle)));
	figure.right_up.SetX(center.GetX() + a * cos((double)SetAngle(angle) / 2) * cos((double)SetAngle(angle) / 2 + SetAngle(rotangle)));
	figure.right_up.SetY(center.GetY() + a * cos((double)SetAngle(angle) / 2) * sin((double)SetAngle(angle) / 2 + SetAngle(rotangle)));
	figure.right_down.SetX(figure.left_down.GetX() + a * cos(SetAngle(rotangle)));
	figure.right_down.SetY(figure.left_down.GetY() + a * sin(SetAngle(rotangle)));
}
void KRhombus::Draw(HDC& a_hdc, bool isactive) {
	KShape::Draw(a_hdc, isactive);
	HPEN Pen1;
	if (is_visible) {
		Pen1 = CreatePen(PS_SOLID, 4, outline);
		SelectObject(a_hdc, Pen1);
	}
	else {
		Pen1 = CreatePen(PS_SOLID, 4, RGB(255, 255, 255));
		SelectObject(a_hdc, Pen1);
	}
	double a1 = floor(sidelen * koef + 0.5);
	FSetTransform(a_hdc, x, y, rotangle);
	CalcPeaks(a1);
	POINT fig[4];
	fig[0].x = figure.left_down.GetX();
	fig[0].y = figure.left_down.GetY();
	fig[1].x = figure.left_up.GetX();
	fig[1].y = figure.left_up.GetY();
	fig[2].x = figure.right_up.GetX();
	fig[2].y = figure.right_up.GetY();
	fig[3].x = figure.right_down.GetX();
	fig[3].y = figure.right_down.GetY();
	Polygon(a_hdc, fig, 4);
	MoveToEx(a_hdc, fig[0].x, fig[0].y, NULL);
	LineTo(a_hdc, fig[2].x, fig[2].y);
	MoveToEx(a_hdc, fig[1].x, fig[1].y, NULL);
	LineTo(a_hdc, fig[3].x, fig[3].y);
	DeleteObject(Pen1);
	is_active = isactive;
}
//-----------------------------------------------------------------
void KTriangle::Construct(int a, int b, double alpha, double rotation, COLORREF acolor, double ax, double ay) {
	figure2.first_side = a;
	figure2.second_side = b;
	figure2.first_angle = alpha;
	figure2.second_angle = CalcAngle(a, SetSide(a, b, alpha), b).first;
}
KTriangle::KTriangle(int a, int b, double alpha, double rotation, COLORREF acolor, double ax, double ay) : KShape(a, alpha, acolor, rotation, ax, ay)
{
	Construct(a, b, alpha, rotation, acolor, ax, ay);
}
std::pair<double, double> KTriangle::CalcAngle(int a1, int b1, int c1)
{
	double angle = acos((double)(-c1 * c1 + a1 * a1 + b1 * b1) / (double)(2 * a1 * b1));
	double angle_deg = (angle * 180 / M_PI);
	return { angle_deg, angle };
}
int KTriangle::SetSide(int a1, int b1, double angle1)
{
	int side = sqrt(a1 * a1 + b1 * b1 - 2 * a1 * b1 * cos(SetAngle(angle1)));
	return side;
}
void KTriangle::CalcPeaks(double a, double b)
{
	double t = (double)(a * sin(SetAngle(figure2.first_angle)) / sqrt(a * a +
		(double)(b * b / 4) - a * b * cos(SetAngle(figure2.first_angle))));
	if (t > 1 && t < 1.1) {
		t = floor(t);
	}
	figure2.peak_2.SetX(center.GetX() + (double)(2 * sqrt(a * a + (double)(b * b / 4) -
		a * b * cos(SetAngle(figure2.first_angle))) / 3.0) * cos(SetAngle(rotangle) +
			asin(t)));

	figure2.peak_2.SetY(center.GetY() + (double)(2 * sqrt(a * a + (double)(b * b / 4) -
		a * b * cos(SetAngle(figure2.first_angle))) / 3.0) * sin(SetAngle(rotangle) +
			asin(t)));

	figure2.peak.SetX(figure2.peak_2.GetX() - a * cos(SetAngle(rotangle) + SetAngle(figure2.first_angle)));
	figure2.peak.SetY(figure2.peak_2.GetY() - a * sin(SetAngle(rotangle) + SetAngle(figure2.first_angle)));
	figure2.peak_3.SetX(figure2.peak.GetX() + b * cos(SetAngle(rotangle)));
	figure2.peak_3.SetY(figure2.peak.GetY() + b * sin(SetAngle(rotangle)));
}
void KTriangle::Draw(HDC& a_hdc, bool isactive)
{
	KShape::Draw(a_hdc, isactive);
	HPEN Pen;
	if (is_visible) {
		Pen = CreatePen(PS_SOLID, 4, outline);
		SelectObject(a_hdc, Pen);
	}
	else {
		Pen = CreatePen(PS_SOLID, 4, RGB(255, 255, 255));
		SelectObject(a_hdc, Pen);
	}
	int a1 = floor(figure2.first_side * koef + 0.5);
	int a2 = floor(figure2.second_side * koef + 0.5);
	FSetTransform(a_hdc, x, y, rotangle);
	CalcPeaks(a1, a2);
	POINT fig[3];
	fig[0].x = figure2.peak.GetX();
	fig[0].y = figure2.peak.GetY();
	fig[1].x = figure2.peak_2.GetX();
	fig[1].y = figure2.peak_2.GetY();
	fig[2].x = figure2.peak_3.GetX();
	fig[2].y = figure2.peak_3.GetY();
	Polygon(a_hdc, fig, 3);
	DeleteObject(Pen);
	is_active = isactive;
}
//-----------------------------------------------------------------
void KPaintTriangle::Draw(HDC& a_hdc, bool isactive)
{
	HBRUSH Brush1;
	HPEN Pen2;
	if (is_visible) {
		Brush1 = CreateSolidBrush(color);
		SelectObject(a_hdc, Brush1);
		Pen2 = CreatePen(PS_SOLID, 4, outline);
		SelectObject(a_hdc, Pen2);
	}
	else {
		Brush1 = CreateSolidBrush(RGB(255, 255, 255));
		SelectObject(a_hdc, Brush1);
		Pen2 = CreatePen(PS_SOLID, 4, RGB(255, 255, 255));
		SelectObject(a_hdc, Pen2);
	}
	int a1 = floor(figure2.first_side * koef + 0.5);
	int a2 = floor(figure2.second_side * koef + 0.5);
	FSetTransform(a_hdc, x, y, rotangle);
	CalcPeaks(a1, a2);
	POINT fig[3];
	fig[0].x = figure2.peak.GetX();
	fig[0].y = figure2.peak.GetY();
	fig[1].x = figure2.peak_2.GetX();
	fig[1].y = figure2.peak_2.GetY();
	fig[2].x = figure2.peak_3.GetX();
	fig[2].y = figure2.peak_3.GetY();
	Polygon(a_hdc, fig, 3);
	DeleteObject(Pen2);
	DeleteObject(Brush1);
	is_active = isactive;
}
