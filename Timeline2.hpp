#pragma once
# include <Siv3D.hpp>
# include <Siv3d/Easing.hpp>
#include <ThirdParty/Catch2/catch.hpp>

class Timeline
{
private:
	enum IDAREA { 
		RELEASE, 
		TITLE, DRAGBAR, TOOLBAR, CURSOR, TRACKLIST, CONTENT, FRAME, LMARK, SELECTION, RMARK,
		NUMAREA
	};

	enum IDGUI {
        rcTITLE, rcDRAGBAR, rcTOOLBAR,rcCURSOR, rcTRACKLIST, rcCONTENT, rcFRAME, rcLMARK, rcSELECTION, rcRMARK,
        NUMGUI
    };
    Rect m_Gui[NUMGUI] = {};

	enum EaseType {
		EASEINLINEAR, EASEINSINE, EASEINQUAD, EASEINCUBIC, EASEINQUART,
		EASEINQUINT, EASEINEXPO, EASEINCIRC, EASEINBACK, EASEINELASTIC,
		EASEINBOUNCE, EASEOUTLINEAR, EASEOUTSINE, EASEOUTQUAD, EASEOUTCUBIC,
		EASEOUTQUART, EASEOUTQUINT, EASEOUTEXPO, EASEOUTCIRC, EASEOUTBACK,
		EASEOUTELASTIC, EASEOUTBOUNCE, EASEINOUTLINEAR, EASEINOUTSINE,
		EASEINOUTQUAD, EASEINOUTCUBIC, EASEINOUTQUART, EASEINOUTQUINT,
		EASEINOUTEXPO, EASEINOUTCIRC, EASEINOUTBACK, EASEINOUTELASTIC,
		EASEINOUTBOUNCE, ELASTICAP
	};

	struct GRABSTATE
	{
		IDAREA area;
		int32 value;
	};
	GRABSTATE m_grabState{ RELEASE, 0 };

	struct Probe
	{
		float* value = nullptr;
	};
	Array<Probe> m_Probes;
	enum PROBE {ID_X, ID_Y};

	struct LinePoint
	{
/*
		EaseType easeType;
		double value;
		uint32 timeStart;
		uint32 timeLength;
		double lineBegin;
		double lineEnd;
*/
		EaseType easeType;
		double value;
		double duration;
	};

	struct Track
	{
		String textCaption = U"Unknown";
		ColorF colorBG = ColorF{};
		uint32 intStartTime = 0;
		uint32 intLengthTime = 0;
		 int32 intHeight = 0;
		uint32 intXDiv = 100;			//横軸の単位幅

		uint32 intTrackNo = 0;
		 int32 baseY = 0;				//トラック内スクロール高さ基準位置
		Rect captionRect = Rect{};

		bool merged = false;
		int32 trackYPos = 0;

		Array<String> lineNames;		//もうタイムラインないの線は水平のみ
		Array<Color> lineColors;
		Array<Array<LinePoint>> linePoints;





		double dblLo = -1.0;
		double dblHi = 1.0;
		double dblYDiv = 1.0;



		~Track() = default;

		Track()
		{
		}

		Track(String textCaption, String colorBG, int32 intStartTime,
			int32 intLengthTime, int32 intHeight, int32 intTrackNo,
			String lineR = U"", String lineG = U"", String lineB = U"",
			String lineC = U"", String lineY = U"", String lineM = U"", String lineK = U"")
			:textCaption(textCaption),
			colorBG(Color(colorBG)),
			intStartTime(intStartTime),
			intLengthTime(intLengthTime),
			intHeight(intHeight),
			intTrackNo(intTrackNo)
		{
			auto& ln = lineNames;
			auto& cl = lineColors;
			if (lineR != U"") { ln.emplace_back(lineR); cl.emplace_back(Color(U"#FF0000")); }
			if (lineG != U"") { ln.emplace_back(lineG); cl.emplace_back(Color(U"#00FF00")); }
			if (lineB != U"") { ln.emplace_back(lineB); cl.emplace_back(Color(U"#0000FF")); }
			if (lineC != U"") { ln.emplace_back(lineC); cl.emplace_back(Color(U"#FFFF00")); }
			if (lineY != U"") { ln.emplace_back(lineY); cl.emplace_back(Color(U"#FF00FF")); }
			if (lineM != U"") { ln.emplace_back(lineM); cl.emplace_back(Color(U"#00FFFF")); }
			if (lineK != U"") { ln.emplace_back(lineK); cl.emplace_back(Color(U"#000000")); }

		}
	};

	ColorF m_colorFrame = ColorF(0.9);
	ColorF m_colorTitle = ColorF(0.8);
	ColorF m_colorToolbar = ColorF(0.1);
	ColorF m_colorDragbar = ColorF(0.2);
	ColorF m_colorContent = ColorF(0.3);
	ColorF m_colorTracklist = ColorF(0.4);

	ColorF m_colorScaleL = ColorF(0.6);
	ColorF m_colorScaleM = ColorF(0.7);
	ColorF m_colorScaleS = ColorF(0.8);


	String m_Caption = U"Unknown";
	bool m_Hidden;
	Rect m_Gadget;

	Font m_iconS;
	Font m_iconM;

	Font m_fontS;
	Font m_fontM;
	Font m_fontL;

