#include "RoverCamera.h"

RoverCamera::RoverCamera()
{
	frame_length=0;
	vc_frame_address =0;
	last_data_length=0;
	
	Serial2.begin(115200);
	
	/*
	capture_photo();
	delay(100);
	 */
	
	VC0706_frame_control(3);
	delay(10);
	
	//motion windows setting
	VC0706_motion_windows_setting(0x1a5a,0x5000);
	delay(10);
	VC0706_motion_windows_setting(0x1a5e,0x0a000f000);
	delay(10);
	VC0706_motion_windows_setting(0x1a62,0x3f010000);
	delay(10);
	VC0706_motion_windows_setting(0x1a66,0x3c007800);
	delay(10);
	VC0706_motion_windows_setting(0x1a6a,0x0b400ef00);
	delay(10);

	//start motion monitoring
	VC0706_motion_control(1);
	delay(10);
	
	VC0706_motion_detection(1);
	delay(10);
	
	Serial2.end();			// clear all rx buffer
	delay(5);
	
	Serial2.begin(115200);
	rx_ready=false;
	
	takingPicture = false;
}

void RoverCamera::takePicture()
{
	takingPicture = true;
	
	buffer_read();

	if(rx_ready){
		rx_ready=false;
		if (VC0706_rx_buffer[2]!=VC0706_COMMAND_COMM_MOTION_DETECTED) return;
		if (VC0706_rx_buffer[3]!=0x00) return;
		
		//stop motion detection for capture photo
		VC0706_motion_control(0);
		delay(10);
		VC0706_motion_detection(0);
		delay(1000);
		
		// capture current photo 1s later
		capture_photo();
		delay(100);
	
		VC0706_frame_control(3);	// resume AV out
		delay(10);

		//prepare next motion monitoring
		VC0706_motion_control(1);
		delay(10);
		VC0706_motion_detection(1);
		delay(10);
		
		Serial2.end();			// clear all rx buffer
		delay(5);
		
		Serial2.begin(115200);
		rx_ready=false;
		
		takingPicture = false;
		
		return;
	}

	delay(300);
}

