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
	�t�H�[�}�b�g�ڍׂ��_���v����
*****************************************************************/
// �J���p�̃G���g���[�ɂ��S�R�G���[�������Ă��Ȃ��̂Œ���
// ���[�J���h���C�o�[�������T���v�������˂Ă���(����)�ł�
// �Ȃ�ׂ��v���[���ȃR�[�h�ŏ���������Ȃ̂ŎQ�l�ɂ��ė~����
int main(int argc, char **argv)
{
	MMRESULT h = MMSYSERR_NOERROR;

	// �e�X�g�p�c�c
	if(1<argc) {
		::PlaySound(argv[1],NULL,SND_FILENAME|SND_SYNC);
		return 0;
	}

	// ���[�J���h���C�o�[���J��
	HACMDRIVERID hadid = 0;
	HACMDRIVER had = NULL;
	HMODULE hmod = ::GetModuleHandle(NULL);	// ���ۂɃ��[�J���h���C�o�[�Ƃ��Ďg�p���鎞�ɂ�LoadLibrary()�Ŏ擾����
	LPARAM  func = (LPARAM)DriverProc;		// ���ۂɃ��[�J���h���C�o�[�Ƃ��Ďg�p���鎞�ɂ�GetProcAddress()�Ŏ擾����
	CHECKRESULT_MM(h=::acmDriverAdd(&hadid,hmod,func,0,ACM_DRIVERADDF_FUNCTION));
	CHECKRESULT_MM(h=::acmDriverOpen(&had,hadid,0));

	//
	// �����܂ł���΃O���[�o���h���C�o�Ɠ����d�g�݂Ŏg�p�ł���I
	//

	// �ڍ׎擾�p�̃��������擾
	DWORD cbFormatSize = 0;
	WAVEFORMATEX *pwfx = NULL;
	if(1) {
		ACMFORMATTAGDETAILS afd = {sizeof(afd)};
		afd.dwFormatTag = WAVE_FORMAT_UNKNOWN;
		CHECKRESULT_MM(h=::acmFormatTagDetailsW(had,&afd,ACM_FORMATTAGDETAILSF_LARGESTSIZE));
		cbFormatSize = afd.cbFormatSize;
		pwfx = (LPWAVEFORMATEX)alloca(cbFormatSize);
	}

	// �T�|�[�g����t�H�[�}�b�g�^�O�����擾
	DWORD cFormatTags = 0;
	if(1) {
		ACMDRIVERDETAILS add = {sizeof(add)};
		CHECKRESULT_MM(h=::acmDriverDetailsW(hadid,&add,0));
		cFormatTags = add.cFormatTags;
	}

	// �w�b�_�o��
	printf("/*\n");
	printf("	Ogg Vorbis CODEC for MSACM\n");
	printf("	Copyright (C)2001 H.Mutsuki\n");
	printf("*/\n");
	printf("\n");
	printf("#include \"codec.h\"\n");
	printf("\n");
	printf("/*****************************************************************\n");
	printf("	�t�H�[�}�b�g�ڍׂ̃_���v�f�[�^\n");
	printf("*****************************************************************/\n");
	printf("/*****************************************************************\n");
	printf("	�����̃t�@�C���͋@�B�I�ɐ������ꂽ���̂ł�(dump.cpp���Q��)\n");
	printf("*****************************************************************/\n");

	// �^�O���Ƀt�H�[�}�b�g�ڍׂ��
	for(DWORD cFormatTag=0; cFormatTag<cFormatTags; cFormatTag++) {
		ACMFORMATTAGDETAILS afd = {sizeof(afd)};
		afd.dwFormatTagIndex = cFormatTag;
		CHECKRESULT_MM(h=::acmFormatTagDetailsW(had,&afd,ACM_FORMATTAGDETAILSF_INDEX));
		for(DWORD cFormat=0; cFormat<afd.cStandardFormats; cFormat++) {
			// �t�H�[�}�b�g�ڍׂ��擾
			ACMFORMATDETAILSW adf = {sizeof(adf)};
			adf.cbStruct		= sizeof(adf);
			adf.dwFormatIndex	= cFormat;
			adf.dwFormatTag		= afd.dwFormatTag;
			adf.fdwSupport		= 0;
			adf.pwfx			= pwfx;
			adf.cbwfx			= afd.cbFormatSize;
			memset(pwfx,0,afd.cbFormatSize);
			CHECKRESULT_MM(h=::acmFormatDetailsW(had,&adf,ACM_FORMATDETAILSF_INDEX));
			// �擾�����ڍׂ�W���o�͂ɑ��o
			printf("static const WORD FormatDetail_%04x_%04x[] = {",cFormatTag,cFormat);
			ASSERT(ALIGNWORD(HMC::GetFormatSize(pwfx))==HMC::GetFormatSize(pwfx));	// ��WORD���E�ɐ��񂵂Ă���K�v������
			for(DWORD n=0,m=HMC::GetFormatSize(pwfx)/sizeof(WORD); n<m; n++) {
				printf("0x%04x,",((LPWORD)pwfx)[n]);
			}
			printf("};\n");
		}
	}

	// �^�O���Ɉꗗ���
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

	// �^�O�ꗗ���
	printf("const WAVEFORMATEX **aDumpedFormatDetails[] = {");
	for(DWORD cFormatTag=0; cFormatTag<cFormatTags; cFormatTag++) {
		ACMFORMATTAGDETAILS afd = {sizeof(afd)};
		afd.dwFormatTagIndex = cFormatTag;
		CHECKRESULT_MM(h=::acmFormatTagDetailsW(had,&afd,ACM_FORMATTAGDETAILSF_INDEX));
		printf("aFormatDetails_%04x,",cFormatTag);
	}
	printf("};\n");

	// �t�b�^
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

	// ���[�J���h���C�o�[�����
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
