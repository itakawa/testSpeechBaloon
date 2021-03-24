
# include <Siv3D.hpp> // OpenSiv3D v0.6
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Random.hpp>

// LineStringをinterpolation個の等間隔の点列に変換
Array<Vec2> ls2point(LineString ls, int32 _interpolation)
{
	if (ls.size() == 0) return {};

//	if (ls.front() != ls.back())
//		ls << ls.front();//最後と最初が一致でなければ繋いでループ

	Array<double> length_table = { 0.0 };	//線分距離0を初回登録

	for (int32 i : step(ls.num_lines()))	//LSの数だけループ
		length_table << length_table.back() + ls.line(i).length();	//LSの線分長を加算しながら線分リストに登録

	const double perimeter = length_table.back();	//全長を取得

	Array<Vec2> sequence = { ls.front() };	//始端を開始位置として登録
	int32 prev_i = 1;
	for (int32 s : Range(1, _interpolation - 1))	//分割Noループ
	{
		const double distance = perimeter * s / _interpolation;	//全長　ｘ ｎ個目 /分割数 = 進度
		for (int32 i : Range(prev_i, (int32)length_table.size() - 1))
		{
			if (distance <= length_table[i])
			{
				const double len_buf = distance - length_table[i - 1];	// 
				sequence << ls[i - 1].lerp(ls[i], len_buf / ls.line(i - 1).length()); //  
				prev_i = i;
				break;
			}
		}
	}
	return sequence;
}

//固定乱数列[256]
const uint8 randNum[] = {
56,97,59,39,75,33,34,68,32,26,15,28,88,14,1 ,66,
94,92,89,4 ,38,84,56,88,33,97,0 ,44,39,9 ,94,0 ,
14,75,1 ,2 ,20,22,96,72,9 ,94,50,80,65,22,4 ,92,
23,14,76,62,18,96,94,27,16,86,53,32,7 ,13,23,64,
75,47,4 ,98,4 ,43,8 ,32,89,31,55,57,37,27,36,76,
90,1 ,19,48,89,82,83,7 ,41,38,24,54,89,5 ,50,47,
71,95,95,54,50,92,53,46,53,80,76,13,70,57,48,99,
24,49,41,99,13,86,14,62,14,55,11,66,32,19,3 ,6 ,
45,85,30,89,74,5 ,29,5 ,91,77,61,47,79,91,21,5 ,
86,30,1 ,75,98,4 ,30,43,85,27,55,68,67,65,94,10,
30,55,48,21,21,99,93,57,71,2 ,68,36,91,1 ,50,61,
33,69,10,49,8 ,29,78,10,52,49,57,68,74,91,21,6 ,
50,83,75,29,87,13,89,97,49,36,4 ,10,97,80,30,35,
0 ,2 ,60,58,63,22,52,3 ,67,72,92,3 ,83,72,88,54,
85,10,27,13,91,34,54,87,35,80,62,59,65,68,17,60,
25,92,33,98,77,28,72,33,90,98,81,34,47,90,58,15};
uint8 randIndex = 0;

float fixedRandom(uint8 seed)
{
	randIndex += seed;
	return (float)randNum[randIndex] / 100;
}

//自由変形
LineString FreeTransform(const LineString &linestring, Quad dst)
{
	double l=linestring[0].x, r=linestring[0].x, t=linestring[0].y, b=linestring[0].y;
	for(uint32 i=1; i<linestring.size(); i++)
	{
		auto& ls = linestring[i];
		if (l > ls.x) l = ls.x;
		if (r < ls.x) r = ls.x;
		if (t > ls.y) t = ls.y;
		if (b < ls.x) b = ls.y;
	}

	return {};
}

