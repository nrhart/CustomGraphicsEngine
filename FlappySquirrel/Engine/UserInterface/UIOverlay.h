#pragma once

#include <string>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>

// Only define Windows + GDI+ once, cleanly
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
#endif

// Reference for GDI+: https://learn.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-getting-started-use
// Helpful StackOverflow article: https://stackoverflow.com/questions/5647322/gdi-font-rendering-especially-in-layered-windows
// Useful GitHub article: https://gist.github.com/retorillo/3a12e0f7e6ae3d49771f2919608f8498

namespace UIOverlay
{
	class Overlay
	{
	public:
		Overlay();
		~Overlay();

		// Initialization and creation
#ifdef _WIN32
		bool Initialize(HINSTANCE hInstance);
		bool CreateForWindow(HWND parent, bool clickThrough = false);
#else
		bool Initialize();
		bool CreateForWindow(void* parent, bool clickThrough = false);
#endif
		// Shutdown and destruction
		// Only call shutdown from the main thread
		void Shutdown();
		void Destroy();

		// Add a PNG image to the overlay
		// wstring filePath, int xCoordinate, int yCoordinate, int imageWidth, int imageHeight
		bool AddPNG(const std::wstring& filePath, int x, int y, int width, int height);
		// Clear all loaded PNG images from the overlay
		void ClearPNGs();
		// Show the overlay at specified position and size with opacity
		bool Show(int x, int y, int width, int height, float opacity = 1.0f);
		// Update the overlay position, size, and opacity
		bool UpdatePosition(int x, int y, int width, int height, float opacity = 1.0f);
		// Redraw the overlay content
		bool Redraw(int x, int y, int width, int height, float opacity = 1.0f);
		// Hide the overlay
		void Hide();
		// Enable or disable being able to click through the overlay
		void SetClickThrough(bool enable);

		// Add text to the overlay
		// wstring textContent, int xCoordinate, int yCoordinate, wstring font, int fontSize, unsigned int ARGBColor, bool bold
		bool AddText(const std::wstring& text, int x, int y, const std::wstring& fontName = L"Segoe UI", int fontSize = 14, unsigned int argbColor = 0xFF000000, bool bold = false);
		// Clear all added text from the overlay
		void ClearText();

	private:
		Overlay(const Overlay&) = delete;
		Overlay& operator=(const Overlay&) = delete;

		// Command structure for thread communication
		struct Command {
			enum Type {
				ADD_PNG,
				CLEAR_PNG,
				ADD_TEXT,
				CLEAR_TEXT,
				SHOW,
				UPDATE_POSITION,
				HIDE,
				SET_CLICKTHROUGH,
				CREATE_WINDOW,
				DESTROY
			};
			Type type;

			std::wstring str1;
			std::wstring str2;
			intptr_t a = 0;
			int b = 0, c = 0, d = 0;
			float f = 0.0f;
			bool flag = false;
		};

		// Main loop for the worker thread
		void ThreadMain();
		// Enqueue a command to the worker thread
		void Enqueue(const Command& cmd);

		// Backend implementations
		bool Backend_CreateWindow(HWND parent, bool clickThrough);
		bool Backend_AddPNG(const std::wstring&, int, int, int, int);
		void Backend_ClearPNGs();
		bool Backend_AddText(const std::wstring&, int, int, const std::wstring&, int, unsigned int, bool);
		void Backend_ClearText();
		bool Backend_Show(int, int, int, int, float);
		void Backend_UpdatePosition(int, int, int, int, float);
		void Backend_Hide();
		void Backend_SetClickThrough(bool enable);
		void Backend_Destroy();
		bool Backend_Redraw(int x, int y, int w, int h, float opacity);


		struct TextEntry
		{
			std::wstring text;
			int x = 0;
			int y = 0;
			std::wstring fontName = L"Segoe UI";
			int fontSize = 14;
			unsigned int argbColor = 0xFF000000;
			bool bold = false;
		};

#ifdef _WIN32
		// Window handles
		HWND m_hwnd = nullptr;
		HWND m_parent = nullptr;
		// GDI+ token
		static ULONG_PTR s_gdiplusToken;
		static bool s_gdiplusInitialized;
#else
		// Window handles
		void* m_hwnd = nullptr;
		void* m_parent = nullptr;
#endif
		bool m_clickThrough = false;

		// Current image cache
		struct ImageEntry
		{
#ifdef _WIN32
			std::unique_ptr<Gdiplus::Bitmap> bitmap;
#else
			std::unique_ptr<void> bitmap;
#endif
			int x = 0;
			int y = 0;
			int width = 0;
			int height = 0;
		};
		std::vector<ImageEntry> m_images;

		int m_imageWidth = 0;
		int m_imageHeight = 0;
		std::wstring m_imagePath;

		// Previous image cache
#ifdef _WIN32
		HBITMAP m_cachedHBitmap = nullptr;
#else
		void* m_cachedHBitmap = nullptr;
#endif
		int m_cachedWidth = 0;
		int m_cachedHeight = 0;
		float m_cachedOpacity = 1.0f;

		// The last draw parameters
		int m_lastX = 0;
		int m_lastY = 0;
		int m_lastWidth = 0;
		int m_lastHeight = 0;
		float m_lastOpacity = 1.0f;

		std::vector<TextEntry> m_textEntries;
		bool m_needsRedraw = true;

		// Threading
		std::thread m_thread;
		std::queue<Command> m_queue;
		std::mutex m_queueMutex;
		std::condition_variable m_cv;
		std::atomic<bool> m_running{ false };

		bool m_trackingEnabled = false;
		std::thread m_trackingThread;
	};
}