// dear imgui: Platform Binding for Windows (standard windows API for 32 and 64 bits applications)
// This needs to be used along with a Renderer (e.g. DirectX11, OpenGL3, Vulkan..)

// Implemented features:
//  [X] Platform: Clipboard support (for Win32 this is actually part of core dear imgui)
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'.
//  [X] Platform: Keyboard arrays indexed using VK_* Virtual Key Codes, e.g. ImGui::IsKeyPressed(VK_SPACE).
//  [X] Platform: Gamepad support. Enabled with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.
//  [X] Platform: Multi-viewport support (multiple windows). Enable with 'io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable'.

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <tchar.h>

// Using XInput library for gamepad (with recent Windows SDK this may leads to executables which won't run on Windows 7)
#ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
#include <XInput.h>
#else
#define IMGUI_IMPL_WIN32_DISABLE_LINKING_XINPUT
#endif
#if defined(_MSC_VER) && !defined(IMGUI_IMPL_WIN32_DISABLE_LINKING_XINPUT)
#pragma comment(lib, "xinput")
//#pragma comment(lib, "Xinput9_1_0")
#endif

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2020-XX-XX: Platform: Added support for multiple windows via the ImGuiPlatformIO interface.
//  2020-03-03: Inputs: Calling AddInputCharacterUTF16() to support surrogate pairs leading to codepoint >= 0x10000 (for more complete CJK inputs)
//  2020-02-17: Added ImGui_ImplWin32_EnableDpiAwareness(), ImGui_ImplWin32_GetDpiScaleForHwnd(), ImGui_ImplWin32_GetDpiScaleForMonitor() helper functions.
//  2020-01-14: Inputs: Added support for #define IMGUI_IMPL_WIN32_DISABLE_GAMEPAD/IMGUI_IMPL_WIN32_DISABLE_LINKING_XINPUT.
//  2019-12-05: Inputs: Added support for ImGuiMouseCursor_NotAllowed mouse cursor.
//  2019-05-11: Inputs: Don't filter value from WM_CHAR before calling AddInputCharacter().
//  2019-01-17: Misc: Using GetForegroundWindow()+IsChild() instead of GetActiveWindow() to be compatible with windows created in a different thread or parent.
//  2019-01-17: Inputs: Added support for mouse buttons 4 and 5 via WM_XBUTTON* messages.
//  2019-01-15: Inputs: Added support for XInput gamepads (if ImGuiConfigFlags_NavEnableGamepad is set by user application).
//  2018-11-30: Misc: Setting up io.BackendPlatformName so it can be displayed in the About Window.
//  2018-06-29: Inputs: Added support for the ImGuiMouseCursor_Hand cursor.
//  2018-06-10: Inputs: Fixed handling of mouse wheel messages to support fine position messages (typically sent by track-pads).
//  2018-06-08: Misc: Extracted imgui_impl_win32.cpp/.h away from the old combined DX9/DX10/DX11/DX12 examples.
//  2018-03-20: Misc: Setup io.BackendFlags ImGuiBackendFlags_HasMouseCursors and ImGuiBackendFlags_HasSetMousePos flags + honor ImGuiConfigFlags_NoMouseCursorChange flag.
//  2018-02-20: Inputs: Added support for mouse cursors (ImGui::GetMouseCursor() value and WM_SETCURSOR message handling).
//  2018-02-06: Inputs: Added mapping for ImGuiKey_Space.
//  2018-02-06: Inputs: Honoring the io.WantSetMousePos by repositioning the mouse (when using navigation and ImGuiConfigFlags_NavMoveMouse is set).
//  2018-02-06: Misc: Removed call to ImGui::Shutdown() which is not available from 1.60 WIP, user needs to call CreateContext/DestroyContext themselves.
//  2018-01-20: Inputs: Added Horizontal Mouse Wheel support.
//  2018-01-08: Inputs: Added mapping for ImGuiKey_Insert.
//  2018-01-05: Inputs: Added WM_LBUTTONDBLCLK double-click handlers for window classes with the CS_DBLCLKS flag.
//  2017-10-23: Inputs: Added WM_SYSKEYDOWN / WM_SYSKEYUP handlers so e.g. the VK_MENU key can be read.
//  2017-10-23: Inputs: Using Win32 ::SetCapture/::GetCapture() to retrieve mouse positions outside the client area when dragging.
//  2016-11-12: Inputs: Only call Win32 ::SetCursor(NULL) when io.MouseDrawCursor is set.

// Multi-Context
// - ImGui_ImplWin32_Init
// - ImGui_ImplWin32_InitPlatformInterface

// Win32 Data
static HWND                 g_hWnd = NULL;
static INT64                g_Time = 0;
static INT64                g_TicksPerSecond = 0;
static ImGuiMouseCursor     g_LastFrameMouseCursor = ImGuiMouseCursor_COUNT;
static ImGuiMouseCursor     g_CurrentMouseCursor = ImGuiMouseCursor_COUNT;
static bool                 g_HasGamepad = false;
static bool                 g_WantUpdateHasGamepad = true;
static bool                 g_WantUpdateMonitors = true;

// Forward Declarations
static void ImGui_ImplWin32_InitPlatformInterface_Shared();
static void ImGui_ImplWin32_InitPlatformInterface_Context(ImGuiContext* ctx);
static void ImGui_ImplWin32_ShutdownPlatformInterface();
static void ImGui_ImplWin32_UpdateMonitors(ImGuiContext* ctx);