/*******************************************************************************
 * Function Name  : VC0706_reset
 * Description    : Reset VC0706
 *                  
 * Input          : None
 *                  
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RoverCamera::VC0706_reset()
{
	tx_vcbuffer[0]=VC0706_PROTOCOL_SIGN;
	tx_vcbuffer[1]=VC0706_SERIAL_NUMBER;
	tx_vcbuffer[2]=VC0706_COMMAND_RESET;
	tx_vcbuffer[3]=0x00;
	
	tx_counter=4;

	buffer_send();
}

/*******************************************************************************
 * Function Name  : VC0706_get_version
 * Description    : Request version string from VC0706
 *                  
 * Input          : None
 *                  
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RoverCamera::VC0706_get_version()
{
	tx_vcbuffer[0]=VC0706_PROTOCOL_SIGN;
	tx_vcbuffer[1]=VC0706_SERIAL_NUMBER;
	tx_vcbuffer[2]=VC0706_COMMAND_GEN_VERSION;
	tx_vcbuffer[3]=0x00;
	
	tx_counter=4;

	buffer_send();
}

/*******************************************************************************
 * Function Name  : VC0706_osd_add_char
 * Description    : ADD OSD CHARACTERS TO CHANNELS(CHANNEL 1)
 *                  
 * Input          : col : Display column
 *	`		  row: Display Row
 *			  osd_string : display string (max 14 characters)
 *                  
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RoverCamera::VC0706_osd_add_char(int col, int row, String osd_string)
{
	unsigned char col_row;
	int string_length;
	int i;
	
	col&=0x0f;
	row&=0x0f;
	col_row=(unsigned char)(col<<4 | row);

	string_length=osd_string.length();
	if (string_length>14)
		string_length=14;		// max 14 osd characters
	
	tx_vcbuffer[0]=VC0706_PROTOCOL_SIGN;
	tx_vcbuffer[1]=VC0706_SERIAL_NUMBER;
	tx_vcbuffer[2]=VC0706_COMMAND_OSD_ADD_CHAR;
	tx_vcbuffer[3]=string_length+2;		
	tx_vcbuffer[4]=string_length;		// character number
	tx_vcbuffer[5]=col_row;					

	for (i=0;i<string_length;i++)
	{
		tx_vcbuffer[i+6]=osd_string.charAt(i);
	}
	
	tx_counter=string_length+6;

	buffer_send();
}

/*******************************************************************************
 * Function Name  : VC0706_w_h_downsize
 * Description    : control width and height downsize attribute
 *                  
 * Input          : scale_width = 0 1:1
 *                  			      = 1 1:2	
 *                  			      = 2 1:4	
 * 			  scale_height= 0 1:1
 *                  			      = 1 1:2
 *                  			      = 2 1:4
 *                  			
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RoverCamera::VC0706_w_h_downsize(int scale_width, int scale_height)
{
	int scale;

	if (scale_width>=2)	scale_width=2;
	if (scale_height>scale_width)	scale_height=scale_width;
	scale=(unsigned char)(scale_height<<2 | scale_width);


	tx_vcbuffer[0]=VC0706_PROTOCOL_SIGN;
	tx_vcbuffer[1]=VC0706_SERIAL_NUMBER;
	tx_vcbuffer[2]=VC0706_COMMAND_DOWNSIZE_SIZE;
	tx_vcbuffer[3]=0x01;

	tx_vcbuffer[4]=scale;		//bit[1:0] width zooming proportion
								//bit[3:2] height zooming proportion

	tx_counter=5;

	buffer_send();
}

/*******************************************************************************
 * Function Name  : VC0706_read_frame_buffer
 * Description    : read image data from FBUF 
 *                  
 * Input          : buffer_address(4 bytes); buffer_length(4 bytes)
 *                  
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RoverCamera::VC0706_read_frame_buffer(unsigned long buffer_address, unsigned long buffer_length)
{

	tx_vcbuffer[0]=VC0706_PROTOCOL_SIGN;
	tx_vcbuffer[1]=VC0706_SERIAL_NUMBER;
	tx_vcbuffer[2]=VC0706_COMMAND_READ_FBUF;
	tx_vcbuffer[3]=0x0c;
	tx_vcbuffer[4]=FBUF_CURRENT_FRAME;
	tx_vcbuffer[5]=0x0a;		// 0x0a=data transfer by MCU mode; 0x0f=data transfer by SPI interface
	tx_vcbuffer[6]=buffer_address>>24;			//starting address
	tx_vcbuffer[7]=buffer_address>>16;			
	tx_vcbuffer[8]=buffer_address>>8;			
	tx_vcbuffer[9]=buffer_address&0x0ff;			
	
	tx_vcbuffer[10]=buffer_length>>24;		// data length
	tx_vcbuffer[11]=buffer_length>>16;
	tx_vcbuffer[12]=buffer_length>>8;		
	tx_vcbuffer[13]=buffer_length&0x0ff;
	tx_vcbuffer[14]=0x00;		// delay time
	tx_vcbuffer[15]=0x0a;
	
	
	tx_counter=16;

	buffer_send();
}

/*******************************************************************************
 * Function Name  : VC0706_frame_control
 * Description    : control frame buffer register
 *                  
 * Input          : frame_control=control flag(1byte)
 *			: 		0 = stop current frame ; 1= stop next frame;2=step frame;3 =resume frame;
 *                  
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RoverCamera::VC0706_frame_control(byte frame_control)
{
	if(frame_control>3)frame_control=3;
	tx_vcbuffer[0]=VC0706_PROTOCOL_SIGN;
	tx_vcbuffer[1]=VC0706_SERIAL_NUMBER;
	tx_vcbuffer[2]=VC0706_COMMAND_FBUF_CTRL;
	tx_vcbuffer[3]=0x01;
	tx_vcbuffer[4]=frame_control;
	tx_counter=5;

	buffer_send();
}

/*******************************************************************************
 * Function Name  : VC0706_motion_detection
 * Description    : get motion monitoring status in communication interface.
 *                  
 * Input          : control_flag = 0 stop motion monitoring
 *					      = 1 start motion monitoring                  
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RoverCamera::VC0706_motion_detection(int control_flag)
{
	if(control_flag>1)control_flag=1;
	tx_vcbuffer[0]=VC0706_PROTOCOL_SIGN;
	tx_vcbuffer[1]=VC0706_SERIAL_NUMBER;
	tx_vcbuffer[2]=VC0706_COMMAND_COMM_MOTION_CTRL;
	tx_vcbuffer[3]=0x01;
	tx_vcbuffer[4]=control_flag;
	tx_counter=5;

	buffer_send();
}

/*******************************************************************************
 * Function Name  : VC0706_motion_control
 * Description    : motion control
 *                  
 * Input          : control_flag = 0 forbid motion monitoring
 *					      = 1 enable motion monitoring                  
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RoverCamera::VC0706_motion_control(int control_flag)
{
	if(control_flag>1)control_flag=1;
	tx_vcbuffer[0]=VC0706_PROTOCOL_SIGN;
	tx_vcbuffer[1]=VC0706_SERIAL_NUMBER;
	tx_vcbuffer[2]=VC0706_COMMAND_MOTION_CTRL;
	tx_vcbuffer[3]=0x03;
	tx_vcbuffer[4]=0x00;			//motion control attribute
	tx_vcbuffer[5]=0x01;			//mcu uart control
	tx_vcbuffer[6]=control_flag;	
	tx_counter=7;

	buffer_send();
}

/*******************************************************************************
 * Function Name  : VC0706_get_framebuffer_length
 * Description    : get byte-lengths in FBUF
 *                  
 * Input          : fbuf_type =current or next frame
 *			            0   =  current frame
 *				     1   =  next frame
 *                  
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RoverCamera::VC0706_get_framebuffer_length(byte fbuf_type)
{
	if(fbuf_type>1)fbuf_type=1;
	tx_vcbuffer[0]=VC0706_PROTOCOL_SIGN;
	tx_vcbuffer[1]=VC0706_SERIAL_NUMBER;
	tx_vcbuffer[2]=VC0706_COMMAND_GET_FBUF_LEN;
	tx_vcbuffer[3]=0x01;
	tx_vcbuffer[4]=fbuf_type;
	tx_counter=5;

	buffer_send();
}

/*******************************************************************************
 * Function Name  : VC0706_uart_power_save
 * Description    : stop current frame for reading
 *                  
 * Input          : power_on =1  start power-save
 *					   = 0  stop power-save
 *                  
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RoverCamera::VC0706_uart_power_save(byte power_save_on)
{
	tx_vcbuffer[0]=VC0706_PROTOCOL_SIGN;
	tx_vcbuffer[1]=VC0706_SERIAL_NUMBER;
	tx_vcbuffer[2]=VC0706_COMMAND_POWER_SAVE_CTRL;
	tx_vcbuffer[3]=0x03;
	tx_vcbuffer[4]=00;			//power save control mode
	tx_vcbuffer[5]=01;			// control by UART
	tx_vcbuffer[6]=power_save_on;			//start power save
	tx_counter=7;

	buffer_send();
}

/*******************************************************************************
 * Function Name  : VC0706_uart_color_control
 * Description    : stop current frame for reading
 *                  
 * Input          : show_mode = 0  automatically step black-white and colour
 *						   1  manually step color, select colour
 *						   2	manually step color, select black-white
 *                  
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RoverCamera::VC0706_uart_color_control(byte show_mode)
{
	if(show_mode>2) show_mode=2;
	tx_vcbuffer[0]=VC0706_PROTOCOL_SIGN;
	tx_vcbuffer[1]=VC0706_SERIAL_NUMBER;
	tx_vcbuffer[2]=VC0706_COMMAND_COLOR_CTRL;
	tx_vcbuffer[3]=0x02;
	tx_vcbuffer[4]=01;		//control by UART
	tx_vcbuffer[5]=show_mode;		// automatically step black-white and colour
	tx_counter=6;

	buffer_send();
}

/*******************************************************************************
 * Function Name  : VC0706_compression_ratio
 * Description	  : stop current frame for reading
 *					
 * Input		  : ration		>13(minimum)
 *						<63(max)
 *					
 * Output		  : None
 * Return		  : None
 *******************************************************************************/
