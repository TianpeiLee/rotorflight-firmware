/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */
/*
 *   porting for ch32h41x by Temperslee    
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "platform.h"

#ifdef USE_ADC

#include "build/debug.h"

#include "drivers/dma_reqmap.h"

#include "drivers/io.h"
#include "io_impl.h"
#include "rcc.h"
#include "dma.h"

#include "drivers/sensor.h"

#include "adc.h"
#include "adc_impl.h"

#include "pg/adc.h"
#include "math.h"

#ifndef ADC1_DMA_STREAM
#define ADC1_DMA_STREAM NULL
#endif
#ifndef ADC2_DMA_STREAM
#define ADC2_DMA_STREAM NULL
#endif

// 独立的 DMA 缓冲区(32位),参照 betaflight,DMA 写这里再拷贝到 adcValues
static volatile DMA_DATA uint32_t adcConversionBuffer[ADC_CHANNEL_COUNT];

const adcDevice_t adcHardware[] = {
    {
        .ADCx = ADC1,
        .rccADC = RCC_HB2(ADC1),
#if !defined(USE_DMA_SPEC)
        .dmaResource = (dmaResource_t *)ADC1_DMA_STREAM,
#endif
    },
    {
        .ADCx = ADC2,
        .rccADC = RCC_HB2(ADC2),
#if !defined(USE_DMA_SPEC)
        .dmaResource = (dmaResource_t *)ADC2_DMA_STREAM,
#endif
    }
};

/* note these could be packed up for saving space */
const adcTagMap_t adcTagMap[] = {

    { DEFIO_TAG_E__PC0, ADC_DEVICES_12,     ADC_Channel_10 },
    { DEFIO_TAG_E__PC1, ADC_DEVICES_12,     ADC_Channel_11 },
    { DEFIO_TAG_E__PC2, ADC_DEVICES_12,     ADC_Channel_12 },
    { DEFIO_TAG_E__PC3, ADC_DEVICES_12,     ADC_Channel_13 },
    { DEFIO_TAG_E__PC4, ADC_DEVICES_12,     ADC_Channel_14 },

    { DEFIO_TAG_E__PA0, ADC_DEVICES_12,     ADC_Channel_0  },
    { DEFIO_TAG_E__PA1, ADC_DEVICES_12,     ADC_Channel_1  },
    { DEFIO_TAG_E__PA2, ADC_DEVICES_12,     ADC_Channel_2  },
    { DEFIO_TAG_E__PA3, ADC_DEVICES_12,     ADC_Channel_3  },
    { DEFIO_TAG_E__PA4, ADC_DEVICES_12,     ADC_Channel_4  },
    { DEFIO_TAG_E__PA5, ADC_DEVICES_12,     ADC_Channel_5  },
    { DEFIO_TAG_E__PA6, ADC_DEVICES_12,     ADC_Channel_6  },
    { DEFIO_TAG_E__PA7, ADC_DEVICES_12,     ADC_Channel_7  },
    
    { DEFIO_TAG_E__PB0, ADC_DEVICES_12,     ADC_Channel_8  },
    { DEFIO_TAG_E__PB1, ADC_DEVICES_12,     ADC_Channel_9  },
};

#define VREFINT_CAL_ADDR  0x1FFF7A2A
#define TS_CAL1_ADDR      0x1FFF7A2C
#define TS_CAL2_ADDR      0x1FFF7A2E

void adcInitDevice(ADC_TypeDef *adcdev, int channelCount)
{
    ADC_InitTypeDef ADC_InitStructure;

    ADC_StructInit(&ADC_InitStructure);
    
    ADC_InitStructure.ADC_Mode                     = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ContinuousConvMode       = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv         = ADC_ExternalTrigConv_None; //ADC_ExternalTrigConv_T1_CC1是否必要
    ADC_InitStructure.ADC_DataAlign                = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel             = channelCount;

    // Multiple injected channel seems to require scan conversion mode to be
    // enabled even if main (non-injected) channel count is 1.
#ifdef USE_ADC_INTERNAL
    ADC_InitStructure.ADC_ScanConvMode             = ENABLE;
#else
    ADC_InitStructure.ADC_ScanConvMode             = channelCount > 1 ? ENABLE : DISABLE; // 1=scan more that one channel in group
#endif
    ADC_Init(adcdev, &ADC_InitStructure);
}