// Functions
bool ImGui_ImplWin32_Init_Shared(void* hwnd)
{
    if (!::QueryPerformanceFrequency((LARGE_INTEGER*)&g_TicksPerSecond))
        return false;
    if (!::QueryPerformanceCounter((LARGE_INTEGER*)&g_Time))
        return false;

    g_hWnd = (HWND)hwnd;

	ImGui_ImplWin32_InitPlatformInterface_Shared();
    
    return true;
}

bool ImGui_ImplWin32_Init_Context(ImGuiContext* ctx) {
    if (!g_hWnd)
        return false;

	// Setup back-end capabilities flags
    ImGuiIO& io = ctx->IO;// ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can set io.MouseHoveredViewport correctly (optional, not easy)
	io.BackendPlatformName = "imgui_impl_win32";

    ImGuiViewport* main_viewport = ctx->Viewports[0];// ImGui::GetMainViewport();
	main_viewport->PlatformHandle = main_viewport->PlatformHandleRaw = (void*)g_hWnd;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		ImGui_ImplWin32_InitPlatformInterface_Context(ctx);

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
	io.KeyMap[ImGuiKey_Tab] = VK_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	io.KeyMap[ImGuiKey_Home] = VK_HOME;
	io.KeyMap[ImGuiKey_End] = VK_END;
	io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
	io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	io.KeyMap[ImGuiKey_Space] = VK_SPACE;
	io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	io.KeyMap[ImGuiKey_KeyPadEnter] = VK_RETURN;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

    return true;
}

void ImGui_ImplWin32_Shutdown_Shared()
{
	ImGui_ImplWin32_ShutdownPlatformInterface();
    g_hWnd = (HWND)0;
}

void ImGui_ImplWin32_Shutdown_Context(ImGuiContext* ctx) {
	ImGuiViewport* main_viewport = ctx->Viewports[0];// ImGui::GetMainViewport();
	main_viewport->PlatformHandle = main_viewport->PlatformHandleRaw = (void*)0;
}

static bool ImGui_ImplWin32_UpdateMouseCursor(ImGuiMouseCursor imgui_cursor)
{
	if (imgui_cursor == ImGuiMouseCursor_None) {
		// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
		::SetCursor(NULL);
	}
	else
	{
		// Show OS mouse cursor
		LPTSTR win32_cursor = IDC_ARROW;
		switch (imgui_cursor)
		{
		case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
		case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
		case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
		case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
		case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
		case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
		case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
		case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
		case ImGuiMouseCursor_NotAllowed:   win32_cursor = IDC_NO; break;
		}
		::SetCursor(::LoadCursor(NULL, win32_cursor));
	}
	return true;
}
static bool ImGui_ImplWin32_UpdateMouseCursor(ImGuiContext* ctx)
{
    ImGuiIO& io = ctx->IO;// ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return false;

    ImGuiMouseCursor imgui_cursor = ctx->MouseCursor;// ImGui::GetMouseCursor();
    if (io.MouseDrawCursor) {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        ::SetCursor(NULL);
    }
    else
        return ImGui_ImplWin32_UpdateMouseCursor(imgui_cursor);
    return true;
}

// This code supports multi-viewports (multiple OS Windows mapped into different Dear ImGui viewports)
// Because of that, it is a little more complicated than your typical single-viewport binding code!
static void ImGui_ImplWin32_UpdateMousePos(ImGuiContext* ctx, ImVec2 offset)
{
    ImGuiIO& io = ctx->IO;// ImGui::GetIO();

	// Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
	// (When multi-viewports are enabled, all imgui positions are same as OS positions)
    if (io.WantSetMousePos)
    {
        POINT pos = { (int)(offset.x + io.MousePos.x), (int)(offset.y + io.MousePos.y) };
        ::ClientToScreen(g_hWnd, &pos);
        ::SetCursorPos(pos.x, pos.y);
	}

	io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	io.MouseHoveredViewport = 0;

	// Set imgui mouse position
	POINT mouse_screen_pos;
	if (!::GetCursorPos(&mouse_screen_pos))
		return;
	if (HWND focused_hwnd = ::GetForegroundWindow())
	{
		if (::IsChild(focused_hwnd, g_hWnd))
			focused_hwnd = g_hWnd;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			// Multi-viewport mode: mouse position in OS absolute coordinates (io.MousePos is (0,0) when the mouse is on the upper-left of the primary monitor)
			// This is the position you can get with GetCursorPos(). In theory adding viewport->Pos is also the reverse operation of doing ScreenToClient().
			if (ImGui::FindViewportByPlatformHandle((void*)focused_hwnd) != NULL)
				io.MousePos = ImVec2((float)mouse_screen_pos.x - offset.x, (float)mouse_screen_pos.y - offset.y);
		}
		else
		{
			// Single viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window.)
			// This is the position you can get with GetCursorPos() + ScreenToClient() or from WM_MOUSEMOVE.
			if (focused_hwnd == g_hWnd)
			{
				POINT mouse_client_pos = mouse_screen_pos;
				::ScreenToClient(focused_hwnd, &mouse_client_pos);
				io.MousePos = ImVec2((float)mouse_client_pos.x - offset.x, (float)mouse_client_pos.y - offset.y);
			}
		}
	}

	// (Optional) When using multiple viewports: set io.MouseHoveredViewport to the viewport the OS mouse cursor is hovering.
	// Important: this information is not easy to provide and many high-level windowing library won't be able to provide it correctly, because
	// - This is _ignoring_ viewports with the ImGuiViewportFlags_NoInputs flag (pass-through windows).
	// - This is _regardless_ of whether another viewport is focused or being dragged from.
	// If ImGuiBackendFlags_HasMouseHoveredViewport is not set by the back-end, imgui will ignore this field and infer the information by relying on the
	// rectangles and last focused time of every viewports it knows about. It will be unaware of foreign windows that may be sitting between or over your windows.
	if (HWND hovered_hwnd = ::WindowFromPoint(mouse_screen_pos))
		if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle((void*)hovered_hwnd))
			if ((viewport->Flags & ImGuiViewportFlags_NoInputs) == 0) // FIXME: We still get our NoInputs window with WM_NCHITTEST/HTTRANSPARENT code when decorated?
				io.MouseHoveredViewport = viewport->ID;
}