	int32 m_frameThickness = 1;
	int32 m_paddingL = 8;
	int32 m_paddingR = 8;
	int32 m_fontHeight = 33;
	int32 m_trackWidth = 400;

	Array<Track> m_Tracks;
	int32 m_intGrid = 16;


	RectF  m_viewRange = RectF{ 0, -1, 900, 2 };	//横は0msから900ms 縦は-1から+1の範囲を表示
	double m_ratioH = 0;	//水平表示比
	double m_rangeVR = 0;	//水平表示幅
	double m_minVR = 0;		//水平表示最小値
	double m_maxVR = 0;		//水平表示最大値

	double m_timeFrom = 10;
	double m_timeTo = 100;
	double m_timeMin = 0;
	double m_timeMax = 3600*1000;
	double m_unitTime = 10;

public:
	double m_timeCursor = 0;

	~Timeline() = default;
	Timeline() = default;

	void setHidden(bool hidden)	{ m_Hidden = hidden; }
	void setFontHeight(int32 height) { m_fontHeight = height; }

	void setup( const Rect& rect,
				int32 paddingL = 8,
				int32 paddingR = 8,
				int32 trackWidth = 400,
				int32 toolbarHeight = 60,
				RectF viewRange = RectF{ 0, -1, 900, 2 },
				const Optional<size_t>& selectedTrackIndex = none)
	{
		m_paddingL = paddingL;
		m_paddingR = paddingR;

		m_Gadget = Rect(rect.pos, Max(rect.w, 40), Max(rect.h, m_fontHeight + (m_frameThickness * 2)));

		m_viewRange = viewRange;

		m_iconS = Font(15, Typeface::Icon_MaterialDesign);
		m_iconM = Font(25, Typeface::Icon_MaterialDesign);

		m_fontS = Font(10, Typeface::Medium);
		m_fontM = Font(15);
		m_fontL = Font(30);
		
		int32 &ft = m_frameThickness;
		int32 fh = m_fontM.fontSize() + 15;
		Rect& title = m_Gui[rcTITLE];
		Rect& frame = m_Gui[rcFRAME];
		Rect& toolbar = m_Gui[rcTOOLBAR];
		Rect& dragbar = m_Gui[rcDRAGBAR];
		Rect& tracklist = m_Gui[rcTRACKLIST];

		title = Rect{ m_Gadget.x, m_Gadget.y     , m_Gadget.w, fh };
		frame = Rect{ title.x, title.y + fh, title.w, m_Gadget.h - fh };
		toolbar = Rect(frame.x + ft, frame.y + ft, trackWidth, toolbarHeight);
		dragbar = Rect(toolbar.x + trackWidth, toolbar.y, frame.w - trackWidth, toolbar.h);
	}

	void addTrack(const Track& track)
	{
		m_Tracks.emplace_back(track);
	}
	struct PARAMLINE
	{
		float* value;
		String linemname;
		Color  linecolor;
	};

	void setData2(String caption,uint32 trackno,uint32 trackheight, Color colortrack, 
				const Array<PARAMLINE> &paramline, double starttime = 0, uint32 lengthtime = 1000 )
	{
		Track tr ;
		tr.linePoints.resize(paramline.size());
		for(uint32 i=0;i<paramline.size(); i++)
		{
			const PARAMLINE &pl = paramline[i];
			m_Probes << Probe {pl.value} ;

			tr.textCaption = caption;
			tr.colorBG = colortrack;
			tr.intStartTime = starttime;
			tr.intLengthTime = lengthtime;	//とりあえず最初は1秒
			tr.intTrackNo = trackno;	//トラック番号
			tr.intHeight = trackheight;	//トラック表示高

			tr.lineNames << pl.linemname;
			tr.lineColors << pl.linecolor;

			LinePoint lp = {EASEINLINEAR, *m_Probes[ID_X].value, 0};
			tr.linePoints[i] << lp;
		}

		m_Tracks << tr ;
	}

	void action()
	{
	}

	bool update()
	{
		bool has_update = false;

		updateTitleAndFrame();
		updateDrag();
		updateCursor();
		updateSelection();
		updateToolbar();

		if (MouseL.up()) m_grabState.area = RELEASE;
		return has_update;
	}

