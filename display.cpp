#include "display.h"

#include <vector> 
#include <limits> 

using namespace std;

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(LINUX)
    #define GLFW_EXPOSE_NATIVE_X11
    #include "GLFW/glfw3native.h"
    #include <X11/Xatom.h>
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	#define GLFW_EXPOSE_NATIVE_WIN32
	#define GLFW_EXPOSE_NATIVE_WGL
	#include <GLFW/glfw3native.h>
#endif

/*---------------------------------------------------------------------------*/

MyDisplay::MyDisplay(int screen_width, int screen_height, bool fullscreen, bool vsync)
{
	
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    // GLFW Hints
    if(true) {
        glfwDefaultWindowHints();

        //glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

        if(fullscreen) {
            glfwWindowHint(GLFW_DECORATED, GL_FALSE);
        }
        else {
            glfwWindowHint(GLFW_DECORATED, GL_TRUE);
        }

        //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_FLOATING, GL_FALSE); // top most
        glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    }

	if(true) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	}

    // check monitors
    this->monitors = glfwGetMonitors(&this->nbMonitor);

    cout << "Found " << this->nbMonitor << " monitor(s)" << endl;

    vector<const GLFWvidmode*> vmodes;

    for (auto i = 0; i < this->nbMonitor; i++ ) {
        const GLFWvidmode* vmode = glfwGetVideoMode(this->monitors[i]);
        const char* mname = glfwGetMonitorName(this->monitors[i]);

        vmodes.push_back(vmode);

        cout << "Video mode for monitor " << i << " (" << mname << ") " << vmode->width << "x" << vmode->height << " refreshRate=" << vmode->refreshRate << endl;
    }

	glfwWindowHint(GLFW_RED_BITS, vmodes[0]->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, vmodes[0]->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, vmodes[0]->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, vmodes[0]->refreshRate);

	// window
	if(!fullscreen) {
		this->screen_width = screen_width;
		this->screen_height = screen_height;
	}
	else {
		if(this->nbMonitor > 1) {
			this->screen_width = vmodes[0]->width + vmodes[1]->width;
			this->screen_height = vmodes[0]->height;
		}
		else {
			this->screen_width = vmodes[0]->width;
			this->screen_height = vmodes[0]->height;
		}
	}

	mainWindow = glfwCreateWindow(this->screen_width, this->screen_height, "", NULL, NULL);

    if (!mainWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // X11 native fullscreen via XEvent / XChangeProperty
    #if defined(linux) || defined(__linux) || defined(__linux__) || defined(LINUX)
        if(true && fullscreen) {
            this->SetNativeFullscreen(fullscreen);
        }
    #elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
        if(true && fullscreen) {
            this->SetNativeFullscreenWindows(fullscreen);
        }  
    #endif

	// screen size
	int actual_screen_width, actual_screen_height;
    glfwGetFramebufferSize(mainWindow, &actual_screen_width, &actual_screen_height);
    cout << "Frame buffer size " << actual_screen_width << "x" << actual_screen_height << endl;

    glfwMakeContextCurrent(mainWindow);
    
    glfwSetWindowPos(mainWindow, 0, 0);
    glfwFocusWindow(mainWindow);

    // string version
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);

    cout << "GPU: " << renderer << endl;
    cout << "OpenGL Version: " << version << endl;

	if(vsync) {
		glfwSwapInterval(1);
	}
	else {
		glfwSwapInterval(0);
	}

	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLEW
    glewExperimental = GL_TRUE;
	
    auto init_res = glewInit();
    if(init_res != GLEW_OK)
    {
        std::cout << glewGetErrorString(glewInit()) << std::endl;
    }

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_POINT_SPRITE);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glEnable(GL_RASTERIZER_DISCARD);
	

}

/*---------------------------------------------------------------------------*/

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)

void MyDisplay::SetNativeFullscreenWindows(bool fullscreen)
{
    HWND hwnd = glfwGetWin32Window(mainWindow);

    SetWindowLong(hwnd, GWL_EXSTYLE, 0);
    SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    float fullscreenW = getScreenSize().x;
    float fullscreenH = getScreenSize().y;

    int xpos = 0;
    int ypos = 0;

    if( this->nbMonitor > 1 ){

        float totalWidth = 0.0;
        float maxHeight  = 0.0;
        int monitorCount;
        GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

        //lets find the total width of all the monitors
        //and we'll make the window height the height of the largest monitor.
        for(int i = 0; i < monitorCount; i++){
            const GLFWvidmode * desktopMode = glfwGetVideoMode(monitors[i]);
            totalWidth += desktopMode->width;
            if( i == 0 || desktopMode->height > maxHeight ){
                maxHeight = desktopMode->height;
            }
        }

        fullscreenW = totalWidth;
        fullscreenH = maxHeight;
    }else{

        int monitorCount;
        GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
        int currentMonitor = getCurrentMonitor();
        glfwGetMonitorPos(monitors[currentMonitor], &xpos, &ypos);
    }

    SetWindowPos(hwnd, HWND_TOPMOST, xpos, ypos, fullscreenW, fullscreenH, SWP_SHOWWINDOW);
}
#endif

