/***************************************************************************//**
 * @file main_single_differential_polled.c
 * @brief Use the IADC to take repeated blocking differential measurements on
 * single set of differential inputs
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Evaluation Quality
 * This code has been minimally tested to ensure that it builds and is suitable 
 * as a demonstration for evaluation purposes only. This code will be maintained
 * at the sole discretion of Silicon Labs.
 ******************************************************************************/

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_iadc.h"
#include "em_gpio.h"
#include "app.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// Set CLK_ADC to 10MHz
#define CLK_SRC_ADC_FREQ          10000000 // CLK_SRC_ADC
#define CLK_ADC_FREQ              10000000 // CLK_ADC - 10MHz max in normal mode

// When changing GPIO port/pins below, make sure to change xBUSALLOC macro's
// accordingly.
#define IADC_INPUT_0_BUS          CDBUSALLOC
#define IADC_INPUT_0_BUSALLOC     GPIO_CDBUSALLOC_CDEVEN0_ADC0
#define IADC_INPUT_1_BUS          CDBUSALLOC
#define IADC_INPUT_1_BUSALLOC     GPIO_CDBUSALLOC_CDODD0_ADC0

/*******************************************************************************
 ***************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

static volatile int32_t sample;
static volatile double singleResult; // Volts


void adc_disable(IADC_TypeDef *iadc)
{

  iadc->EN_CLR = IADC_EN_EN;

}

void adc_enable(IADC_TypeDef *iadc)
{
 iadc->EN_SET = IADC_EN_EN;
}



/**************************************************************************//**
 * @brief  Initialize IADC function
 *****************************************************************************/
void initIADC (void)
{
  // Declare init structs
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
  IADC_SingleInput_t initSingleInput = IADC_SINGLEINPUT_DEFAULT;

  // Enable IADC0 and GPIO clock branches
  CMU_ClockEnable(cmuClock_IADC0, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Reset IADC to reset configuration in case it has been modified by
  // other code
  IADC_reset(IADC0);

  // Select clock for IADC
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);  // FSRCO - 20MHz

  // Modify init structs and initialize
  init.warmup = iadcWarmupKeepWarm;

  // Set the HFSCLK prescale value here
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

  // Configuration 0 is used by both scan and single conversions by default
  // Use unbuffered AVDD as reference
  initAllConfigs.configs[0].reference = iadcCfgReferenceInt1V2;

  // Divides CLK_SRC_ADC to set the CLK_ADC frequency
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                             CLK_ADC_FREQ,
                                             0,
                                             iadcCfgModeNormal,
                                             init.srcClkPrescale);

  // Assign pins to positive and negative inputs in differential mode
  initSingleInput.posInput   = iadcPosInputAvdd;   //VDD/4
  initSingleInput.negInput   = iadcNegInputGnd;   // GND

  // Initialize the IADC
  IADC_init(IADC0, &init, &initAllConfigs);

  // Initialize the Single conversion inputs
  IADC_initSingle(IADC0, &initSingle, &initSingleInput);

  // Allocate the analog bus for ADC0 inputs
  GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;
  GPIO->IADC_INPUT_1_BUS |= IADC_INPUT_1_BUSALLOC;

  adc_disable(IADC0);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/


uint16_t star_adc_tran(void)
{
  adc_enable(IADC0);
  int32_t max_adc =0,min_adc =0xffff,Sum_adc =0;
  // Start IADC conversion
  for(uint8_t i=0; i<9;i++){
      IADC_command(IADC0, iadcCmdStartSingle);

      // Wait for conversion to be complete
      while((IADC0->STATUS & (_IADC_STATUS_CONVERTING_MASK
                  | _IADC_STATUS_SINGLEFIFODV_MASK)) != IADC_STATUS_SINGLEFIFODV); //while combined status bits 8 & 6 don't equal 1 and 0 respectively

      // Get ADC result
      sample = IADC_pullSingleFifoResult(IADC0).data;

      if(i>2){  //扔掉前3次
        if(sample >max_adc){
          max_adc = sample;
        }if(sample <min_adc){
          min_adc = sample;
        }
        Sum_adc +=sample;
      }
  //    printf("sample %ld\n",sample);

  }
  Sum_adc -=(max_adc + min_adc);  //采9次，扔3次+2次大小值，取4份平均值
  sample = Sum_adc/4;
  // Calculate input voltage:
  //  For differential inputs, the resultant range is from -Vref to +Vref, i.e.,
  //  for Vref = AVDD = 3.30V, 12 bits represents 6.60V full scale IADC range.
  //vref =1.2v    2.4
  singleResult = (sample * 4.8) / 0xFFF;

  adc_disable(IADC0);
//  printf("sample =%ld  singleResult = %f \n",sample,singleResult);
  return (uint16_t)(singleResult*1000);
}


/*
int main(void)
{
  CHIP_Init();

  // Initialize the IADC
  initIADC();

  // Infinite loop
  while(1)
  {
    // Start IADC conversion
    IADC_command(IADC0, iadcCmdStartSingle);

    // Wait for conversion to be complete
    while((IADC0->STATUS & (_IADC_STATUS_CONVERTING_MASK
                | _IADC_STATUS_SINGLEFIFODV_MASK)) != IADC_STATUS_SINGLEFIFODV); //while combined status bits 8 & 6 don't equal 1 and 0 respectively

    // Get ADC result
    sample = IADC_pullSingleFifoResult(IADC0).data;

    // Calculate input voltage:
    //  For differential inputs, the resultant range is from -Vref to +Vref, i.e.,
    //  for Vref = AVDD = 3.30V, 12 bits represents 6.60V full scale IADC range.
    singleResult = (sample * 6.6) / 0xFFF;
  }
}
*/