// Gamepad navigation mapping
static void ImGui_ImplWin32_UpdateGamepads(ImGuiContext* ctx)
{
#ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
    ImGuiIO& io = ctx->IO;// ImGui::GetIO();
    memset(io.NavInputs, 0, sizeof(io.NavInputs));
    if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
        return;

    // Calling XInputGetState() every frame on disconnected gamepads is unfortunately too slow.
    // Instead we refresh gamepad availability by calling XInputGetCapabilities() _only_ after receiving WM_DEVICECHANGE.
    if (g_WantUpdateHasGamepad)
    {
        XINPUT_CAPABILITIES caps;
		g_HasGamepad = (XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS);
		// set false in EndFrame
		//g_WantUpdateHasGamepad = false;
    }

    XINPUT_STATE xinput_state;
    io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
    if (g_HasGamepad && XInputGetState(0, &xinput_state) == ERROR_SUCCESS)
    {
        const XINPUT_GAMEPAD& gamepad = xinput_state.Gamepad;
        io.BackendFlags |= ImGuiBackendFlags_HasGamepad;

        #define MAP_BUTTON(NAV_NO, BUTTON_ENUM)     { io.NavInputs[NAV_NO] = (gamepad.wButtons & BUTTON_ENUM) ? 1.0f : 0.0f; }
        #define MAP_ANALOG(NAV_NO, VALUE, V0, V1)   { float vn = (float)(VALUE - V0) / (float)(V1 - V0); if (vn > 1.0f) vn = 1.0f; if (vn > 0.0f && io.NavInputs[NAV_NO] < vn) io.NavInputs[NAV_NO] = vn; }
        MAP_BUTTON(ImGuiNavInput_Activate,      XINPUT_GAMEPAD_A);              // Cross / A
        MAP_BUTTON(ImGuiNavInput_Cancel,        XINPUT_GAMEPAD_B);              // Circle / B
        MAP_BUTTON(ImGuiNavInput_Menu,          XINPUT_GAMEPAD_X);              // Square / X
        MAP_BUTTON(ImGuiNavInput_Input,         XINPUT_GAMEPAD_Y);              // Triangle / Y
        MAP_BUTTON(ImGuiNavInput_DpadLeft,      XINPUT_GAMEPAD_DPAD_LEFT);      // D-Pad Left
        MAP_BUTTON(ImGuiNavInput_DpadRight,     XINPUT_GAMEPAD_DPAD_RIGHT);     // D-Pad Right
        MAP_BUTTON(ImGuiNavInput_DpadUp,        XINPUT_GAMEPAD_DPAD_UP);        // D-Pad Up
        MAP_BUTTON(ImGuiNavInput_DpadDown,      XINPUT_GAMEPAD_DPAD_DOWN);      // D-Pad Down
        MAP_BUTTON(ImGuiNavInput_FocusPrev,     XINPUT_GAMEPAD_LEFT_SHOULDER);  // L1 / LB
        MAP_BUTTON(ImGuiNavInput_FocusNext,     XINPUT_GAMEPAD_RIGHT_SHOULDER); // R1 / RB
        MAP_BUTTON(ImGuiNavInput_TweakSlow,     XINPUT_GAMEPAD_LEFT_SHOULDER);  // L1 / LB
        MAP_BUTTON(ImGuiNavInput_TweakFast,     XINPUT_GAMEPAD_RIGHT_SHOULDER); // R1 / RB
        MAP_ANALOG(ImGuiNavInput_LStickLeft,    gamepad.sThumbLX,  -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32768);
        MAP_ANALOG(ImGuiNavInput_LStickRight,   gamepad.sThumbLX,  +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
        MAP_ANALOG(ImGuiNavInput_LStickUp,      gamepad.sThumbLY,  +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
        MAP_ANALOG(ImGuiNavInput_LStickDown,    gamepad.sThumbLY,  -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32767);
        #undef MAP_BUTTON
        #undef MAP_ANALOG
    }
#endif // #ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
}

static BOOL CALLBACK ImGui_ImplWin32_UpdateMonitors_EnumFunc(HMONITOR monitor, HDC, LPRECT, LPARAM)
{
	MONITORINFO info = { 0 };
	info.cbSize = sizeof(MONITORINFO);
	if (!::GetMonitorInfo(monitor, &info))
		return TRUE;
	ImGuiPlatformMonitor imgui_monitor;
	imgui_monitor.MainPos = ImVec2((float)info.rcMonitor.left, (float)info.rcMonitor.top);
	imgui_monitor.MainSize = ImVec2((float)(info.rcMonitor.right - info.rcMonitor.left), (float)(info.rcMonitor.bottom - info.rcMonitor.top));
	imgui_monitor.WorkPos = ImVec2((float)info.rcWork.left, (float)info.rcWork.top);
	imgui_monitor.WorkSize = ImVec2((float)(info.rcWork.right - info.rcWork.left), (float)(info.rcWork.bottom - info.rcWork.top));
	imgui_monitor.DpiScale = ImGui_ImplWin32_GetDpiScaleForMonitor(monitor);
	ImGuiPlatformIO& io = ImGui::GetPlatformIO();
	if (info.dwFlags & MONITORINFOF_PRIMARY)
		io.Monitors.push_front(imgui_monitor);
	else
		io.Monitors.push_back(imgui_monitor);
	return TRUE;
}

static void ImGui_ImplWin32_UpdateMonitors(ImGuiContext* ctx)
{
	auto orig_ctx = ImGui::GetCurrentContext();
	ImGui::SetCurrentContext(ctx);
	ImGui::GetPlatformIO().Monitors.resize(0);
	::EnumDisplayMonitors(NULL, NULL, ImGui_ImplWin32_UpdateMonitors_EnumFunc, NULL);
	// set false in EndFrame
	//g_WantUpdateMonitors = false;
	ImGui::SetCurrentContext(orig_ctx);
}

void ImGui_ImplWin32_NewFrame_Context(ImGuiContext* ctx, ImVec2 offset, float width, float height)
{
    ImGuiIO& io = ctx->IO; // ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

	io.DisplaySize = ImVec2(width, height);
	if (g_WantUpdateMonitors)
		ImGui_ImplWin32_UpdateMonitors(ctx);

    // Setup time step
    INT64 current_time;
    ::QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
    io.DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;

    // Read keyboard modifiers inputs
    io.KeyCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
    io.KeyShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
    io.KeyAlt = (::GetKeyState(VK_MENU) & 0x8000) != 0;
    io.KeySuper = false;
    // io.KeysDown[], io.MousePos, io.MouseDown[], io.MouseWheel: filled by the WndProc handler below.

    // Update OS mouse position
    ImGui_ImplWin32_UpdateMousePos(ctx, offset);

	if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0 && !io.MouseDrawCursor) {
		// Update OS mouse cursor with the cursor requested by imgui
		ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ctx->MouseCursor; // ImGui::GetMouseCursor();
		if (g_CurrentMouseCursor == ImGuiMouseCursor_COUNT
			|| g_CurrentMouseCursor != ImGuiMouseCursor_COUNT && mouse_cursor != g_CurrentMouseCursor && mouse_cursor != ImGuiMouseCursor_Arrow)
		{
			g_CurrentMouseCursor = mouse_cursor;
		}
    }

    // Update game controllers (if enabled and available)
    ImGui_ImplWin32_UpdateGamepads(ctx);
}

void ImGui_ImplWin32_NewFrame_Shared() {
    if (g_CurrentMouseCursor != ImGuiMouseCursor_COUNT) {
		g_LastFrameMouseCursor = g_CurrentMouseCursor;
		g_CurrentMouseCursor = ImGuiMouseCursor_COUNT;
		ImGui_ImplWin32_UpdateMouseCursor(g_LastFrameMouseCursor);
    }
	::QueryPerformanceCounter((LARGE_INTEGER*)&g_Time);
}

void ImGui_ImplWin32_EndFrame() {
	g_WantUpdateHasGamepad = false;
	g_WantUpdateMonitors = false;
}

// Allow compilation with old Windows SDK. MinGW doesn't have default _WIN32_WINNT/WINVER versions.
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020E
#endif
#ifndef DBT_DEVNODES_CHANGED
#define DBT_DEVNODES_CHANGED 0x0007
#endif

// Win32 message handler (process Win32 mouse/keyboard inputs, etc.)
// Call from your application's message handler.
// When implementing your own back-end, you can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if Dear ImGui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
// Generally you may always pass all inputs to Dear ImGui, and hide them from your application based on those two flags.
// PS: In this Win32 handler, we use the capture API (GetCapture/SetCapture/ReleaseCapture) to be able to read mouse coordinates when dragging mouse outside of our window bounds.
// PS: We treat DBLCLK messages as regular mouse down messages, so this code will work on windows classes that have the CS_DBLCLKS flag set. Our own example app code doesn't set this flag.
#if 0
// Copy this line into your .cpp file to forward declare the function.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler_Shared(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler_Context(ImGuiContext* ctx, bool ingore_mouse, bool ingore_keyboard, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler_Shared(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DEVICECHANGE:
        if ((UINT)wParam == DBT_DEVNODES_CHANGED)
            g_WantUpdateHasGamepad = true;
		return 0;
	case WM_DISPLAYCHANGE:
		g_WantUpdateMonitors = true;
		return 0;
    }
    return 0;
}

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler_Context(ImGuiContext* ctx, bool ingore_mouse, bool ingore_keyboard, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ImGuiIO& io = ctx->IO;// ImGui::GetIO();

    constexpr auto IsAnyMouseDown = [](ImGuiContext* ctx) {
        for (size_t n = 0; n < IM_ARRAYSIZE(ctx->IO.MouseDown); n++) {
            if (ctx->IO.MouseDown[n])
                return true;
        }
        return false;
    };

    switch (msg)
    {
    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
    {
        if (ingore_mouse)
            return 0;
        int button = 0;
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = 0; }
        if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = 1; }
        if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = 2; }
        if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
        if (!IsAnyMouseDown(ctx) && ::GetCapture() == NULL)
            ::SetCapture(hwnd);
        io.MouseDown[button] = true;
        return 0;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
	{
		if (ingore_mouse)
			return 0;
        int button = 0;
        if (msg == WM_LBUTTONUP) { button = 0; }
        if (msg == WM_RBUTTONUP) { button = 1; }
        if (msg == WM_MBUTTONUP) { button = 2; }
        if (msg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
        io.MouseDown[button] = false;
        if (!IsAnyMouseDown(ctx) && ::GetCapture() == hwnd)
            ::ReleaseCapture();
        return 0;
    }
	case WM_MOUSEWHEEL:
		if (ingore_mouse)
			return 0;
        io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
        return 0;
	case WM_MOUSEHWHEEL:
		if (ingore_mouse)
			return 0;
        io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
        return 0;
    case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (ingore_keyboard)
			return 0;
        if (wParam < 256)
            io.KeysDown[wParam] = 1;
        return 0;
    case WM_KEYUP:
	case WM_SYSKEYUP:
		if (ingore_keyboard)
			return 0;
        if (wParam < 256)
            io.KeysDown[wParam] = 0;
        return 0;
	case WM_CHAR:
		if (ingore_keyboard)
			return 0;
        // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
        if (wParam > 0 && wParam < 0x10000)
            io.AddInputCharacterUTF16((unsigned short)wParam);
        return 0;
    case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor(ctx))
            return 1;
        return 0;
    default:
        return 0;
    }
}

