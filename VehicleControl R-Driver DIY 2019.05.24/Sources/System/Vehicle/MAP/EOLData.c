/********************************************************************************
* �������ƣ��ײ�����
* �ļ����� EOLData.c
* �������� �� EOL����
* ���ü�¼��
* ��ע��   
* �޶���¼��
*		����		      �汾		  ����		����
*******************************************************************************/

#include "GlobalDef.h"
#include "EOLData.h"


//������EOL����
#pragma CONST_SEG __GPAGE_SEG EOLENG_FLASH

volatile const EOLData_infoEOLEngS_TYPE EOLData_infoEOLEngS=
{
  0xFFFF,//numSizeUW
  0xFFFF,//dataChkSumUW
};

#pragma CONST_SEG DEFAULT

#pragma CONST_SEG __GPAGE_SEG EOLVEH_FLASH

volatile const EOLData_infoEOLVehS_TYPE EOLData_infoEOLVehS=
{
  216,//numSizeUW
  "Customer0", //K2REI_VMCS0UB_CA[_KWP2000_LEN_K2REI_VMCS0],OEM�����ַ���0
  "000.000.00.000.00", //K2REI_VMECUSWVERUB_CA[_KWP2000_LEN_K2REI_VMECUSWVER],ECU�����汾��
  "Customer1", //K2REI_VMCS1UB_CA[_KWP2000_LEN_K2REI_VMCS1],OEM�����ַ���1
  "0000000000000000", //K2REI_VMVINUB_CA[_KWP2000_LEN_K2REI_VMVIN],����VIN��
  "000000000000", //K2REI_VMECUHWNRUB_CA[_KWP2000_LEN_K2REI_VMECUHWNR],ECUӲ����ʶ
  "Foton", //K2REI_VMSYSNMUB_CA[_KWP2000_LEN_K2REI_VMSYSNM],�������ͺ�
  "000000000", //K2REI_VMTESTERSNUB_CA[_KWP2000_LEN_K2REI_VMTESTERSN],Vehicle Manufacturer Repair Shop Code or Tester Serial Number
  {
    8,1,18
  }, //K2REI_VMPMDATEUB_CA[_KWP2000_LEN_K2REI_VMPMDATE],����ˢд����
  {
    8,1,18
  }, //K2REI_VMEGTSTDATEUB_CA[_KWP2000_LEN_K2REI_VMEGTSTDATE],��������������
  "0000000000", //K2REI_VMDSIDUB_CA[_KWP2000_LEN_K2REI_VMDSID],Vehicle Manufacturer Dataset Identification
  "Customer2", //K2REI_VMCS2UB_CA[_KWP2000_LEN_K2REI_VMCS2],OEM�����ַ���2
  "Customer3", //K2REI_VMCS3UB_CA[_KWP2000_LEN_K2REI_VMCS3],OEM�����ַ���3
  "Customer4", //K2REI_VMCS4UB_CA[_KWP2000_LEN_K2REI_VMCS4],OEM�����ַ���4
  "Customer5", //K2REI_VMCS5UB_CA[_KWP2000_LEN_K2REI_VMCS5],OEM�����ַ���5
  0xBE3D,//dataChkSumUW
};

#pragma CONST_SEG DEFAULT

#pragma CONST_SEG __GPAGE_SEG EOLOTHER_FLASH

volatile const EOLData_infoEOLOtherS_TYPE EOLData_infoEOLOtherS=
{
  0xFFFF,//numSizeUW
  0xFFFF,//dataChkSumUW
};

#pragma CONST_SEG DEFAULT