	void drawLine( const RectF &rectT, const Track &tr, const int32& tx, const int32& ty )
	{
//		RasterizerState rs = RasterizerState::Default2D;
//		rs.scissorEnable = true;
//		Graphics2D::SetScissorRect(tr);
//		ScopedRenderStates2D renderState(rs);
/*
		//ライン描画
		for (int32 ii = 0; ii < tr.lineNames.size(); ii++)
		{
			if (tr.lineNames[ii] == U"") continue;

			double loVR = m_viewRange.y;						//垂直表示最小値 H=基準位置からの値範囲
			double hiVR = m_viewRange.y + m_viewRange.h;		//垂直表示最大値 Y=縦軸の基準位置

			auto& linepoints = tr.linePoints[ii];
			for (int32 p = 0; p < linepoints.size(); p++)
			{
				double offsetY = rectT.h / 2;						//高さの単位幅
				auto& point = linepoints[p];

				double th = rectT.h;
				double x1 = tr.intStartTime + point.timeStart;	//始端マーカー
				double bx = Clamp(x1, m_minVR, m_maxVR);

				double by = offsetY + point.lineBegin * th / (hiVR - loVR);

				double x2 = tr.intStartTime + point.timeStart + point.timeLength;//終端マーカー
				double ex = Clamp(x2, m_minVR, m_maxVR);
				double ey = offsetY + point.lineEnd * th / (hiVR - loVR);

				double len = (double)point.timeLength;			//区間の長さ
				double div = m_ratioH * len / 4;				//線分の分割数
				double stept = len / div;						//線分の長さ

				double delta = ey - by;	//デルタ
				double e = 0;		//イージング係数
				double x = 0;       //Xの値
				double y = 0;		//Yの値
				double xx = 0;      //XXの値
				double yy = 0;		//YYの値

				for (int32 iii = 1; iii <= div; iii++)
				{
					switch (point.easeType)
					{
					case EASEINLINEAR:e = EaseInLinear((double)iii / div); break;
					case EASEINSINE:e = EaseInSine((double)iii / div); break;
					case EASEINQUAD:e = EaseInQuad((double)iii / div); break;
					case EASEINCUBIC:e = EaseInCubic((double)iii / div); break;
					case EASEINQUART:e = EaseInQuart((double)iii / div); break;
					case EASEINQUINT:e = EaseInQuint((double)iii / div); break;
					case EASEINEXPO:e = EaseInExpo((double)iii / div); break;
					case EASEINCIRC:e = EaseInCirc((double)iii / div); break;
					case EASEINBACK:e = EaseInBack((double)iii / div); break;
					case EASEINELASTIC:e = EaseInElastic((double)iii / div); break;
					case EASEINBOUNCE:e = EaseInBounce((double)iii / div); break;

					case EASEOUTLINEAR:e = EaseOutLinear((double)iii / div); break;
					case EASEOUTSINE:e = EaseOutSine((double)iii / div); break;
					case EASEOUTQUAD:e = EaseOutQuad((double)iii / div); break;
					case EASEOUTCUBIC:e = EaseOutCubic((double)iii / div); break;
					case EASEOUTQUART:e = EaseOutQuart((double)iii / div); break;
					case EASEOUTQUINT:e = EaseOutQuint((double)iii / div); break;
					case EASEOUTEXPO:e = EaseOutExpo((double)iii / div); break;
					case EASEOUTCIRC:e = EaseOutCirc((double)iii / div); break;
					case EASEOUTBACK:e = EaseOutBack((double)iii / div); break;
					case EASEOUTELASTIC:e = EaseOutElastic((double)iii / div); break;
					case EASEOUTBOUNCE:e = EaseOutBounce((double)iii / div); break;

					case EASEINOUTLINEAR:e = EaseInOutLinear((double)iii / div); break;
					case EASEINOUTSINE:e = EaseInOutSine((double)iii / div); break;
					case EASEINOUTQUAD:e = EaseInOutQuad((double)iii / div); break;
					case EASEINOUTCUBIC:e = EaseInOutCubic((double)iii / div); break;
					case EASEINOUTQUART:e = EaseInOutQuart((double)iii / div); break;
					case EASEINOUTQUINT:e = EaseInOutQuint((double)iii / div); break;
					case EASEINOUTEXPO:e = EaseInOutExpo((double)iii / div); break;
					case EASEINOUTCIRC:e = EaseInOutCirc((double)iii / div); break;
					case EASEINOUTBACK:e = EaseInOutBack((double)iii / div); break;
					case EASEINOUTELASTIC:e = EaseInOutElastic((double)iii / div); break;
					case EASEINOUTBOUNCE:e = EaseInOutBounce((double)iii / div); break;

					}

					xx = iii * stept;
					yy = e * delta;

					if (Clamp(x1 + xx, m_minVR, m_maxVR) == x1 + xx &&
					   (Clamp(by + yy, 0.0, th) == by + yy && Clamp(by + y, 0.0, th) == by + y)) //クランプされなければ描画
							Line((int32)m_Gui[rcDRAGBAR].x + (x1 - m_minVR + x) * m_ratioH, ty + by + y,
							(int32)m_Gui[rcDRAGBAR].x + (x1 - m_minVR + xx) * m_ratioH, ty + by + yy).draw(3, tr.lineColors[ii]);
					x = xx;
					y = yy;
				}

				if (x2 == ex && Clamp(ey, 0.0, th) == ey) //クランプされなければ描画
					Line((int32)m_Gui[rcDRAGBAR].x + (x1 - m_minVR + x) * m_ratioH, ty + by + y,
						(int32)m_Gui[rcDRAGBAR].x + (x2 - m_minVR) * m_ratioH, ty + ey).draw(3, tr.lineColors[ii]);

				if (x1 == bx && Clamp(by, 0.0, th) == by) //クランプされなければ描画
				{
					Vec2 pos((int32)m_Gui[rcDRAGBAR].x + (bx - m_minVR) * m_ratioH, ty + by);
					const Rect& rect = Rect(pos.x - 10, pos.y - 10, 20, 20);
					m_iconS(U"\xF0E7E").draw(Arg::center(pos), rect.mouseOver() ? Color(U"#FFFFFF") : tr.lineColors[ii]);
				}

				if (x2 == ex && Clamp(ey, 0.0, th) == ey)//クランプされなければ描画
				{
					Vec2 pos((int32)m_Gui[rcDRAGBAR].x + (ex - m_minVR) * m_ratioH, ty + ey);
					const Rect& rect = Rect(pos.x - 10, pos.y - 10, 20, 20);
					m_iconS(U"\xF006E").draw(Arg::center(pos), rect.mouseOver() ? Color(U"#FFFFFF") : tr.lineColors[ii]);
				}
			}
		}
		*/
	}