struct RADSORT
{
	Vec2 point;
	double rad;
};
bool ccw(const RADSORT& a, const RADSORT& b) { return a.rad < b.rad; }
bool cw(const RADSORT& a, const RADSORT& b) { return a.rad > b.rad; }
Array<Vec2> sortDirection(Vec2 center, Array<Vec2> &points,bool direction)
{
	Array<RADSORT> rs;
	for (Vec2 point : points) rs << RADSORT{point,Math::Atan2(point.y-center.y, point.x - center.x)};
	if(direction) std::sort(rs.begin(), rs.end(), ccw);
	else          std::sort(rs.begin(), rs.end(), cw);
	Array<Vec2> sorted;
	for (auto &r:rs) sorted << r.point;
	return sorted;
}

#include "Siv3D/Geometry2D.hpp"

Shape2D SpeechBalloon1(Vec2 center = Vec2{100,100}, // 中心点
	                   uint32 ngon = 6,
	                    int32 angle = 0,
	                   uint32 size = 10,
	                   Vec2 anchor = Vec2(0,0), 
	                   Vec2 scale = Vec2(1,1), 
	                   Vec2 stretch = Vec2(1,1), 
	                   Vec2 div = Vec2(0,0),
	                   uint32 nsubpoint = 10, 
	                    int32 detail = 10, 
	                    float burst = 1.4, 
	                    uint8 seed = 60, 
	              const Vec2& target = Vec2{0,0})
{
	const Mat3x2 imat = Mat3x2::Identity();

	//直線パターン
	Array<Quad> QSTRAIGHT { Quad({0,0},{0,1},{10,1},{10,0}) };
	for (auto &q : QSTRAIGHT) { q.p0 /= 10; q.p1 /= 10; q.p2 /= 10;q.p3 /= 10; }

	//ラーメンパターン
	Array<Quad> QRAMEN{ Quad({0,0},{0,1},{5,1},{5,0}), Quad({4,1},{4,2},{5,2},{5,1}), Quad({3,2},{3,3},{5,3},{5,2}),
						Quad({1,1},{1,4},{2,4},{2,1}), Quad({1,4},{1,5},{7,5},{7,4}), Quad({6,0},{6,4},{7,4},{7,0}),
						Quad({7,0},{7,1},{10,1},{10,0}) };
	for (auto &q : QRAMEN) { q.p0 /= 10; q.p1 /= 10; q.p2 /= 10;q.p3 /= 10; }

	//波パターン
	Array<Quad> QSIN;
	int32 step = 36;
	for (int32 i = step; i <= 360; i += step)
	{
		double sx0 = (double)(i - step) / 360;
		double sx1 = (double)i / 360;
		Vec2 p0 = Vec2(sx0, 0);
		Vec2 p1 = Vec2(sx0, -Math::Sin(ToRadians(i - step)) + 2);
		Vec2 p2 = Vec2(sx1, -Math::Sin(ToRadians(i)) + 2);
		Vec2 p3 = Vec2(sx1, 0);
		QSIN << Quad{ p0,p1,p2,p3 };
	}

	//円弧パターン
	Array<Quad> QCIR;
	step = 18;
	for (int32 i = step; i <= 180; i += step)
	{
		double cx0 = ((1- Math::Cos(ToRadians(i - step))) / 2);
		double cx1 = ((1- Math::Cos(ToRadians(i))) / 2);
		QCIR << Quad{ cx0, 0, 
			          cx0, fabs(Math::Sin(ToRadians(i - step))),
			          cx1, fabs(Math::Sin(ToRadians(i))),
			          cx1, 0 };
	}

	//基準多角形
	Polygon bngon = Shape2D::Ngon(ngon, size, center, ToRadians(45)).asPolygon();
	LineString bls = bngon.outline();
	if (bls.front() != bls.back()) bls << bls.front();	//最後と最初が一致でなければ繋いでループ

	Array<Vec2> bp = ls2point(bls, nsubpoint);

//	Circle(center, size).drawFrame(1,Palette::Green);

	//DEBUG用ワイヤーフレーム指定
	bool wireframe = false;

	Array<Quad> bquad;
	for (uint32 i = 1; i <= bp.size(); i++)
	{
		uint32 from = i-1;
		uint32 to = i;
		if (i == bp.size()) to = 0;	//最後は最初へ繋ぐ
		double length = Geometry2D::Distance(bp[from], bp[to]);//底辺の長さ
		double retio = (double)size / length ;	//拡縮比

		double rotate = Math::Atan2(bp[to].y - bp[from].y, bp[to].x - bp[from].x);
		Mat3x2 mata = imat.scaled(Vec2(length, detail)).rotated(rotate);
		Mat3x2 mats = imat.scaled(scale);
		Mat3x2 matr = imat.rotated(ToRadians(angle));

		Array<Quad> bq;
		for (const auto& q : QCIR)
		{
			Vec2 psrc[] = { q.p0, q.p1, q.p2, q.p3 };
			Vec2 pdst[4] = {};
			for (uint32 i = 0; i < 4; i++)
			{
				Vec2 pos = bp[from] - center;	//原点へ移動
				pos = pos + mata.transformPoint(psrc[i]);



				pos = pos * scale;
				pos = matr.transformPoint(pos);
				pdst[i] = pos + center;			//元の場所
			}
			if (wireframe) bq << Quad(pdst[0], pdst[1], pdst[2], pdst[3]).drawFrame();
			else           bq << Quad(pdst[0], pdst[1], pdst[2], pdst[3]).draw();
		}
		Vec2 p0 = bq.front().p0.xy();
		Vec2 p1 = bq.back().p3.xy();
		if( wireframe ) bq << Quad(center, center, p1, p0).drawFrame();
		else            bq << Quad(center, center, p1, p0).draw();
		bquad.append(bq);
	}

	// bquad.clear();

	// target
	LineString lsTa, lsTb;
	if (target != Vec2(0, 0))
	{
		double ot90 = Math::Atan2(center.y - target.y, center.x - target.x) + ToRadians(90);
		Vec2 ot = Vec2(Math::Cos(ot90), Math::Sin(ot90));
		double r = 30;
		Vec2 ot1 = ot * r;	Vec2 ot2 = ot * -r;
		if (wireframe) Line(center + ot1, center + ot2).draw(Palette::Black);
		if (wireframe) Line(anchor + ot1, anchor + ot2).draw(Palette::Black);
		lsTa.append(Bezier2(center + ot1, anchor + ot1, target).getLineString(seed));
		lsTb.append(Bezier2(center + ot2, anchor + ot2, target).getLineString(seed));
	}
	Array<Vec2> poTa = lsTa.asArray();
	Array<Vec2> poTb = lsTb.asArray();
//	for (auto ptp : poTa)Circle(ptp, 4).draw(Palette::Green);
//	for (auto ptp : poTb)Circle(ptp, 4).draw(Palette::Green);

	for (uint32 i = 1; i < seed; i++ )
	{
		if (wireframe) bquad << Quad(poTa[i-1], poTa[i], poTb[i], poTb[i-1]).drawFrame();
		else           bquad << Quad(poTa[i-1], poTa[i], poTb[i], poTb[i-1]).draw();
	}
	Vec2 p0 = bquad.back().p1.xy();
	Vec2 p1 = bquad.back().p2.xy();
	if (wireframe) bquad << Quad(target, target, p1, p0).drawFrame();
	else           bquad << Quad(target, target, p1, p0).draw();

	Array<Float2> vertices;
	for (const Quad &q : bquad)
	{
		vertices << q.p0; vertices << q.p1;	vertices << q.p2; 
		vertices << q.p0; vertices << q.p2; vertices << q.p3; 
	}

	Array<TriangleIndex> indices(4* bquad.size());
	{
		TriangleIndex* pDst = indices.data();

		for (Vertex2D::IndexType i = 0; i < 2*bquad.size(); ++i)
		{
			pDst->i0 = 3 * i + 0;
			pDst->i1 = 3 * i + 1;
			pDst->i2 = 3 * i + 2;
			++pDst;

			pDst->i0 = 3 * i + 0;
			pDst->i1 = 3 * i + 3;
			pDst->i2 = 3 * i + 2;
			++pDst;
		}
	}

	return{ std::move(vertices), std::move(indices) };
}

