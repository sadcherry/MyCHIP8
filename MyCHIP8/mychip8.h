//һЩCHIP8�ı�����������غ�������

//��������
unsigned char Gfx[64*32];	//chip8�Դ�
unsigned char V[16]; //16���Ĵ�����V0~VF 
unsigned char Memory[4096]; //4K�ڴ�
unsigned short int I; //��ַ�Ĵ���
unsigned short int Pc; //����ָ��
unsigned short int Stack[16];//ջ
unsigned short int Sp; //ջָ��
unsigned char Keyboard[16];//16����ֵ
unsigned short int Opcode; //������
unsigned char Delaytimer;//��ʱ��ʱ��
unsigned char Soundtimer;//������ʱ��
unsigned char DrawFlag;	//��ͼ��ʶ

//��������
void InitializeChip8();	//CHIP8��ʼ��
void HandleOpcode();	//�����봦�����Ĳ���
int  LoadApp(const char *filename); //����Ӧ�ã���Ϸ��
