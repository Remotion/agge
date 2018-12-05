#pragma once

#include <agge/bitmap.h>
#include <agge/clipper.h>
#include <agge/platform/bitmap.h>
#include <agge/rasterizer.h>
#include <agge/renderer_parallel.h>
#include <agge/stroke.h>

#include <atlbase.h>
#include <vector>
#include <utility>
#include <memory>
#include <d2d1.h>

struct bar
{
	int o, h, l, c;
};

namespace Gdiplus
{
	class GraphicsPath;
	class PointF;
}

typedef std::vector< std::pair<std::pair<agge::real_t, agge::real_t>, unsigned> > AggPath;
typedef agge::bitmap<agge::pixel32, agge::platform::raw_bitmap> bitmap;

class CChildView : public CWnd
{
	typedef std::pair<RECT, COLORREF> ellipse_t;

	enum BufferType {	bufferNone, bufferDIB, bufferDDB	};
	enum DrawMode { dmodeGDI, dmodeGDIP, dmodeD2D, dmodeAGG };

	CComPtr<ID2D1Factory> _factory;
	CComPtr<ID2D1DCRenderTarget> _rtarget;
	CComPtr<ID2D1SolidColorBrush> _brush, _brushTick, _brushBackgroundSolid;
	CComPtr<ID2D1BitmapBrush> _brushBackground;

	bitmap _agg_bitmap;
	agge::rasterizer< agge::clipper<int> > _agg_rasterizer;
	agge::renderer_parallel _renderer;
	agge::stroke _stroke;

	DrawMode _drawMode;
	bool _drawLines, _drawBars, _drawEllipses, _drawSpiral;
	BufferType _mode;
	CBitmap _buffer;
	CStatusBar *_status_bar;
	double _onpaint_timing, _fill_timing, _drawing_timing, _blit_timing;
	int _averaging_index;

	std::vector<bar> _bars[30];
	std::vector<ellipse_t> _ellipses;

	std::unique_ptr<Gdiplus::GraphicsPath> _gdip_path;
	CComPtr<ID2D1PathGeometry> _d2d_path;
	AggPath _agg_path;
	AggPath _agg_path_flatten;

	DECLARE_MESSAGE_MAP();

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnSetPrimitiveLines();
	afx_msg void OnUpdateMenuPrimitiveLines(CCmdUI *pCmd);
	afx_msg void OnSetPrimitiveBars();
	afx_msg void OnUpdateMenuPrimitiveBars(CCmdUI *pCmd);
	afx_msg void OnSetPrimitiveEllipses();
	afx_msg void OnUpdateMenuPrimitiveEllipses(CCmdUI *pCmd);
	afx_msg void OnSetPrimitiveSpiral();
	afx_msg void OnUpdateMenuPrimitiveSpiral(CCmdUI *pCmd);

	afx_msg void OnSetBufferTypeDDB();
	afx_msg void OnUpdateMenuBufferTypeDDB(CCmdUI *pCmd);
	afx_msg void OnSetBufferTypeDIB();
	afx_msg void OnUpdateMenuBufferTypeDIB(CCmdUI *pCmd);
	afx_msg void OnSetBufferTypeNone();
	afx_msg void OnUpdateMenuBufferTypeNone(CCmdUI *pCmd);

	afx_msg void OnSetModeGDI();
	afx_msg void OnUpdateMenuModeGDI(CCmdUI *pCmd);
	afx_msg void OnSetModeGDIPlus();
	afx_msg void OnUpdateMenuModeGDIPlus(CCmdUI *pCmd);
	afx_msg void OnSetModeD2D();
	afx_msg void OnUpdateMenuModeD2D(CCmdUI *pCmd);
	afx_msg void OnSetModeAGG();
	afx_msg void OnUpdateMenuModeAGG(CCmdUI *pCmd);


	LRESULT OnPostedInvalidate(WPARAM, LPARAM);

	void drawLines(CDC &dc, const CSize &client, const std::vector<bar> &bars);
	void drawBars(CDC &dc, const CSize &client, const std::vector<bar> &bars);
	void drawEllipses(CDC &dc, const CSize &client, const std::vector<ellipse_t> &ellipses);
	void drawSpiral(CDC &dc, const CSize &client);

	void drawLines(Gdiplus::Graphics &graphics, const CSize &client, const std::vector<bar> &bars);
	void drawBars(Gdiplus::Graphics &graphics, const CSize &client, const std::vector<bar> &bars);
	void drawEllipses(Gdiplus::Graphics &graphics, const CSize &client, const std::vector<ellipse_t> &ellipses);
	void drawSpiral(Gdiplus::Graphics &graphics, const CSize &client);

	void drawLines(ID2D1RenderTarget *graphics, const CSize &client, const std::vector<bar> &bars);
	void drawBars(ID2D1RenderTarget *graphics, const CSize &client, const std::vector<bar> &bars);
	void drawEllipses(ID2D1RenderTarget *graphics, const CSize &client, const std::vector<ellipse_t> &ellipses);
	void drawSpiral(ID2D1RenderTarget *graphics, const CSize &client);

	void drawLines(bitmap &b, const CSize &client, const std::vector<bar> &bars);
	void drawBars(bitmap &b, const CSize &client, const std::vector<bar> &bars);
	void drawEllipses(bitmap &b, const CSize &client, const std::vector<ellipse_t> &ellipses);
	void drawSpiral(bitmap &b, const CSize &client);

public:
	CChildView();
	virtual ~CChildView();

	void SetStatusBar(CStatusBar *status_bar);
};

inline void CChildView::SetStatusBar(CStatusBar *status_bar)
{	_status_bar = status_bar;	}