	void drawTrack()
	{
		const int32 trackSpacing = 2;
		int32 ypos = 2;

		int32 tx = m_Gui[rcTRACKLIST].x + m_frameThickness;

		for (int32 i = 0; i < m_Tracks.size(); ++i)
		{
			Track& tr = m_Tracks[i];
			int32& trackheight = m_Tracks[tr.intTrackNo].intHeight;
			int32 ty = m_Gui[rcTRACKLIST].y + m_frameThickness + tr.trackYPos;

			//トラック描画
			RectF rectT(tr.intStartTime, ty, tr.intLengthTime, trackheight);

			if (m_maxVR < rectT.x || (rectT.x + rectT.w) < m_minVR) {} //範囲外
			else
			{
				double lb = (rectT.x < m_minVR) ? m_minVR - rectT.x : 0;
				double rb = (rectT.x + rectT.w > m_maxVR) ? (rectT.x + rectT.w) - m_maxVR : 0;
				rectT.w = (rectT.w - lb - rb) * m_ratioH;
				rectT.x = (int32)m_Gui[rcDRAGBAR].x + (Clamp(rectT.x, m_minVR, m_maxVR) - m_minVR) * m_ratioH;
				rectT.draw(tr.colorBG);

				//ライン描画
				drawLine( rectT, tr, tx, ty );
			}
		}
	}

	void drawTrackList()
	{
		const int32 trackSpacing = 2;
		int32 ypos = 2;

		for (int32 i = 0; i < m_Tracks.size(); ++i)
		{
			Track& tr = m_Tracks[i];
			tr.trackYPos = ypos;

			if (tr.intTrackNo != -1 && i != tr.intTrackNo)
			{
				tr.merged = true;
				tr.trackYPos = m_Tracks[tr.intTrackNo].trackYPos;
			}
			else
			{
				ypos += tr.intHeight + trackSpacing;
			}

			int32 tx = m_Gui[rcTRACKLIST].x + m_frameThickness;
			int32 ty = m_Gui[rcTRACKLIST].y + m_frameThickness + tr.trackYPos;

			//トラックリスト描画
			if (tr.merged == false)
			{
				int32 &trackheight = m_Tracks[tr.intTrackNo].intHeight;
				tr.captionRect = Rect(tx, ty, m_trackWidth, trackheight);
				tr.captionRect.draw(ColorF(0.2));
				m_fontM(tr.textCaption).draw(tr.captionRect.movedBy(m_paddingL, 0), ColorF(0.7));
				int32 fs = m_fontM.fontSize();
				int32 is = m_iconM.fontSize() + 1;

				for (int32 i = 0;i < tr.lineNames.size();i++)
				{
					int32 y = tr.baseY + i * fs * 3 + fs;
					if (y < 0 || y > trackheight) continue;

					const String m_Toolbar[] = { U"\xF09D8",U"\xF06FF",U"\xF09DA",U"\xF0FA7",U"\xF06F4",U"\xF0B45",
												   U"\xF02C3",U"\xF09C3",U"\xF0031",U"\xF06F1",U"\xF0339" };
					int32 bx = tr.captionRect.x + m_paddingL;
					int32 by = tr.captionRect.y + tr.baseY + i * fs * 3 + fs;
					m_fontM(tr.lineNames[i]).draw(bx, by, tr.lineColors[i]);
					Rect btn(bx + fs * 4, by, is, is);
					for (auto& tbi : m_Toolbar)
					{
						m_iconM(tbi).draw(btn, btn.mouseOver() ? ColorF(0.9) : ColorF(0.5));
						btn.x += btn.w;
					}
				}

				//トラック背景グリッド描画
				double unit;
				if (m_rangeVR < 1000)        unit = m_intGrid;
				else if (m_rangeVR < 10000)  unit = m_intGrid * 10;
				else if (m_rangeVR < 100000) unit = m_intGrid * 100;
				else                         unit = m_intGrid * 1000;

				int32 cnt =  (int32)m_minVR / (int32)unit;
				double xxx = (int32)m_minVR % (int32)unit;
				for (int32 x = m_minVR - xxx; x < m_maxVR; x += unit)
				{
					RectF rectT(x, ty, unit, trackheight);
					auto lb = (rectT.x < m_minVR) ? m_minVR - rectT.x : 0;
					auto rb = (rectT.x + rectT.w > m_maxVR) ? (rectT.x + rectT.w) - m_maxVR : 0;
					rectT.w = (rectT.w - lb - rb) * m_ratioH;
					rectT.x = (int32)m_Gui[rcDRAGBAR].x + (Clamp(rectT.x, m_minVR, m_maxVR) - m_minVR) * m_ratioH;
					rectT.draw((++cnt & 1) ? ColorF(0.30) : ColorF(0.31));
				}

			}

		}
	}

