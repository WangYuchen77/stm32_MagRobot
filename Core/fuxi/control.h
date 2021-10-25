// copyright: @fuxi_robot corporation
#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "main.h"
#include <stdio.h>
#include <string.h>

#define MOTOR_SPEED_MAX 5500
#define MOTOR_SPEED_MIN 1000
#define MOTOR_SPEED_INITIAL 2000
#define MOTOR_SPEED_DELTA 500

// card_id function
#define CARD_ID_STOPPED 90
#define CARD_ID_CHARGE 91
#define CARD_ID_SPEED_LOW 92
#define CARD_ID_SPEED_MIDDLE 93
#define CARD_ID_SPEED_HIGH 94
#define CARD_ID_SLEEP_15 95
#define CARD_ID_SLEEP_30 96
#define CARD_ID_SLEEP_60 97

#define MAGNETIC_BUFFER_LEN 8

#define abs(x) ((x)>0?(x):-(x))
// robot status
enum Robot_status
{
	READY = 0,
	STOPPED,
	RUNNING,
	DERAILED
};

// command people
enum Command_kind
{
	RUN = 0,
	STOP,
	SET_SPEED,
	SET_TARGET,
	CHARGE,
	WALK_FORWARD,
	WALK_REAR,
	WALK_LEFT,
	WALK_RIGHT,
	QUERY_STATUS,
	QUERY_SPEED,
	QUERY_TARGET,
	QUERY_ALL,
	COMMAND_ERROR,
};

enum Motor_state
{
	MOTOR_ENABLE = 0,
	MOTOR_DISABLE,
};

struct Command
{
	enum Command_kind m_command_kind;
	int m_command_value;
};

struct Robot
{
	enum Robot_status m_robot_status;
	int m_speed;
	int m_target;
	int m_motor_left_value;
	int m_motor_right_value;
	int m_direction;
	
	int m_magnetic_value;
	int m_magnetic_status_0_1[MAGNETIC_BUFFER_LEN];
};


int run(struct Robot robot, struct Command command);

int query(struct Robot robot, struct Command command, enum Motor_state motor, int query_ind);
struct Command translate_infor_2_command(char info_buffer[256]);
struct Robot translate_magnetic_2_motor(struct Robot robot, int magnetic_value);




#endif
