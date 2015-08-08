#include "mychip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void InitializeChip8()
{
	unsigned int i;

	for(i=0;i<15;i++)
	{
		V[i] = 0;		//寄存器清零
		Stack[i] = 0;	//栈清零
		Keyboard[i] = 0;//按键清零	
	}

	for(i=0;i<4095;i++)
		Memory[i] = 0;	//内存清零

	for(i=0; i<2048; ++i)
		Gfx[i] = 0;		//显存清零

	I = 0;				//地址寄存器清零
	Sp = 0;				//栈指针清零
	Pc = 0x200;			//PC指针指向程序开始的地方
	Opcode = 0;			//操作码清零
	Delaytimer = 0;		//定时器清零
	Soundtimer = 0;

	DrawFlag = 1;	//绘画标识为真

	srand(time(NULL)); //产生随机数种子
}

void HandleOpcode()
{
	int i;

	Opcode = Memory[Pc] << 8 | Memory[Pc+1]; //取操作码，高位在低地址

	switch(Opcode & 0xF000)
	{
	case 0x0000:
		switch(Opcode & 0x000F)
		{
		case 0x0000:	//Clears the screen.
			for(i=0; i<2048; i++)
				Gfx[i] = 0x0;
			DrawFlag = 1;
			Pc += 2;
			break;
		case 0x000E:	//Returns from a subroutine.
			Sp -= 1;
			Pc = Stack[Sp];
			Pc += 2; //Important!
			break;
		default:
			printf ("Unknown Opcode [0x0000]: 0x%X\n", Opcode);	
		}
		break;

	case 0x1000: // Jumps to address NNN.
		Pc = Opcode & 0x0FFF;
		break;

	case 0x2000: //Calls subroutine at NNN.
		Stack[Sp] = Pc;
		Sp++;
		Pc = Opcode & 0x0FFF;
		break;

	case 0x3000: //Skips the next instruction if VX equals NN.
		if( V[(Opcode&0x0F00) >> 8] == (Opcode&0x00FF))
			Pc += 4;
		else
			Pc += 2;
		break;

	case 0x4000: // Skips the next instruction if VX doesn't equal NN.
		if( V[(Opcode&0x0F00) >> 8] != (Opcode&0x00FF))
			Pc += 4;
		else
			Pc += 2;
		break;

	case 0x5000: //Skips the next instruction if VX equals VY.
		if( V[(Opcode&0x0F00)>>8] == V[(Opcode&0x00F0)>>4])
			Pc += 4;
		else
			Pc += 2;
		break;

	case 0x6000: //Sets VX to NN.
		V[(Opcode&0x0F00) >> 8] = Opcode & 0xFF;
		Pc += 2;
		break;

	case 0x7000:	// 	Adds NN to VX.
		V[(Opcode&0x0F00)>>8] += Opcode & 0xFF;
		Pc += 2;
		break;

	case 0x8000:	
		switch(Opcode & 0x000F)
		{
		case 0x0000:	// 	Sets VX to the value of VY.
			V[(Opcode&0x0F00)>>8] = V[(Opcode&0x00F0)>>4];
			Pc += 2;
			break;

		case 0x0001:	//Sets VX to VX or VY.
			V[(Opcode&0x0F00)>>8] |= V[(Opcode&0x00F0)>>4];
			Pc += 2;
			break;

		case 0x0002:	//Sets VX to VX and VY.
			V[(Opcode&0x0F00)>>8] &= V[(Opcode&0x00F0)>>4];
			Pc += 2;
			break;

		case 0x0003:	// 	Sets VX to VX xor VY.
			V[(Opcode&0x0F00)>>8] ^= V[(Opcode&0x00F0)>>4];
			Pc += 2;
			break;

		case 0x0004:	//Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
			if (V[(Opcode&0x0F00)>>8] + V[(Opcode&0x00F0)>>4] > 255)
				V[15] = 1;
			else
				V[15] = 0;
			V[(Opcode&0x0F00)>>8] += V[(Opcode&0x00F0)>>4];
			Pc += 2;
			break;

		case 0x0005:	// 	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
			if(V[(Opcode & 0x00F0) >> 4] > V[(Opcode & 0x0F00) >> 8]) 
				V[15] = 0;
			else
				V[15] = 1;
			V[(Opcode&0x0F00)>>8] -= V[(Opcode&0x00F0)>>4];
			Pc += 2;
			break;

		case 0x0006:	// 	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
			V[15] = V[(Opcode&0x0F00)>>8] & 0x0001;
			V[(Opcode&0x0F00)>>8] >>= 1;	
			Pc += 2;
			break;

		case 0x0007:	// 	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
			if(V[(Opcode & 0x0F00) >> 8] > V[(Opcode & 0x00F0) >> 4])
				V[15] = 0;
			else
				V[15] = 1;
			V[(Opcode&0x0F00)>>8] = V[(Opcode&0x00F0)>>4] - V[(Opcode&0x0F00)>>8];
			Pc += 2;
			break;

		case 0x000E:	//Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
			V[15] = V[(Opcode&0x0F00)>>8] >> 7;
			V[(Opcode&0x0F00)>>8] <<= 1;	
			Pc += 2;
			break;
		default:
			printf ("Unknown Opcode [0x8000]: 0x%X\n", Opcode);
		}
		break;
	case 0x9000:	// 	Skips the next instruction if VX doesn't equal VY.
		if (V[(Opcode&0x0F00)>>8] != V[(Opcode&0x00F0)>>4] )
			Pc += 4;
		else
			Pc += 2;
		break;

	case 0xA000:	//Sets I to the address NNN.
		I = Opcode & 0x0FFF;
		Pc += 2;
		break;

	case 0xB000:	// 	Jumps to the address NNN plus V0.
		Pc = V[0] + (Opcode & 0x0FFF);
		break;

	case 0xC000:	//Sets VX to a random number, masked by NN.
		V[(Opcode&0x0F00)>>8] = (rand() % 0xFF) & (Opcode & 0xFF); //random
		Pc += 2;
		break;

	case 0xD000:	// 	Sprites stored in memory at location in index register (I), maximum 8bits wide. Wraps around the screen. If when drawn, clears a pixel, register VF is set to 1 otherwise it is zero. All drawing is XOR drawing (i.e. it toggles the screen pixels)
	{
		unsigned short x = V[(Opcode & 0x0F00) >> 8];
		unsigned short y = V[(Opcode & 0x00F0) >> 4];
		unsigned short height = Opcode & 0x000F;
		unsigned short pixel;
		unsigned int yline, xline;

		V[15] = 0;
		for (yline = 0; yline < height; yline++)
		{
			pixel = Memory[I + yline];
			for(xline = 0; xline < 8; xline++)
			{
				if((pixel & (0x80 >> xline)) != 0)
				{
					if(Gfx[(x + xline + ((y + yline) * 64))] == 1)
					{
						V[0xF] = 1;                                    
					}
					Gfx[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}
						
		DrawFlag = 1;
		Pc += 2;
	}
	break;

	case 0xE000:
		switch(Opcode & 0x00FF)
		{
		case 0x009E:	// 	Skips the next instruction if the key stored in VX is pressed.
			if(Keyboard[V[(Opcode & 0x0F00) >> 8]]!= 0)
				Pc += 4;
			else
				Pc += 2;
			break;
		case 0x00A1:	//Skips the next instruction if the key stored in VX isn't pressed.
			if(Keyboard[V[(Opcode & 0x0F00) >> 8]] == 0)
				Pc += 4;
			else
				Pc += 2;
			break;
		default:
			printf ("Unknown Opcode [0xE000]: 0x%X\n", Opcode);
		}
		break;

	case 0xF000:	
		switch(Opcode & 0xFF)
		{
		case 0x07:	//Sets VX to the value of the delay timer.
			V[(Opcode&0x0F00)>>8] = Delaytimer;
			Pc += 2;
			break;
		case 0x0A:	//A key press is awaited, and then stored in VX.
		{
			unsigned char keyPress = 0;

			for(i = 0; i < 16; ++i)
			{
				if(Keyboard[i] != 0)
				{
					V[(Opcode & 0x0F00) >> 8] = i;
					keyPress = 1;
				}
			}
			if(!keyPress)						
				return;

			Pc += 2;
		}
		break;

		case 0x15:	//Sets the delay timer to VX.
			Delaytimer = V[(Opcode&0x0F00)>>8];
			Pc += 2;
			break;

		case 0x18:	// 	Sets the sound timer to VX.
			Soundtimer = V[(Opcode&0x0F00)>>8];
			Pc += 2;
			break;

		case 0x1E:	//Adds VX to I
			if(I + V[(Opcode & 0x0F00) >> 8] > 0xFFF)	
				V[15] = 1;
			else
				V[15] = 0;
			I += V[(Opcode&0x0F00) >> 8];
			Pc += 2;
			break;

		case 0x29:	//Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
			I = V[(Opcode&0x0F00)>>8] * 0x5;
			Pc += 2;
			break;

		case 0x33:	// 	Stores the Binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
			Memory[I] = V[(Opcode&0x0F00)>>8] / 100;
			Memory[I+1] = V[(Opcode&0x0F00)>>8] / 10 % 10;
			Memory[I+2] = (V[(Opcode&0x0F00)>>8] % 100) % 10;
			Pc += 2;
			break;

		case 0x55:	// 	Stores V0 to VX in memory starting at address I

			for(i=0; i<((Opcode&0x0F00)>>8); i++)
			{
				Memory[I+i] = V[i];
			
			}
			I += ((Opcode & 0x0F00) >> 8) + 1;
			Pc += 2;
			break;

		case 0x65:	// 	Fills V0 to VX with values from memory starting at address I
			for(i=0; i<((Opcode&0x0F00)>>8); i++)
			{
				V[i] = Memory[I+i];
			}
			I += ((Opcode & 0x0F00) >> 8) + 1;
			Pc += 2;
			break;
		default:
					printf ("Unknown Opcode [0xF000]: 0x%X\n", Opcode);
		}
		break;
		default:
			printf ("Unknown Opcode: 0x%X\n", Opcode);
	}

			
	if(Delaytimer > 0)
		--Delaytimer;

	if(Soundtimer > 0)
	{
		if(Soundtimer == 1)
			printf("BEEP!\n");
		--Soundtimer;
	}	

}

int LoadApp(const char *filename)
{
	long lSize;
	char *buffer=NULL;
	FILE *pFile =NULL;
	size_t result;
	int i;

	InitializeChip8();	

	pFile = fopen(filename, "rb");
	if (pFile == NULL)
	{
		fputs ("File error", stderr);
	}

	fseek(pFile, 0, SEEK_END);//移动指针到文件末尾
	lSize = ftell(pFile);//获得文件大小
	rewind(pFile);//移动指针到文件开头
	printf("Filesize: %d\n", (int)lSize);

	buffer = (char*)malloc(sizeof(char)*lSize);
	if (buffer == NULL)
	{
		fputs ("Memory error", stderr); 
		return 0;
	}

	// Copy the file into the buffer
	result = fread (buffer, 1, lSize, pFile);
	if (result != lSize) 
	{
		fputs("Reading error",stderr); 
		return 0;
	}

	// Copy buffer to Chip8 memory
	if((4096-512) > lSize)
	{
		for(i = 0; i < lSize; ++i)
			Memory[i + 512] = buffer[i];
	}
	else
		printf("Error: ROM too big for memory");

	fclose(pFile);
	free(buffer);

    return 1;
}