	void updateDrag()
	{
		Rect& title = m_Gui[rcTITLE];
		Rect& frame = m_Gui[rcFRAME];
		Rect& toolbar = m_Gui[rcTOOLBAR];
		Rect& dragbar = m_Gui[rcDRAGBAR];
		Rect& tracklist = m_Gui[rcTRACKLIST];

		Rect& select = m_Gui[rcSELECTION];
		Rect& lmark = m_Gui[rcLMARK];
		Rect& rmark = m_Gui[rcRMARK];
		Rect& cursor = m_Gui[rcCURSOR];
		Rect& content = m_Gui[rcCONTENT];

		//左ドラッグ開始
		if      (select.leftClicked())  
			m_grabState.area = SELECTION;
		else if (lmark.leftClicked())
			m_grabState.area = LMARK;
		else if (rmark.leftClicked())
			m_grabState.area = RMARK;
		else if (cursor.leftClicked())  m_grabState.area = CURSOR;
		else if (dragbar.leftClicked())	m_grabState.area = DRAGBAR;

		// 左ドラッグ
		double vx = (Cursor::Delta().x * m_viewRange.w / dragbar.w);
		double min = m_viewRange.x;
		double max = m_viewRange.x + m_viewRange.w;

		if (m_grabState.area == SELECTION)
		{
			m_timeFrom = Clamp(m_timeFrom + vx, min, max);
			m_timeTo = Clamp(m_timeTo + vx, min, max);
		}

		if (m_grabState.area == SELECTION)
		{
			m_timeFrom = Clamp(m_timeFrom + vx, min, max);
			m_timeTo = Clamp(m_timeTo + vx, min, max);
		}
		else if (m_grabState.area == LMARK)   m_timeFrom = Clamp(m_timeFrom + vx, min, m_timeTo);
		else if (m_grabState.area == RMARK)   m_timeTo = Clamp(m_timeTo + vx, m_timeFrom, max);
		else if (m_grabState.area == CURSOR )
		{
			if( isRecTime == INACTIVE ) m_timeCursor = Clamp(m_timeCursor + vx, min, max);
		}
		else if (m_grabState.area == DRAGBAR) m_viewRange.x = Clamp(m_viewRange.x - vx, m_timeMin, m_timeMax);

		//ドラッグバー上Wheel操作
		if (dragbar.mouseOver() && Mouse::Wheel())
		{
			if (KeyControl.pressed())  //スクロール(Wheel+Ctrl)
			{
				double vel = Mouse::Wheel() * m_viewRange.w;
				m_viewRange.x = Clamp(m_viewRange.x + vel, m_timeMin, m_timeMax);//最大1時間
			}
			else                       //拡縮(Wheel)
			{
				double vel = Mouse::Wheel() * m_viewRange.w / 10.0;
				m_viewRange.w = Clamp(m_viewRange.w + vel, 1.0, 100000.0);
			}
		}
	}

	void updateTitleAndFrame()
	{
		Rect& title = m_Gui[rcTITLE];
		Rect& frame = m_Gui[rcFRAME];
		Rect& toolbar = m_Gui[rcTOOLBAR];
		Rect& dragbar = m_Gui[rcDRAGBAR];
		Rect& tracklist = m_Gui[rcTRACKLIST];

		int32& ft = m_frameThickness;
		int32 fh = m_fontM.fontSize() + 15;

		title = Rect{ m_Gadget.x, m_Gadget.y     , m_Gadget.w, fh };
		frame = Rect{ title.x, title.y + fh, title.w, m_Gadget.h - fh };
		toolbar = Rect{ frame.x + ft, frame.y + ft, toolbar.w, toolbar.h };
		dragbar = Rect{ toolbar.x + toolbar.w, toolbar.y, frame.w - toolbar.w, toolbar.h };
		tracklist = Rect{ toolbar.x, toolbar.y+ toolbar.h, toolbar.w, m_Gadget.h - toolbar.h - title.h};

		if (m_grabState.area == RELEASE && title.leftClicked()) m_grabState.area = TITLE;
		if (m_grabState.area == TITLE)
		{
			Point po = Cursor::Delta();
			title.x = m_Gadget.x += po.x;
			title.y = m_Gadget.y += po.y;
		}

	}

	void draw()
	{
		if (m_Hidden) return;

		Rect& title = m_Gui[rcTITLE];
		Rect& frame = m_Gui[rcFRAME];
		Rect& toolbar = m_Gui[rcTOOLBAR];
		Rect& dragbar = m_Gui[rcDRAGBAR];
		Rect& tracklist = m_Gui[rcTRACKLIST];

		Rect& select = m_Gui[rcSELECTION];
		Rect& lmark = m_Gui[rcLMARK];
		Rect& rmark = m_Gui[rcRMARK];
		Rect& cursor = m_Gui[rcCURSOR];
		Rect& content = m_Gui[rcCONTENT];

		title.drawShadow(Vec2(6, 6), 24, 3).draw(m_colorTitle);
		frame.drawShadow(Vec2(6, 6), 24, 3).draw(m_colorFrame);
		m_iconM(U"\xF0697").draw(title, Palette::Black);
		m_fontM(m_Caption).draw(title.moveBy(m_iconM.fontSize(),0), Palette::Black);

		toolbar.draw(m_colorToolbar);
		dragbar.draw(m_colorDragbar);

		drawScale( Point((int32)dragbar.x, toolbar.y + 40) );
		drawTrackList();
		drawTrack();
		drawToolbar();
		drawCursor();
		drawSelection();
	}

