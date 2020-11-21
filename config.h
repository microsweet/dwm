#include <X11/XF86keysym.h>

/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const unsigned int gappih    = 3;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 3;       /* vert inner gap between windows */
static const unsigned int gappoh    = 3;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 3;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const Bool viewontag         = True;     /* Switch view on tag switch */
static const char *fonts[]          = { "SauceCodePro Nerd Font Mono:size=14" };
static const char dmenufont[]       = "SauceCodePro Nerd Font Mono:size=16";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#ffffff";
static const char col_cyan[]        = "#37474F";
static const char col_border[]        = "#42A5F5";
static const unsigned int baralpha = 0xd0;
static const unsigned int borderalpha = OPAQUE;
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_border  },
	[SchemeHid]  = { col_cyan,  col_gray1, col_border  },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* staticstatus */
static const int statmonval = 0;

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
//static const char *tags[] = { "一", "二", "三", "四", "五", "六", "七", "八", "九" };
//static const char *tags[] = { "\uf120", "\uf7ae", "\uf121", "\uf04b", "\ue62e", "\uf251", "\ue727", "\uf537", "\uf684" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      		instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     		NULL,       NULL,       0,            1,           -1 },
	{ "Android Emulator", 	NULL,       NULL,       0,            1,           -1 },
	{ "Emulator", 		NULL,       NULL,       0,            1,           -1 },
	{ "quemu-system-i386", 	NULL,       NULL,       0,            1,           -1 },
	{ "firefox",  		NULL,       NULL,       1 << 1,       0,           -1 },
	{ "Google-chrome",  	NULL,       NULL,       2,            0,            0},
	{ "Eclipse",  		NULL,       NULL,       1 << 2,       0,            0},
	{ "thunder",  		NULL,       NULL,       0,            1,            0},
};

/* layout(s) */
static const float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "Tile",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
//static const char *dmenucmd[] = { "rofi", "-show drun -config $HOME/.config/rofi/themes/default.rasi",  NULL };
//static const char *dmenucmd[] = { "rofi -show drun",  NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *browsercmd[]  = { "firefox", NULL };

//static const char *upvol[]   = { "/home/microsweet/scripts/vol-up.sh",  NULL };
//static const char *downvol[] = { "/home/microsweet/scripts/vol-down.sh",  NULL };
//static const char *mutevol[] = { "/home/microsweet/scripts/vol-toggle.sh",  NULL };
static const char *upvol[]   = { "pamixer", "-i", "5",  NULL };
static const char *downvol[] = { "pamixer", "-d", "5",  NULL };
static const char *mutevol[] = { "pamixer", "-t",  NULL };

static const char *wpcmd[]  = { "/home/microsweet/scripts/wp-change.sh", NULL };
static const char *sktogglecmd[]  = { "/home/microsweet/scripts/sck-tog.sh", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "80x24", NULL };

//static const char *setcolemakcmd[]  = { "/home/microsweet/scripts/setxmodmap-colemak.sh", NULL };
//static const char *setqwertycmd[]  = { "/home/microsweet/scripts/setxmodmap-qwerty.sh", NULL };

static const char *powermenucmd[] = { "/home/microsweet/.config/rofi/scripts/Powermenu.sh", NULL };
static const char *suspendcmd[]  = { "/home/microsweet/scripts/suspend.sh", NULL };

static const char *screenshotcmd[] = { "flameshot", "gui", NULL };