Array<Shape2D> SpeechBalloon2( Rect rect = Rect(0,0,100,100) )
{
	return{};
}

enum PARAM {NGON,ANGLE,SIZE,NSUB,DETAIL,BURST,SEED,
		    ORGX,ORGY,ANCX,ANCY,TRGX,TRGY,STRX,STRY,SCAX,SCAY,
			M_ORG,M_ANC,M_TRG,
			NUMGUI};
	
enum GRABAREA { RELEASE, G_ORG, G_ANC, G_TRG };
struct GRABSTATE
{
    GRABAREA area ;
    int32 value ;
} ;

template <typename V,typename T> 
V guiValue(const Font &font,const String &text,Rect &guirect,double wheel, V value,T min,T max)
{
	if (guirect.mouseOver() && wheel != 0) value = Math::Clamp(value+wheel,min,max); 
    font(text).draw(guirect, guirect.mouseOver() ? ColorF(1) : ColorF(0)); 
	return value;
}

template <typename V> 
V guiDrag(const Font &font,const String &text, Rect &guirect,Vec2 delta,GRABSTATE &grabstate, GRABAREA garea, 
	      ColorF activecolor,ColorF inactivecolor, V value )
{
	font(text).draw(Arg::center(value),guirect.mouseOver() ? activecolor : inactivecolor );
    if (grabstate.area == RELEASE && guirect.leftClicked()) grabstate.area = garea;
	if (grabstate.area == garea) { value += Vec2(Cursor::Delta()); guirect.x = value.x; guirect.y = value.y; }
	return value;
}

