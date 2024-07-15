/* See LICENSE file for copyright and license details. */
// NOTE: imports

#include "./themes/tokyonight.h"
#include <X11/X.h>


/* NOTE: Constants */
#define TERMINAL  "st"
#define TERMCLASS "St"
#define MENU      "dmenu_run"
#define BROWSER   "google-chrome-stable"
/* appearance */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 30;       /* vert outer gap between windows and screen edge */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int swallowfloating    = 1;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static int smartgaps                = 1;        /* 1 means no outer gap when there is only one window */
static const char *fonts[]          = { "JetBrains Mono Nerd Font:size=10"};

static const XPoint stickyicon[]    = { {0,0}, {4,0}, {4,8}, {2,6}, {0,8}, {0,0} }; /* represents the icon as an array of vertices */
static const XPoint stickyiconbb    = {4,8};	/* defines the bottom right corner of the polygon's bounding box (speeds up scaling) */

// NOTE: Scratchpad 

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {TERMINAL, "-n", "spterm", "-g", "60x20", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
};

/* NOTE: Tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

/* NOTE: Rules */
static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",    NULL,     NULL,           0,         1,          0,           0,        -1 },
	{ TERMCLASS,      NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ NULL,	     "spterm", NULL,	       SPTAG(0),  1,	      1,           0,        -1 },
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/*  NOTE: layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static int attachbelow = 1;    /* 1 means attach after the currently active window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/* NOTE: key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \
	{ MOD, XK_v,     ACTION##stack, {.i = 0 } },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
/* NOTE: Commands */
static const char *termcmd[]      = { TERMINAL, NULL };
static const char *menucmd[]      = { MENU,"-l","10", NULL };
static const char *filemanager[]  = { "obsidian", NULL };
static const char *applauncher[]  = { "/home/aboud/.local/bin/scripts/dmenu/applauncher.sh"};
static const char *musicplayer[]  = { TERMINAL, "-e", "ncmpcpp", NULL };
static const char *texteditor[]   = { TERMINAL, "-e", "nvim", NULL };
static const char *taskmanager[]  = { TERMINAL, "-e", "btop", NULL };
static const char *todo[]	  = { "zathura", "/home/aboud/github/todolist/main.pdf", NULL };
static const char *lockscreen[]	  = { "slock", NULL };

Autostarttag autostarttaglist[] = {
	{.cmd = todo, .tags = 1 << 7 },
	{.cmd = NULL, .tags = 0 },
};

#include "selfrestart.c"
#include <X11/XF86keysym.h>