//--------------------------------------------------------------------------------------------------------
// DPI-related helpers (optional)
//--------------------------------------------------------------------------------------------------------
// - Use to enable DPI awareness without having to create an application manifest.
// - Your own app may already do this via a manifest or explicit calls. This is mostly useful for our examples/ apps.
// - In theory we could call simple functions from Windows SDK such as SetProcessDPIAware(), SetProcessDpiAwareness(), etc.
//   but most of the functions provided by Microsoft require Windows 8.1/10+ SDK at compile time and Windows 8/10+ at runtime,
//   neither we want to require the user to have. So we dynamically select and load those functions to avoid dependencies.
//---------------------------------------------------------------------------------------------------------
// This is the scheme successfully used by GLFW (from which we borrowed some of the code) and other apps aiming to be highly portable.
// ImGui_ImplWin32_EnableDpiAwareness() is just a helper called by main.cpp, we don't call it automatically.
// If you are trying to implement your own back-end for your own engine, you may ignore that noise.
//---------------------------------------------------------------------------------------------------------

// Implement some of the functions and types normally declared in recent Windows SDK.
#if !defined(_versionhelpers_H_INCLUDED_) && !defined(_INC_VERSIONHELPERS)
static BOOL IsWindowsVersionOrGreater(WORD major, WORD minor, WORD sp)
{
    OSVERSIONINFOEXW osvi = { sizeof(osvi), major, minor, 0, 0, { 0 }, sp, 0, 0, 0, 0 };
    DWORD mask = VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR;
    ULONGLONG cond = ::VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
    cond = ::VerSetConditionMask(cond, VER_MINORVERSION, VER_GREATER_EQUAL);
    cond = ::VerSetConditionMask(cond, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
    return ::VerifyVersionInfoW(&osvi, mask, cond);
}
#define IsWindows8Point1OrGreater()  IsWindowsVersionOrGreater(HIBYTE(0x0602), LOBYTE(0x0602), 0) // _WIN32_WINNT_WINBLUE
#endif

#ifndef DPI_ENUMS_DECLARED
typedef enum { PROCESS_DPI_UNAWARE = 0, PROCESS_SYSTEM_DPI_AWARE = 1, PROCESS_PER_MONITOR_DPI_AWARE = 2 } PROCESS_DPI_AWARENESS;
typedef enum { MDT_EFFECTIVE_DPI = 0, MDT_ANGULAR_DPI = 1, MDT_RAW_DPI = 2, MDT_DEFAULT = MDT_EFFECTIVE_DPI } MONITOR_DPI_TYPE;
#endif
#ifndef _DPI_AWARENESS_CONTEXTS_
DECLARE_HANDLE(DPI_AWARENESS_CONTEXT);
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE    (DPI_AWARENESS_CONTEXT)-3
#endif
#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 (DPI_AWARENESS_CONTEXT)-4
#endif
typedef HRESULT(WINAPI* PFN_SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);                     // Shcore.lib + dll, Windows 8.1+
typedef HRESULT(WINAPI* PFN_GetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);        // Shcore.lib + dll, Windows 8.1+
typedef DPI_AWARENESS_CONTEXT(WINAPI* PFN_SetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT); // User32.lib + dll, Windows 10 v1607+ (Creators Update)

// Helper function to enable DPI awareness without setting up a manifest
void ImGui_ImplWin32_EnableDpiAwareness()
{
    // if (IsWindows10OrGreater()) // This needs a manifest to succeed. Instead we try to grab the function pointer!
    {
        static HINSTANCE user32_dll = ::LoadLibraryA("user32.dll"); // Reference counted per-process
        if (PFN_SetThreadDpiAwarenessContext SetThreadDpiAwarenessContextFn = (PFN_SetThreadDpiAwarenessContext)::GetProcAddress(user32_dll, "SetThreadDpiAwarenessContext"))
        {
            SetThreadDpiAwarenessContextFn(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            return;
        }
    }
    if (IsWindows8Point1OrGreater())
    {
        static HINSTANCE shcore_dll = ::LoadLibraryA("shcore.dll"); // Reference counted per-process
        if (PFN_SetProcessDpiAwareness SetProcessDpiAwarenessFn = (PFN_SetProcessDpiAwareness)::GetProcAddress(shcore_dll, "SetProcessDpiAwareness"))
        {
            SetProcessDpiAwarenessFn(PROCESS_PER_MONITOR_DPI_AWARE);
            return;
        }
    }
#if _WIN32_WINNT >= 0x0600
    ::SetProcessDPIAware();
#endif
}

#if defined(_MSC_VER) && !defined(NOGDI)
#pragma comment(lib, "gdi32")   // Link with gdi32.lib for GetDeviceCaps()
#endif

float ImGui_ImplWin32_GetDpiScaleForMonitor(void* monitor)
{
    UINT xdpi = 96, ydpi = 96;
    static BOOL bIsWindows8Point1OrGreater = IsWindows8Point1OrGreater();
    if (bIsWindows8Point1OrGreater)
    {
        static HINSTANCE shcore_dll = ::LoadLibraryA("shcore.dll"); // Reference counted per-process
        if (PFN_GetDpiForMonitor GetDpiForMonitorFn = (PFN_GetDpiForMonitor)::GetProcAddress(shcore_dll, "GetDpiForMonitor"))
            GetDpiForMonitorFn((HMONITOR)monitor, MDT_EFFECTIVE_DPI, &xdpi, &ydpi);
    }
#ifndef NOGDI
    else
    {
        const HDC dc = ::GetDC(NULL);
        xdpi = ::GetDeviceCaps(dc, LOGPIXELSX);
        ydpi = ::GetDeviceCaps(dc, LOGPIXELSY);
        ::ReleaseDC(NULL, dc);
    }
#endif
    IM_ASSERT(xdpi == ydpi); // Please contact me if you hit this assert!
    return xdpi / 96.0f;
}

float ImGui_ImplWin32_GetDpiScaleForHwnd(void* hwnd)
{
    HMONITOR monitor = ::MonitorFromWindow((HWND)hwnd, MONITOR_DEFAULTTONEAREST);
    return ImGui_ImplWin32_GetDpiScaleForMonitor(monitor);
}

//---------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the back-end to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

// Helper structure we store in the void* RenderUserData field of each ImGuiViewport to easily retrieve our backend data.
struct ImGuiViewportDataWin32
{
	HWND    Hwnd;
	bool    HwndOwned;
	DWORD   DwStyle;
	DWORD   DwExStyle;

	ImGuiViewportDataWin32() { Hwnd = NULL; HwndOwned = false;  DwStyle = DwExStyle = 0; }
	~ImGuiViewportDataWin32() { IM_ASSERT(Hwnd == NULL); }
};

static void ImGui_ImplWin32_GetWin32StyleFromViewportFlags(ImGuiViewportFlags flags, DWORD* out_style, DWORD* out_ex_style)
{
	if (flags & ImGuiViewportFlags_NoDecoration)
		*out_style = WS_POPUP;
	else
		*out_style = WS_OVERLAPPEDWINDOW;

	if (flags & ImGuiViewportFlags_NoTaskBarIcon)
		*out_ex_style = WS_EX_TOOLWINDOW;
	else
		*out_ex_style = WS_EX_APPWINDOW;

	if (flags & ImGuiViewportFlags_TopMost)
		*out_ex_style |= WS_EX_TOPMOST;
}

static void ImGui_ImplWin32_CreateWindow(ImGuiViewport* viewport)
{
	ImGuiViewportDataWin32* data = IM_NEW(ImGuiViewportDataWin32)();
	viewport->PlatformUserData = data;

	// Select style and parent window
	ImGui_ImplWin32_GetWin32StyleFromViewportFlags(viewport->Flags, &data->DwStyle, &data->DwExStyle);
	HWND parent_window = NULL;
	if (viewport->ParentViewportId != 0)
		if (ImGuiViewport* parent_viewport = ImGui::FindViewportByID(viewport->ParentViewportId))
			parent_window = (HWND)parent_viewport->PlatformHandle;

	// Create window
	RECT rect = { (LONG)viewport->Pos.x, (LONG)viewport->Pos.y, (LONG)(viewport->Pos.x + viewport->Size.x), (LONG)(viewport->Pos.y + viewport->Size.y) };
	::AdjustWindowRectEx(&rect, data->DwStyle, FALSE, data->DwExStyle);
	data->Hwnd = ::CreateWindowEx(
		data->DwExStyle, _T("ImGui Platform"), _T("Untitled"), data->DwStyle,   // Style, class name, window name
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,    // Window area
		parent_window, NULL, ::GetModuleHandle(NULL), NULL);                    // Parent window, Menu, Instance, Param
	data->HwndOwned = true;
	viewport->PlatformRequestResize = false;
	viewport->PlatformHandle = viewport->PlatformHandleRaw = data->Hwnd;
}

static void ImGui_ImplWin32_DestroyWindow(ImGuiViewport* viewport)
{
	if (ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*)viewport->PlatformUserData)
	{
		if (::GetCapture() == data->Hwnd)
		{
			// Transfer capture so if we started dragging from a window that later disappears, we'll still receive the MOUSEUP event.
			::ReleaseCapture();
			::SetCapture(g_hWnd);
		}
		if (data->Hwnd && data->HwndOwned)
			::DestroyWindow(data->Hwnd);
		data->Hwnd = NULL;
		IM_DELETE(data);
	}
	viewport->PlatformUserData = viewport->PlatformHandle = NULL;
}

static void ImGui_ImplWin32_ShowWindow(ImGuiViewport* viewport)
{
	ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*)viewport->PlatformUserData;
	IM_ASSERT(data->Hwnd != 0);
	if (viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing)
		::ShowWindow(data->Hwnd, SW_SHOWNA);
	else
		::ShowWindow(data->Hwnd, SW_SHOW);
}

