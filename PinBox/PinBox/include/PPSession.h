#pragma once
#ifndef _PP_SESSION_H_
#define _PP_SESSION_H_

//=======================================================================
// PinBox Video Session
// container streaming data can be movie or screen capture
// With:
// 1, Movie :
// TODO: using ffmpeg to decode movie stream from server
// TODO: support 3D movie on N3DS
// TODO: mainly support RGB565 for lower size and speed up stream
// 2, Screen Capture:
// TODO: capture entire screen or a part of screen that config in server
//-----------------------------------------------------------------------
// Note: each session is running standalone and only 1 session can be run
// at a time.
//=======================================================================

#include "PPNetwork.h"
#include <webp/decode.h>
#include "opusfile.h"
#include "Mutex.h"
#include <map>


enum PPSession_Type { PPSESSION_NONE, PPSESSION_MOVIE, PPSESSION_SCREEN_CAPTURE, PPSESSION_INPUT_CAPTURE};

#define MSG_COMMAND_SIZE 9

#define PPREQUEST_AUTHEN 50
#define PPREQUEST_HEADER 10
#define PPREQUEST_BODY 15
// authentication code
#define MSG_CODE_REQUEST_AUTHENTICATION_MOVIE 1
#define MSG_CODE_REQUEST_AUTHENTICATION_SCREEN_CAPTURE 2
#define MSG_CODE_REQUEST_AUTHENTICATION_INPUT 3
#define MSG_CODE_RESULT_AUTHENTICATION_SUCCESS 5
#define MSG_CODE_RESULT_AUTHENTICATION_FAILED 6
// screen capture code
#define MSG_CODE_REQUEST_START_SCREEN_CAPTURE 10
#define MSG_CODE_REQUEST_STOP_SCREEN_CAPTURE 11
#define MSG_CODE_REQUEST_CHANGE_SETTING_SCREEN_CAPTURE 12
#define MSG_CODE_REQUEST_NEW_SCREEN_FRAME 15
#define MSG_CODE_REQUEST_SCREEN_RECEIVED_FRAME 16
#define MSG_CODE_REQUEST_NEW_AUDIO_FRAME 18
#define MSG_CODE_REQUEST_RECEIVED_AUDIO_FRAME 19


// input
#define MSG_CODE_REQUEST_START_INPUT_CAPTURE 40
#define MSG_CODE_REQUEST_STOP_INPUT_CAPTURE 41
#define MSG_CODE_SEND_INPUT_CAPTURE 42
#define MSG_CODE_SEND_INPUT_CAPTURE_IDLE 44

// audio
#define AUDIO_CHANNEL	0x08
typedef struct
{
	u8			*start;
	u32			size;
	u32			width;
	u32			height;
} QueueFrame;

typedef struct
{
	u8* pieceAddr;
	u32 pieceStart;
	u32 pieceSize;
	bool decoded = false;
} FrameData;
class PPSessionManager;

class PPSession
{
private:
	PPSessionManager				*g_manager;
	PPSession_Type					g_sessionType = PPSESSION_NONE;
	PPNetwork*						g_network = nullptr;
	PPMessage*						g_tmpMessage = nullptr;
	bool							g_authenticated = false;
	PPNetworkCallback				g_onAuthenSuccessed = nullptr;

private:
	void initSession();

	void processMovieSession(u8* buffer, size_t size);
	void processScreenCaptureSession(u8* buffer, size_t size);
	void processInputSession(u8* buffer, size_t size);

public:
	int								sessionID = -1;
	~PPSession();

	void InitMovieSession();
	void InitScreenCaptureSession(PPSessionManager* manager);
	void InitInputCaptureSession(PPSessionManager* manager);

	void StartSession(const char* ip, const char* port, s32 prio, PPNetworkCallback authenSuccessed);
	void CloseSession();




	//-----------------------------------------------------
	// screen capture
	//-----------------------------------------------------
public:
	//-----------------------------------------------------
	// profile setting
	typedef struct
	{
		std::string profileName = "Default";
		bool waitToReceivedFrame = false;
		u32 smoothStepFrames = 3;
		u32 sourceQuality = 75;
		u32 sourceScale = 100;
	} SSProfile;
	//----------------------------------------------------
	bool								SS_v_isStartStreaming = false;
	bool								SS_setting_waitToReceivedFrame = true;
	u32									SS_setting_smoothStepFrames = 1;		// this setting allow frame switch smoother if there is delay when received frame
	u32									SS_setting_sourceQuality = 75;			// webp quality control
	u32									SS_setting_sourceScale = 100;			// frame size control eg: 75% = 0.75 of real size
	//----------------------------------------------------


	//-----------------------------------------------------
	// input
	//-----------------------------------------------------
private:
	bool								IN_isStart = false;


public:
	//-----------------------------------------------------
	// common
	//-----------------------------------------------------
	void								RequestForheader();




	//-----------------------------------------------------
	// screen capture
	//-----------------------------------------------------
	void								SS_StartStream();
	void								SS_StopStream();
	void								SS_ChangeSetting();
	void								SS_SendReceivedFrame();

	void								SS_Reset();
	//-----------------------------------------------------
	// movie
	//-----------------------------------------------------


	//-----------------------------------------------------
	// input
	//-----------------------------------------------------
	void								IN_Start();
	bool								IN_SendInputData(u32 down, u32 up, short cx, short cy, short ctx, short cty);
	void								IN_SendIdleInput();
	void								IN_Stop();
};

#endif