static const Key keys[] = {
	/* modifier                     key        function        argument */
	STACKKEYS(MODKEY,                            focus)
	STACKKEYS(MODKEY|ShiftMask,                  push)
	TAGKEYS(			XK_1,		0)
	TAGKEYS(			XK_2,		1)
	TAGKEYS(			XK_3,		2)
	TAGKEYS(			XK_4,		3)
	TAGKEYS(			XK_5,		4)
	TAGKEYS(			XK_6,		5)
	TAGKEYS(			XK_7,		6)
	TAGKEYS(			XK_8,		7)
	TAGKEYS(			XK_9,		8)
	{ MODKEY|ControlMask|ShiftMask, XK_q,                    quit,             {1} }, 
	{ MODKEY|ControlMask|ShiftMask, XK_r,                    self_restart,     {0} },
	{ MODKEY,			XK_0,                    view,             {.ui = ~0 } },
	{ MODKEY|ShiftMask,	        XK_0,                    tag,              {.ui = ~0 } },
	{ MODKEY,			XK_q,                    killclient,       {0} },

    //    NOTE: Browser

	{ MODKEY,			XK_w,                    spawn,            {.v = (const char*[]){ BROWSER, NULL } } },
        { MODKEY,                	XK_e,		         spawn,            {.v = (const char*[]){ BROWSER,"gmail.com", NULL } } },
	{ MODKEY|Mod1Mask,		XK_d,                    spawn,            {.v = (const char*[]){ BROWSER,"datacamp.com", NULL } } },
	{ MODKEY|Mod1Mask,		XK_y,                    spawn,            {.v = (const char*[]){ BROWSER,"youtube.com", NULL } } },
	{ MODKEY|Mod1Mask,		XK_c,                    spawn,            {.v = (const char*[]){ BROWSER,"coursera.org", NULL } } },
	{ MODKEY|Mod1Mask,		XK_l,                    spawn,            {.v = (const char*[]){ BROWSER,"lichess.org", NULL } } },
	{ MODKEY|Mod1Mask|ShiftMask,	XK_l,                    spawn,            {.v = (const char*[]){ BROWSER,"linkedin.com", NULL } } },
	{ MODKEY|Mod1Mask,	        XK_s,                    spawn,            {.v = (const char*[]){ BROWSER,"https://excalidraw.com/", NULL } } },
	{ MODKEY|Mod1Mask,		XK_w,                    spawn,            {.v = (const char*[]){ BROWSER,"web.whatsapp.com/", NULL } } },
	{ MODKEY|Mod1Mask,		XK_f,                    spawn,            {.v = (const char*[]){ BROWSER,"facebook.com", NULL } } },

    // NOTE: Applications

	{ MODKEY,			XK_Return,          	 spawn,            {.v = termcmd     } },
	{ MODKEY,			XK_d,                    spawn,            {.v = menucmd     } },
	{ MODKEY,			XK_r,                    spawn,            {.v = filemanager } },
        { MODKEY,			XK_n,		         spawn,            {.v = texteditor  } },
        { MODKEY,			XK_m,		         spawn,            {.v = musicplayer } },
	{ MODKEY|ShiftMask,		XK_r,                    spawn,            {.v = taskmanager } },
	{ MODKEY|ShiftMask,		XK_l,                    spawn,            {.v = lockscreen  } },
	{ Mod1Mask,     		XK_space,                spawn,            {.v = applauncher } },

    // NOTE: Layout and Movements

	{ MODKEY,			XK_t,                    setlayout,        {.v = &layouts[0]} }, /* tile */
	{ MODKEY|ShiftMask,		XK_Return,	         togglescratch,    {.ui = 0} }, 
	{ MODKEY|ShiftMask,		XK_a,                    defaultgaps,      {0} },
	{ MODKEY,			XK_a,                    togglegaps,       {0} },
	{ MODKEY|ShiftMask,		XK_space,                togglefloating,   {0} },
	{ MODKEY,			XK_f,	                 togglefullscr,    {0} },
	{ MODKEY,                       XK_s,			 togglesticky,     {0} },
	{ MODKEY,			XK_b,		         togglebar,        {0} },
	{ MODKEY|ShiftMask,             XK_Tab,			 toggleattachbelow,{0} },
	{ MODKEY,			XK_h,	                 setmfact,         {.f = -0.05} },
	{ MODKEY,			XK_l,	                 setmfact,         {.f = +0.05} },
	{ MODKEY,			XK_z,		         incrgaps,         {.i = +3 } },
	{ MODKEY,			XK_x,		         incrgaps,         {.i = -3 } },
	{ MODKEY,			XK_period,	 	 focusmon,	   {.i = -1 } },
	{ MODKEY,			XK_comma,		 focusmon,	   {.i = +1 } },
	{ MODKEY|ShiftMask,		XK_period,		 tagmon,	   {.i = -1 } },
	{ MODKEY|ShiftMask,		XK_comma,		 tagmon,	   {.i = +1 } },

   // NOTE: Scripts
	{ MODKEY,			XK_Insert,	         spawn,            SHCMD("xdotool type $(grep -v '^#' /home/aboud/.local/bin/scripts/dmenu/bookmarks | dmenu -l 10 | cut -d' ' -f1)") },
        { MODKEY|ShiftMask,		XK_e,		         spawn,            SHCMD("xdotool type $(grep -v '^#' /home/aboud/.local/bin/scripts/dmenu/emails | dmenu -l 10 | cut -d' ' -f1)") },
        { MODKEY,			XK_s,	                 spawn,            SHCMD("maim -u | feh -F - & maim -s -k /home/aboud/personal/pictures/$(date +%s).png && kill $!") },

	// NOTE:DMENU
	{ MODKEY|ShiftMask,		XK_d,                    spawn,            {.v = (const char*[]){ "/home/aboud/.local/bin/scripts/dmenu/passmenu", NULL } } },
	{ MODKEY|ShiftMask,		XK_s,                    spawn,            {.v = (const char*[]){ "/home/aboud/.local/bin/scripts/excalidraw.sh", NULL } } },
	{ MODKEY,			XK_p,			 spawn,            {.v = (const char*[]){ TERMINAL,"-e","/home/aboud/.local/bin/scripts/programming/new.sh", NULL } } },
	{ MODKEY|ShiftMask,		XK_n,			 spawn,            {.v = (const char*[]){ TERMINAL,"-e","nvim","/home/aboud/github/todolist/", NULL } } },
	{ MODKEY|ShiftMask,		XK_m,                    spawn,            {.v = (const char*[]){ "/home/aboud/.local/bin/scripts/dmenu/notes/displaynotes.sh", NULL } } },
	{ MODKEY,			XK_space,		 spawn,            {.v = (const char*[]){"/home/aboud/.local/bin/scripts/lang",NULL } }  },
	{ MODKEY|ShiftMask,		XK_w,                    spawn,            {.v = (const char*[]){ "/home/aboud/.local/bin/scripts/dmenu/networkmanager_applet", NULL } } },
	{ MODKEY|ShiftMask,		XK_b,                    spawn,            {.v = (const char*[]){ "/home/aboud/.local/bin/scripts/dmenu/bm.sh", NULL } } },
	{ MODKEY|ShiftMask,		XK_q,                    spawn,            {.v = (const char*[]){ "/home/aboud/.local/bin/scripts/dmenu/powermenu.sh", NULL } } },
	{ Mod1Mask|ShiftMask,		XK_y,                    spawn,            {.v = (const char*[]){ "/home/aboud/.local/bin/scripts/dmenu/youtubesearch", NULL } } },
	{ Mod1Mask|ShiftMask,		XK_g,                    spawn,            {.v = (const char*[]){ "/home/aboud/.local/bin/scripts/dmenu/googlesearch", NULL } } },
        { Mod1Mask,			XK_period,               spawn,            {.v = (const char*[]){ "python","/home/aboud/.local/bin/scripts/dmenu/obsidian.py", NULL } } },

    // NOTE: XF86 Keys
	
	{ 0,			       XF86XK_MonBrightnessUp,	 spawn,            SHCMD("light -A 5") },
	{ 0,			       XF86XK_MonBrightnessDown, spawn,	           SHCMD("light -U 5") },
        { 0,			       XF86XK_Calculator,	 spawn,            {.v = (const char*[]){ TERMINAL, "-e", "python", NULL } } },
	{ 0,			       XF86XK_AudioMute,	 spawn,            {.v = (const char*[]){ "wpctl","set-mute", "@DEFAULT_SINK@", "toggle", NULL } } },
	{ 0,			       XF86XK_AudioRaiseVolume,  spawn,            {.v = (const char*[]){ "wpctl","set-volume", "@DEFAULT_SINK@", "5%+", NULL } } },
	{ 0,			       XF86XK_AudioLowerVolume,  spawn,            {.v = (const char*[]){ "wpctl","set-volume", "@DEFAULT_SINK@", "5%-", NULL } } },
	{ 0,			       XF86XK_AudioPlay,	 spawn,            {.v = (const char*[]){ "mpc", "toggle", NULL } } },
	{ 0,			       XF86XK_AudioPrev,	 spawn,            {.v = (const char*[]){ "mpc", "prev", NULL } } },
	{ 0,			       XF86XK_AudioNext,	 spawn,            {.v = (const char*[]){ "mpc", "next", NULL } } },
	{ MODKEY,		       XF86XK_AudioPrev,	 spawn,            {.v = (const char*[]){ "mpc", "seek","-10", NULL } } },
	{ MODKEY,		       XF86XK_AudioNext,	 spawn,            {.v = (const char*[]){ "mpc", "seek","+10", NULL } } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

