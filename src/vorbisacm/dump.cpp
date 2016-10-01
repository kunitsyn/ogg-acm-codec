/*
	Ogg Vorbis CODEC for MSACM
	Copyright (C)2001 H.Mutsuki
*/

#include "hmc/util.h"

/*****************************************************************
*****************************************************************/
#ifdef _HAVE_DUMPEDFORMATDETAILS
#include "codec.h"
#include "dump.inl"
#else
#include "hmc/mmutil.h"
#include "hmc/module.h"
#include "codec.h"
#include <msacm.h>
#pragma comment(lib, "msacm32.lib")
/*****************************************************************
	フォーマット詳細をダンプする
*****************************************************************/
// 開発用のエントリーにつき全然エラー処理していないので注意
// ローカルドライバーを扱うサンプルも兼ねている(つもり)です
// なるべくプレーンなコードで書いたつもりなので参考にして欲しい
int main(int argc, char **argv)
{
	MMRESULT h = MMSYSERR_NOERROR;

	// テスト用……
	if(1<argc) {
		::PlaySound(argv[1],NULL,SND_FILENAME|SND_SYNC);
		return 0;
	}

	// ローカルドライバーを開く
	HACMDRIVERID hadid = 0;
	HACMDRIVER had = NULL;
	HMODULE hmod = ::GetModuleHandle(NULL);	// 実際にローカルドライバーとして使用する時にはLoadLibrary()で取得する
	LPARAM  func = (LPARAM)DriverProc;		// 実際にローカルドライバーとして使用する時にはGetProcAddress()で取得する
	CHECKRESULT_MM(h=::acmDriverAdd(&hadid,hmod,func,0,ACM_DRIVERADDF_FUNCTION));
	CHECKRESULT_MM(h=::acmDriverOpen(&had,hadid,0));

	//
	// ここまでくればグローバルドライバと同じ仕組みで使用できる！
	//

	// 詳細取得用のメモリを取得
	DWORD cbFormatSize = 0;
	WAVEFORMATEX *pwfx = NULL;
	if(1) {
		ACMFORMATTAGDETAILS afd = {sizeof(afd)};
		afd.dwFormatTag = WAVE_FORMAT_UNKNOWN;
		CHECKRESULT_MM(h=::acmFormatTagDetailsW(had,&afd,ACM_FORMATTAGDETAILSF_LARGESTSIZE));
		cbFormatSize = afd.cbFormatSize;
		pwfx = (LPWAVEFORMATEX)alloca(cbFormatSize);
	}

	// サポートするフォーマットタグ数を取得
	DWORD cFormatTags = 0;
	if(1) {
		ACMDRIVERDETAILS add = {sizeof(add)};
		CHECKRESULT_MM(h=::acmDriverDetailsW(hadid,&add,0));
		cFormatTags = add.cFormatTags;
	}

	// ヘッダ出力
	printf("/*\n");
	printf("	Ogg Vorbis CODEC for MSACM\n");
	printf("	Copyright (C)2001 H.Mutsuki\n");
	printf("*/\n");
	printf("\n");
	printf("#include \"codec.h\"\n");
	printf("\n");
	printf("/*****************************************************************\n");
	printf("	フォーマット詳細のダンプデータ\n");
	printf("*****************************************************************/\n");
	printf("/*****************************************************************\n");
	printf("	※このファイルは機械的に生成されたものです(dump.cppを参照)\n");
	printf("*****************************************************************/\n");

	// タグ毎にフォーマット詳細を列挙
	for(DWORD cFormatTag=0; cFormatTag<cFormatTags; cFormatTag++) {
		ACMFORMATTAGDETAILS afd = {sizeof(afd)};
		afd.dwFormatTagIndex = cFormatTag;
		CHECKRESULT_MM(h=::acmFormatTagDetailsW(had,&afd,ACM_FORMATTAGDETAILSF_INDEX));
		for(DWORD cFormat=0; cFormat<afd.cStandardFormats; cFormat++) {
			// フォーマット詳細を取得
			ACMFORMATDETAILSW adf = {sizeof(adf)};
			adf.cbStruct		= sizeof(adf);
			adf.dwFormatIndex	= cFormat;
			adf.dwFormatTag		= afd.dwFormatTag;
			adf.fdwSupport		= 0;
			adf.pwfx			= pwfx;
			adf.cbwfx			= afd.cbFormatSize;
			memset(pwfx,0,afd.cbFormatSize);
			CHECKRESULT_MM(h=::acmFormatDetailsW(had,&adf,ACM_FORMATDETAILSF_INDEX));
			// 取得した詳細を標準出力に送出
			printf("static const WORD FormatDetail_%04x_%04x[] = {",cFormatTag,cFormat);
			ASSERT(ALIGNWORD(HMC::GetFormatSize(pwfx))==HMC::GetFormatSize(pwfx));	// ※WORD境界に整列している必要がある
			for(DWORD n=0,m=HMC::GetFormatSize(pwfx)/sizeof(WORD); n<m; n++) {
				printf("0x%04x,",((LPWORD)pwfx)[n]);
			}
			printf("};\n");
		}
	}

	// タグ毎に一覧を列挙
	for(DWORD cFormatTag=0; cFormatTag<cFormatTags; cFormatTag++) {
		ACMFORMATTAGDETAILS afd = {sizeof(afd)};
		afd.dwFormatTagIndex = cFormatTag;
		CHECKRESULT_MM(h=::acmFormatTagDetailsW(had,&afd,ACM_FORMATTAGDETAILSF_INDEX));
		printf("static CONST WAVEFORMATEX *aFormatDetails_%04x[] = {",cFormatTag);
		for(DWORD cFormat=0; cFormat<afd.cStandardFormats; cFormat++) {
			printf("(WAVEFORMATEX *)FormatDetail_%04x_%04x,",cFormatTag,cFormat);
		}
		printf("};\n");
	}

	// タグ一覧を列挙
	printf("const WAVEFORMATEX **aDumpedFormatDetails[] = {");
	for(DWORD cFormatTag=0; cFormatTag<cFormatTags; cFormatTag++) {
		ACMFORMATTAGDETAILS afd = {sizeof(afd)};
		afd.dwFormatTagIndex = cFormatTag;
		CHECKRESULT_MM(h=::acmFormatTagDetailsW(had,&afd,ACM_FORMATTAGDETAILSF_INDEX));
		printf("aFormatDetails_%04x,",cFormatTag);
	}
	printf("};\n");

	// フッタ
	printf("\n");
	printf("/*****************************************************************\n");
	printf("*****************************************************************/\n");
	printf("\n");
	printf("/* Local Variables:						*/\n");
	printf("/* tab-width:4							*/\n");
	printf("/* c-basic-offset:4						*/\n");
	printf("/* c-indent-level:4						*/\n");
	printf("/* c-label-offset:-4					*/\n");
	printf("/* c-continued-statement-offset:4		*/\n");
	printf("/* file-coding-system:*sjis*dos			*/\n");
	printf("/* buffer-file-coding-system:sjis-dos	*/\n");
	printf("/* End:									*/\n");

	// ローカルドライバーを閉じる
	CHECKRESULT_MM(h=::acmDriverClose(had,0));
	CHECKRESULT_MM(h=::acmDriverRemove(hadid,0));
	return h;
}
/*****************************************************************
*****************************************************************/
#endif	// _HAVE_DUMPEDFORMATDETAILS
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
