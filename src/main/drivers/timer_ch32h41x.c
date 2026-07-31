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

#include "platform.h"

#ifdef USE_TIMER

#include "common/utils.h"

#include "drivers/dma.h"
#include "drivers/io.h"
#include "drivers/timer_def.h"

#include "rcc.h"
#include "timer.h"


const timerDef_t timerDefinitions[HARDWARE_TIMER_DEFINITION_COUNT] = {
    { .TIMx = TIM1,  .rcc = RCC_HB2(TIM1),  .inputIrq = TIM1_CC_IRQn },
    { .TIMx = TIM2,  .rcc = RCC_HB1(TIM2),  .inputIrq = TIM2_IRQn },
    { .TIMx = TIM3,  .rcc = RCC_HB1(TIM3),  .inputIrq = TIM3_IRQn },
    { .TIMx = TIM4,  .rcc = RCC_HB1(TIM4),  .inputIrq = TIM4_IRQn },
    { .TIMx = TIM5,  .rcc = RCC_HB1(TIM5),  .inputIrq = TIM5_IRQn },
    { .TIMx = TIM6,  .rcc = RCC_HB1(TIM6),  .inputIrq = TIM6_IRQn },
    { .TIMx = TIM7,  .rcc = RCC_HB1(TIM7),  .inputIrq = TIM7_IRQn },
    { .TIMx = TIM8,  .rcc = RCC_HB2(TIM8),  .inputIrq = TIM8_CC_IRQn },
    { .TIMx = TIM9,  .rcc = RCC_HB2(TIM9),  .inputIrq = TIM9_IRQn },
    { .TIMx = TIM10, .rcc = RCC_HB2(TIM10), .inputIrq = TIM10_IRQn },
    { .TIMx = TIM11, .rcc = RCC_HB2(TIM11), .inputIrq = TIM11_IRQn },
    { .TIMx = TIM12, .rcc = RCC_HB2(TIM12), .inputIrq = TIM12_IRQn },
};

