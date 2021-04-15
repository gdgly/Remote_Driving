

#ifndef __EOLDATA_H

    #define __EOLDATA_H
    
    #include "TypeDef.h"
    
    typedef struct
    {
        uint16 numSizeUW;
        uint16 dataChkSumUW;
    }EOLData_infoEOLEngS_TYPE;

    #pragma CONST_SEG __GPAGE_SEG EOLENG_FLASH
        extern volatile const EOLData_infoEOLEngS_TYPE EOLData_infoEOLEngS;
    #pragma CONST_SEG DEFAULT

    typedef struct
    {
        uint16 numSizeUW;
    //Vehicle
        uint8 K2REI_VMCS0UB_CA[_KWP2000_LEN_K2REI_VMCS0];  //OEM�����ַ���0
        uint8 K2REI_VMECUSWVERUB_CA[_KWP2000_LEN_K2REI_VMECUSWVER];  //ECU����汾
        uint8 K2REI_VMCS1UB_CA[_KWP2000_LEN_K2REI_VMCS1];  //OEM�����ַ���1
        uint8 K2REI_VMVINUB_CA[_KWP2000_LEN_K2REI_VMVIN];  //����VIN��
        uint8 K2REI_VMECUHWNRUB_CA[_KWP2000_LEN_K2REI_VMECUHWNR];  //ECUӲ����
        uint8 K2REI_VMSYSNMUB_CA[_KWP2000_LEN_K2REI_VMSYSNM];  //�������ͺ�
        uint8 K2REI_VMTESTERSNUB_CA[_KWP2000_LEN_K2REI_VMTESTERSN];  //Vehicle Manufacturer Repair Shop Code or Tester Serial Number
        uint8 K2REI_VMPMDATEUB_CA[_KWP2000_LEN_K2REI_VMPMDATE];  //Vehicle Manufacturer Program Data
        uint8 K2REI_VMEGTSTDATEUB_CA[_KWP2000_LEN_K2REI_VMEGTSTDATE];  //Vehicle Manufacturer Engine Test Date
        uint8 K2REI_VMDSIDUB_CA[_KWP2000_LEN_K2REI_VMDSID];  //Vehicle Manufacturer Dataset Identification
        uint8 K2REI_VMCS2UB_CA[_KWP2000_LEN_K2REI_VMCS2];  //OEM�����ַ���2
        uint8 K2REI_VMCS3UB_CA[_KWP2000_LEN_K2REI_VMCS3];  //OEM�����ַ���3
        uint8 K2REI_VMCS4UB_CA[_KWP2000_LEN_K2REI_VMCS4];  //OEM�����ַ���4
        uint8 K2REI_VMCS5UB_CA[_KWP2000_LEN_K2REI_VMCS5];  //OEM�����ַ���5/
        uint16 dataChkSumUW;
    }EOLData_infoEOLVehS_TYPE;

    #pragma CONST_SEG __GPAGE_SEG EOLVEH_FLASH
        extern volatile const EOLData_infoEOLVehS_TYPE EOLData_infoEOLVehS;
    #pragma CONST_SEG DEFAULT

    typedef struct
    {
        uint16 numSizeUW;
        uint16 dataChkSumUW;
    }EOLData_infoEOLOtherS_TYPE;

    #pragma CONST_SEG __GPAGE_SEG EOLOTHER_FLASH
        extern volatile const EOLData_infoEOLOtherS_TYPE EOLData_infoEOLOtherS;
    #pragma CONST_SEG DEFAULT

    //Vehicle
    #define K2REI_VMCS0UB_CA EOLData_infoEOLVehS.K2REI_VMCS0UB_CA
    #define K2REI_VMECUSWVERUB_CA EOLData_infoEOLVehS.K2REI_VMECUSWVERUB_CA
    #define K2REI_VMCS1UB_CA EOLData_infoEOLVehS.K2REI_VMCS1UB_CA
    #define K2REI_VMVINUB_CA EOLData_infoEOLVehS.K2REI_VMVINUB_CA
    #define K2REI_VMECUHWNRUB_CA EOLData_infoEOLVehS.K2REI_VMECUHWNRUB_CA
    #define K2REI_VMSYSNMUB_CA EOLData_infoEOLVehS.K2REI_VMSYSNMUB_CA
    #define K2REI_VMTESTERSNUB_CA EOLData_infoEOLVehS.K2REI_VMTESTERSNUB_CA
    #define K2REI_VMPMDATEUB_CA EOLData_infoEOLVehS.K2REI_VMPMDATEUB_CA
    #define K2REI_VMEGTSTDATEUB_CA EOLData_infoEOLVehS.K2REI_VMEGTSTDATEUB_CA
    #define K2REI_VMDSIDUB_CA EOLData_infoEOLVehS.K2REI_VMDSIDUB_CA
    #define K2REI_VMCS2UB_CA EOLData_infoEOLVehS.K2REI_VMCS2UB_CA
    #define K2REI_VMCS3UB_CA EOLData_infoEOLVehS.K2REI_VMCS3UB_CA
    #define K2REI_VMCS4UB_CA EOLData_infoEOLVehS.K2REI_VMCS4UB_CA
    #define K2REI_VMCS5UB_CA EOLData_infoEOLVehS.K2REI_VMCS5UB_CA


#endif