void RoverCamera::VC0706_compression_ratio(int ratio)
{
	if(ratio>63)ratio=63;
	if(ratio<13)ratio=13;
	int vc_comp_ratio=(ratio-13)*4+53;
	tx_vcbuffer[0]=VC0706_PROTOCOL_SIGN;
	tx_vcbuffer[1]=VC0706_SERIAL_NUMBER;
	tx_vcbuffer[2]=VC0706_COMMAND_WRITE_DATA;
	tx_vcbuffer[3]=0x05;
	tx_vcbuffer[4]=01;		//chip register
	tx_vcbuffer[5]=0x01;	//data num ready to write
	tx_vcbuffer[6]=0x12;	//register address
	tx_vcbuffer[7]=0x04;
	tx_vcbuffer[8]=vc_comp_ratio; //data

	tx_counter=9;

	buffer_send();
}

/*******************************************************************************
 * Function Name  : VC0706_motion_windows_setting
 * Description	  : motion windows setting
 *					
 * Input		  : register_address(2 bytes); 
 *				data(4 bytes)= data ready to write
 *					
 * Output		  : None
 * Return		  : None
 *******************************************************************************/
void RoverCamera::VC0706_motion_windows_setting(unsigned int register_address, unsigned long data)
{
	tx_vcbuffer[0]=VC0706_PROTOCOL_SIGN;
	tx_vcbuffer[1]=VC0706_SERIAL_NUMBER;
	tx_vcbuffer[2]=VC0706_COMMAND_WRITE_DATA;
	tx_vcbuffer[3]=0x08;
	tx_vcbuffer[4]=01;		//chip register
	tx_vcbuffer[5]=0x04;	//data num ready to write
	tx_vcbuffer[6]=register_address>>8;	//register address
	tx_vcbuffer[7]=register_address&0x0ff;;

	tx_vcbuffer[8]=data>>24;		// data ready to write
	tx_vcbuffer[9]=data>>16;
	tx_vcbuffer[10]=data>>8;		
	tx_vcbuffer[11]=data&0x0ff;
	
	tx_counter=12;

	buffer_send();
}
	