static void ImGui_ImplWin32_UpdateWindow(ImGuiViewport* viewport)
{
	// (Optional) Update Win32 style if it changed _after_ creation.
	// Generally they won't change unless configuration flags are changed, but advanced uses (such as manually rewriting viewport flags) make this useful.
	ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*)viewport->PlatformUserData;
	IM_ASSERT(data->Hwnd != 0);
	DWORD new_style;
	DWORD new_ex_style;
	ImGui_ImplWin32_GetWin32StyleFromViewportFlags(viewport->Flags, &new_style, &new_ex_style);

	// Only reapply the flags that have been changed from our point of view (as other flags are being modified by Windows)
	if (data->DwStyle != new_style || data->DwExStyle != new_ex_style)
	{
		data->DwStyle = new_style;
		data->DwExStyle = new_ex_style;
		::SetWindowLong(data->Hwnd, GWL_STYLE, data->DwStyle);
		::SetWindowLong(data->Hwnd, GWL_EXSTYLE, data->DwExStyle);
		RECT rect = { (LONG)viewport->Pos.x, (LONG)viewport->Pos.y, (LONG)(viewport->Pos.x + viewport->Size.x), (LONG)(viewport->Pos.y + viewport->Size.y) };
		::AdjustWindowRectEx(&rect, data->DwStyle, FALSE, data->DwExStyle); // Client to Screen
		::SetWindowPos(data->Hwnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		::ShowWindow(data->Hwnd, SW_SHOWNA); // This is necessary when we alter the style
		viewport->PlatformRequestMove = viewport->PlatformRequestResize = true;
	}
}