	void drawScale( Point base )
	{
		int32 unitS = m_unitTime;

		Rect& dragbar = m_Gui[rcDRAGBAR];

//		RasterizerState rs = RasterizerState::Default2D;
//		rs.scissorEnable = true;
//		Graphics2D::SetScissorRect(dragbar);
//		ScopedRenderStates2D renderState(rs);

		m_ratioH = dragbar.w / m_viewRange.w;	//水平表示比
		m_rangeVR = m_viewRange.w;				//水平表示幅
		m_minVR = m_viewRange.x;				//水平表示最小値
		m_maxVR = m_viewRange.x + m_rangeVR;	//水平表示最大値

		if      (m_rangeVR < 100)   drawScaleLine( base,     1, 100 );
		else if (m_rangeVR < 1000)  drawScaleLine( base, unitS, 1000 );
		else if (m_rangeVR < 10000) drawScaleLine( base, unitS, 10000 );
		else                        drawScaleLine( base, unitS, 100000 );

	}

	void drawScaleLine(Point base, int32 units, int32 digit )
	{
		double xxx = (int32)m_minVR % 10;
		for (int32 x = m_minVR - xxx; x < m_maxVR; x += units)
		{
			int32 xx = base.x + (x - m_minVR) * m_ratioH;
			if (xx < base.x) continue;

			if (x % (digit/10) == 0)
			{
				String ms = U"{}"_fmt(x);
				m_fontM(ms).draw(Arg::center(xx, base.y - 2 - m_fontM.fontSize())), m_colorScaleM;
				Line(xx, base.y - 8, xx, base.y).draw(1, m_colorScaleM);
			}

			else
			{
				if (digit == 100)
				{
					Line(xx, base.y - 6, xx, base.y).draw(1, m_colorScaleL);
				}
				else if (digit == 1000)
				{
					if (x % 10 == 0) Line(xx, base.y - 8, xx, base.y).draw(1, m_colorScaleL);
					else Line(xx, base.y - 6, xx, base.y).draw(1, m_colorScaleL);
				}
				else if (digit == 10000)
				{
					if      (x % 100 == 0) Line(xx, base.y - 7, xx, base.y).draw(1, m_colorScaleM);
					else if (x % 10 == 0) Line(xx, base.y - 5, xx, base.y).draw(1, m_colorScaleL);
				}
				else
				{
					if      (x % 1000 == 0) Line(xx, base.y - 8, xx, base.y).draw(1, m_colorScaleS);
					else if (x % 100 == 0) Line(xx, base.y - 6, xx, base.y).draw(1, m_colorScaleS);
				}
			}
		}
	}

	//ツールバーボタン描画
	enum ID_BTN {ID_PLAY,ID_STEP,ID_STOP,ID_REC,ID_EDIT,ID_CURSOR,ID_FROM,ID_TO,ID_ANCHOR,
	             ID_CONFIG,ID_LINK };
	String BTN_PLAY   = U"\xF040A",   BTN_STEP   = U"\xF040E",	BTN_STOP   = U"\xF04DB",
	       BTN_REC    = U"\xF044B",   BTN_EDIT   = U"\xF12C6",	BTN_CURSOR = U"\xF0523",
	       BTN_FROM   = U"\xF0367",   BTN_TO     = U"\xF0361",	BTN_ANCHOR = U"\xF0031",
	       BTN_CONFIG = U"\xF06F1",   BTN_LINK   = U"\xF0339";
	struct Toolbar
	{
		String code;
		double sec;
		ColorF colorON;
		ColorF colorOFF;
	};
	const double INACTIVE = 0.0;
	Array<Toolbar> m_Toolbar = { { BTN_PLAY,  0.0, ColorF(0.9) ,ColorF(0.3) },
								 { BTN_STEP,  0.0, ColorF(0.9) ,ColorF(0.3) },
								 { BTN_STOP,  0.0, ColorF(0.9) ,ColorF(0.3) },
								 { BTN_REC,   0.0, ColorF(0.9) ,ColorF(0.3) },
								 { BTN_EDIT,  0.0, ColorF(0.9) ,ColorF(0.3) },
								 { BTN_CURSOR,0.0, ColorF(0.9) ,ColorF(0.3) },
								 { BTN_FROM,  0.0, ColorF(0.9) ,ColorF(0.3) },
								 { BTN_TO,    0.0, ColorF(0.9) ,ColorF(0.3) },
								 { BTN_ANCHOR,0.0, ColorF(0.9) ,ColorF(0.3) },
								 { BTN_CONFIG,0.0, ColorF(0.9) ,ColorF(0.3) },
								 { BTN_LINK,  0.0, ColorF(0.9) ,ColorF(0.3) } };

	double& isPlayTime = m_Toolbar[ID_PLAY].sec;
	double& isRecTime = m_Toolbar[ID_REC].sec;
	double& isStepTime = m_Toolbar[ID_STEP].sec;