/*******************************************************************************
 * Function Name  : debug_send
 * Description	  : Transmit buffer to Arduino Serial Monitor
 *					
 * Input		  : tx_vcbuffer
 *					
 * Output		  : None
 * Return		  : None
 *******************************************************************************/
void RoverCamera::debug_send()
{
	int i=0;

	for (i=0;i<tx_counter;i++)
	{
		Serial.print(tx_vcbuffer[i], HEX);
		Serial.print(", ");
	}

	Serial.println("");
}

/*******************************************************************************
 * Function Name  : buffer_send
 * Description    : Transmit buffer to VC0706
 *                  
 * Input          : tx_vcbuffer
 *                  
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RoverCamera::buffer_send()
{
	int i=0;

	for (i=0;i<tx_counter;i++)
		Serial2.write(tx_vcbuffer[i]);

	tx_ready=true;
}

/*******************************************************************************
 * Function Name  : buffer_read
 * Description    : Receive buffer from VC0706
 *                  
 * Input          : None
 *                  
 * Output         : rx_buffer, rx_ready
 * Return         : None
 *******************************************************************************/
void RoverCamera::buffer_read()
{
	bool validity=true;

	if (rx_ready)			// if something unread in buffer, just quit
		return;

	rx_counter=0;
	VC0706_rx_buffer[0]=0;
	while (Serial2.available() > 0) 
	{
		VC0706_rx_buffer[rx_counter++]= Serial2.read();
		//delay(1);
	}

	if (VC0706_rx_buffer[0]!=0x76)
		validity=false;
	if (VC0706_rx_buffer[1]!=VC0706_SERIAL_NUMBER)
		validity=false;

	if (validity) rx_ready=true;
}

void RoverCamera::capture_photo()
{
	VC0706_compression_ratio(63);
	delay(100);

	VC0706_frame_control(3);
	delay(10);
	
	VC0706_frame_control(0);
	delay(10);
	rx_ready=false;
	rx_counter=0;
	
	Serial2.end();			// clear all rx buffer
	delay(5);
	
	Serial2.begin(115200);

	//get frame buffer length
	VC0706_get_framebuffer_length(0);
	delay(10);
	buffer_read();

	// store frame buffer length for coming reading
	frame_length=(VC0706_rx_buffer[5]<<8)+VC0706_rx_buffer[6];
	frame_length=frame_length<<16;
	frame_length=frame_length+(0x0ff00&(VC0706_rx_buffer[7]<<8))+VC0706_rx_buffer[8];

	vc_frame_address =READ_DATA_BLOCK_NO;
		
	while(vc_frame_address<frame_length){	
		VC0706_read_frame_buffer(vc_frame_address-READ_DATA_BLOCK_NO, READ_DATA_BLOCK_NO);
		delay(9);

		//get the data with length=READ_DATA_BLOCK_NObytes 
		rx_ready=false;
		rx_counter=0;
		buffer_read();

		// write data to temp.jpg
		Serial.write(VC0706_rx_buffer+5,READ_DATA_BLOCK_NO);
	
		//read next READ_DATA_BLOCK_NO bytes from frame buffer
		vc_frame_address=vc_frame_address+READ_DATA_BLOCK_NO;
	}

	// get the last data
	vc_frame_address=vc_frame_address-READ_DATA_BLOCK_NO;
	last_data_length=frame_length-vc_frame_address;

	VC0706_read_frame_buffer(vc_frame_address,last_data_length);
	delay(9);
	
	//get the data 
	rx_ready=false;
	rx_counter=0;
	buffer_read();
			
	Serial.write(VC0706_rx_buffer+5,last_data_length);
	
	Serial.print("@@@@@");
}