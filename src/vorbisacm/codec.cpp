/*
	Ogg Vorbis CODEC for MSACM
	Copyright (C)2001 H.Mutsuki
*/

#include "hmc/util.h"
#include "hmc/mmutil.h"
#include "hmc/dialog.h"
#include "hmc/registry.h"

#include "codec.h"
#include "encode.h"
#include "decode.h"

#include "resource.h"

#pragma warning(push,1)
#include "vorbis/codec.h"
#include "vorbis/vorbisenc.h"
#pragma warning(pop)

#ifndef _DEBUG
#ifdef RELEASEDEBUG
#undef TRACE
#define TRACE HMC::Trace
#endif
#endif

/*****************************************************************
	Ogg Vorbis for MSACM
*****************************************************************/
/*****************************************************************
	グローバル
*****************************************************************/
BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpvReserved)
{
	BOOL bRes = FALSE;
	switch(fdwReason) {
	case DLL_PROCESS_ATTACH:
#if defined(_DEBUG) || defined(RELEASEDEBUG)
		// ビルド、デバッグ、リビルド…のサイクルを円滑に行うため、
		// 特定のプロセスがこのモジュール(vorbis.acm)をロードしロックするのを防ぐ
		// ※explorer.exeはプロパティシート等の目的でこのモジュールをロードする場合がある
		// (そして困ったことにプロセスが終了するまでアンロードされないのだ)
		{
			HMC::TSTRING s;
			HMODULE h = ::GetModuleHandle(NULL);
			::GetModuleFileName(h,s,s.chars());
			HMC::MakeNameFName(s,s);
			if(s.isEqualIgnoreCase("explorer.exe")) {
				break;
			}
		}
#endif
		CHECKRESULT_API(::DisableThreadLibraryCalls(hModule));
		bRes = TRUE;
		break;
	case DLL_PROCESS_DETACH:
		bRes = TRUE;
		break;
	case DLL_THREAD_ATTACH:
		bRes = TRUE;
		break;
	case DLL_THREAD_DETACH:
		bRes = TRUE;
		break;
	}
	return bRes;
}
extern "C" LRESULT WINAPI DriverProc(DWORD dwId, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	return CacmDriverOggVorbis::DriverProc(dwId,hdrvr,uMsg,lParam1,lParam2);
}
/*****************************************************************
	各種定数
*****************************************************************/
static const DWORD aSamplesPerSec[] = {
	48000, 44100, 22050, 11025
};
static const WORD aChannels[] = { 2, 1 };
static const WORD aBitsPerSample[] = { 16 };
static const DWORD aAvgBytesPerSec[][11] = {
	{ 64000/8, 80000/8, 96000/8, 112000/8, 128000/8, 160000/8, 192000/8, 240000/8, 256000/8, 350000/8, 450000/8 }, // 48K,Stereo
	{ 48000/8, 64000/8, 72000/8,  80000/8,  88000/8,  96000/8, 112000/8, 128000/8, 144000/8, 192000/8, 256000/8 }, // 48K,Mono
	{ 64000/8, 80000/8, 96000/8, 112000/8, 128000/8, 160000/8, 192000/8, 240000/8, 256000/8, 350000/8, 450000/8 }, // 44K,Stereo
	{ 48000/8, 64000/8, 72000/8,  80000/8,  88000/8,  96000/8, 112000/8, 128000/8, 144000/8, 192000/8, 256000/8 }, // 44K,Mono
	{ 56000/8, 72000/8, 80000/8,  88000/8,  96000/8, 112000/8, 144000/8, 176000/8, 192000/8, 256000/8, 320000/8 }, // 22K,Stereo
	{ 36000/8, 42000/8, 48000/8,  52000/8,  56000/8,  64000/8,  80000/8,  88000/8,  96000/8, 128000/8, 168000/8 }, // 22K,Mono
	{ 36000/8, 44000/8, 50000/8,  52000/8,  56000/8,  64000/8,  80000/8,  96000/8, 112000/8, 144000/8, 168000/8 }, // 11K,Stereo
	{ 22000/8, 26000/8, 28000/8,  30000/8,  32000/8,  34000/8,  40000/8,  48000/8,  56000/8,  72000/8,  88000/8 }, // 11K,Mono
};
static const float aQuality[][11] = {
	{ 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f }, // 48Kf,Stereo
	{ 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f }, // 48Kf,Mono
	{ 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f }, // 44Kf,Stereo
	{ 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f }, // 44Kf,Mono
	{ 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f }, // 22Kf,Stereo
	{ 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f }, // 22Kf,Mono
	{ 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f }, // 11Kf,Stereo
	{ 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f }, // 11Kf,Mono
};
/*****************************************************************
	フォーマットテーブル
*****************************************************************/
// WAVE_FORMAT_PCM側詳細リスト
const FORMATDETAIL aPCMFormatIndexToDetail[] = {
	// WAVE_FORMAT_PCM(48K,16Bits,Stereo)
	{ aSamplesPerSec[0], aChannels[0], aBitsPerSample[0], 0, 0.0 },
	// WAVE_FORMAT_PCM(48K,16Bits,Mono)
	{ aSamplesPerSec[0], aChannels[1], aBitsPerSample[0], 0, 0.0 },
	// WAVE_FORMAT_PCM(44K,16Bits,Stereo)
	{ aSamplesPerSec[1], aChannels[0], aBitsPerSample[0], 0, 0.0 },
	// WAVE_FORMAT_PCM(44K,16Bits,Mono)
	{ aSamplesPerSec[1], aChannels[1], aBitsPerSample[0], 0, 0.0 },
	// WAVE_FORMAT_PCM(22K,16Bits,Stereo)
	{ aSamplesPerSec[2], aChannels[0], aBitsPerSample[0], 0, 0.0 },
	// WAVE_FORMAT_PCM(22K,16Bits,Mono)
	{ aSamplesPerSec[2], aChannels[1], aBitsPerSample[0], 0, 0.0 },
	// WAVE_FORMAT_PCM(11K,16Bits,Stereo)
	{ aSamplesPerSec[3], aChannels[0], aBitsPerSample[0], 0, 0.0 },
	// WAVE_FORMAT_PCM(11K,16Bits,Mono)
	{ aSamplesPerSec[3], aChannels[1], aBitsPerSample[0], 0, 0.0 },
};
// WAVE_FORMAT_VORBIS側詳細リスト
const FORMATDETAIL aOggFormatIndexToDetail[] = {
	// WAVE_FORMAT_VORBIS(48K,16Bits,Stereo)
	{ aSamplesPerSec[0], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[0][ 0], aQuality[0][ 0] },
	{ aSamplesPerSec[0], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[0][ 1], aQuality[0][ 1] },
	{ aSamplesPerSec[0], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[0][ 2], aQuality[0][ 2] },
	{ aSamplesPerSec[0], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[0][ 3], aQuality[0][ 3] },
	{ aSamplesPerSec[0], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[0][ 4], aQuality[0][ 4] },
	{ aSamplesPerSec[0], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[0][ 5], aQuality[0][ 5] },
	{ aSamplesPerSec[0], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[0][ 6], aQuality[0][ 6] },
	{ aSamplesPerSec[0], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[0][ 7], aQuality[0][ 7] },
	{ aSamplesPerSec[0], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[0][ 8], aQuality[0][ 8] },
	{ aSamplesPerSec[0], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[0][ 9], aQuality[0][ 9] },
	{ aSamplesPerSec[0], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[0][10], aQuality[0][10] },
	// WAVE_FORMAT_VORBIS(48K,16Bits,Mono)
	{ aSamplesPerSec[0], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[1][ 0], aQuality[1][ 0] },
	{ aSamplesPerSec[0], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[1][ 1], aQuality[1][ 1] },
	{ aSamplesPerSec[0], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[1][ 2], aQuality[1][ 2] },
	{ aSamplesPerSec[0], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[1][ 3], aQuality[1][ 3] },
	{ aSamplesPerSec[0], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[1][ 4], aQuality[1][ 4] },
	{ aSamplesPerSec[0], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[1][ 5], aQuality[1][ 5] },
	{ aSamplesPerSec[0], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[1][ 6], aQuality[1][ 6] },
	{ aSamplesPerSec[0], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[1][ 7], aQuality[1][ 7] },
	{ aSamplesPerSec[0], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[1][ 8], aQuality[1][ 8] },
	{ aSamplesPerSec[0], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[1][ 9], aQuality[1][ 9] },
	{ aSamplesPerSec[0], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[1][10], aQuality[1][10] },
	// WAVE_FORMAT_VORBIS(44K,16Bits,Stereo)
	{ aSamplesPerSec[1], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[2][ 0], aQuality[2][ 0] },
	{ aSamplesPerSec[1], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[2][ 1], aQuality[2][ 1] },
	{ aSamplesPerSec[1], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[2][ 2], aQuality[2][ 2] },
	{ aSamplesPerSec[1], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[2][ 3], aQuality[2][ 3] },
	{ aSamplesPerSec[1], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[2][ 4], aQuality[2][ 4] },
	{ aSamplesPerSec[1], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[2][ 5], aQuality[2][ 5] },
	{ aSamplesPerSec[1], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[2][ 6], aQuality[2][ 6] },
	{ aSamplesPerSec[1], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[2][ 7], aQuality[2][ 7] },
	{ aSamplesPerSec[1], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[2][ 8], aQuality[2][ 8] },
	{ aSamplesPerSec[1], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[2][ 9], aQuality[2][ 9] },
	{ aSamplesPerSec[1], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[2][10], aQuality[2][10] },
	// WAVE_FORMAT_VORBIS(44K,16Bits,Mono)
	{ aSamplesPerSec[1], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[3][ 0], aQuality[3][ 0] },
	{ aSamplesPerSec[1], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[3][ 1], aQuality[3][ 1] },
	{ aSamplesPerSec[1], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[3][ 2], aQuality[3][ 2] },
	{ aSamplesPerSec[1], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[3][ 3], aQuality[3][ 3] },
	{ aSamplesPerSec[1], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[3][ 4], aQuality[3][ 4] },
	{ aSamplesPerSec[1], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[3][ 5], aQuality[3][ 5] },
	{ aSamplesPerSec[1], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[3][ 6], aQuality[3][ 6] },
	{ aSamplesPerSec[1], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[3][ 7], aQuality[3][ 7] },
	{ aSamplesPerSec[1], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[3][ 8], aQuality[3][ 8] },
	{ aSamplesPerSec[1], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[3][ 9], aQuality[3][ 9] },
	{ aSamplesPerSec[1], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[3][10], aQuality[3][10] },
	// WAVE_FORMAT_VORBIS(22K,16Bits,Stereo)
	{ aSamplesPerSec[2], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[4][ 0], aQuality[4][ 0] },
	{ aSamplesPerSec[2], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[4][ 1], aQuality[4][ 1] },
	{ aSamplesPerSec[2], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[4][ 2], aQuality[4][ 2] },
	{ aSamplesPerSec[2], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[4][ 3], aQuality[4][ 3] },
	{ aSamplesPerSec[2], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[4][ 4], aQuality[4][ 4] },
	{ aSamplesPerSec[2], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[4][ 5], aQuality[4][ 5] },
	{ aSamplesPerSec[2], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[4][ 6], aQuality[4][ 6] },
	{ aSamplesPerSec[2], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[4][ 7], aQuality[4][ 7] },
	{ aSamplesPerSec[2], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[4][ 8], aQuality[4][ 8] },
	{ aSamplesPerSec[2], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[4][ 9], aQuality[4][ 9] },
	{ aSamplesPerSec[2], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[4][10], aQuality[4][10] },
	// WAVE_FORMAT_VORBIS(22K,16Bits,Mono)
	{ aSamplesPerSec[2], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[5][ 0], aQuality[5][ 0] },
	{ aSamplesPerSec[2], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[5][ 1], aQuality[5][ 1] },
	{ aSamplesPerSec[2], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[5][ 2], aQuality[5][ 2] },
	{ aSamplesPerSec[2], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[5][ 3], aQuality[5][ 3] },
	{ aSamplesPerSec[2], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[5][ 4], aQuality[5][ 4] },
	{ aSamplesPerSec[2], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[5][ 5], aQuality[5][ 5] },
	{ aSamplesPerSec[2], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[5][ 6], aQuality[5][ 6] },
	{ aSamplesPerSec[2], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[5][ 7], aQuality[5][ 7] },
	{ aSamplesPerSec[2], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[5][ 8], aQuality[5][ 8] },
	{ aSamplesPerSec[2], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[5][ 9], aQuality[5][ 9] },
	{ aSamplesPerSec[2], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[5][10], aQuality[5][10] },
	// WAVE_FORMAT_VORBIS(11K,16Bits,Stereo)
	{ aSamplesPerSec[3], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[6][ 0], aQuality[6][ 0] },
	{ aSamplesPerSec[3], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[6][ 1], aQuality[6][ 1] },
	{ aSamplesPerSec[3], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[6][ 2], aQuality[6][ 2] },
	{ aSamplesPerSec[3], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[6][ 3], aQuality[6][ 3] },
	{ aSamplesPerSec[3], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[6][ 4], aQuality[6][ 4] },
	{ aSamplesPerSec[3], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[6][ 5], aQuality[6][ 5] },
	{ aSamplesPerSec[3], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[6][ 6], aQuality[6][ 6] },
	{ aSamplesPerSec[3], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[6][ 7], aQuality[6][ 7] },
	{ aSamplesPerSec[3], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[6][ 8], aQuality[6][ 8] },
	{ aSamplesPerSec[3], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[6][ 9], aQuality[6][ 9] },
	{ aSamplesPerSec[3], aChannels[0], aBitsPerSample[0], aAvgBytesPerSec[6][10], aQuality[6][10] },
	// WAVE_FORMAT_VORBIS(11K,16Bits,Mono)
	{ aSamplesPerSec[3], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[7][ 0], aQuality[7][ 0] },
	{ aSamplesPerSec[3], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[7][ 1], aQuality[7][ 1] },
	{ aSamplesPerSec[3], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[7][ 2], aQuality[7][ 2] },
	{ aSamplesPerSec[3], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[7][ 3], aQuality[7][ 3] },
	{ aSamplesPerSec[3], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[7][ 4], aQuality[7][ 4] },
	{ aSamplesPerSec[3], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[7][ 5], aQuality[7][ 5] },
	{ aSamplesPerSec[3], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[7][ 6], aQuality[7][ 6] },
	{ aSamplesPerSec[3], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[7][ 7], aQuality[7][ 7] },
	{ aSamplesPerSec[3], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[7][ 8], aQuality[7][ 8] },
	{ aSamplesPerSec[3], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[7][ 9], aQuality[7][ 9] },
	{ aSamplesPerSec[3], aChannels[1], aBitsPerSample[0], aAvgBytesPerSec[7][10], aQuality[7][10] },
};
static const WORD aFormatTagIndexToTag[] = {
	WAVE_FORMAT_PCM,
	WAVE_FORMAT_VORBIS1,
	WAVE_FORMAT_VORBIS2,
	WAVE_FORMAT_VORBIS3,
	WAVE_FORMAT_VORBIS1P,
	WAVE_FORMAT_VORBIS2P,
	WAVE_FORMAT_VORBIS3P,
};
static const WCHAR *aFormatTagIndexToName[] = {
	L"",
	L"Ogg Vorbis (mode1)",
	L"Ogg Vorbis (mode2)",
	L"Ogg Vorbis (mode3)",
	L"Ogg Vorbis (mode1+)",
	L"Ogg Vorbis (mode2+)",
	L"Ogg Vorbis (mode3+)",
};
static const DWORD aFormatTagIndexToSize[] = {
	sizeof(PCMWAVEFORMAT),
	sizeof(OGGWAVEFORMAT),
	sizeof(OGGWAVEFORMAT)+OGGWAVEFORMATMAXEXTRABYTES,
	sizeof(OGGWAVEFORMAT),
	sizeof(OGGWAVEFORMAT),
	sizeof(OGGWAVEFORMAT)+OGGWAVEFORMATMAXEXTRABYTES,
	sizeof(OGGWAVEFORMAT),
};
static const DWORD aFormatTagIndexToFormats[] = {
	ARRAYLEN(aPCMFormatIndexToDetail),
	ARRAYLEN(aOggFormatIndexToDetail),
	ARRAYLEN(aOggFormatIndexToDetail),
	ARRAYLEN(aOggFormatIndexToDetail),
	ARRAYLEN(aOggFormatIndexToDetail),
	ARRAYLEN(aOggFormatIndexToDetail),
	ARRAYLEN(aOggFormatIndexToDetail),
};
/*****************************************************************
	ユーティリティ
*****************************************************************/
/*****************************************************************
	フォーマットが正しいことを確認する
*****************************************************************/
BOOL pcmIsValidFormat(CONST WAVEFORMATEX *pwfx)
{
	if(!pwfx) return FALSE;
	size_t n = 0;
	for(; n<ARRAYLEN(aPCMFormatIndexToDetail); n++) {
		if(pwfx->nSamplesPerSec!=aPCMFormatIndexToDetail[n].nSamplesPerSec) continue;
		if(pwfx->nChannels     !=aPCMFormatIndexToDetail[n].nChannels     ) continue;
		if(pwfx->wBitsPerSample!=aPCMFormatIndexToDetail[n].wBitsPerSample) continue;
		if(pwfx->nBlockAlign     != HMC::GetBytesPerSample(pwfx)) continue;	// テーブルではなく定式にあてはまればＯＫ
		if(pwfx->nAvgBytesPerSec != HMC::GetBytesPerSec   (pwfx)) continue;	// テーブルではなく定式にあてはまればＯＫ
		break;
	}
	return n<ARRAYLEN(aPCMFormatIndexToDetail);
}
/*****************************************************************
	フォーマットが正しいことを確認する
*****************************************************************/
BOOL oggIsValidFormat(CONST WAVEFORMATEX *pwfx)
{
	if(!pwfx) return FALSE;
	if(!(pwfx->wFormatTag==WAVE_FORMAT_VORBIS1 ||
		 pwfx->wFormatTag==WAVE_FORMAT_VORBIS2 ||
		 pwfx->wFormatTag==WAVE_FORMAT_VORBIS3 ||
		 pwfx->wFormatTag==WAVE_FORMAT_VORBIS1P||
		 pwfx->wFormatTag==WAVE_FORMAT_VORBIS2P||
		 pwfx->wFormatTag==WAVE_FORMAT_VORBIS3P)) return FALSE;
	size_t n = 0;
	for(; n<ARRAYLEN(aOggFormatIndexToDetail); n++) {
		if(pwfx->nSamplesPerSec!=aOggFormatIndexToDetail[n].nSamplesPerSec) continue;
		if(pwfx->nChannels     !=aOggFormatIndexToDetail[n].nChannels     ) continue;
		if(pwfx->wBitsPerSample!=aOggFormatIndexToDetail[n].wBitsPerSample) continue;
		//if(pwfx->nBlockAlign     != HMC::GetBytesPerSample(pwfx)) continue;	// ※敢えてチェックしない
		//if(pwfx->nAvgBytesPerSec != HMC::GetBytesPerSec   (pwfx)) continue;	// ※敢えてチェックしない
		break;
	}
	return n<ARRAYLEN(aOggFormatIndexToDetail);
}
/*****************************************************************
	AvgBytesPerSecをテーブルから取得する
*****************************************************************/
DWORD oggAvgBytesPerSec(CONST WAVEFORMATEX *pwfx, DWORD mode)
{
	DWORD n = 0;
	switch(pwfx->nSamplesPerSec) {
	case 48000:
		switch(pwfx->nChannels) {
		case 2: n=aAvgBytesPerSec[0][mode]; break;
		case 1: n=aAvgBytesPerSec[1][mode]; break;
		}
		break;
	case 44100:
		switch(pwfx->nChannels) {
		case 2: n=aAvgBytesPerSec[2][mode]; break;
		case 1: n=aAvgBytesPerSec[3][mode]; break;
		}
		break;
	case 22050:
		switch(pwfx->nChannels) {
		case 2: n=aAvgBytesPerSec[4][mode]; break;
		case 1: n=aAvgBytesPerSec[5][mode]; break;
		}
		break;
	case 11025:
		switch(pwfx->nChannels) {
		case 2: n=aAvgBytesPerSec[6][mode]; break;
		case 1: n=aAvgBytesPerSec[7][mode]; break;
		}
		break;
	}
	return n;
}
/*****************************************************************
	テーブルから近似するフォーマットを検索する
*****************************************************************/
DWORD oggFormatToIndex(CONST WAVEFORMATEX *pwfx)
{
	DWORD index = 0;
	LONG  delta = LONG_MAX;
	for(DWORD n=0; n<ARRAYLEN(aOggFormatIndexToDetail); n++) {
		if(!(pwfx->wFormatTag==WAVE_FORMAT_VORBIS1 ||
			 pwfx->wFormatTag==WAVE_FORMAT_VORBIS2 ||
			 pwfx->wFormatTag==WAVE_FORMAT_VORBIS3 ||
			 pwfx->wFormatTag==WAVE_FORMAT_VORBIS1P||
			 pwfx->wFormatTag==WAVE_FORMAT_VORBIS2P||
			 pwfx->wFormatTag==WAVE_FORMAT_VORBIS3P)) continue;
		if(pwfx->nSamplesPerSec!=aOggFormatIndexToDetail[n].nSamplesPerSec) continue;
		if(pwfx->nChannels     !=aOggFormatIndexToDetail[n].nChannels     ) continue;
		if(pwfx->wBitsPerSample!=aOggFormatIndexToDetail[n].wBitsPerSample) continue;
		LONG d = pwfx->nAvgBytesPerSec - aOggFormatIndexToDetail[n].nAvgBytesPerSec;
		if(d==0) {
			index = n;
			break;
		}
		if(abs(d)<abs(delta)) {
			index = n;
			delta = d;
		}
	}
	return index;
}
/*****************************************************************
	フォーマットの詳細を取得する
*****************************************************************/
DWORD oggFormatMakeupExtraByte(WAVEFORMATEX *pwfx)
{
	// 共通部分初期化
	memset((LPBYTE)pwfx+sizeof(WAVEFORMATEX),0,pwfx->cbSize);
	((LPOGGWAVEFORMAT)pwfx)->dwVorbisACMVersion = VORBISACM_VERSION;
	((LPOGGWAVEFORMAT)pwfx)->dwLibVorbisVersion = LIBVORBIS_VERSION;

#ifdef _HAVE_DUMPEDFORMATDETAILS
	// ダンプ済みフォーマット詳細の取得を試みる
	WORD  t = pwfx->wFormatTag;
	WORD  c = pwfx->nChannels;
	DWORD s = pwfx->nSamplesPerSec;
	DWORD i = oggFormatToIndex(pwfx);
	DWORD r = aOggFormatIndexToDetail[i].nAvgBytesPerSec;
	for(DWORD n=0; n<ARRAYLEN(aFormatTagIndexToTag); n++) {
		if(aDumpedFormatDetails[n][0]->wFormatTag==pwfx->wFormatTag) {
			for(DWORD m=0; m<ARRAYLEN(aOggFormatIndexToDetail); m++) {
				const WAVEFORMATEX *p = aDumpedFormatDetails[n][m];
				if(p->wFormatTag     !=t) continue;
				if(p->nChannels      !=c) continue;
				if(p->nSamplesPerSec !=s) continue;
				if(p->nAvgBytesPerSec!=r) continue;
				if(p->cbSize<=pwfx->cbSize) {
					ASSERT(((LPOGGWAVEFORMAT)pwfx)->dwVorbisACMVersion==((LPOGGWAVEFORMAT)p)->dwVorbisACMVersion);
					ASSERT(((LPOGGWAVEFORMAT)pwfx)->dwLibVorbisVersion==((LPOGGWAVEFORMAT)p)->dwLibVorbisVersion);
					memcpy(pwfx,p,HMC::GetFormatSize(p));
					return HMC::GetFormatSize(pwfx);
				}
			}
		}
	}
#endif

	// 通常の方法でフォーマット詳細を得る
	if(pwfx->wFormatTag==WAVE_FORMAT_VORBIS2 || pwfx->wFormatTag==WAVE_FORMAT_VORBIS2P) {
		COggVorbisEncode data(NULL);
		data.Create(pwfx,pwfx);
		DWORD len = pwfx->cbSize - (sizeof(OGGWAVEFORMAT) - sizeof(WAVEFORMATEX));
		LPBYTE ps = (LPBYTE)pwfx + sizeof(OGGWAVEFORMAT);
		LPBYTE pd = ps;
		for(LPBYTE pe=ps+len; pd<pe;) {
			int result = ogg_stream_flush(&data.os,&data.og);
			if(result==0) break;
			if(!(pd + data.og.header_len + data.og.body_len < pe)) {
				ASSERT(0);				// サイズ不足！！
				return 0;
			}
			memcpy(pd,data.og.header,data.og.header_len);
			memcpy(pd+data.og.header_len,data.og.body,data.og.body_len);
			pd += data.og.header_len + data.og.body_len;
		}
		// cbSize生成
		pwfx->cbSize = sizeof(OGGWAVEFORMAT) - sizeof(WAVEFORMATEX);
		pwfx->cbSize += pd - ps;
		pwfx->cbSize = ALIGNWORD(pwfx->cbSize);	// ※後続チャンクのアライメントを保持する
	} else {
		pwfx->cbSize = sizeof(OGGWAVEFORMAT) - sizeof(WAVEFORMATEX);
	}
	return HMC::GetFormatSize(pwfx);
}
/*****************************************************************
	最大のフォーマットサイズを取得する(未使用)
*****************************************************************/
DWORD GetLargestFormat(WAVEFORMATEX *pwfx)
{
	DWORD cbFormatSize = 0;
#ifdef _HAVE_DUMPEDFORMATDETAILS
	for(DWORD n=0; n<ARRAYLEN(aFormatTagIndexToTag); n++) {
		if(aDumpedFormatDetails[n][0]->wFormatTag!=WAVE_FORMAT_PCM) {
			for(DWORD m=0; m<ARRAYLEN(aOggFormatIndexToDetail); m++) {
				const WAVEFORMATEX *p = aDumpedFormatDetails[n][m];
				if(cbFormatSize<HMC::GetFormatSize(p)) {
					cbFormatSize = HMC::GetFormatSize(p);
					pwfx->wFormatTag = p->wFormatTag;
					pwfx->cbSize = (WORD)(cbFormatSize-sizeof(WAVEFORMATEX));
				}
			}
		}
	}
#else
	cbFormatSize = sizeof(WAVEFORMATEX) + OGGWAVEFORMATMAXEXTRABYTES;
	pwfx->wFormatTag = WAVE_FORMAT_VORBIS2;
	pwfx->cbSize = (WORD)(cbFormatSize-sizeof(WAVEFORMATEX));
#endif
	return cbFormatSize;
}
/*****************************************************************
	フォーマット詳細を書式化する
*****************************************************************/
DWORD FormatFormatDetail(WCHAR *szFormat, size_t cbBuf, const FORMATDETAIL *pDetail)
{
	NUMBERFMT nf;
	nf.NumDigits	= 0;
	nf.LeadingZero	= 0;
	nf.Grouping		= 3;
	nf.lpDecimalSep	= ".";
	nf.lpThousandSep= ",";
	nf.NegativeOrder= 0;

	HMC::TString<ACMFORMATDETAILS_FORMAT_CHARS> t,s,b,c;
	t.format("%u",pDetail->nSamplesPerSec);
	CHECKRESULT_API(::GetNumberFormat(LOCALE_USER_DEFAULT,0,t,&nf,s,s.chars()));
	t.format("%u",pDetail->nAvgBytesPerSec*8/1000);
	CHECKRESULT_API(::GetNumberFormat(LOCALE_USER_DEFAULT,0,t,&nf,b,b.chars()));
	if(pDetail->nChannels==1) {
		c = "Mono";
	} else if(pDetail->nChannels==2) {
		c = "Stereo";
	} else {
		c.format("%uch",pDetail->nChannels);
	}
	t.format("%s Hz, %s, About %skbps (Q:%1.1f)",&s,&c,&b,pDetail->flQuality);
	ASSERT(t.length()<ACMFORMATDETAILS_FORMAT_CHARS);

	if(t.length()<cbBuf) {
		HMC::TStringW<ACMFORMATDETAILS_FORMAT_CHARS> w(t);
		wcscpy(szFormat,w);
		return w.length();
	} else {
		wcscpy(szFormat,L"");
		return 0;
	}
}