	void drawToolbar()
	{
		int32 is = m_iconM.fontSize() + 1;
		Rect btn(m_Gui[rcTOOLBAR].x + m_paddingL, m_Gui[rcTOOLBAR].y, is, is);

		for (int32 i=0; i<m_Toolbar.size(); i++)
		{
			auto& tbi = m_Toolbar[i];
			if( tbi.code == BTN_PLAY && tbi.sec != INACTIVE ) 
				m_iconM(tbi.code).draw(btn, ColorF(0,0.9,0) );

			else if( tbi.code == BTN_REC && tbi.sec != INACTIVE ) 
				m_iconM(tbi.code).draw(btn, ColorF(0.9,0,0) );

			else if( tbi.code == BTN_STEP && tbi.sec != INACTIVE ) 
				m_iconM(tbi.code).draw(btn, ColorF(0,0,0.9) );

			else 
				m_iconM(tbi.code).draw(btn, btn.mouseOver() ? ColorF(0.9) : ColorF(0.3));

			btn.x += btn.w;
		}
	}

	//ツールバー制御
	void updateToolbar()
	{
		int32 is = m_iconM.fontSize() + 1;
		Rect btn(m_Gui[rcTOOLBAR].x + m_paddingL, m_Gui[rcTOOLBAR].y, is, is);
		for (int32 i=0; i<m_Toolbar.size(); i++)
		{
			auto& tbi = m_Toolbar[i];
			if( KeyF2.up() )
			{
				isPlayTime = INACTIVE ;
				if( isRecTime == INACTIVE ) isRecTime = Scene::Time();
			}

			if( btn.mouseOver() && btn.leftClicked() )
			{
				if( tbi.code == BTN_PLAY )
				{
					isRecTime = isStepTime = INACTIVE ;
					if( isPlayTime == INACTIVE ) isPlayTime = Scene::Time();
				}
				else if( tbi.code == BTN_STEP )
				{
					isRecTime = isPlayTime = INACTIVE ;
					if( isStepTime == INACTIVE ) isStepTime = Scene::Time();

				}
				else if( tbi.code == BTN_STOP )
				{
					isPlayTime = isRecTime = isStepTime = INACTIVE ;
				}
				else if( tbi.code == BTN_REC )
				{
					isPlayTime = isStepTime = INACTIVE ;
					if( isRecTime == INACTIVE ) isRecTime = Scene::Time();
				}
				else if( tbi.code == BTN_EDIT )
				{
				}
				else if( tbi.code == BTN_CURSOR )
				{
				}
				else if( tbi.code == BTN_FROM )
				{
				}
				else if( tbi.code == BTN_TO )
				{
				}
				else if( tbi.code == BTN_ANCHOR )
				{
				}
				else if( tbi.code == BTN_CONFIG )
				{
				}
				else if( tbi.code == BTN_LINK )
				{
				}
			}
			btn.x += btn.w;

			if(isRecTime != INACTIVE)
			{
				static double prevtime = 0.0;

				double time = (Scene::Time()-isRecTime) *1000;
				double period = m_timeTo-m_timeFrom;
				m_timeCursor = (period < time) ? m_timeFrom : time;  
				if(period < time)	// 満了
				{
					m_timeCursor = m_timeFrom;
					isPlayTime = INACTIVE;
				}
				else                // 記録中
				{
					m_timeCursor = time;

					if( prevtime+16 < m_timeCursor )
					{
						LinePoint lp = {EASEINLINEAR, *m_Probes[ID_X].value, 0};
						m_Tracks[0].linePoints[ID_X].back().duration = m_timeCursor - m_timeFrom; 
						m_Tracks[0].linePoints[ID_X] << lp;

						lp.value = *m_Probes[ID_Y].value;
						m_Tracks[0].linePoints[ID_Y].back().duration = m_timeCursor - m_timeFrom; 
						m_Tracks[0].linePoints[ID_Y] << lp;
					
						prevtime = m_timeCursor;
					}

					else if( prevtime > m_timeCursor )	//カーソル位置が最初に戻り
					{
						m_Tracks[0].linePoints[ID_X].clear();
						m_Tracks[0].linePoints[ID_Y].clear();
						LinePoint lp = {EASEINLINEAR, *m_Probes[ID_X].value, 0};
						m_Tracks[0].linePoints[ID_X] << lp;
						lp.value = *m_Probes[ID_Y].value;
						m_Tracks[0].linePoints[ID_Y] << lp;
						prevtime = m_timeCursor;
					}
				}
			}

			if(isPlayTime != INACTIVE || isStepTime != INACTIVE)	//再生中
			{
				if(isPlayTime != INACTIVE)	//再生中のみ自動更新 
					m_timeCursor = (fmod((Scene::Time()-isPlayTime)*1000, (m_timeTo-m_timeFrom))+m_timeFrom); 

				auto& lpx = m_Tracks[0].linePoints[ID_X];
				auto& lpy = m_Tracks[0].linePoints[ID_Y];
				for(uint32 i=0;i<lpx.size();i++)
				{
					if( lpx[i].duration > m_timeCursor )
					{
						*m_Probes[ID_X].value = lpx[i].value; 
						*m_Probes[ID_Y].value = lpy[i].value; 
						break ;
					}
				}
			}

		}
	}