static Key keys[] = {
	/* modifier            key                      function        argument */
	//{ MODKEY,              XK_d,                    spawn,          {.v = dmenucmd } },
	{ MODKEY,              XK_d,                    spawn,          SHCMD("rofi -show drun ~/.config/rofi/themes/default.rasi") },
	{ MODKEY,              XK_Return,               spawn,          {.v = termcmd } },
	{ MODKEY,              XK_c,                    spawn,          {.v = browsercmd } },
	//{ MODKEY|ShiftMask,    XK_w,                    spawn,          {.v = setqwertycmd } },
	//{ MODKEY|ShiftMask,    XK_m,                    spawn,          {.v = setcolemakcmd } },
	//{ MODKEY|ShiftMask,    XK_p,                    spawn,          {.v = suspendcmd } },
	{ MODKEY,	       XK_y,                    spawn,          {.v = powermenucmd } },
	{ MODKEY|ControlMask,  XK_s,                    spawn,          {.v = sktogglecmd } },
	{ 0,                   XF86XK_AudioLowerVolume, spawn,          {.v = downvol } },
	{ 0,                   XF86XK_AudioMute,        spawn,          {.v = mutevol } },
	{ 0,                   XF86XK_AudioRaiseVolume, spawn,          {.v = upvol   } },
	{ 0,                   XF86XK_MonBrightnessDown,  spawn,          SHCMD("/home/microsweet/.Scripts/DSbacklight.sh down") },
	{ 0,                   XF86XK_MonBrightnessUp,    spawn,          SHCMD("/home/microsweet/.Scripts/DSbacklight.sh up") },
	{ MODKEY,              XK_minus,          	spawn,          {.v = downvol } },
	{ MODKEY,              XK_backslash,            spawn,          {.v = mutevol } },
	{ MODKEY,              XK_equal,         	spawn,          {.v = upvol   } },
	//{ MODKEY,              XK_b,                    spawn,          {.v = wpcmd } },
	{ 0,                   XK_Print,                spawn,          {.v = screenshotcmd } },
	{ MODKEY|ShiftMask,    XK_k,                    rotatestack,    {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_j,                    rotatestack,    {.i = -1 } },
	{ MODKEY,              XK_k,                    focusstack,     {.i = +1 } },
	{ MODKEY,              XK_j,                    focusstack,     {.i = -1 } },
	{ MODKEY,              XK_h,                    viewtoleft,     {0} },
	{ MODKEY,              XK_l,                    viewtoright,    {0} },
	{ MODKEY|ShiftMask,    XK_h,                    tagtoleft,      {0} },
	{ MODKEY|ShiftMask,    XK_l,                    tagtoright,     {0} },
	//{ MODKEY|ShiftMask,    XK_h,                    incnmaster,     {.i = +1 } },
	//{ MODKEY|ShiftMask,    XK_l,                    incnmaster,     {.i = -1 } },
	{ MODKEY|ControlMask,  XK_h,                    setmfact,       {.f = -0.05} },
	{ MODKEY|ControlMask,  XK_l,                    setmfact,       {.f = +0.05} },
	{ MODKEY,              XK_i,                    hidewin,        {0} },
	{ MODKEY|ShiftMask,    XK_i,                    restorewin,     {0} },
	{ MODKEY,              XK_o,                    hideotherwins,  {0}},
	{ MODKEY|ShiftMask,    XK_o,                    restoreotherwins, {0}},
	{ MODKEY|ShiftMask,    XK_Return,               zoom,           {0} },
	{ MODKEY,              XK_b,                  	view,           {0} },
	{ MODKEY|ShiftMask,    XK_q,                    killclient,     {0} },
	{ MODKEY,              XK_e,                    setlayout,      {.v = &layouts[0]} },
	{ MODKEY,              XK_w,                    setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,    XK_f,                    fullscreen,     {0} },
	{ MODKEY,              XK_space,                setlayout,      {0} },
	{ MODKEY|ShiftMask,    XK_space,                togglefloating, {0} },
	{ MODKEY,              XK_apostrophe,           togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,              XK_0,                    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,    XK_0,                    tag,            {.ui = ~0 } },
	{ MODKEY,              XK_comma,                focusmon,       {.i = -1 } },
	{ MODKEY,              XK_period,               focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_comma,                tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,    XK_period,               tagmon,         {.i = +1 } },
	{ MODKEY,    	       XK_9,               	spawn,         	SHCMD("i3lock -i ~/.config/i3/Pictures/lock.png") },
	{ MODKEY,    	       XK_n,               	spawn,         	SHCMD("~/scripts/notify.sh") },
	TAGKEYS(               XK_1,                      0)
	TAGKEYS(               XK_2,                      1)
	TAGKEYS(               XK_3,                      2)
	TAGKEYS(               XK_4,                      3)
	TAGKEYS(               XK_5,                      4)
	TAGKEYS(               XK_6,                      5)
	TAGKEYS(               XK_7,                      6)
	TAGKEYS(               XK_8,                      7)
	//TAGKEYS(               XK_9,                      8)
	{ MODKEY|ControlMask,  XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button1,        togglewin,      {0} },
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

