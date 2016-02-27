/***************************************************
	 ���ܣ��¶Ȳɼ����ֱ�����
	 	   ������·���������¶�ֵ��������EEPROM�У�
		   ���粻��ʧ��

***************************************************/
#define uchar unsigned char
#define uint unsigned int
sbit DS18B20_DQ=P2^0;
uchar Tem_dispbuf[5]={0,0,0,0,0};// ��ʾ�����ݴ�
uchar  data DS18B20_Temp_data[4]={0x00,0x00,0x00,0x00};	  //�����¶�ֵ������
uchar code  DS18B20_TEM_Deccode[16]={0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,			//�¶�С��λ�������
                      				 0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09};
bit DS18B20_Presence;			//18b20��λ�ɹ���ʾλ  =0 �ɹ�   =1 ʧ��
bit Symbol_flag=0;				//
void Delay_6us(uchar xus)	//6*xus+11us����ʱʱ��(������õ���)
{
	  while(xus--);
}

void DS18B20_RESET()	  //��λ
{
 	DS18B20_DQ=1;
	Delay_6us(2);

	DS18B20_DQ=0;
	Delay_6us(80);		        //����480us�ĵ͵�ƽ�ź�

	DS18B20_DQ=1;		        //���ߵȴ�����18b20�Ĵ��������ź�
	Delay_6us(10);

	DS18B20_Presence=DS18B20_DQ;

	Delay_6us(40);
	DS18B20_DQ=1;		        //����ʹ�����ڿ���״̬

}

void Write_DS18B20_OneChar(uchar dat)	  //дһ���ֽ�
{
	uchar i=0;
	for(i=8;i>0;i--)
	{
		DS18B20_DQ=0;
		DS18B20_DQ=dat&0x01;
		Delay_6us(5);
		DS18B20_DQ=1;
		dat>>=1;
	}
}

uchar Read_DS18B20_OneChar()		//��һ���ֽ�
{
	uchar dat=0;
	uchar i=0;
	for(i=8;i>0;i--)
	{
		DS18B20_DQ=0;
		dat>>=1;
		DS18B20_DQ=1;

		if(DS18B20_DQ)
		dat|=0x80;
		Delay_6us(4);
	}
	return dat;
}

void Read_18B20_Temperature()
{
	static unsigned char START_FLAG=0;		//��ȡDS18B20�ڲ�EEPROM��־λ
	DS18B20_RESET();						//��λ18B20
	if(!DS18B20_Presence)					//��λ�ɹ�
	{
		Write_DS18B20_OneChar(0XCC);		//���������к�
		Write_DS18B20_OneChar(0X44);		//�����¶�ת��
		Delay_6us(82);						//�ȴ��¶�ת��ʱ��500us����

		DS18B20_RESET();					//��λ18B20
		Write_DS18B20_OneChar(0XCC);		//����ƥ��ROMָ��
		Write_DS18B20_OneChar(0XBE);
		DS18B20_Temp_data[0]=Read_DS18B20_OneChar(); //Temperature LSB
		DS18B20_Temp_data[1]=Read_DS18B20_OneChar(); //Temperature MSB
		if(START_FLAG<3)
		{
			DS18B20_Temp_data[2]=Read_DS18B20_OneChar();  //�¶�����
			DS18B20_Temp_data[3]=Read_DS18B20_OneChar();  //�¶�����
			START_FLAG++;
		}
	}
		if(DS18B20_Temp_data[1]>0x80)
		Symbol_flag=1;
		else
		Symbol_flag=0;
		Tem_dispbuf[0]=DS18B20_TEM_Deccode[DS18B20_Temp_data[0]&0x0f];			//С��λ
		Tem_dispbuf[4]=((DS18B20_Temp_data[1]&0x0f)<<4)|((DS18B20_Temp_data[0]&0xf0)>>4);	//ȡ���¶�ֵ������λ
		Tem_dispbuf[3]=Tem_dispbuf[4]/100;
		Tem_dispbuf[2]=Tem_dispbuf[4]%100/10;
		Tem_dispbuf[1]=Tem_dispbuf[4]%10; 	
	
}

//void Set_DS18B20(uchar TH,uchar TL)  //�޸ı����¶Ⱥ��¶�ת������
//{
////	uchar i;
//	DS18B20_RESET();					//��λ18B20
//	Write_DS18B20_OneChar(0XCC);
//	Write_DS18B20_OneChar(0x4E); 	 //д��"д�ݴ���"����,�޸�TH��TL�ͷֱ������üĴ���
//							    //��дTH,��дTL,���д���üĴ���
//	Write_DS18B20_OneChar(TH);		//д�����趨���¶ȱ�������
//	Write_DS18B20_OneChar(TL);		//д�����趨���¶ȱ�������
//	Write_DS18B20_OneChar(0x7f);		//д���üĴ���,��ʽΪ0 R1 R0 1,1 1 1 1
//									//R1R0=00�ֱ���¦9λ,R1R0=11�ֱ���Ϊ12λ   
//									//��������
//	Delay_6us(10);					//�ȴ������ȶ�
//	DS18B20_RESET();
//	Write_DS18B20_OneChar(0xCC);    	// ����������кŵĲ���
//	Write_DS18B20_OneChar(0x48);		//�ص�����
//
//	DS18B20_RESET();
//	Write_DS18B20_OneChar(0xCC);    	// ����������кŵĲ���
//	Write_DS18B20_OneChar(0xB8);
//	while(!DS18B20_DQ);
//}  