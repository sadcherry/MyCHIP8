//一些CHIP8的变量声明和相关函数声明

//变量声明
unsigned char Gfx[64*32];	//chip8显存
unsigned char V[16]; //16个寄存器，V0~VF 
unsigned char Memory[4096]; //4K内存
unsigned short int I; //地址寄存器
unsigned short int Pc; //程序指针
unsigned short int Stack[16];//栈
unsigned short int Sp; //栈指针
unsigned char Keyboard[16];//16个键值
unsigned short int Opcode; //操作码
unsigned char Delaytimer;//延时定时器
unsigned char Soundtimer;//声音定时器
unsigned char DrawFlag;	//绘图标识

//函数声明
void InitializeChip8();	//CHIP8初始化
void HandleOpcode();	//操作码处理，核心部分
int  LoadApp(const char *filename); //加载应用（游戏）