static ImVec2 ImGui_ImplWin32_GetWindowPos(ImGuiViewport* viewport)
{
	ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*)viewport->PlatformUserData;
	IM_ASSERT(data->Hwnd != 0);
	POINT pos = { 0, 0 };
	::ClientToScreen(data->Hwnd, &pos);
	return ImVec2((float)pos.x, (float)pos.y);
}

static void ImGui_ImplWin32_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
{
	ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*)viewport->PlatformUserData;
	IM_ASSERT(data->Hwnd != 0);
	RECT rect = { (LONG)pos.x, (LONG)pos.y, (LONG)pos.x, (LONG)pos.y };
	::AdjustWindowRectEx(&rect, data->DwStyle, FALSE, data->DwExStyle);
	::SetWindowPos(data->Hwnd, NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

static ImVec2 ImGui_ImplWin32_GetWindowSize(ImGuiViewport* viewport)
{
	ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*)viewport->PlatformUserData;
	IM_ASSERT(data->Hwnd != 0);
	RECT rect;
	::GetClientRect(data->Hwnd, &rect);
	return ImVec2(float(rect.right - rect.left), float(rect.bottom - rect.top));
}

static void ImGui_ImplWin32_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
	ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*)viewport->PlatformUserData;
	IM_ASSERT(data->Hwnd != 0);
	RECT rect = { 0, 0, (LONG)size.x, (LONG)size.y };
	::AdjustWindowRectEx(&rect, data->DwStyle, FALSE, data->DwExStyle); // Client to Screen
	::SetWindowPos(data->Hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

static void ImGui_ImplWin32_SetWindowFocus(ImGuiViewport* viewport)
{
	ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*)viewport->PlatformUserData;
	IM_ASSERT(data->Hwnd != 0);
	::BringWindowToTop(data->Hwnd);
	::SetForegroundWindow(data->Hwnd);
	::SetFocus(data->Hwnd);
}

