#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <signal.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

int height = 3;  // default height
char *color = "green";  // default color

static volatile sig_atomic_t done = 0;

void sigterm_handler(int signum) {
    done = 1;
}

long getAtomValue(Display *d, const char *atomName) {
    Atom actual_type_return;
    int actual_format_return;
    unsigned long nitems_return;
    unsigned long bytes_after_return;
    unsigned char *prop_return = NULL;

    Atom atom = XInternAtom(d, atomName, True);

    if (atom == None) {
        fprintf(stderr, "No such atom\n");
        return -1;
    }

    if (XGetWindowProperty(d, RootWindow(d, DefaultScreen(d)), atom, 0, (~0L),
                           False, AnyPropertyType, &actual_type_return,
                           &actual_format_return, &nitems_return,
                           &bytes_after_return, &prop_return) != Success) {
        fprintf(stderr, "Cannot get %s property\n", atomName);
        XFree(prop_return);
        return -1;
    }

    if (actual_format_return != 32) {
        fprintf(stderr, "Property format is not 32\n");
        XFree(prop_return);
        return -1;
    }

    long value = *(long*)prop_return;
    XFree(prop_return);

    return value;
}

int getNumberOfWorkspaces(Display *d) {
    return (int) getAtomValue(d, "_NET_NUMBER_OF_DESKTOPS");
}

int getCurrentWorkspace(Display *d) {
    return (int) getAtomValue(d, "_NET_CURRENT_DESKTOP");
}

void selectCurrentDesktopChangeEvent(Display *d) {
    Window root = DefaultRootWindow(d);
    Atom atom = XInternAtom(d, "_NET_CURRENT_DESKTOP", True);
    XSelectInput(d, root, PropertyChangeMask);
}

Window create_workspace_window(Display *d, GC gc, int screen, int workspace_width, int workspace_x) {
    Window w = XCreateSimpleWindow(d, RootWindow(d, screen), workspace_x, 0, workspace_width, height, 1,
                                   BlackPixel(d, screen), BlackPixel(d, screen));
    XSetWindowAttributes attr;
    attr.override_redirect = True;
    XChangeWindowAttributes(d, w, CWOverrideRedirect, &attr);
    XMapWindow(d, w);
    XFillRectangle(d, w, gc, 0, 0, workspace_width, height); // fill rectangle with green
    XFlush(d);
    return w;
}

void destroy_workspace_window(Display *d, Window w) {
    if (w != None) {
        XDestroyWindow(d, w);
    }
}

void handleError(const char *errorMessage, Display *d, GC gc) {
    fprintf(stderr, "%s\n", errorMessage);
    if (gc) {
        XFreeGC(d, gc);
    }
    if (d) {
        XCloseDisplay(d);
    }
    exit(EXIT_FAILURE);
}


void initializeSignalHandler() {
    struct sigaction sa;
    sa.sa_handler = &sigterm_handler;
    sigaction(SIGTERM, &sa, NULL);
}

Display* initializeDisplay() {
    Display *d = XOpenDisplay(NULL);
    if (d == NULL) {
        handleError("Cannot open display", d, NULL);
    }
    return d;
}

GC createGraphicsContext(Display *d) {
    int screen = DefaultScreen(d);
    XGCValues values;
    GC gc = XCreateGC(d, RootWindow(d, screen), 0, &values);
    return gc;
}

void allocateColor(Display *d, GC gc) {
    int screen = DefaultScreen(d);
    Colormap colormap = DefaultColormap(d, screen);
    XColor xcolor;
    Status colorStatus = XParseColor(d, colormap, color, &xcolor);
    if (colorStatus == 0) {
        handleError("Cannot parse color", d, gc);
    }
    colorStatus = XAllocColor(d, colormap, &xcolor);
    if (colorStatus == 0) {
        handleError("Cannot allocate color", d, gc);
    }
    XSetForeground(d, gc, xcolor.pixel);
}


void initializeProgram(Display **d, GC *gc, int *screen, int *width) {
    initializeSignalHandler();

    *d = initializeDisplay();
    *screen = DefaultScreen(*d);
    *width = DisplayWidth(*d, *screen);

    *gc = createGraphicsContext(*d);

    allocateColor(*d, *gc);

    selectCurrentDesktopChangeEvent(*d);
}


void processWorkspaces(Display *d, GC gc, int screen, int width) {
    int lastActiveWorkspace = -1;
    int num_workspaces = getNumberOfWorkspaces(d);
    if (num_workspaces == -1) {
        handleError("Cannot get the number of workspaces", d, gc);
    }

    Window w = None;
    XEvent e;
    while (!done) {
        XNextEvent(d, &e);

        if (e.type != PropertyNotify) {
            continue;
        }

        XPropertyEvent *pe = (XPropertyEvent *)&e;
        if (pe->atom != XInternAtom(d, "_NET_CURRENT_DESKTOP", True)) {
            continue;
        }

        int activeWorkspace = getCurrentWorkspace(d);
        if (activeWorkspace == -1) {
            destroy_workspace_window(d, w);
            handleError("Cannot get the current workspace", d, gc);
        }

        if (activeWorkspace == lastActiveWorkspace) {
            continue;
        }

        lastActiveWorkspace = activeWorkspace;
        destroy_workspace_window(d, w);

        int workspace_width = width / num_workspaces;
        int workspace_x = activeWorkspace * workspace_width;
        w = create_workspace_window(d, gc, screen, workspace_width, workspace_x);
    }

    destroy_workspace_window(d, w);
}

void cleanupProgram(Display *d, GC gc) {
    XFreeGC(d, gc);
    XCloseDisplay(d);
}

int main(int argc, char **argv) {
    int opt;
    while((opt = getopt(argc, argv, "h:c:")) != -1) {
        switch(opt) {
            case 'h':
                height = atoi(optarg);
                break;
            case 'c':
                color = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-h height] [-c color]\n", argv[0]);
                return EXIT_FAILURE;
        }
    }

    Display *d;
    GC gc;
    int screen, width;

    initializeProgram(&d, &gc, &screen, &width);
    processWorkspaces(d, gc, screen, width);
    cleanupProgram(d, gc);

    return EXIT_SUCCESS;
}
