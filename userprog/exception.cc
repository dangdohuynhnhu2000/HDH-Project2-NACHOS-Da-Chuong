// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#define MAX_STRING_LENGTH   255


//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void IncreasePC()
{
	int counter = machine->ReadRegister(PCReg);
   	machine->WriteRegister(PrevPCReg, counter);
    	counter = machine->ReadRegister(NextPCReg);
    	machine->WriteRegister(PCReg, counter);
   	machine->WriteRegister(NextPCReg, counter + 4);
}

// Input: - User space address (int)
// - Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space
char* User2System(int virtAddr,int limit)
{
 	int i;// index
 	int oneChar;
 	char* kernelBuf = NULL;
 	kernelBuf = new char[limit +1];//need for terminal string
 	if (kernelBuf == NULL)
 		return kernelBuf;
 	memset(kernelBuf,0,limit+1);
 	//printf("\n Filename u2s:");
 	for (i = 0 ; i < limit ;i++)
 	{
 		machine->ReadMem(virtAddr+i,1,&oneChar);
 		kernelBuf[i] = (char)oneChar;
 		//printf("%c",kernelBuf[i]);
 		if (oneChar == 0)
 			break;
 	}
 	return kernelBuf;
} 

// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr,int len,char* buffer)
{
 	if (len < 0) return -1;
 	if (len == 0)return len;
 	int i = 0;
 	int oneChar = 0 ;
 	do{
 		oneChar= (int) buffer[i];
 		machine->WriteMem(virtAddr+i,1,oneChar);
 		i ++;
 	}while(i < len && oneChar != 0);
 	return i;
} 


void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    switch(which)
    {
        case NoException:
	    return;
	case PageFaultException:
	    printf("No valid translation found!");
            interrupt->Halt();
            break;
	case ReadOnlyException:
	    printf("Write attempted to page marked \"read-only\"");
            interrupt->Halt();
            break;
	case BusErrorException:
	    printf("Translation resulted in an invalid physical address.");
            interrupt->Halt();
            break;
	case AddressErrorException:
	    printf("Unaligned reference or one that was beyond the end of the address space.");
            interrupt->Halt();
            break;
	case OverflowException:
	    printf("Integer overflow in add or sub.");
            interrupt->Halt();
            break;
	case IllegalInstrException:
	    printf("Unimplemented or reserved instr.");
            interrupt->Halt();
            break;
	case NumExceptionTypes:
	    printf("Num Exception Types");
	    interrupt->Halt();
            break;
	case SyscallException:
	    switch(type)
	    {
		case SC_Halt:
   		    interrupt->Halt();
		    break;
		case SC_ReadInt:
		{

			char* buffer;
			int max_buff = 255;
			buffer = new char[max_buff + 1];
			int numCount = gSynchConsole->Read(buffer, max_buff);
			int number = 0; //gia tri tra ve cua ham
				
			//chuyen doi tu buffer -> so nguyen

			bool isNegative = false; //gia su nhap vao la so duong
			int firstIndex = 0;
			int lastIndex = 0;
			if(buffer[0] == '-') //nhap vao so am
			{
				if(numCount == 1)
				{
					gSynchConsole->Write("\nDay khong phai so nguyen!", 26);
					machine -> WriteRegister(2,0);
					IncreasePC();
					delete buffer;
					return;	
				}
				else
				{
					isNegative = true;
					firstIndex = 1;
					lastIndex = 1;
				}
			}
			if (buffer == "") //chuoi rong
			{
				gSynchConsole->Write("\nDay khong phai so nguyen!", 26);
				machine -> WriteRegister(2,0);
				IncreasePC();
				delete buffer;
				return;
			}
			char outRange[11];
			int count;
			if(isNegative)
			{
				strcpy(outRange, "-2147483647");
				count = 11;
			}
			else 
			{
				strcpy(outRange, "2147483647");	
				count = 10;	
			}
			
			//neu so nhap vao vuot qua so nguyen thi tra ve 0
			if (numCount > count) 
			{
				
				gSynchConsole->Write("\nSo vuot ngoai mien bieu dien!", 30);
				machine -> WriteRegister(2,0);
				IncreasePC();
				delete buffer;
				return;
			}
			if (numCount == count)
			{
				for (int i = firstIndex; i < count; i++)
				{
					if (buffer[i] > outRange[i])
					{
						gSynchConsole->Write("\nSo vuot ngoai mien bieu dien!", 30);
						machine -> WriteRegister(2,0);
						IncreasePC();
						delete buffer;
						return;
					}
				}
			}
			
			for(int i = firstIndex; i < numCount; i++)
			{
				if(buffer[i] == '.')
				{
						
					for(int j = i+1;j < numCount;j++)
					{
						if(buffer[j] != '0')
						{
							gSynchConsole->Write("\nDay khong phai so nguyen!", 26);
							machine -> WriteRegister(2,0);
							IncreasePC();
							delete buffer;
							return;
						}
					}
					lastIndex = i -1;
					break;
				}
				else if(buffer[i] < '0' || buffer[i] > '9')
				{
					gSynchConsole->Write("\nDay khong phai so nguyen!", 26);
					machine -> WriteRegister(2,0);
					IncreasePC();
					delete buffer;
					return;
				}
				lastIndex = i;
			}
			//neu nhap vao so nguyen hop le -> tien hanh chuyen chuoi thanh int
			for(int i = firstIndex;i<=lastIndex;i++)
			{
				number = number * 10 + (int)(buffer[i] - 48);
				
			}
			if(isNegative)
			{
				number = number * -1;
			}		
			machine->WriteRegister(2,number);

			IncreasePC();

			delete buffer;

			return;


		}
		case SC_PrintInt:
		{
		    int number = machine->ReadRegister(4);


		    if(number == 0)
                    {
			char* buffer = new char[1];
			buffer[0] = '0';
                        gSynchConsole->Write(buffer, 1); // Neu la so 0 thi chi can in ra man hinh "0"
		        IncreasePC();
                        return;    
                    }


		    bool is_negative = false;
		    int len_number = 0;
		    int temp;
		    int first_index = 0;
		    char* buffer;

		    if (number<0)
		    {
			number = number * -1;
			is_negative = true;
			first_index = 1;
		    }

		    //tinh so chu so cua number
		    temp = number;
		    do
		    {
			len_number++;
			temp = temp/10;
		    } while(temp>=1);

		    buffer = new char[len_number+2];

		// neu la so am thi them dau '-' truoc chuoi
		    if (is_negative == true)
		    {
			buffer[0] = '-';
		    }
		    for (int i = first_index + len_number-1; i>=first_index; i--)
		    {
			buffer[i] = char((number%10)+48); //chuyen tung chu so thanh ki tu
			number = number/10;
		    }
		    buffer[first_index + len_number] = '\0';
		    gSynchConsole->Write(buffer, len_number + first_index);
		    delete buffer;
		    IncreasePC();

		    break;
	    }

	    case SC_ReadChar:
	    {
		int maxByte = 255;
		char* buffer = new char[maxByte];
		int numBytes = gSynchConsole->Read(buffer, maxByte);
		if(numBytes > 1) //nhap vao nhieu hon 1 ky tu => khong hop le
		{
			//neu nhap vao nhieu hon 1 ky tu => lay ki tu dau tien
			char c = buffer[0];
			machine->WriteRegister(2,c);
		}
		else if(numBytes == 0) //Ky tu rong
		{
			gSynchConsole->Write("\nKhong co ki tu nao duoc nhap!", 30); 
			machine->WriteRegister(2,0);
		}
		else
		{
			//ky tu nhap vao hop le
			char c = buffer[0];
			machine->WriteRegister(2,c);
		}

		IncreasePC();
		delete buffer;
		break;
	    }

	    case SC_PrintChar:
	    {
	     	char ch = (char) machine->ReadRegister(4);
		gSynchConsole->Write(&ch, 1);
		IncreasePC();
		return;
	    }

	    case SC_ReadString:
	    {
		char* buffer;
		int virtAddress, length;
		
		//doc cac tham so nguoi dung truyen vao
		virtAddress = machine->ReadRegister(4);
		length = machine->ReadRegister(5);

		//copy chuoi tu vung nho user sang system
		buffer = User2System(virtAddress, length);
		
		//lay chuoi nguoi dung nhap vao
		gSynchConsole->Read(buffer, length);

		//coppy chuoi tu vung nho cua system sang vung nho cua user
		System2User(virtAddress, length, buffer);
		
		delete buffer;
		IncreasePC();
		break;
	    }

	    case SC_PrintString:
	    {
		int virt_address; 
		int i=0;
		int len_buffer = 0;
		char* buffer;

		virt_address = machine->ReadRegister(4);

		//Sao chep buffer tu User memory space vao System memory space 
		buffer = User2System(virt_address, MAX_STRING_LENGTH);

		//tinh do dai cua chuoi can in 
		while(buffer[i]!='\0')
		{
			len_buffer++;
			i++;
			
			//Truong hop chuoi can in vuot qua chieu dai quy dinh
			if (len_buffer == MAX_STRING_LENGTH)
			{
				gSynchConsole->Write("\nChuoi vuot qua do dai MAX_STRING_LENGTH!", 41);
				IncreasePC();
				return;
			}
		}
		
		//in ra man hinh console
		gSynchConsole->Write(buffer, len_buffer+1);

		delete buffer;	
		IncreasePC();	
		break;
	     }

	    default: 
		break;
		
	    }
	    break;
     }

}
