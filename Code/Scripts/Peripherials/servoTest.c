
// Servo : TIM3->CCR3 (500;1000)

/*
 * TIM3->CCR4 = 990;
	TIM3->CCR3 = 990;


	  // Start servo1 timer
	  HAL_TIM_PWM_Start(Get_TIM3_Instance(), TIM_CHANNEL_3);
	  HAL_TIM_PWM_Start(Get_TIM3_Instance(), TIM_CHANNEL_4);
	//ser1.ccr = &(TIM3->CCR3);
	 */

	/*
	bmp_getData();
	gps_getData();
	//gps_printData();
	radio_transmit();

	if (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_RESET && !isClicked)
	{
		HAL_GPIO_WritePin(LEDC_GPIO_Port, LEDC_Pin, GPIO_PIN_SET);
		isClicked = true;
		flippo = false;
		click = millis();
	}

	if (millis() - click >= 1000 * 45 && !flippo)
	{
		flippo = true;
		TIM3->CCR3 = 500;
		TIM3->CCR4 = 500;
		HAL_GPIO_WritePin(LEDC_GPIO_Port, LEDC_Pin, GPIO_PIN_RESET);
	}
*/