static bool ImGui_ImplWin32_GetWindowFocus(ImGuiViewport* viewport)
{
	ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*)viewport->PlatformUserData;
	IM_ASSERT(data->Hwnd != 0);
	return ::GetForegroundWindow() == data->Hwnd;
}

static bool ImGui_ImplWin32_GetWindowMinimized(ImGuiViewport* viewport)
{
	ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*)viewport->PlatformUserData;
	IM_ASSERT(data->Hwnd != 0);
	return ::IsIconic(data->Hwnd) != 0;
}

static void ImGui_ImplWin32_SetWindowTitle(ImGuiViewport* viewport, const char* title)
{
	// ::SetWindowTextA() doesn't properly handle UTF-8 so we explicitely convert our string.
	ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*)viewport->PlatformUserData;
	IM_ASSERT(data->Hwnd != 0);
	int n = ::MultiByteToWideChar(CP_UTF8, 0, title, -1, NULL, 0);
	ImVector<wchar_t> title_w;
	title_w.resize(n);
	::MultiByteToWideChar(CP_UTF8, 0, title, -1, title_w.Data, n);
	::SetWindowTextW(data->Hwnd, title_w.Data);
}

static void ImGui_ImplWin32_SetWindowAlpha(ImGuiViewport* viewport, float alpha)
{
	ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*)viewport->PlatformUserData;
	IM_ASSERT(data->Hwnd != 0);
	IM_ASSERT(alpha >= 0.0f && alpha <= 1.0f);
	if (alpha < 1.0f)
	{
		DWORD style = ::GetWindowLongW(data->Hwnd, GWL_EXSTYLE) | WS_EX_LAYERED;
		::SetWindowLongW(data->Hwnd, GWL_EXSTYLE, style);
		::SetLayeredWindowAttributes(data->Hwnd, 0, (BYTE)(255 * alpha), LWA_ALPHA);
	}
	else
	{
		DWORD style = ::GetWindowLongW(data->Hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED;
		::SetWindowLongW(data->Hwnd, GWL_EXSTYLE, style);
	}
}