	//カーソル更新
	void updateCursor()
	{
		int32 fs = m_fontS.fontSize();
		int32 is = m_iconM.fontSize() + 1;
		int32 ty = m_Gui[rcTRACKLIST].y + m_frameThickness;

		Rect& rectC = m_Gui[rcCURSOR];
		rectC = Rect{ (int32)m_timeCursor, ty, is, m_Gui[rcTOOLBAR].h };

		if (m_maxVR < m_timeCursor)      rectC.x = m_maxVR;
		else if (m_timeCursor < m_minVR) rectC.x = m_minVR;

		rectC.x = (int32)m_Gui[rcDRAGBAR].x + 
					(Clamp((double)rectC.x, m_minVR, m_maxVR) - m_minVR) * m_ratioH;
		rectC = Rect(rectC.x- is / 2, rectC.y - m_Gui[rcTOOLBAR].h, is, is);
	}

	//カーソル描画
	void drawCursor()
	{
		int32 fs = m_fontM.fontSize();
		int32 is = m_iconM.fontSize() + 1;
		int32 ty = m_Gui[rcTRACKLIST].y + m_frameThickness;

		Rect& rectC = m_Gui[rcCURSOR];

		Vec2 cc = Vec2(rectC.x + is / 2, m_Gui[rcDRAGBAR].y + is);

		ColorF colOFF,colON;
		if(isRecTime != INACTIVE)
		{
			colOFF = ColorF(0.9, 0, 0, 0.3);
			colON = ColorF(0.3, 0, 0, 0.3);
		}
		else if(isPlayTime != INACTIVE)
		{
			colOFF = ColorF(0, 0.9, 0, 0.3);
			colON = ColorF(0, 0.3, 0, 0.3);
		}
		else if(isStepTime != INACTIVE)
		{
			colOFF = ColorF(0, 0, 0.9, 0.3);
			colON = ColorF(0, 0, 0.3, 0.3);
		}
		else
		{
			colOFF = ColorF(0.9, 0.9, 0.9, 0.3);
			colON = ColorF(0.3, 0.3, 0.3, 0.3);
		}

		m_iconM(U"\xF0523").draw(rectC, rectC.mouseOver() ? colON : colOFF);

		static double move = 0.0;
		move += Scene::DeltaTime() * 10;
		Line(cc, cc + Vec2(0, m_Gui[rcTRACKLIST].h + m_Gui[rcDRAGBAR].h - is))
									.draw(LineStyle::SquareDot(move), 2, colOFF);
		String duration = U"{}"_fmt((uint32)m_timeCursor);
		m_fontM(duration).draw(Arg::center(cc.x, cc.y - fs));
	}

	//領域選択更新
	void updateSelection()
	{
		int32 fs = m_fontS.fontSize();
		int32 is = m_iconM.fontSize() + 1;
		int32 ty = m_Gui[rcTRACKLIST].y + m_frameThickness;

		Rect rectS = Rect{ (int32)m_timeFrom, ty, (int32)m_timeTo - (int32)m_timeFrom, ty  };
		if (m_maxVR < m_timeFrom) rectS.x = m_maxVR; //範囲外
		if (m_timeTo < m_minVR) rectS.x = m_minVR;
		double lb = (rectS.x < m_minVR) ? m_minVR - rectS.x : 0;
		double rb = (rectS.x + rectS.w > m_maxVR) ? (rectS.x + rectS.w) - m_maxVR : 0;
		rectS.w = (rectS.w - lb - rb) * m_ratioH;
		rectS.x = m_Gui[rcDRAGBAR].x + (Clamp((double)rectS.x, m_minVR, m_maxVR) - m_minVR) * m_ratioH;

		m_Gui[rcLMARK] = Rect{ rectS.x-is ,ty-is+5,is, is };
		m_Gui[rcRMARK] = Rect{ rectS.x+ rectS.w ,ty-is+5,is, is };
	}

	//領域選択描画
	void drawSelection()
	{
		int32 fs = m_fontS.fontSize();
		int32 is = m_iconM.fontSize() + 1;
		int32 ty = m_Gui[rcTRACKLIST].y + m_frameThickness;

		Rect& rectL = m_Gui[rcLMARK];
		Rect& rectR = m_Gui[rcRMARK];

		ColorF colOFF = ColorF(0.9, 0.9, 0.9, 0.3);
		ColorF colON = ColorF(0, 0, 0, 0.3);

		static double move = 0.0;
		move += Scene::DeltaTime() *-10;

		Vec2 ll = Vec2(rectL.x + is, m_Gui[rcDRAGBAR].y + is);
		m_iconM(U"\xF0367").draw(rectL, rectL.mouseOver() ? colON : colOFF);
		Line(ll, ll + Vec2(0, m_Gui[rcTRACKLIST].h + m_Gui[rcDRAGBAR].h - is))
									.draw(LineStyle::SquareDot(move), 2, colON);
		String timeL = U"{}"_fmt((uint32)m_timeFrom);
		m_fontM(timeL).draw(Arg::center(ll.x, ll.y+fs+14));

		Vec2 rr = Vec2(rectR.x , m_Gui[rcDRAGBAR].y + is);
		m_iconM(U"\xF0361").draw(rectR, rectR.mouseOver() ? colON : colOFF);
		Line(rr, rr + Vec2(0, m_Gui[rcTRACKLIST].h + m_Gui[rcDRAGBAR].h - is))
									.draw(LineStyle::SquareDot(move), 2, colON);
		String timeR = U"{}"_fmt((uint32)m_timeTo);
		m_fontM(timeR).draw(Arg::center(rr.x, rr.y + fs+14));
	}

	void main()
	{
		update();
		draw();
	}
};