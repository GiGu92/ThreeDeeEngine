#pragma once

#include <DirectXMath.h>
#include <3rdParty/plog/Log.h>

extern class IDriver* drv;
extern class WorldRenderer* wr;
extern void create_driver_d3d11();
extern void* get_hwnd();
extern void exit_program();
extern const char* get_log_file_path();

// TODO: move to some kind of utils
wchar_t* utf8_to_wcs(const char* utf8_str, wchar_t* wcs_buf, int wcs_buf_len);
char* wcs_to_utf8(const wchar_t* wcs_str, char* utf8_buf, int utf8_buf_len);

#define RAD_TO_DEG 57.2957795f;	
#define DEG_TO_RAD 0.0174532925f;
inline float to_deg(float rad) { return rad * RAD_TO_DEG; }
inline float to_rad(float deg) { return deg * DEG_TO_RAD; }

struct ProfileScopeHelper
{
	ProfileScopeHelper(const char* label);
	~ProfileScopeHelper();
};

#define TOY_PS_CC0(a, b) a##b
#define TOY_PS_CC1(a, b) TOY_PS_CC0(a, b)
#define PROFILE_SCOPE(label) ProfileScopeHelper TOY_PS_CC1(profileScope, __LINE__)(label)

// For DirectXMath
using namespace DirectX;