void Main()
{
    Window::SetStyle(WindowStyle::Sizable);     // ウィンドウを手動リサイズ可能にする
    Scene::SetBackground(ColorF(0.8, 0.9, 1.0));// ウィンドウサイズに合わせて拡縮
	const Font fontM(50);
	const Font fontS(15);
	const Font iconM(30,Typeface::Icon_MaterialDesign);

	Vec2 anchor{210,210};
	Vec2 stretch{1,1};
	Vec2 div{0,0};
	Vec2 scale{1,1};
	Vec2 origin{200,200};
	Vec2 target{220,220};

	uint32 ngon = 6;
	 int32 angle = 0;
	uint32 size = 200;
	uint32 nsubngon = 40;
	int32 detail = -10;
	float burst = 1.0;
	uint8 nsubtarget = 5;

	GRABSTATE grabState { RELEASE,0 };

	uint32 fs = fontS.fontSize() + 8;
    Rect rc = Rect(5, Scene::Height()-100, fs*4,fs);
	Rect guiRects[NUMGUI] = {};

	for (uint32 i = 0; i < NUMGUI; i++)
	{
		if      (i==M_ORG) { guiRects[i] = Rect(origin.x - 10, origin.y - 10, 30, 30); }
		else if (i==M_ANC) { guiRects[i] = Rect(anchor.x - 10, anchor.y - 10, 30, 30); }
		else if (i==M_TRG) { guiRects[i] = Rect(target.x - 10, target.y - 10, 30, 30); }
		else
		{
			guiRects[i] = rc;
			rc.x += fs * 5;
			if (rc.x > Scene::Width() - 20) { rc.x = 5; rc.y += fs; }
		}
	}

	while (System::Update())
	{
		String text = U"Speech\nBaloon";
		RectF rectf = RectF{ origin.x, origin.y, fontM.fontSize()*text.size()/3, fontM.height()*2 };

//		SpeechBalloon1(origin, ngon,angle,size,anchor,scale,stretch,div,nsub,detail,burst,seed,target).draw(Palette::Bisque);
		SpeechBalloon1(origin, ngon, angle, size, anchor, scale, stretch, div, nsubngon, detail, burst, nsubtarget, target).drawFrame(2,Palette::Bisque);

		fontM(text).draw(Arg::center(Vec2(origin.x, origin.y)), ColorF(0));

//DEBUG PARAM
		double wheel = Mouse::Wheel();
		ngon = guiValue( fontS, U"多角形:{}"_fmt(ngon), guiRects[NGON], wheel, ngon, 3, 16);
		angle = guiValue( fontS, U"角度:{}"_fmt(angle), guiRects[ANGLE], wheel, angle, -360, 360);
		size = guiValue( fontS, U"大きさ:{}"_fmt(size), guiRects[SIZE], wheel, size, 10, 400);
		nsubngon = guiValue( fontS, U"分割A:{}"_fmt(nsubngon), guiRects[NSUB], wheel, nsubngon, 4, 100);
		detail = guiValue( fontS, U"パターン:{}"_fmt(detail), guiRects[DETAIL], wheel, detail, -100, 100);
		burst = guiValue( fontS, U"burst:{}"_fmt(burst), guiRects[BURST], wheel/10, burst, 0, 100);
		nsubtarget = guiValue( fontS, U"分割B:{}"_fmt(nsubtarget), guiRects[SEED], wheel, nsubtarget, 0, 100);

		origin.x = guiValue( fontS, U"原点X:{}"_fmt(origin.x), guiRects[ORGX], wheel, origin.x, 0, Scene::Width());
		origin.y = guiValue( fontS, U"原点Y:{}"_fmt(origin.y), guiRects[ORGY], wheel, origin.y, 0, Scene::Height());
		anchor.x = guiValue( fontS, U"アンカーX:{}"_fmt(anchor.x), guiRects[ANCX], wheel, anchor.x, 0, Scene::Width());
		anchor.y = guiValue( fontS, U"アンカーY:{}"_fmt(anchor.y), guiRects[ANCY], wheel, anchor.y, 0, Scene::Height());
		target.x = guiValue( fontS, U"ターゲットX:{}"_fmt(target.x), guiRects[TRGX], wheel, target.x, 0, Scene::Width());
		target.y = guiValue( fontS, U"ターゲットY:{}"_fmt(target.y), guiRects[TRGY], wheel, target.y, 0, Scene::Height());
		stretch.x = guiValue( fontS, U"引伸し:{}"_fmt(stretch.x), guiRects[STRX], wheel/100, stretch.x, 0, 2);
		stretch.y = guiValue( fontS, U"strY:{}"_fmt(stretch.y), guiRects[STRY], wheel/100, stretch.y, 0, 2);
		scale.x = guiValue( fontS, U"拡縮X:{}"_fmt(scale.x), guiRects[SCAX], wheel/100, scale.x, 0, 10);
		scale.y = guiValue( fontS, U"拡縮Y:{}"_fmt(scale.y), guiRects[SCAY], wheel/100, scale.y, 0, 10);

//DEBUG MARKER
		origin = guiDrag(iconM,U"\xF1382", guiRects[M_ORG],Vec2(Cursor::Delta()),grabState,G_ORG,ColorF(0,0,1),ColorF(1),origin );
		anchor = guiDrag(iconM,U"\xF0031", guiRects[M_ANC],Vec2(Cursor::Delta()),grabState,G_ANC,ColorF(1,0,0),ColorF(1),anchor );
		target = guiDrag(iconM,U"\xF04FE", guiRects[M_TRG],Vec2(Cursor::Delta()),grabState,G_TRG,ColorF(1,0,1),ColorF(1),target );

		if (MouseL.up()) grabState.area = RELEASE;

//		for(auto sb2: SpeechBalloon2()) sb2.draw();
	}
}

