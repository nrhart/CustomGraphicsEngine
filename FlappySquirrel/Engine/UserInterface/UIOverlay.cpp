#include "UIOverlay.h"
#include <chrono>

using namespace UIOverlay;

Overlay::Overlay() {}
Overlay::~Overlay()
{
    Destroy();
}

#ifdef _WIN32

bool Overlay::CreateForWindow(HWND parent, bool clickThrough)

#else
bool Overlay::CreateForWindow(void* parent, bool clickThrough)
#endif
{
    Command cmd;
    cmd.type = Command::CREATE_WINDOW;
    cmd.a = reinterpret_cast<intptr_t>(parent);
    cmd.flag = clickThrough;

    Enqueue(cmd);
    return true;
}

void Overlay::Destroy()
{
    if (!m_running.exchange(false))
        return;

    {
        Command cmd;
        cmd.type = Command::DESTROY;
        Enqueue(cmd);
    }
    if (m_cachedHBitmap)
    {
        DeleteObject(m_cachedHBitmap);
        m_cachedHBitmap = nullptr;
    }

    m_cv.notify_all();
    m_trackingEnabled = false;
    if (m_trackingThread.joinable())
        m_trackingThread.join();

    m_running = false;

    if (m_thread.joinable())
        m_thread.join();

}

void Overlay::Enqueue(const Command& cmd)
{
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_queue.push(cmd);
    }
    m_cv.notify_one();
}

bool Overlay::AddPNG(const std::wstring& path, int x, int y, int w, int h)
{
    Command cmd;
    cmd.type = Command::ADD_PNG;
    cmd.str1 = path;
    cmd.a = x; cmd.b = y; cmd.c = w; cmd.d = h;
    Enqueue(cmd);
    return true;
}

void Overlay::ClearPNGs()
{
    Command cmd;
    cmd.type = Command::CLEAR_PNG;
    Enqueue(cmd);
}

bool Overlay::AddText(const std::wstring& t, int x, int y,
    const std::wstring& font,
    int size, unsigned int argb, bool bold)
{
    Command cmd;
    cmd.type = Command::ADD_TEXT;
    cmd.str1 = t;
    cmd.str2 = font;
    cmd.a = x; cmd.b = y;
    cmd.c = size;
    cmd.d = argb;
    cmd.flag = bold;
    Enqueue(cmd);
    return true;
}

void Overlay::ClearText()
{
    Command cmd;
    cmd.type = Command::CLEAR_TEXT;
    Enqueue(cmd);
}

bool Overlay::Show(int x, int y, int w, int h, float opacity)
{
    Command cmd;
    cmd.type = Command::SHOW;
    cmd.a = x; cmd.b = y; cmd.c = w; cmd.d = h;
    cmd.f = opacity;
    Enqueue(cmd);
    return true;
}

bool Overlay::UpdatePosition(int x, int y, int w, int h, float opacity)
{
    Command cmd;
    cmd.type = Command::UPDATE_POSITION;
    cmd.a = x; cmd.b = y; cmd.c = w; cmd.d = h;
    cmd.f = opacity;
    Enqueue(cmd);
    return true;
}

bool UIOverlay::Overlay::Redraw(int x, int y, int width, int height, float opacity)
{
	Backend_Redraw(x, y, width, height, opacity);
    return true;
}

void Overlay::Hide()
{
    Command cmd;
    cmd.type = Command::HIDE;
    Enqueue(cmd);
}

void Overlay::SetClickThrough(bool enable)
{
    Command cmd;
    cmd.type = Command::SET_CLICKTHROUGH;
    cmd.flag = enable;
    Enqueue(cmd);
}

void Overlay::Backend_ClearPNGs()
{
    if (!m_images.empty())
    {
        m_images.clear();
        m_needsRedraw = true;
    }
}

bool Overlay::Backend_Show(int x, int y, int width, int height, float opacity)
{
    if (!m_hwnd || width <= 0 || height <= 0)
        return false;

    {
        m_lastX = x;
        m_lastY = y;
        m_lastWidth = width;
        m_lastHeight = height;
        m_lastOpacity = opacity;
        m_needsRedraw = true;
    }

    // Redraw the content (image + text)
    return Redraw(x, y, width, height, opacity);
}

bool Overlay::Backend_AddText(const std::wstring& text, int x, int y, const std::wstring& fontName, int fontSize, unsigned int argbColor, bool bold)
{
    TextEntry t;
    t.text = text;
    t.x = x;
    t.y = y;
    t.fontName = fontName;
    t.fontSize = fontSize;
    t.argbColor = argbColor;
    t.bold = bold;
    {
        m_textEntries.push_back(t);
        m_needsRedraw = true;
    }
    if (m_hwnd && m_lastWidth > 0 && m_lastHeight > 0)
    {
        return Redraw(m_lastX, m_lastY, m_lastWidth, m_lastHeight, m_lastOpacity);
    }
    return true;
}

void Overlay::Backend_ClearText()
{
    m_textEntries.clear();
    m_needsRedraw = true;
    if (m_hwnd && m_lastWidth > 0 && m_lastHeight > 0)
    {
        Redraw(m_lastX, m_lastY, m_lastWidth, m_lastHeight, m_lastOpacity);
    }
}

void Overlay::Backend_Destroy()
{
    m_trackingEnabled = false;

    if (m_hwnd && IsWindow(m_hwnd))
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }

    m_parent = nullptr;

    m_images.clear();
    m_textEntries.clear();
    m_imagePath.clear();
    m_imageWidth = m_imageHeight = 0;

    m_lastX = m_lastY = 0;
    m_lastWidth = m_lastHeight = 0;
    m_lastOpacity = 1.0f;
    m_needsRedraw = false;

    if (m_cachedHBitmap)
    {
        DeleteObject(m_cachedHBitmap);
        m_cachedHBitmap = nullptr;
    }
    m_cachedWidth = 0;
    m_cachedHeight = 0;
    m_cachedOpacity = 1.0f;
}