static float ImGui_ImplWin32_GetWindowDpiScale(ImGuiViewport* viewport)
{
	ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*)viewport->PlatformUserData;
	IM_ASSERT(data->Hwnd != 0);
	return ImGui_ImplWin32_GetDpiScaleForHwnd(data->Hwnd);
}

// FIXME-DPI: Testing DPI related ideas
static void ImGui_ImplWin32_OnChangedViewport(ImGuiViewport* viewport)
{
	(void)viewport;
#if 0
	ImGuiStyle default_style;
	//default_style.WindowPadding = ImVec2(0, 0);
	//default_style.WindowBorderSize = 0.0f;
	//default_style.ItemSpacing.y = 3.0f;
	//default_style.FramePadding = ImVec2(0, 0);
	default_style.ScaleAllSizes(viewport->DpiScale);
	ImGuiStyle& style = ImGui::GetStyle();
	style = default_style;
#endif
}

static LRESULT CALLBACK ImGui_ImplWin32_WndProcHandler_PlatformWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler_Shared(hWnd, msg, wParam, lParam))
		return true;

	if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle((void*)hWnd))
	{
		switch (msg)
		{
		case WM_CLOSE:
			viewport->PlatformRequestClose = true;
			return 0;
		case WM_MOVE:
			viewport->PlatformRequestMove = true;
			break;
		case WM_SIZE:
			viewport->PlatformRequestResize = true;
			break;
		case WM_MOUSEACTIVATE:
			if (viewport->Flags & ImGuiViewportFlags_NoFocusOnClick)
				return MA_NOACTIVATE;
			break;
		case WM_NCHITTEST:
			// Let mouse pass-through the window. This will allow the back-end to set io.MouseHoveredViewport properly (which is OPTIONAL).
			// The ImGuiViewportFlags_NoInputs flag is set while dragging a viewport, as want to detect the window behind the one we are dragging.
			// If you cannot easily access those viewport flags from your windowing/event code: you may manually synchronize its state e.g. in
			// your main loop after calling UpdatePlatformWindows(). Iterate all viewports/platform windows and pass the flag to your windowing system.
			if (viewport->Flags & ImGuiViewportFlags_NoInputs)
				return HTTRANSPARENT;
			break;
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

static void ImGui_ImplWin32_InitPlatformInterface_Shared() {
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = ImGui_ImplWin32_WndProcHandler_PlatformWindow;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = ::GetModuleHandle(NULL);
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("ImGui Platform");
	wcex.hIconSm = NULL;
	::RegisterClassEx(&wcex);
}

static void ImGui_ImplWin32_InitPlatformInterface_Context(ImGuiContext* ctx) {
	ImGui_ImplWin32_UpdateMonitors(ctx);

	// Register platform interface (will be coupled with a renderer interface)
	ImGuiPlatformIO& platform_io = ctx->PlatformIO;// ImGui::GetPlatformIO();
	platform_io.Platform_CreateWindow = ImGui_ImplWin32_CreateWindow;
	platform_io.Platform_DestroyWindow = ImGui_ImplWin32_DestroyWindow;
	platform_io.Platform_ShowWindow = ImGui_ImplWin32_ShowWindow;
	platform_io.Platform_SetWindowPos = ImGui_ImplWin32_SetWindowPos;
	platform_io.Platform_GetWindowPos = ImGui_ImplWin32_GetWindowPos;
	platform_io.Platform_SetWindowSize = ImGui_ImplWin32_SetWindowSize;
	platform_io.Platform_GetWindowSize = ImGui_ImplWin32_GetWindowSize;
	platform_io.Platform_SetWindowFocus = ImGui_ImplWin32_SetWindowFocus;
	platform_io.Platform_GetWindowFocus = ImGui_ImplWin32_GetWindowFocus;
	platform_io.Platform_GetWindowMinimized = ImGui_ImplWin32_GetWindowMinimized;
	platform_io.Platform_SetWindowTitle = ImGui_ImplWin32_SetWindowTitle;
	platform_io.Platform_SetWindowAlpha = ImGui_ImplWin32_SetWindowAlpha;
	platform_io.Platform_UpdateWindow = ImGui_ImplWin32_UpdateWindow;
	platform_io.Platform_GetWindowDpiScale = ImGui_ImplWin32_GetWindowDpiScale; // FIXME-DPI
	platform_io.Platform_OnChangedViewport = ImGui_ImplWin32_OnChangedViewport; // FIXME-DPI
#if HAS_WIN32_IME
	platform_io.Platform_SetImeInputPos = ImGui_ImplWin32_SetImeInputPos;
#endif

	// Register main window handle (which is owned by the main application, not by us)
	// This is mostly for simplicity and consistency, so that our code (e.g. mouse handling etc.) can use same logic for main and secondary viewports.
	ImGuiViewport* main_viewport = ctx->Viewports[0];// ImGui::GetMainViewport();
	ImGuiViewportDataWin32* data = IM_NEW(ImGuiViewportDataWin32)();
	data->Hwnd = g_hWnd;
	data->HwndOwned = false;
	main_viewport->PlatformUserData = data;
	main_viewport->PlatformHandle = (void*)g_hWnd;
}

static void ImGui_ImplWin32_ShutdownPlatformInterface()
{
	::UnregisterClass(_T("ImGui Platform"), ::GetModuleHandle(NULL));
}

//---------------------------------------------------------------------------------------------------------