#ifdef USE_ADC_INTERNAL
void adcInitInternalInjected(const adcConfig_t *config)
{
    ADC_TempSensorVrefintCmd(ENABLE);
    ADC_InjectedDiscModeCmd(ADC1, DISABLE);
    ADC_InjectedSequencerLengthConfig(ADC1, 2);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_Vrefint, 1, ADC_SampleTime_CyclesMode7);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_TempSensor, 2, ADC_SampleTime_CyclesMode7);

    // 配置注入组触发源为软件触发(JEXTSEL=JSWSTART=0x7000),
    // 否则 ADC_SoftwareStartInjectedConvCmd 不生效,JEOC 永不置位
    ADC_ExternalTrigInjectedConvConfig(ADC1, (uint32_t)0x7000);

    // adcVREFINTCAL = config->vrefIntCalibration ? config->vrefIntCalibration : *(uint16_t *)VREFINT_CAL_ADDR;
    // adcTSCAL1 = config->tempSensorCalibration1 ? config->tempSensorCalibration1 : *(uint16_t *)TS_CAL1_ADDR;
    // adcTSCAL2 = config->tempSensorCalibration2 ? config->tempSensorCalibration2 : *(uint16_t *)TS_CAL2_ADDR;

    // adcTSSlopeK = (110 - 30) * 1000 / (adcTSCAL2 - adcTSCAL1);

    adcVREFINTCAL = VREFINT_EXPECTED;
    // adcTSCAL1 needs to be the raw ADC value at 25C, but we can't adjust it for VREF+ because we haven't calculated it yet
    // So this will have to be aproximate (and the use should be fixed at some point)
    adcTSCAL1 = (TEMPSENSOR_CAL1_V * 4095.0f) / 3.3f;
    // TEMPSENSOR_SLOPE is given in mv/C.
    // adcTSSlopeK has the opposite sign compared to the CH32 standard because adcInternalComputeTemperature subtracts
    // the calibration value from the reading where as the CH32 docs subtract the reading from the calibration.
    // 3300/4095 converts the reading to mV and the factor of 1000 is needed to prevent the slope from rounding to 0.
    // The intermediate result when this is used is in mC, and adcInternalComputeTemperature then divides by 1000
    // to get an answer in C
    adcTSSlopeK = lrintf(-3300.0f*1000.0f/4095.0f/TEMPSENSOR_SLOPE);

}

// Note on sampling time for temperature sensor and vrefint:
// Both sources have minimum sample time of 10us.
// With prescaler = 8:
// 168MHz : fAPB2 = 84MHz, fADC = 10.5MHz, tcycle = 0.090us, 10us = 105cycle < 144cycle
// 240MHz : fAPB2 = 120MHz, fADC = 15.0MHz, tcycle = 0.067usk 10us = 150cycle < 480cycle
//
// 480cycles@15.0MHz = 32us

static bool adcInternalConversionInProgress = false;

bool adcInternalIsBusy(void)
{
    if (adcInternalConversionInProgress) {
        if (ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC) != RESET) {
            adcInternalConversionInProgress = false;
        }
    }

    return adcInternalConversionInProgress;
}

void adcInternalStartConversion(void)
{
    ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);
    ADC_SoftwareStartInjectedConvCmd(ADC1,ENABLE);
    adcInternalConversionInProgress = true;
}

uint16_t adcInternalReadVrefint(void)
{
    return ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
}

uint16_t adcInternalReadTempsensor(void)
{
    return ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2);
}
#endif

void adcInit(const adcConfig_t *config)
{
    uint8_t i;
    uint8_t configuredAdcChannels = 0;

    memset(&adcOperatingConfig, 0, sizeof(adcOperatingConfig));

    if (config->vbat.enabled) {
        adcOperatingConfig[ADC_BATTERY].tag = config->vbat.ioTag;
    }
    if (config->current.enabled) {
        adcOperatingConfig[ADC_CURRENT].tag = config->current.ioTag;
    }
    if (config->rssi.enabled) {
        adcOperatingConfig[ADC_RSSI].tag = config->rssi.ioTag;
    }
    if (config->vbec.enabled) {
        adcOperatingConfig[ADC_VBEC].tag = config->vbec.ioTag;
    }
    if (config->vbus.enabled) {
        adcOperatingConfig[ADC_VBUS].tag = config->vbus.ioTag;
    }
    if (config->vext.enabled) {
        adcOperatingConfig[ADC_VEXT].tag = config->vext.ioTag;
    }

    ADCDevice device = ADC_CFG_TO_DEV(config->device);

    if (device == ADCINVALID) {
        return;
    }

    adcDevice_t adc = adcHardware[device];

    bool adcActive = false;
    for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
        if (!adcVerifyPin(adcOperatingConfig[i].tag, device)) {
            continue;
        }

        adcActive = true;
        IOInit(IOGetByTag(adcOperatingConfig[i].tag), OWNER_ADC_BATT + i, 0);
        IOConfigGPIO(IOGetByTag(adcOperatingConfig[i].tag), IO_CONFIG(DIR_IN,GPIO_MODE_IN_AN, GPIO_SPEED_VERY_HIGH, GPIO_PULL_NONE));
        adcOperatingConfig[i].adcChannel = adcChannelByTag(adcOperatingConfig[i].tag);
        adcOperatingConfig[i].dmaIndex = configuredAdcChannels++;
        adcOperatingConfig[i].sampleTime = ADC_SampleTime_CyclesMode7;
        adcOperatingConfig[i].enabled = true;
    }