#if defined(USE_TIMER_MGMT)
const timerHardware_t fullTimerHardware[FULL_TIMER_CHANNEL_COUNT] = {
    // Auto-generated from 'timer_def.h'


    DEF_TIM(TIM1,  CH1,  PE9,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM1,  CH1,  PA8,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM1,  CH2,  PE11, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM1,  CH2,  PA9,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM1,  CH3,  PE13, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM1,  CH3,  PA10, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM1,  CH4,  PE14, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM1,  CH4,  PA11, TIM_USE_ANY, 0, 0, 0),

    DEF_TIM(TIM1,  CH1N,  PE8,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM1,  CH1N,  PA7,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM1,  CH1N,  PB13, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM1,  CH2N,  PE10, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM1,  CH2N,  PB0,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM1,  CH2N,  PB14, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM1,  CH3N,  PE12, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM1,  CH3N,  PB1,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM1,  CH3N,  PB15, TIM_USE_ANY, 0, 0, 0),

    DEF_TIM(TIM8,  CH1,  PE3, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM8,  CH2,  PE4, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM8,  CH3,  PE5, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM8,  CH4,  PE6, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM8,  CH1,  PC6, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM8,  CH2,  PC7, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM8,  CH3,  PC8, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM8,  CH4,  PC9, TIM_USE_ANY, 0, 0, 0),

    DEF_TIM(TIM8,  CH1N,  PC1,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM8,  CH2N,  PC2,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM8,  CH3N,  PC3,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM8,  CH1N,  PA5,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM8,  CH2N,  PB14, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM8,  CH3N,  PB15, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM8,  CH1N,  PA7,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM8,  CH2N,  PB0,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM8,  CH3N,  PB1,  TIM_USE_ANY, 0, 0, 0),

    DEF_TIM(TIM2,  CH1,  PA0,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM2,  CH1,  PA5,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM2,  CH1,  PA15, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM2,  CH2,  PA1,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM2,  CH2,  PB3,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM2,  CH3,  PA2,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM2,  CH3,  PB10, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM2,  CH4,  PA3,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM2,  CH4,  PB11, TIM_USE_ANY, 0, 0, 0),

    DEF_TIM(TIM3,  CH1,  PA6, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM3,  CH1,  PB4, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM3,  CH1,  PC6, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM3,  CH1,  PD3, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM3,  CH2,  PA7, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM3,  CH2,  PB5, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM3,  CH2,  PC7, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM3,  CH2,  PD4, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM3,  CH3,  PB0, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM3,  CH3,  PC8, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM3,  CH3,  PD5, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM3,  CH4,  PB1, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM3,  CH4,  PC9, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM3,  CH4,  PD6, TIM_USE_ANY, 0, 0, 0),

    DEF_TIM(TIM4,  CH1,  PB6,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM4,  CH1,  PD12, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM4,  CH1,  PE3,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM4,  CH2,  PB7,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM4,  CH2,  PD13, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM4,  CH2,  PE4,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM4,  CH3,  PB8,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM4,  CH3,  PD14, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM4,  CH3,  PE5,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM4,  CH4,  PB9,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM4,  CH4,  PD15, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM4,  CH4,  PE6,  TIM_USE_ANY, 0, 0, 0),

    DEF_TIM(TIM5,  CH1,  PA0,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM5,  CH1,  PD12, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM5,  CH1,  PC1,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM5,  CH2,  PA1,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM5,  CH2,  PD13, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM5,  CH2,  PC2,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM5,  CH3,  PA2,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM5,  CH3,  PD14, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM5,  CH3,  PC3,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM5,  CH4,  PA3,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM5,  CH4,  PD15, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM5,  CH4,  PB0,  TIM_USE_ANY, 0, 0, 0),

    DEF_TIM(TIM9,  CH1,  PB14, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM9,  CH1,  PA0,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM9,  CH1,  PC9,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM9,  CH2,  PB15, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM9,  CH2,  PA1,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM9,  CH2,  PC10, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM9,  CH2,  PB10, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM9,  CH3,  PE5,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM9,  CH3,  PA2,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM9,  CH3,  PC12, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM9,  CH3,  PB12, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM9,  CH4,  PE6,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM9,  CH4,  PA3,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM9,  CH4,  PC11, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM9,  CH4,  PB11, TIM_USE_ANY, 0, 0, 0),


    DEF_TIM(TIM10,  CH1,  PF8, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM10,  CH1,  PA6, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM10,  CH1,  PB6, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM10,  CH2,  PF9, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM10,  CH2,  PA7, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM10,  CH2,  PB7, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM10,  CH3,  PF6, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM10,  CH3,  PA3, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM10,  CH3,  PB8, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM10,  CH4,  PF7, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM10,  CH4,  PA4, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM10,  CH4,  PB9, TIM_USE_ANY, 0, 0, 0),

    DEF_TIM(TIM11,  CH1,  PD3, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM11,  CH1,  PF6, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM11,  CH1,  PE0, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM11,  CH2,  PD4, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM11,  CH2,  PF7, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM11,  CH2,  PE1, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM11,  CH3,  PD5, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM11,  CH3,  PF8, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM11,  CH3,  PD7, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM11,  CH4,  PD6, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM11,  CH4,  PF9, TIM_USE_ANY, 0, 0, 0),

    DEF_TIM(TIM12,  CH1,  PB1,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM12,  CH1,  PE3,  TIM_USE_ANY, 0, 0, 0),

    DEF_TIM(TIM12,  CH2,  PB2,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM12,  CH2,  PE4,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM12,  CH2,  PE13, TIM_USE_ANY, 0, 0, 0),

    DEF_TIM(TIM12,  CH3,  PF12, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM12,  CH3,  PE5,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM12,  CH3,  PE14, TIM_USE_ANY, 0, 0, 0),

    DEF_TIM(TIM12,  CH4,  PF13, TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM12,  CH4,  PE6,  TIM_USE_ANY, 0, 0, 0),
    DEF_TIM(TIM12,  CH4,  PE15, TIM_USE_ANY, 0, 0, 0),

};
#endif


uint32_t timerClock(TIM_TypeDef *tim)
{
    UNUSED(tim);
    return HCLKClock;
}
#endif
