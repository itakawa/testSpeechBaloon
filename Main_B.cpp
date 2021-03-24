
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

// <rect> 長方形エリア
// <target> 吹き出しの先端の位置(0,0)で吹き出し無し
//＜nbomb＞山の数(外周を1.0＝100個として)
//＜detail＞頂点数
// <scale> 山の高さ
// <seed> 山の間隔用乱数の種
// <color> 色
Shape2D SpeechBalloon1(const RectF& rectf = RectF{ 100,100,100,200 }, uint32 ngon = 6,int32 angle = 0,uint32 size = 10,
	                   Vec2 anchor = Vec2(0,0), Vec2 scale = Vec2(1,1), Vec2 stretch = Vec2(1,1), Vec2 div = Vec2(0,0),
	                   uint32 nsubpoint = 30, uint32 detail = 100, float burst = 1.4, uint8 seed = 60, const Vec2& target = Vec2{0,0})
{
	const float h = static_cast<float>(rectf.h);
	const float w = static_cast<float>(rectf.w);
	const float y = static_cast<float>(rectf.y);
	const float x = static_cast<float>(rectf.x);

	const Vec2 center = Vec2(x, y);
	Polygon pongon = Shape2D::Ngon(ngon, size, center, angle*3.14159/180).asPolygon();
	LineString ls = pongon.outline();
	ls.draw();//DEBUG
//	Circle circle(x,y,size*2);//DEBUG


	Array<Vec2> Opoints;		//円原点のリスト

	randIndex = seed;
	uint32 onum = 30;
	for (uint32 i = 0; i < onum; i++)
	{
		double rx = fixedRandom(seed) *size*2-size ;
		double ry = fixedRandom(seed) *size*2-size ;
		Vec2 po = Vec2(rx, ry);
		if (pongon.contains(po+center)) Opoints << po+center;
	}

	Opoints = sortDirection(center,Opoints,true); //時計回りソート

	Array<Circle> Circles;	//円リスト
	for (const auto& point : Opoints) Circles << Circle(point, fixedRandom(seed+1) * 50);

	Array<Vec2> Bpoints;	//バブルの頂点リスト
	for (uint32 i = 0;i < Opoints.size(); i++)
	{
		Array<Vec2> BCPs = ls2point(Circles[i].asPolygon().outline(CloseRing::Yes), nsubpoint);

		//切り口をCenterに向ける
		float co = Math::Atan2(Opoints[i].y-center.y, Opoints[i].x-center.x );
		Mat3x2 mat = Mat3x2::Identity();
		mat = mat.rotated( co-ToRadians(90) );

		Circle(BCPs[0], 7).drawFrame(2,Palette::Red);	//DEBUG
		for (uint32 ii = 0;ii < BCPs.size(); ii++)
		{
			BCPs[ii] = mat.transformPoint(BCPs[ii]-Opoints[i]);
			BCPs[ii] = BCPs[ii] + Opoints[i];
		}
		Circle(BCPs[0], 7).draw(Palette::Red);	//DEBUG

		Bpoints.append(BCPs);
	}
	Circle(center, 7).draw(Palette::Blue);	//DEBUG


	//不要な頂点を削除
	for (uint32 i = 0;i < Opoints.size(); i++)
	{
		for (uint32 ii = Bpoints.size() - 1; ii != 0; --ii)
		{
			if (Circle(Opoints[i], Circles[i].r - 1).contains(Bpoints[ii]))
				Bpoints.erase(Bpoints.begin() + ii);
		}
	}
/*
	//最隣ソート
	Array<Vec2> Bpoints2;	//バブルの頂点リスト2
	for (uint32 i = Bpoints.size() - 1; i != 0; --i)
	{
		uint32 min = 0;
		for (uint32 ii = Bpoints.size() - 1; ii != 0; --ii)
		{

		}
	}
*/
	//アフィン変換
	for (uint32 i = 0; i < Bpoints.size(); i++)
	{
		Mat3x2 mat = Mat3x2::Identity();
		mat = mat.rotated(3.14159 / 180 * angle);
		mat = mat.scaled(scale);

		Bpoints[i] = mat.transformPoint(Bpoints[i] - center);
		Bpoints[i] = Bpoints[i] + center;
	}

	for (uint32 i = 0; i < Bpoints.size(); i++)
	{
		Circle(Bpoints[i], 5).draw(ColorF((float)i / Bpoints.size()));	//DEBUG
		if (i) Line(Bpoints[i - 1], Bpoints[i]).draw(ColorF((float)i / Bpoints.size()));	//DEBUG
	}




	LineString lsTa, lsTb;
	if (target != Vec2(0, 0))
	{
		Vec2 origin = { rectf.x,rectf.y };
		double ot90 = Math::Atan2(origin.y - target.y, origin.x - target.x) + Math::Pi / 180 * 90;
		Vec2 ot = Vec2(Math::Cos(ot90), Math::Sin(ot90));
		double r = 30;
		Vec2 ot1 = ot * r;	Vec2 ot2 = ot * -r;
		Line(origin + ot1, origin + ot2).draw();
		Line(anchor + ot1, anchor + ot2).draw();
		lsTa.append(Bezier2(origin + ot1, anchor + ot1, target).getLineString(10));
		lsTb.append(Bezier2(origin + ot2, anchor + ot2, target).getLineString(10));
	}


	Array<Vec2> poTa = ls2point(lsTa, nsubpoint / 4 + 1);
	Array<Vec2> poTb = ls2point(lsTb, nsubpoint / 4 + 1);

	Polygon pT = Polygon(poTa.append(poTb.reversed()) << center);
	pT.draw(Palette::Beige);

	for (auto ptp : poTa)Circle(ptp, 4).draw(Palette::Green);
	for (auto ptp : poTb)Circle(ptp, 4).draw(Palette::Green);

	return{};
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

//	for (uint32 i = 0; i < 256; i++) randNum[i] = Random(100);

	Vec2 anchor{210,210};
	Vec2 stretch{1,1};
	Vec2 div{0,0};
	Vec2 scale{1,1};
	Vec2 origin{200,200};
	Vec2 target{220,220};

	uint32 ngon = 3;
	 int32 angle = 0;
	uint32 size = 200;
	uint32 nsub = 40;
	uint32 detail = 100;
	float burst = 1.0;
	uint8 seed = 60;

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
		RectF rectf = RectF{ origin.x, origin.y, fontM.fontSize()*text.size()/2, fontM.height()*2 };

		SpeechBalloon1(rectf,ngon,angle,size,anchor,scale,stretch,div,nsub,detail,burst,seed,target).draw();
		
		fontM(text).draw( Arg::center(Vec2(rectf.x,rectf.y)), ColorF(0)).drawFrame();
		
//DEBUG PARAM
		double wheel = Mouse::Wheel();
		ngon = guiValue( fontS, U"ngon:{}"_fmt(ngon), guiRects[NGON], wheel, ngon, 3, 16);
		angle = guiValue( fontS, U"angle:{}"_fmt(angle), guiRects[ANGLE], wheel, angle, -360, 360);
		size = guiValue( fontS, U"size:{}"_fmt(size), guiRects[SIZE], wheel, size, 10, 400);
		nsub = guiValue( fontS, U"nsub:{}"_fmt(nsub), guiRects[NSUB], wheel, nsub, 4, 100);
		detail = guiValue( fontS, U"detail:{}"_fmt(detail), guiRects[DETAIL], wheel, detail, 0, 100);
		burst = guiValue( fontS, U"burst:{}"_fmt(burst), guiRects[BURST], wheel/10, burst, 0, 100);
		seed = guiValue( fontS, U"seed:{}"_fmt(seed), guiRects[SEED], wheel, seed, 0, 100);

		origin.x = guiValue( fontS, U"orgX:{}"_fmt(origin.x), guiRects[ORGX], wheel, origin.x, 0, Scene::Width());
		origin.y = guiValue( fontS, U"orgY:{}"_fmt(origin.y), guiRects[ORGY], wheel, origin.y, 0, Scene::Height());
		anchor.x = guiValue( fontS, U"ancX:{}"_fmt(anchor.x), guiRects[ANCX], wheel, anchor.x, 0, Scene::Width());
		anchor.y = guiValue( fontS, U"ancY:{}"_fmt(anchor.y), guiRects[ANCY], wheel, anchor.y, 0, Scene::Height());
		target.x = guiValue( fontS, U"trgX:{}"_fmt(target.x), guiRects[TRGX], wheel, target.x, 0, Scene::Width());
		target.y = guiValue( fontS, U"trgY:{}"_fmt(target.y), guiRects[TRGY], wheel, target.y, 0, Scene::Height());
		stretch.x = guiValue( fontS, U"strX:{}"_fmt(stretch.x), guiRects[STRX], wheel/100, stretch.x, 0, 2);
		stretch.y = guiValue( fontS, U"strY:{}"_fmt(stretch.y), guiRects[STRY], wheel/100, stretch.y, 0, 2);
		scale.x = guiValue( fontS, U"scaX:{}"_fmt(scale.x), guiRects[SCAX], wheel/100, scale.x, 0, 10);
		scale.y = guiValue( fontS, U"scaY:{}"_fmt(scale.y), guiRects[SCAY], wheel/100, scale.y, 0, 10);

//DEBUG MARKER
		origin = guiDrag(iconM,U"\xF1382", guiRects[M_ORG],Vec2(Cursor::Delta()),grabState,G_ORG,ColorF(0,0,1),ColorF(1),origin );
		anchor = guiDrag(iconM,U"\xF0031", guiRects[M_ANC],Vec2(Cursor::Delta()),grabState,G_ANC,ColorF(1,0,0),ColorF(1),anchor );
		target = guiDrag(iconM,U"\xF04FE", guiRects[M_TRG],Vec2(Cursor::Delta()),grabState,G_TRG,ColorF(1,0,1),ColorF(1),target );

		if (MouseL.up()) grabState.area = RELEASE;

//		for(auto sb2: SpeechBalloon2()) sb2.draw();
	}
}