#ifndef USE_ADC_INTERNAL
    if (!adcActive) {
        return;
    }
#endif

    RCC_ADCCLKConfig(RCC_ADCCLKSource_HCLK);
	RCC_ADCHCLKCLKAsSourceConfig(RCC_PPRE2_DIV0,RCC_HCLK_ADCPRE_DIV8);



#ifdef USE_ADC_INTERNAL
    // If device is not ADC1 or there's no active channel, then initialize ADC1 separately
    if (device != ADCDEV_1 || !adcActive) {
        RCC_ClockCmd(adcHardware[ADCDEV_1].rccADC, ENABLE);
        adcInitDevice(ADC1, 2);
        ADC_Cmd(ADC1, ENABLE);
    }

    // Initialize for injected conversion
    adcInitInternalInjected(config);

    if (!adcActive) {
        return;
    }
#endif

    adcInitDevice(adc.ADCx, configuredAdcChannels);

    uint8_t rank = 1;
    for (i = 0; i < ADC_CHANNEL_COUNT; i++) {
        if (!adcOperatingConfig[i].enabled) {
            continue;
        }
        ADC_RegularChannelConfig(adc.ADCx, adcOperatingConfig[i].adcChannel, rank++, adcOperatingConfig[i].sampleTime);
    }


#ifdef USE_DMA_SPEC
    const dmaChannelSpec_t *dmaSpec = dmaGetChannelSpecByPeripheral(DMA_PERIPH_ADC, device, config->dmaopt[device]);

    if (!dmaSpec || !dmaAllocate(dmaGetIdentifier(dmaSpec->ref), OWNER_ADC, RESOURCE_INDEX(device))) {
        return;
    }

    dmaIdentifier_e dmaIdentifier = dmaGetIdentifier(dmaSpec->ref);
    dmaEnable(dmaIdentifier);
    xDMA_DeInit(dmaSpec->ref);
#else
    if (!dmaAllocate(dmaGetIdentifier(adc.dmaResource), OWNER_ADC, 0)) {
        return;
    }

    dmaEnable(dmaGetIdentifier(adc.dmaResource));

    xDMA_DeInit(adc.dmaResource);
#endif

    DMA_InitTypeDef DMA_InitStructure={0};

    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&adc.ADCx->RDATAR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)adcConversionBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = configuredAdcChannels;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = configuredAdcChannels > 1 ? DMA_MemoryInc_Enable : DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

#ifdef USE_DMA_SPEC
    xDMA_Init(dmaSpec->ref, &DMA_InitStructure);
    xDMA_Cmd(dmaSpec->ref, ENABLE);
    dmaMuxEnable(dmaGetIdentifier(dmaSpec->ref), dmaSpec->dmaMuxId);
#else
    xDMA_Init(adc.dmaResource, &DMA_InitStructure);
    xDMA_Cmd(adc.dmaResource, ENABLE);
    dmaMuxEnable(dmaGetIdentifier(dmaSpec->ref), dmaSpec->dmaMuxId);
#endif


    // adc.ADCx->CTLR1 |= (1<<31);
    // ADC_Cmd(adc.ADCx, ENABLE);
    // ADC_BufferCmd(adc.ADCx, ENABLE);
 
    // ADC_LowPowerModeCmd(adc.ADCx,DISABLE); 

    // ADC_TempSensorVrefintCmd(ENABLE);  //only ADC1

    // ADC_DMACmd(adc.ADCx, ENABLE);
    // ADC_Cmd(adc.ADCx, ENABLE);

    // ADC_SoftwareStartConvCmd(adc.ADCx,ENABLE);


    ADC_DMACmd(adc.ADCx, ENABLE);
    adc.ADCx->CTLR1 |= (1<<31);
    ADC_Cmd(adc.ADCx, ENABLE);
    ADC_BufferCmd(adc.ADCx, ENABLE);
    ADC_LowPowerModeCmd(adc.ADCx,DISABLE);
    ADC_TempSensorVrefintCmd(ENABLE);  //only ADC1
}

void adcGetChannelValues(void)
{
    for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
        if (adcOperatingConfig[i].enabled) {
            adcValues[adcOperatingConfig[i].dmaIndex] = adcConversionBuffer[adcOperatingConfig[i].dmaIndex];
        }
    }
}
#endif
