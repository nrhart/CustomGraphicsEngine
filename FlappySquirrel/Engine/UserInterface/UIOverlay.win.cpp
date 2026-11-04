#include "UIOverlay.h"
#include <memory>
#include <thread>
#include <chrono>
#include <cmath>
#include <algorithm>

#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "Gdi32.lib")

using namespace Gdiplus;

namespace UIOverlay
{
	static const wchar_t* kWindowName = L"UIOverlayWindow";

	LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}


	ULONG_PTR Overlay::s_gdiplusToken = 0;
	bool Overlay::s_gdiplusInitialized = false;

	bool Overlay::Initialize(HINSTANCE hInstance)
	{
		if (s_gdiplusInitialized)
			return true;

		if (m_running.load())
			return true;

		GdiplusStartupInput gdiplusStartupInput;
		Status st = GdiplusStartup(&s_gdiplusToken, &gdiplusStartupInput, nullptr);
		if (st != Ok)
			return false;
		s_gdiplusInitialized = true;

		// Initialize overlay window
		WNDCLASSEXW wc = {};
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = OverlayWndProc;
		wc.hInstance = hInstance;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.lpszClassName = kWindowName;
		RegisterClassExW(&wc);
		m_running = true;
		m_thread = std::thread(&Overlay::ThreadMain, this);
		return true;
	}

	void Overlay::Shutdown()
	{
		Destroy();
		if (s_gdiplusInitialized)
		{
			GdiplusShutdown(s_gdiplusToken);
			s_gdiplusToken = 0;
			s_gdiplusInitialized = false;
		}
	}

	bool Overlay::Backend_CreateWindow(HWND parent, bool clickThrough)
	{
		if (!s_gdiplusInitialized)
			return false;


		m_parent = parent;
		m_clickThrough = clickThrough;

		DWORD exStyle = WS_EX_LAYERED | WS_EX_NOACTIVATE;
		if (m_clickThrough)
			exStyle |= WS_EX_TRANSPARENT;

		m_hwnd = CreateWindowExW(
			exStyle,
			kWindowName,
			L"",
			WS_POPUP,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			m_parent,
			nullptr,
			GetModuleHandle(nullptr),
			nullptr
		);

		if (!m_hwnd)
			return false;

		SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		// Ensure the overlay stays on top of the parent
		SetWindowPos(m_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);


		// Track the parent window if given
		m_trackingEnabled = (m_parent != nullptr);
		if (m_parent && m_trackingEnabled)
		{
			m_trackingThread = std::thread([this]()
				{
					RECT lastParentRect = {};
					if (m_parent)
					{
						if (!GetWindowRect(m_parent, &lastParentRect))
							lastParentRect = { 0,0,0,0 };
					}
					while (m_trackingEnabled)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(5));

						if (!m_parent || !IsWindow(m_parent))
							continue;

						RECT parentRect;
						if (!GetWindowRect(m_parent, &parentRect))
							continue;

						if (memcmp(&parentRect, &lastParentRect, sizeof(RECT)) != 0)
						{
							lastParentRect = parentRect;
							int newX, newY, w, h;
							{
								newX = m_lastX;
								newY = m_lastY;
								w = m_lastWidth;
								h = m_lastHeight;
							}

							POINT pt = { newX, newY };
							if (m_parent)
								ClientToScreen(m_parent, &pt);

							// Move the window without rerendering
							SetWindowPos(m_hwnd, HWND_TOP, pt.x, pt.y, w, h, SWP_NOACTIVATE);
						}
					}
				});
		}

		return true;
	}
	bool Overlay::Backend_AddPNG(const std::wstring& filePath, int x, int y, int width = 0, int height = 0)
	{
		if (!s_gdiplusInitialized)
			return false;

		Bitmap* bmp = Bitmap::FromFile(filePath.c_str(), FALSE);
		if (!bmp || bmp->GetLastStatus() != Ok)
		{
			delete bmp;
			return false;
		}

		// If no width/height specified, use the image’s natural size
		int w = width > 0 ? width : static_cast<int>(bmp->GetWidth());
		int h = height > 0 ? height : static_cast<int>(bmp->GetHeight());

		ImageEntry entry;
		entry.bitmap.reset(bmp);
		entry.x = x;
		entry.y = y;
		entry.width = w;
		entry.height = h;

		m_images.push_back(std::move(entry));
		m_needsRedraw = true;

		return true;
	}

	void Overlay::Backend_SetClickThrough(bool enable)
	{
		m_clickThrough = enable;
		if (!m_hwnd) return;
		LONG_PTR ex = GetWindowLongPtr(m_hwnd, GWL_EXSTYLE);
		if (enable)
			ex |= WS_EX_TRANSPARENT;
		else
			ex &= ~WS_EX_TRANSPARENT;
		SetWindowLongPtr(m_hwnd, GWL_EXSTYLE, ex);
	}
	static HBITMAP CreateDIBSectionTopDown(int width, int height, void** outBits)
	{
		BITMAPINFO bmi = {};
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		void* bits = nullptr;
		HDC screenDC = GetDC(nullptr);
		HBITMAP hBmp = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
		ReleaseDC(nullptr, screenDC);
		if (outBits) *outBits = bits;
		return hBmp;
	}
	static bool UpdateLayeredFromHBITMAP(HWND hwnd, HBITMAP hBmp, POINT ptPos, SIZE sizeWnd, BYTE alpha)
	{
		HDC screenDC = GetDC(nullptr);
		HDC memDC = CreateCompatibleDC(screenDC);
		HBITMAP oldBmp = static_cast<HBITMAP>(SelectObject(memDC, hBmp));
		POINT ptSrc = { 0, 0 };

		BLENDFUNCTION blend = {};
		blend.BlendOp = AC_SRC_OVER;
		blend.BlendFlags = 0;
		blend.SourceConstantAlpha = alpha;
		blend.AlphaFormat = AC_SRC_ALPHA;

		BOOL res = UpdateLayeredWindow(hwnd, screenDC, &ptPos, &sizeWnd, memDC, &ptSrc, 0, &blend, ULW_ALPHA);

		SelectObject(memDC, oldBmp);
		DeleteDC(memDC);
		ReleaseDC(nullptr, screenDC);
		return res != FALSE;
	}

	bool Overlay::Backend_Redraw(int x, int y, int width, int height, float opacity)
	{
		if (!m_hwnd || !IsWindow(m_hwnd) || width <= 0 || height <= 0)
			return false;

		if (m_images.empty() && m_textEntries.empty())
			return false;

		std::unique_ptr<Bitmap> target(new Bitmap(width, height, PixelFormat32bppPARGB));
		if (!target || target->GetLastStatus() != Ok)
			return false;

		// Draw content
		{
			Graphics g(target.get());
			g.SetCompositingMode(CompositingModeSourceOver);
			g.SetInterpolationMode(InterpolationModeHighQualityBicubic);

			for (const auto& img : m_images)
			{
				if (img.bitmap)
					g.DrawImage(img.bitmap.get(), img.x, img.y, img.width, img.height);
			}

			for (const auto& te : m_textEntries)
			{
				FontFamily family(te.fontName.c_str());
				FontStyle style = te.bold ? FontStyleBold : FontStyleRegular;
				Font font(&family, static_cast<Gdiplus::REAL>(te.fontSize), style, UnitPixel);
				SolidBrush brush(Color((te.argbColor >> 24) & 0xFF,
					(te.argbColor >> 16) & 0xFF,
					(te.argbColor >> 8) & 0xFF,
					te.argbColor & 0xFF));
				g.SetTextRenderingHint(TextRenderingHintAntiAlias);
				g.DrawString(te.text.c_str(), -1, &font, PointF(static_cast<Gdiplus::REAL>(te.x), static_cast<Gdiplus::REAL>(te.y)), &brush);
			}
		}

		// Lock bitmap
		BitmapData bd;
		Rect r(0, 0, width, height);
		if (target->LockBits(&r, ImageLockModeRead, PixelFormat32bppPARGB, &bd) != Ok)
			return false;

		void* dibBits = nullptr;
		HBITMAP hDib = CreateDIBSectionTopDown(width, height, &dibBits);
		if (!hDib)
		{
			target->UnlockBits(&bd);
			return false;
		}

		// Copy pixels
		const BYTE* srcPtr = static_cast<const BYTE*>(bd.Scan0);
		int srcStride = bd.Stride;
		int dstStride = width * 4;
		for (int row = 0; row < height; ++row)
		{
			memcpy(static_cast<BYTE*>(dibBits) + row * dstStride, srcPtr + row * srcStride, dstStride);
		}
		target->UnlockBits(&bd);

		// Replace old HBITMAP safely
		if (m_cachedHBitmap)
		{
			DeleteObject(m_cachedHBitmap);
			m_cachedHBitmap = nullptr;
		}
		m_cachedHBitmap = hDib;

		m_cachedWidth = width;
		m_cachedHeight = height;
		m_cachedOpacity = opacity;
		m_needsRedraw = false;

		// Determine screen position
		POINT topLeft = { x, y };
		if (m_parent)
			ClientToScreen(m_parent, &topLeft);

		SIZE sizeWnd = { width, height };
		const BYTE alpha = static_cast<BYTE>(std::clamp(opacity, 0.0f, 1.0f) * 255.0f);

		bool ok = UpdateLayeredFromHBITMAP(m_hwnd, m_cachedHBitmap, topLeft, sizeWnd, alpha);
		if (ok)
		{
			SetWindowPos(m_hwnd, HWND_TOP, topLeft.x, topLeft.y, width, height, SWP_NOREDRAW);
			ShowWindow(m_hwnd, SW_SHOW);
		}
		return ok;
	}


	void Overlay::Backend_Hide()
	{
		if (m_hwnd)
			ShowWindow(m_hwnd, SW_HIDE);
	}

	void Overlay::Backend_UpdatePosition(int x, int y, int width, int height, float opacity)
	{
		if (!m_hwnd || !IsWindow(m_hwnd))
			return;

		if (width <= 0) width = 1;
		if (height <= 0) height = 1;

		RECT rect;
		if (!GetWindowRect(m_hwnd, &rect))
			return;

		int curWidth = rect.right - rect.left;
		int curHeight = rect.bottom - rect.top;

		// Only update if position or size changed
		if (rect.left == x && rect.top == y && curWidth == width && curHeight == height)
			return;

		SetWindowPos(
			m_hwnd,
			nullptr,
			x, y,
			width, height,
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER
		);
	}


	void Overlay::ThreadMain()
	{
		try
		{
			MSG msg;
			PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);

			HWND createdWindow = nullptr;
			bool windowCreated = false;

			while (m_running)
			{
				Command cmd;

				{
					std::unique_lock<std::mutex> lock(m_queueMutex);
					m_cv.wait(lock, [&] { return !m_running || !m_queue.empty(); });

					if (!m_running)
						break;

					cmd = m_queue.front();
					m_queue.pop();
				}

				if (cmd.type == Command::CREATE_WINDOW)
				{
					HWND parent = reinterpret_cast<HWND>(cmd.a);

					if (!Backend_CreateWindow(parent, cmd.flag))
					{
						OutputDebugStringA("UIOverlay: Backend_CreateWindow FAILED\n");
					}
					else
					{
						createdWindow = m_hwnd;
						windowCreated = true;
						OutputDebugStringA("UIOverlay: Window created successfully\n");
					}

					break;
				}
			}

			if (!windowCreated)
			{
				OutputDebugStringA("UIOverlay: Thread exiting - window never created\n");
				return;
			}

			while (m_running)
			{
				MSG msg;
				while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				Command cmd;
				bool hasCommand = false;

				{
					std::unique_lock<std::mutex> lock(m_queueMutex);
					if (!m_queue.empty())
					{
						cmd = m_queue.front();
						m_queue.pop();
						hasCommand = true;
					}
				}

				if (hasCommand)
				{
					switch (cmd.type)
					{
					case Command::ADD_PNG:
						Backend_AddPNG(cmd.str1, static_cast<int>(cmd.a), cmd.b, cmd.c, cmd.d);
						break;

					case Command::CLEAR_PNG:
						Backend_ClearPNGs();
						break;

					case Command::ADD_TEXT:
						Backend_AddText(cmd.str1, static_cast<int>(cmd.a), cmd.b, cmd.str2, cmd.c, cmd.d, cmd.flag);
						break;

					case Command::CLEAR_TEXT:
						Backend_ClearText();
						break;

					case Command::SHOW:
						Backend_Show(static_cast<int>(cmd.a), cmd.b, cmd.c, cmd.d, cmd.f);
						break;

					case Command::UPDATE_POSITION:
						Backend_UpdatePosition(static_cast<int>(cmd.a), cmd.b, cmd.c, cmd.d, cmd.f);
						break;

					case Command::HIDE:
						Backend_Hide();
						break;

					case Command::SET_CLICKTHROUGH:
						Backend_SetClickThrough(cmd.flag);
						break;

					case Command::DESTROY:
						Backend_Destroy();
						break;
					}
				}
			}

			Backend_Destroy();
		}
		catch (const std::exception& e)
		{
			OutputDebugStringA("UIOverlay Thread Exception: ");
			OutputDebugStringA(e.what());
			OutputDebugStringA("\n");
		}
		catch (...)
		{
			OutputDebugStringA("UIOverlay Thread: UNKNOWN EXCEPTION\n");
		}

	}
}