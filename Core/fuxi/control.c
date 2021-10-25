#include "control.h"

int query(struct Robot robot, struct Command command, enum Motor_state motor, int query_ind)
{
	switch (query_ind)
	{
		// query state
		case 0:
			printf("< answer >< OK >[ query ][ all ]\n");
			printf("----- robot   - state: %d , speed: %d , target: %d , left: %d , right: %d , direction:%d\n",  
			robot.m_robot_status, 
			robot.m_speed, 
			robot.m_target,
			robot.m_motor_left_value,
			robot.m_motor_right_value,
			robot.m_direction);
		printf("----- robot   - magnetic value: %d , status: %d %d %d% d %d %d %d %d\n"
			,robot.m_magnetic_value
			,robot.m_magnetic_status_0_1[0]
			,robot.m_magnetic_status_0_1[1]
			,robot.m_magnetic_status_0_1[2]
			,robot.m_magnetic_status_0_1[3]
			,robot.m_magnetic_status_0_1[4]
			,robot.m_magnetic_status_0_1[5]
			,robot.m_magnetic_status_0_1[6]
			,robot.m_magnetic_status_0_1[7]);
			printf("----- motor   - status: %d\n", motor);
			printf("----- command - kind: %d , value: %d\n", command.m_command_kind, command.m_command_value);
			break;
		case 1:
			switch (robot.m_robot_status)
			{
				case READY:
					printf("< answer >< OK >[ query ][ state ]: READY\n");
					break;
				case STOPPED:
					printf("< answer >< OK >[ query ][ state ]: STOPPED\n");
					break;
				case RUNNING:
					printf("< answer >< OK >[ query ][ state ]: RUNNING\n");
					break;
				case DERAILED:
					printf("< answer >< OK >[ query ][ state ]: DERAILED\n");
					break;
			}
			break;
		// query speed
		case 2:
			printf("< answer >< OK >[ query ][ speed ]: %d\n", robot.m_speed);
			break;
		// query target
		case 3:
			if (robot.m_target != -1)
			{
				printf("< answer >< OK >[ query ][ target ]: %d\n", robot.m_target);
			}
			else
			{
				printf("< answer >< OK >[ query ][ target ]: no target!\n");
			}
			break; 
	}
	return 0;
}



int char_2_int(char* source)
{
	int number_length = strlen(source);
	int number_sum = 0;
	int number_positive = 1;
	int k = 0;
	
	if (source[0] -'-' == 0)
	{
		number_positive = -1;
		k = 1;
	}
	for(int i = k; i < number_length; i++)
	{
		number_sum = number_sum * 10 + (source[i] -'0');
	}
	number_sum = number_sum * number_positive;
	
//	printf("understand 111\n");
//	for (int i = 0 ; i < strlen(source); i++)
//	{
//		printf("%c", source[i]);
//	}
//	printf("\n");
//	
	return number_sum;
}

struct Command translate_infor_2_command(char info_buffer[256])
{
	struct Command command_output;

	int buffer_size = 1;
	int buffer_length = 0;
	
	for (int i = 0 ; i < 256; i ++)
	{
		if (info_buffer[i] == ' ')	
		{
			buffer_size = 2;
		}
		if (info_buffer[i] == ';')
		{
			buffer_length = i+1;
			break;
		}
	}
	
//	printf("info_buffer 111\n");
//	for (int i = 0 ; i < strlen(info_buffer); i++)
//	{
//		printf("%c", info_buffer[i]);
//	}
//	printf("\n");

	// move out ';'
	char* a = info_buffer;
	char TagName[256];
	memset(TagName,0x00,sizeof(TagName)); 
	strncpy(TagName, a, buffer_length-1);
	char* aa = TagName;
	
//	printf("after 111\n");
//	for (int i = 0 ; i < strlen(aa); i++)
//	{
//		printf("%c", aa[i]);
//	}
//	printf("\n");

