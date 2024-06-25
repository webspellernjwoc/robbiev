/***********************************************************************//** 
 * \file  pm_102.c
 * \brief  power management
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-10 <td>V0.0  <td>WNN   <td>initial for csi
 * </table>
 * *********************************************************************
*/

#include <drv/common.h>
#include <csp_syscon.h>
#include <soc.h>

#ifdef CONFIG_USER_PM
extern pm_core_t g_tPmCore;
#endif 

static csi_error_t soc_sleep(csi_pm_mode_t mode)
{
	csi_error_t ret = CSI_OK;
	switch (mode)
	{
		case (PM_MODE_SLEEP):
			__WFI();
			break;
		case (PM_MODE_DEEPSLEEP):
			__STOP();
			break;
		default:
			ret = CSI_ERROR;
			break;
	}
	return ret;
}
/**
  \brief       choose the pmu mode to enter
  \param[in]   handle  pmu handle to operate.
  \param[in]   mode    \ref pmu_mode_e
  \return      error code
*/
csi_error_t soc_pm_enter_sleep(csi_pm_mode_t mode)
{
	
	switch (mode)
	{
		case PM_MODE_SLEEP:		
			#ifdef CONFIG_USER_PM
			g_tPmCore.prepare_to_sleep();
			#endif
			soc_sleep(PM_MODE_SLEEP);	
			#ifdef CONFIG_USER_PM
			g_tPmCore.wkup_frm_sleep();		
			#endif
			break;
		case PM_MODE_DEEPSLEEP:
			#ifdef CONFIG_USER_PM
			g_tPmCore.prepare_to_deepsleep();
			#endif
			soc_sleep(PM_MODE_DEEPSLEEP);	
			#ifdef CONFIG_USER_PM
			g_tPmCore.wkup_frm_deepsleep();
			#endif
			break;
		default:
            return CSI_ERROR;
	}
	return CSI_OK;
}

/**
  \brief       Config the wakeup source.
  \param[in]   wakeup_num wakeup source num
  \param[in]   enable  flag control the wakeup source is enable or not
  \return      error code
*/
csi_error_t soc_pm_config_wakeup_source(wakeupn_type_e eWkupSrc, bool bEnable)
{
    uint32_t wIrqNum;
	csi_error_t ret = CSI_OK;
	csp_syscon_t *ptSysconBase  = (csp_syscon_t*)APB_SYS_BASE;
	switch (eWkupSrc)
		{
			case (WKUP_IWDT):
				wIrqNum = 1;
				break;
			case (WKUP_RTC):
				wIrqNum = 12;
				break;
			case (WKUP_LPT):
				wIrqNum = 26;
				break;
			case (WKUP_LVD):
				wIrqNum = 1;
				break;
			case (WKUP_TCH):
				wIrqNum = 25;
				break;
			case (WKUP_EXI0):
				wIrqNum = 7;
				break;
			case (WKUP_EXI1):
				wIrqNum = 8;
				break;
			case (WKUP_EXI2):
				wIrqNum = 21;
				break;
			case (WKUP_EXI3):
				wIrqNum = 22;
				break;
			case (WKUP_EXI4):
				wIrqNum = 23;
			default: 
				return CSI_ERROR;
		}
	if (eWkupSrc<5) {
		if (bEnable)
			csi_vic_set_wakeup_irq(wIrqNum);
		else
			csi_vic_clear_wakeup_irq(wIrqNum);
		return ret;
	}
	
	if (bEnable){
		ptSysconBase->WKCR |= 0x1 << (eWkupSrc);
		csi_vic_set_wakeup_irq(wIrqNum);
	}
	else{
		ptSysconBase->WKCR &= ~(0x1 << (eWkupSrc));
		csi_vic_clear_wakeup_irq(wIrqNum);
	}
	return ret;
}