/*****************************************************************
	vorbis.acmドライバ
*****************************************************************/
/*****************************************************************
	オブジェクトの作成、削除
*****************************************************************/
void CacmDriverOggVorbis::init()
{
}
void CacmDriverOggVorbis::destroy()
{
	BASE::destroy();
	init();
}
// ドライバインスタンスを::operator new()で確保する場合、プロセス終了時、
// DRV_CLOSE処理以前に_CRT_INITによってメモリが開放されてしまう場合がある
// (_CRT_INITは存在するインスタンスを単にメモリリークとして処理してしまう)
// 消滅したドライバインスタンスに対してドライバメッセージが送信されると
// W9xの場合システムをクラッシュさせることがあるので非常に注意が必要
void *CacmDriverOggVorbis::operator new(size_t dwBytes)
{
	HANDLE hHeap = ::GetProcessHeap();
	LPVOID pv = ::HeapAlloc(hHeap,0L,dwBytes);
	return pv;	// GetLastError()してはいけないことに注意
}
void CacmDriverOggVorbis::operator delete(void *pvMem)
{
	HANDLE hHeap = ::GetProcessHeap();
	BOOL b = ::HeapFree(hHeap,0L,pvMem);
	CHECKRESULT_API(b);
}
/*****************************************************************
	ディスパッチャ
*****************************************************************/
LRESULT CacmDriverOggVorbis::DriverProc(DWORD dwId, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
#ifdef _DEBUG
	if(uMsg==DRV_OPEN) {
		TRACE("%08x,%08x,DRV_OPEN ,%08x,%08x\n",dwId,hdrvr,lParam1,lParam2);
	}
	if(uMsg==DRV_CLOSE) {
		TRACE("%08x,%08x,DRV_CLOSE,%08x,%08x\n",dwId,hdrvr,lParam1,lParam2);
	}
#endif
	LRESULT lRes = 0L;
	LPTYPE p = LPTYPE(dwId);
	do {
		if(uMsg==DRV_LOAD) {
			lRes = 1L;			// 0以外の値を返すと成功
			break;
		} else if(uMsg==DRV_OPEN) {
			LPACMDRVOPENDESC paod = (LPACMDRVOPENDESC)lParam2;
			if(paod) {
				if(paod->fccType!=ACMDRIVERDETAILS_FCCTYPE_AUDIOCODEC) {
					lRes = 0L;	// 無効なCODECタイプ
					break;
				}
			}
			p = new TYPE();		// 新規インスタンス作成
			if(!p) {
				if(paod) {
					paod->dwError = MMSYSERR_NOMEM;
				}
				lRes = 0L;		// 新規インスタンスの作成に失敗した場合は0を返す
				break;
			}
			p->map(hdrvr);		// ハンドルと関連付け(CDrvrがclose()するのを防ぐため非オーナーとする)
		}
		if(p) {
			if(p->handle()==hdrvr) {
				lRes = p->driverProc(uMsg,lParam1,lParam2);
				if(uMsg==DRV_OPEN) {
					lRes = (LRESULT)p;	// 返り値は次回からdwIdとして使われる
				} else if(uMsg==DRV_CLOSE) {
					delete p;	// インスタンス削除
				}
			} else {
				lRes = MMSYSERR_INVALHANDLE;
			}
		} else {
			lRes = BASE::DriverProc(dwId,hdrvr,uMsg,lParam1,lParam2);
		}
	} while(0);
	return lRes;
}
/*****************************************************************
	ハンドラ
*****************************************************************/
LRESULT CacmDriverOggVorbis::dmConfigure(LPARAM lParam1, LPARAM lParam2)
{
	LRESULT lRes = DRVCNF_OK;
#if 0
	HWND hwnd = (HWND)lParam1;
	LPDRVCONFIGINFO pdci = (LPDRVCONFIGINFO)lParam2;
	HMC::CDialog dlg;
	HMODULE hModule = ::GetDriverModuleHandle(handle());
	int i = dlg.run(hwnd,hModule,MAKEINTRESOURCE(IDD_CONFIG));
	switch(i) {
	case IDOK:
		lRes = DRVCNF_RESTRAT;
		break;
	case IDCANCEL:
		lRes = DRVCNF_CANCEL;
		break;
	}
#endif
	return lRes;
}
LRESULT CacmDriverOggVorbis::dmQueryConfigure(LPARAM lParam1, LPARAM lParam2)
{
	//return 1L;	// 設定ダイアログをサポートする
	return 0L;	// 設定ダイアログをサポートしない
}
LRESULT CacmDriverOggVorbis::dmDriverDetails(LPARAM lParam1, LPARAM lParam2)
{
	LPACMDRIVERDETAILS padd = (LPACMDRIVERDETAILS)lParam1;
	ACMDRIVERDETAILS add = { min(padd->cbStruct,sizeof(ACMDRIVERDETAILS)) };
	add.fccType		= ACMDRIVERDETAILS_FCCTYPE_AUDIOCODEC;
	add.fccComp		= ACMDRIVERDETAILS_FCCCOMP_UNDEFINED;
	add.wMid		= MM_HMS;
	add.wPid		= MM_HMS_VORBIS;
	add.vdwACM		= MAKE_ACM_VERSION(3,50,0);
	add.vdwDriver	= MAKE_ACM_VERSION(4, 0,0);
	add.fdwSupport	= ACMDRIVERDETAILS_SUPPORTF_CODEC;
	add.cFormatTags	= ARRAYLEN(aFormatTagIndexToTag);	// サポートするフォーマット(PCM,OGG)
	add.cFilterTags	= 0;								// サポートするフィルタ(無し)
	add.hicon		= ::LoadIcon(::GetDriverModuleHandle(handle()),MAKEINTRESOURCE(0));
	wcscpy(add.szShortName	,L"Ogg Vorbis");
	wcscpy(add.szLongName	,L"Ogg Vorbis Audio CODEC");
	wcscpy(add.szCopyright	,L"Copyright (C)2001 H.Mutsuki");
	swprintf(add.szLicensing,L"Xiphophorus libVorbis I %08x\nCopyright (C)2001 Xiphophorus",LIBVORBIS_VERSION);
	wcscpy(add.szFeatures	,L"Compresses and decompresses Ogg Vorbis audio data.\nThis software is based on a code of 'libVorbis'.");
	memcpy(padd,&add,add.cbStruct);
	return MMSYSERR_NOERROR;
}
LRESULT CacmDriverOggVorbis::dmDriverAbout(LPARAM lParam1, LPARAM lParam2)
{
	HWND hwnd = (HWND)lParam1;
	LRESULT lRes = MMSYSERR_NOERROR;
	do {
		// hwnd==-1の場合は問い合わせのみ
		if(hwnd==(HWND)-1) {
			lRes = MMSYSERR_NOTSUPPORTED;
			break;
		} else {
			lRes = MMSYSERR_NOTSUPPORTED;
			//lRes = MMSYSERR_NOTENABLED;
#if 0
			HMC::CDialog dlg;
			HMODULE hModule = ::GetDriverModuleHandle(handle());
			dlg.run(hwnd,hModule,MAKEINTRESOURCE(IDD_ABOUT));
#endif
			break;
		}
	} while(0);
	return lRes;
}
LRESULT CacmDriverOggVorbis::dmFormatSuggest(LPARAM lParam1, LPARAM lParam2)
{
	LPACMDRVFORMATSUGGEST padfs = (LPACMDRVFORMATSUGGEST)lParam1;
	LPWAVEFORMATEX pwfxSrc = padfs->pwfxSrc;
	LPWAVEFORMATEX pwfxDst = padfs->pwfxDst;
	DWORD fdwSuggest = padfs->fdwSuggest & ACM_FORMATSUGGESTF_TYPEMASK;

	LRESULT lRes = MMSYSERR_NOERROR;
	do {
		if(fdwSuggest & ~(ACM_FORMATSUGGESTF_WFORMATTAG |
						  ACM_FORMATSUGGESTF_NCHANNELS |
						  ACM_FORMATSUGGESTF_NSAMPLESPERSEC |
						  ACM_FORMATSUGGESTF_WBITSPERSAMPLE)) {
			lRes = MMSYSERR_NOTSUPPORTED;
			break;
		}
		// PCM <-> OGG 共通
		if(fdwSuggest & ACM_FORMATSUGGESTF_NCHANNELS) {
			if(pwfxDst->nChannels != pwfxSrc->nChannels) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
		} else {
			pwfxDst->nChannels = pwfxSrc->nChannels;
		}
		if(fdwSuggest & ACM_FORMATSUGGESTF_NSAMPLESPERSEC) {
			if(pwfxDst->nSamplesPerSec != pwfxSrc->nSamplesPerSec) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
		} else {
			pwfxDst->nSamplesPerSec = pwfxSrc->nSamplesPerSec;
		}
		if(fdwSuggest & ACM_FORMATSUGGESTF_WBITSPERSAMPLE) {
			if(pwfxDst->wBitsPerSample != pwfxSrc->wBitsPerSample) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
		} else {
			pwfxDst->wBitsPerSample = pwfxSrc->wBitsPerSample;
		}
		// PCM -> OGG 変換(エンコード)の場合
		if(pwfxSrc->wFormatTag==WAVE_FORMAT_PCM) {
			if(!pcmIsValidFormat(pwfxSrc)) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
			if(fdwSuggest & ACM_FORMATSUGGESTF_WFORMATTAG) {
				if(!(pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS1 ||
					 pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS2 ||
					 pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS3 ||
					 pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS1P||
					 pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS2P||
					 pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS3P)) {
					lRes = ACMERR_NOTPOSSIBLE;
					break;
				}
			} else {
				pwfxDst->wFormatTag = WAVE_FORMAT_VORBIS2P;				// mode2+
			}
			pwfxDst->nAvgBytesPerSec = oggAvgBytesPerSec(pwfxDst,4);	// Q:0.4
			pwfxDst->nBlockAlign     = 1;//HMC::GetBytesPerSample(pwfxDst);
			DWORD n = 0;
			for(; n<ARRAYLEN(aFormatTagIndexToTag); n++) {
				if(pwfxDst->wFormatTag==aFormatTagIndexToTag[n]) {
					pwfxDst->cbSize = (WORD)(aFormatTagIndexToSize[n] - sizeof(WAVEFORMATEX));
					break;
				}
			}
			ASSERT(n<ARRAYLEN(aFormatTagIndexToTag));
			// WAVE_FORMAT_VORBIS エキストラバイト充填
			if(!oggFormatMakeupExtraByte(pwfxDst)) {
				lRes = ACMERR_NOTPOSSIBLE;
			}
		}
		// OGG -> PCM 変換(デコード)の場合
		else if(pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS1 ||
				pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS2 ||
				pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS3 ||
				pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS1P||
				pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS2P||
				pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS3P) {
			if(!oggIsValidFormat(pwfxSrc)) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
			if(fdwSuggest & ACM_FORMATSUGGESTF_WFORMATTAG) {
				if(pwfxDst->wFormatTag != WAVE_FORMAT_PCM) {
					lRes = ACMERR_NOTPOSSIBLE;
					break;
				}
			} else {
				pwfxDst->wFormatTag = WAVE_FORMAT_PCM;
			}
			pwfxDst->nAvgBytesPerSec = HMC::GetBytesPerSec(pwfxDst);
			pwfxDst->nBlockAlign     = HMC::GetBytesPerSample(pwfxDst);
		}
	} while(0);
	return lRes;
}
LRESULT CacmDriverOggVorbis::dmFormatTagDetails(LPARAM lParam1, LPARAM lParam2)
{
	LPACMFORMATTAGDETAILS padft = (LPACMFORMATTAGDETAILS)lParam1;
	DWORD fdwDetails = (DWORD)lParam2 & ACM_FORMATTAGDETAILSF_QUERYMASK;
	LRESULT lRes = MMSYSERR_NOERROR;
	do {
		// フォーマットのみを決定するための分岐(fdwDetailsはフラグではないらしい)
		if(fdwDetails==ACM_FORMATTAGDETAILSF_INDEX) {
			if(padft->dwFormatTagIndex<ARRAYLEN(aFormatTagIndexToTag)) {
				padft->dwFormatTag = aFormatTagIndexToTag[padft->dwFormatTagIndex];
			} else {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
		} else if(fdwDetails==ACM_FORMATTAGDETAILSF_LARGESTSIZE) {
			if(padft->dwFormatTag==WAVE_FORMAT_PCM) {
			} else if(padft->dwFormatTag==WAVE_FORMAT_UNKNOWN) {
				padft->dwFormatTag = WAVE_FORMAT_VORBIS2;	// WAVE_FORMAT_VORBIS2が一番大きい(いいのか？)
			} else if(padft->dwFormatTag==WAVE_FORMAT_VORBIS1 ||
					  padft->dwFormatTag==WAVE_FORMAT_VORBIS2 ||
					  padft->dwFormatTag==WAVE_FORMAT_VORBIS3 ||
					  padft->dwFormatTag==WAVE_FORMAT_VORBIS1P||
					  padft->dwFormatTag==WAVE_FORMAT_VORBIS2P||
					  padft->dwFormatTag==WAVE_FORMAT_VORBIS3P) {
			} else {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
		} else if(fdwDetails==ACM_FORMATTAGDETAILSF_FORMATTAG) {
			if(!(padft->dwFormatTag==WAVE_FORMAT_PCM ||
				 padft->dwFormatTag==WAVE_FORMAT_VORBIS1 ||
				 padft->dwFormatTag==WAVE_FORMAT_VORBIS2 ||
				 padft->dwFormatTag==WAVE_FORMAT_VORBIS3 ||
				 padft->dwFormatTag==WAVE_FORMAT_VORBIS1P||
				 padft->dwFormatTag==WAVE_FORMAT_VORBIS2P||
				 padft->dwFormatTag==WAVE_FORMAT_VORBIS3P)) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
		} else {
			lRes = MMSYSERR_NOTSUPPORTED;
			break;
		}
		// フォーマット詳細を充填
		DWORD n = 0;
		for(; n<ARRAYLEN(aFormatTagIndexToTag); n++) {
			if(padft->dwFormatTag==aFormatTagIndexToTag[n]) {
				padft->dwFormatTagIndex = n;
				padft->dwFormatTag		= aFormatTagIndexToTag[n];
				padft->cbFormatSize		= aFormatTagIndexToSize[n];
				padft->fdwSupport		= ACMDRIVERDETAILS_SUPPORTF_CODEC;
				padft->cStandardFormats	= aFormatTagIndexToFormats[n];
				wcscpy(padft->szFormatTag,aFormatTagIndexToName[n]);
				break;
			}
		}
		if(!(n<ARRAYLEN(aFormatTagIndexToTag))) {
			lRes = ACMERR_NOTPOSSIBLE;
			break;
		}
		padft->cbStruct = min(padft->cbStruct,sizeof(*padft));
	} while(0);
	return lRes;
}
LRESULT CacmDriverOggVorbis::dmFormatDetails(LPARAM lParam1, LPARAM lParam2)
{
	LPACMFORMATDETAILS padf = (LPACMFORMATDETAILS)lParam1;
	DWORD fdwDetails = (DWORD)lParam2 & ACM_FORMATDETAILSF_QUERYMASK;
	LRESULT lRes = MMSYSERR_NOERROR;
	do {
		if(fdwDetails==ACM_FORMATDETAILSF_INDEX) {
			LPWAVEFORMATEX pwfx = padf->pwfx;
			DWORD dwFormatIndex = padf->dwFormatIndex;
			if(padf->dwFormatTag==WAVE_FORMAT_PCM) {
				if(!(dwFormatIndex<ARRAYLEN(aPCMFormatIndexToDetail))) {
					lRes = ACMERR_NOTPOSSIBLE;
					break;
				}
				// WAVE_FORMAT_PCM 詳細充填
				padf->cbStruct			= min(padf->cbStruct,sizeof(*padf));
				padf->fdwSupport		= ACMDRIVERDETAILS_SUPPORTF_CODEC;
				pwfx->wFormatTag		= (WORD)padf->dwFormatTag;	// WAVE_FORMAT_PCM;
				pwfx->nSamplesPerSec	= aPCMFormatIndexToDetail[dwFormatIndex].nSamplesPerSec;
				pwfx->nChannels			= aPCMFormatIndexToDetail[dwFormatIndex].nChannels;
				pwfx->wBitsPerSample	= aPCMFormatIndexToDetail[dwFormatIndex].wBitsPerSample;
				pwfx->wBitsPerSample	= pwfx->wBitsPerSample;
				pwfx->nAvgBytesPerSec	= HMC::GetBytesPerSample(pwfx);
				FormatFormatDetail(padf->szFormat,ARRAYLEN(padf->szFormat),&aPCMFormatIndexToDetail[dwFormatIndex]);
			} else if(padf->dwFormatTag==WAVE_FORMAT_VORBIS1 ||
					  padf->dwFormatTag==WAVE_FORMAT_VORBIS2 ||
					  padf->dwFormatTag==WAVE_FORMAT_VORBIS3 ||
					  padf->dwFormatTag==WAVE_FORMAT_VORBIS1P||
					  padf->dwFormatTag==WAVE_FORMAT_VORBIS2P||
					  padf->dwFormatTag==WAVE_FORMAT_VORBIS3P) {
				if(!(dwFormatIndex<ARRAYLEN(aOggFormatIndexToDetail))) {
					lRes = ACMERR_NOTPOSSIBLE;
					break;
				}
				// WAVE_FORMAT_VORBIS 詳細充填
				padf->cbStruct			= min(padf->cbStruct,sizeof(*padf));
				padf->fdwSupport		= ACMDRIVERDETAILS_SUPPORTF_CODEC;
				pwfx->wFormatTag		= (WORD)padf->dwFormatTag;	// VORBIS1,VORBIS2,VORBIS3
				pwfx->nSamplesPerSec	= aOggFormatIndexToDetail[dwFormatIndex].nSamplesPerSec;
				pwfx->nChannels			= aOggFormatIndexToDetail[dwFormatIndex].nChannels;
				pwfx->wBitsPerSample	= aOggFormatIndexToDetail[dwFormatIndex].wBitsPerSample;
				pwfx->nAvgBytesPerSec	= aOggFormatIndexToDetail[dwFormatIndex].nAvgBytesPerSec;
				pwfx->nBlockAlign		= 1;//HMC::GetBytesPerSample(pwfx);
				FormatFormatDetail(padf->szFormat,ARRAYLEN(padf->szFormat),&aOggFormatIndexToDetail[dwFormatIndex]);
				DWORD n = 0;
				for(; n<ARRAYLEN(aFormatTagIndexToTag); n++) {
					if(padf->dwFormatTag==aFormatTagIndexToTag[n]) {
						pwfx->cbSize = (WORD)(aFormatTagIndexToSize[n] - sizeof(WAVEFORMATEX));
						break;
					}
				}
				ASSERT(n<ARRAYLEN(aFormatTagIndexToTag));
				// WAVE_FORMAT_VORBIS エキストラバイト充填
				if(!oggFormatMakeupExtraByte(pwfx)) {
					lRes = ACMERR_NOTPOSSIBLE;
				}
			} else {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
		} else if(fdwDetails==ACM_FORMATDETAILSF_FORMAT) {
			LPWAVEFORMATEX pwfx = padf->pwfx;
			WORD wFormatTag = pwfx->wFormatTag;
			if(wFormatTag==WAVE_FORMAT_PCM) {
				if(!pcmIsValidFormat(pwfx)) {
					lRes = ACMERR_NOTPOSSIBLE;
					break;
				}
				wcscpy(padf->szFormat,L"");
			} else if(wFormatTag==WAVE_FORMAT_VORBIS1 ||
					  wFormatTag==WAVE_FORMAT_VORBIS2 ||
					  wFormatTag==WAVE_FORMAT_VORBIS3 ||
					  wFormatTag==WAVE_FORMAT_VORBIS1P||
					  wFormatTag==WAVE_FORMAT_VORBIS2P||
					  wFormatTag==WAVE_FORMAT_VORBIS3P) {
				if(!oggIsValidFormat(pwfx)) {
					lRes = ACMERR_NOTPOSSIBLE;
					break;
				}
				wcscpy(padf->szFormat,L"");
			} else {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
		} else {
			lRes = MMSYSERR_NOTSUPPORTED;
			break;
		}
	} while(0);
	return lRes;
}
LRESULT CacmDriverOggVorbis::dmStreamOpen(LPARAM lParam1, LPARAM lParam2)
{
	LPACMDRVSTREAMINSTANCE padsi = (LPACMDRVSTREAMINSTANCE)lParam1;
	LRESULT lRes = MMSYSERR_NOERROR;
	do {
		// このCODECは非同期変換をサポートしない
		if(padsi->fdwOpen & ACM_STREAMOPENF_ASYNC) {
			lRes = ACMERR_NOTPOSSIBLE;
			break;
		}
		// レート、チャネル、ビット変換はサポートしない
		// ※ここで弾かないとフォーマット一覧に全てのフォーマットが列挙される
		if(!(padsi->pwfxDst->nSamplesPerSec==padsi->pwfxSrc->nSamplesPerSec &&
			 padsi->pwfxDst->nChannels     ==padsi->pwfxSrc->nChannels &&
			 padsi->pwfxDst->wBitsPerSample==padsi->pwfxSrc->wBitsPerSample)) {
			lRes = ACMERR_NOTPOSSIBLE;
			break;
		}
		if(padsi->pwfxSrc->wFormatTag==WAVE_FORMAT_PCM) {
			if(!(padsi->pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS1 ||
				 padsi->pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS2 ||
				 padsi->pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS3 ||
				 padsi->pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS1P||
				 padsi->pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS2P||
				 padsi->pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS3P)) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
			// PCM -> OGG 変換(エンコード)の場合
			if(!pcmIsValidFormat(padsi->pwfxSrc) || !oggIsValidFormat(padsi->pwfxDst)) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
			if(padsi->fdwOpen & ACM_STREAMOPENF_QUERY) {
				break;	// 問い合わせのみの場合はここまでで終了
			}
			// 新規エンコードインスタンス作成
			COggVorbisEncode *p = new COggVorbisEncode(this);
			if(!p) {
				lRes = MMSYSERR_NOMEM;
				break;
			}
			lRes = p->Create(padsi->pwfxSrc,padsi->pwfxDst);
			if(lRes==MMSYSERR_NOERROR) {
				// ストリームインスタンス更新
				ASSERT(padsi->dwDriver==0);
				padsi->dwDriver  = (DWORD)p;
				padsi->fdwDriver = STREAMMODE_VORBIS_ENCODE;
			} else {
				delete p;
			}
		} else if(padsi->pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS1 ||
				  padsi->pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS2 ||
				  padsi->pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS3 ||
				  padsi->pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS1P||
				  padsi->pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS2P||
				  padsi->pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS3P) {
			// OGG -> PCM 変換(デコード)の場合
			if(padsi->pwfxDst->wFormatTag!=WAVE_FORMAT_PCM) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
			if(!oggIsValidFormat(padsi->pwfxSrc) || !pcmIsValidFormat(padsi->pwfxDst)) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
			if(padsi->fdwOpen & ACM_STREAMOPENF_QUERY) {
				break;	// 問い合わせのみの場合はここまでで終了
			}
			// 新規デコードインスタンス作成
			COggVorbisDecode *p = new COggVorbisDecode(this);
			if(!p) {
				lRes = MMSYSERR_NOMEM;
				break;
			}
			lRes = p->Create(padsi->pwfxSrc,padsi->pwfxDst);
			if(lRes==MMSYSERR_NOERROR) {
				// ストリームインスタンス更新
				ASSERT(padsi->dwDriver==0);
				padsi->dwDriver  = (DWORD)p;
				padsi->fdwDriver = STREAMMODE_VORBIS_DECODE;
			} else {
				delete p;
			}
		} else {
			// 未定義の変換の場合
			lRes = ACMERR_NOTPOSSIBLE;
			break;
		}
	} while(0);
	return lRes;
}
LRESULT CacmDriverOggVorbis::dmStreamClose(LPARAM lParam1, LPARAM lParam2)
{
	LPACMDRVSTREAMINSTANCE padsi = (LPACMDRVSTREAMINSTANCE)lParam1;
	LRESULT lRes = MMSYSERR_NOERROR;
	if(padsi->fdwDriver==STREAMMODE_VORBIS_ENCODE) {
		COggVorbisEncode *p = (COggVorbisEncode *)padsi->dwDriver;
		ASSERT(p);
		padsi->dwDriver  = 0;
		padsi->fdwDriver = 0;
		delete p;
	}
	else if(padsi->fdwDriver==STREAMMODE_VORBIS_DECODE) {
		COggVorbisDecode *p = (COggVorbisDecode *)padsi->dwDriver;
		ASSERT(p);
		padsi->dwDriver  = 0;
		padsi->fdwDriver = 0;
		delete p;
	}
	else {
		lRes = MMSYSERR_NOTENABLED;
	}
	return lRes;
}
LRESULT CacmDriverOggVorbis::dmStreamSize(LPARAM lParam1, LPARAM lParam2)
{
	LPACMDRVSTREAMINSTANCE padsi = (LPACMDRVSTREAMINSTANCE)lParam1;
	LPACMDRVSTREAMSIZE padss = (LPACMDRVSTREAMSIZE)lParam2;
	LRESULT lRes = MMSYSERR_NOERROR;
	DWORD cbDstPerSec = HMC::GetAvgBytesPerSec(padsi->pwfxDst);
	DWORD cbSrcPerSec = HMC::GetAvgBytesPerSec(padsi->pwfxSrc);
	DWORD cbDstPerUnit = HMC::GetBytesPerSample(padsi->pwfxDst);
	DWORD cbSrcPerUnit = HMC::GetBytesPerSample(padsi->pwfxSrc);
	DWORD fdwSize = padss->fdwSize & ACM_STREAMSIZEF_QUERYMASK;
	do {
		// サポートするフォーマットであることをチェック
		if(padsi->pwfxSrc->wFormatTag==WAVE_FORMAT_PCM) {
			if(!(pcmIsValidFormat(padsi->pwfxSrc) && oggIsValidFormat(padsi->pwfxDst))) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
		} else if(padsi->pwfxDst->wFormatTag==WAVE_FORMAT_PCM) {
			if(!(oggIsValidFormat(padsi->pwfxSrc) && pcmIsValidFormat(padsi->pwfxDst))) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
		} else {
			lRes = ACMERR_NOTPOSSIBLE;
			break;
		}
		// アライメントに満たない場合はエラーを返す(条件が<=であることに注意)
		if(fdwSize==ACM_STREAMSIZEF_SOURCE) {
			if(padss->cbSrcLength<=cbSrcPerUnit) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
		} else if(fdwSize==ACM_STREAMSIZEF_DESTINATION) {
			if(padss->cbDstLength<=cbDstPerUnit) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
		} else {
			lRes = ACMERR_NOTPOSSIBLE;
			break;
		}
		// SOURCE/DESTINATINの場合でそれぞれ算出
		if(fdwSize==ACM_STREAMSIZEF_SOURCE) {
			DWORD cbDstLength = (DWORD)((ULONGLONG)padss->cbSrcLength * cbDstPerSec / cbSrcPerSec);
			cbDstLength = cbDstLength * 2;	// レート変動に対応するために比を調整
			if(cbDstLength<cbDstPerUnit) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
			padss->cbDstLength = ALIGN(cbDstLength,cbDstPerUnit);
		} else if(fdwSize==ACM_STREAMSIZEF_DESTINATION) {
			DWORD cbSrcLength = (DWORD)((ULONGLONG)padss->cbDstLength * cbSrcPerSec / cbDstPerSec);
			cbSrcLength = cbSrcLength / 2;	// レート変動に対応するために比を調整
			if(cbSrcLength<cbSrcPerUnit) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
			padss->cbSrcLength = ALIGN(cbSrcLength,cbSrcPerUnit);
		}
	} while(0);
	ASSERT(lRes!=MMSYSERR_NOERROR || (cbSrcPerUnit<=padss->cbSrcLength && cbSrcPerUnit<=padss->cbDstLength));
	return lRes;
}
LRESULT CacmDriverOggVorbis::dmStreamConvert(LPARAM lParam1, LPARAM lParam2)
{
	LPACMDRVSTREAMINSTANCE padsi = (LPACMDRVSTREAMINSTANCE)lParam1;
	LPACMDRVSTREAMHEADER padsh = (LPACMDRVSTREAMHEADER)lParam2;
	LRESULT lRes = MMSYSERR_NOERROR;
	// Ogg Vorbis エンコード部
	if(padsi->fdwDriver==STREAMMODE_VORBIS_ENCODE) {
		COggVorbisEncode *p = (COggVorbisEncode *)padsi->dwDriver;
		ASSERT(p);
		lRes = p->ConvertCallback(padsi,padsh);
		if(lRes!=MMSYSERR_NOERROR) {
			padsi->dwDriver  = 0;
			padsi->fdwDriver = 0;
			delete p;
		}
	}
	// Ogg Vorbis デコード部
	else if(padsi->fdwDriver==STREAMMODE_VORBIS_DECODE) {
		COggVorbisDecode *p = (COggVorbisDecode *)padsi->dwDriver;
		ASSERT(p);
		lRes = p->ConvertCallback(padsi,padsh);
		if(lRes!=MMSYSERR_NOERROR) {
			padsi->dwDriver  = 0;
			padsi->fdwDriver = 0;
			delete p;
		}
	}
	else {
		lRes = MMSYSERR_NOTENABLED;
	}
	return lRes;
}
/*****************************************************************
*****************************************************************/

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