/*---------------------------------------------------------------------------*/
// see https://github.com/charlesveasey/ofxMultiGLFWWindow/blob/master/src/ofxMultiGLFWWindow.cpp

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(LINUX)

void MyDisplay::SetNativeFullscreen(bool fullscreen)
{
    Window nativeWin = glfwGetX11Window(mainWindow);
    Display* display = glfwGetX11Display();

    if(this->nbMonitor > 1)
    {
        // find the monitors at the edges of the virtual desktop
        int minx = numeric_limits<int>::max();
        int miny = numeric_limits<int>::max();
        int maxx = numeric_limits<int>::min();
        int maxy = numeric_limits<int>::min();
        int x,y,w,h;
        int monitorLeft=0, monitorRight=0, monitorTop=0, monitorBottom=0;

        for(int i = 0; i < this->nbMonitor; i++)
        {
            glfwGetMonitorPos(this->monitors[i], &x, &y);
            glfwGetMonitorPhysicalSize(this->monitors[i], &w, &h);

            if(x<minx){
                monitorLeft = i;
                minx = x;
            }
            if(y<miny){
                monitorTop = i;
                miny = y;
            }
            if(x+w>maxx){
                monitorRight = i;
                maxx = x+w;
            }
            if(y+h>maxy){
                monitorBottom = i;
                maxy = y+h;
            }
        }

        // send fullscreen_monitors event with the edges monitors
        Atom m_net_fullscreen_monitors= XInternAtom(display, "_NET_WM_FULLSCREEN_MONITORS", false);

        XEvent xev;

        xev.xclient.type = ClientMessage;
        xev.xclient.serial = 0;
        xev.xclient.send_event = True;
        xev.xclient.window = nativeWin;
        xev.xclient.message_type = m_net_fullscreen_monitors;
        xev.xclient.format = 32;

        xev.xclient.data.l[0] = monitorTop;
        xev.xclient.data.l[1] = monitorBottom;
        xev.xclient.data.l[2] = monitorLeft;
        xev.xclient.data.l[3] = monitorRight;
        xev.xclient.data.l[4] = 1;
        XSendEvent(display, RootWindow(display, DefaultScreen(display)), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    }

    // send fullscreen event
    Atom m_net_state= XInternAtom(display, "_NET_WM_STATE", false);
    Atom m_net_fullscreen= XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", false);

    XEvent xev;

    xev.xclient.type = ClientMessage;
    xev.xclient.serial = 0;
    xev.xclient.send_event = True;
    xev.xclient.window = nativeWin;
    xev.xclient.message_type = m_net_state;
    xev.xclient.format = 32;

    if (fullscreen)
        xev.xclient.data.l[0] = 1;
    else
        xev.xclient.data.l[0] = 0;

    xev.xclient.data.l[1] = m_net_fullscreen;
    xev.xclient.data.l[2] = 0;
    xev.xclient.data.l[3] = 0;
    xev.xclient.data.l[4] = 0;

    XSendEvent(display, RootWindow(display, DefaultScreen(display)), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);

    // tell the window manager to bypass composition for this window in fullscreen for speed it'll probably help solving vsync issues
    Atom m_bypass_compositor = XInternAtom(display, "_NET_WM_BYPASS_COMPOSITOR", False);
    unsigned long value = fullscreen ? 1 : 0;
    XChangeProperty(display, nativeWin, m_bypass_compositor, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&value, 1);

    XFlush(display);
}
#endif

/*---------------------------------------------------------------------------*/

void MyDisplay::Clear(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/*---------------------------------------------------------------------------*/

void MyDisplay::SwapBuffers()
{
	glfwSwapBuffers(mainWindow);
}

/*---------------------------------------------------------------------------*/

MyDisplay::~MyDisplay()
{
    glfwDestroyWindow(mainWindow);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}