	char seg[] = " ";
	char *command_buffer = strtok(aa, seg);
	if(command_buffer!=NULL)
	{
		char *second_str = strtok(NULL, seg);
		
//		printf("second 111\n");
//		for (int i = 0 ; i < strlen(second_str); i++)
//		{
//			printf("%c", second_str[i]);
//		}
//		printf("\n");
		
		if(second_str!=NULL)
		{
			if(strcmp(command_buffer, "speed") == 0)
			{
				int speed = char_2_int(second_str);
				
				if (abs(speed) > abs(MOTOR_SPEED_MAX) || abs(speed) < abs(MOTOR_SPEED_MIN))
				{
					printf("< command >< ERROR >: Speed is too high or too slow, error!\n");
					command_output.m_command_kind = COMMAND_ERROR;
					command_output.m_command_value = -1;
				}
				else
				{
					printf("< command >[ set ][ speed ]: %d\n", speed);
					command_output.m_command_kind = SET_SPEED;
					command_output.m_command_value = speed;
				}	
			}
			else if(strcmp(command_buffer, "to") == 0)
			{
				int target = char_2_int(second_str);
				printf("<command>[ set ][ target ]: %d\n", target);
				
				command_output.m_command_kind = SET_TARGET;
				command_output.m_command_value = target;
			}
			else if(strcmp(command_buffer, "query") == 0)
			{
				if (strcmp(second_str, "state") == 0)
				{
					printf("< command >[ query ][ state ]\n");
					command_output.m_command_kind = QUERY_STATUS;
					command_output.m_command_value = -1;
				}
				else if (strcmp(second_str, "speed") == 0)
				{
					printf("< command >[ query ][ speed ]\n");
					command_output.m_command_kind = QUERY_SPEED;
					command_output.m_command_value = -1;
				}
				else if (strcmp(second_str, "target") == 0)
				{
					printf("< command >[ query ][ target ]\n");
					command_output.m_command_kind = QUERY_TARGET;
					command_output.m_command_value = -1;
				}
				else if (strcmp(second_str, "all") == 0)
				{
					printf("< command >[ query ][ all ]\n");
					command_output.m_command_kind = QUERY_ALL;
					command_output.m_command_value = -1;
				}
				else
				{
					printf("< command >< ERROR >: Command is error!\n");
					command_output.m_command_kind = COMMAND_ERROR;
					command_output.m_command_value = -1;
				}
			}
		}
		else
		{
			if(strcmp(command_buffer, "start") == 0)
			{
				printf("< command >[ start ]\n");
				command_output.m_command_kind = RUN;
				command_output.m_command_value = -1;
			}
			else if(strcmp(command_buffer, "stop") == 0)
			{
				printf("< command >[ stop ]\n");
				command_output.m_command_kind = STOP;
				command_output.m_command_value = -1;
			}
			else if(strcmp(command_buffer, "charge") == 0)
			{
				printf("< command >[ charge ]\n");
				command_output.m_command_kind = CHARGE;
				command_output.m_command_value = CARD_ID_CHARGE;
			}
			else if(strcmp(command_buffer, "forward") == 0)
			{
				printf("< command >[ walk ][ forward ]\n");
				command_output.m_command_kind = WALK_FORWARD;
				command_output.m_command_value = -1;
			}
			else if(strcmp(command_buffer, "back") == 0)
			{
				printf("< command >[ walk ][ rear ]\n");
				command_output.m_command_kind = WALK_REAR;
				command_output.m_command_value = -1;
			}
			else if(strcmp(command_buffer, "left") == 0)
			{
				printf("< command >[ walk ][ left ]\n");
				command_output.m_command_kind = WALK_LEFT;
				command_output.m_command_value = -1;
			}
			else if(strcmp(command_buffer, "right") == 0)
			{
				printf("< command >[ walk ][ right ]\n");
				command_output.m_command_kind = WALK_RIGHT;
				command_output.m_command_value = -1;
			}
			else
			{
				printf("< command >< ERROR >: Command is error!\n");
				command_output.m_command_kind = COMMAND_ERROR;
				command_output.m_command_value = -1;
			}
		}
	}
	return command_output;
	
}

struct Robot translate_magnetic_2_motor(struct Robot robot, int magnetic_value)
{
	robot.m_magnetic_value  = magnetic_value;
	int temp = magnetic_value;
	int i = 0;
	while ( i < MAGNETIC_BUFFER_LEN)
	{
		robot.m_magnetic_status_0_1[i] = 0;
		robot.m_magnetic_status_0_1[i] = temp%2;
		temp = temp/2;
		i++;
	}
	
	int magnetic_value_left = robot.m_magnetic_status_0_1[0] + robot.m_magnetic_status_0_1[1] 
			+ robot.m_magnetic_status_0_1[2] +robot.m_magnetic_status_0_1[3];
	int magnetic_value_right = robot.m_magnetic_status_0_1[4] + robot.m_magnetic_status_0_1[5] 
			+ robot.m_magnetic_status_0_1[6] +robot.m_magnetic_status_0_1[7];
	
	int magnetic_diff = magnetic_value_left - magnetic_value_right;
		
	//printf("%d", magnetic_diff);
	if(magnetic_diff == 4)
	{
		robot.m_motor_left_value = robot.m_speed + 2 * MOTOR_SPEED_DELTA;
		robot.m_motor_right_value = robot.m_speed; 
		//printf("0");
	}
	else if (magnetic_diff == 3)
	{
		robot.m_motor_left_value = robot.m_speed + 1.5 * MOTOR_SPEED_DELTA;
		robot.m_motor_right_value = robot.m_speed;
		//printf("1");
	}
	else if (magnetic_diff == 2)
	{
		robot.m_motor_left_value = robot.m_speed + 1 * MOTOR_SPEED_DELTA;
		robot.m_motor_right_value = robot.m_speed;
	}
	else if (magnetic_diff == 1)
	{
		robot.m_motor_left_value = robot.m_speed + 0.5 * MOTOR_SPEED_DELTA;
		robot.m_motor_right_value = robot.m_speed;
	}
	else if (magnetic_diff == -1)
	{
		robot.m_motor_left_value = robot.m_speed;
		robot.m_motor_right_value = robot.m_speed + 0.5 * MOTOR_SPEED_DELTA;
	}
	else if (magnetic_diff == -2)
	{
		robot.m_motor_left_value = robot.m_speed;
		robot.m_motor_right_value = robot.m_speed + 1 * MOTOR_SPEED_DELTA;
	}
	else if (magnetic_diff == -3)
	{
		robot.m_motor_left_value = robot.m_speed;
		robot.m_motor_right_value = robot.m_speed + 1.5 * MOTOR_SPEED_DELTA;
	}
	else if (magnetic_diff == -4)
	{
		robot.m_motor_left_value = robot.m_speed;
		robot.m_motor_right_value = robot.m_speed + 2 * MOTOR_SPEED_DELTA;
	}
	else
	{
		robot.m_motor_left_value = robot.m_speed;
		robot.m_motor_right_value = robot.m_speed;
	}
	
	return robot;
}







 

